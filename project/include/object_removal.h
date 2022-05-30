#pragma once

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui/highgui_c.h>
#include "user_interface.h"

class ObjectRemoval {
public:
	cv::Mat inputImage;
	cv::Mat maskImage;
	cv::Mat outputImage;

	bool isVertical;
	bool resizeToOrigin;
public:
	static bool isDrawing;
	static cv::Point currentPoint;
	static void onMouse(int event, int x, int y, int flag, void *param) {
		cv::Mat &img = *(cv::Mat *)param;

		switch (event) {
		case CV_EVENT_MOUSEMOVE:
			currentPoint = cv::Point(x, y);
			break;
		case CV_EVENT_LBUTTONDOWN:
			isDrawing = true;
			currentPoint = cv::Point(x, y);
			break;
		case CV_EVENT_LBUTTONUP:
			isDrawing = false;
			break;
		}
	}

	ObjectRemoval() {}
	ObjectRemoval(const char* filename, bool isVertical = false, bool resizeToOrigin = true) : isVertical(isVertical), resizeToOrigin(resizeToOrigin) {
		if (!load_image(filename, inputImage)) {
			maskImage = cv::Mat(inputImage.size(), CV_8UC3);
			maskImage = cv::Scalar(0, 0, 0);
			outputImage = inputImage.clone();
			getMask();
			updateImage();
		}
	}

	// ���û�����mask
	void getMask() {
		std::string windowTitle = "Draw the removal part. Esc to confirm.";
		cv::Mat backImage;
		cv::namedWindow(windowTitle, cv::WINDOW_AUTOSIZE);
		cv::setMouseCallback(windowTitle, onMouse, (void *)&maskImage);

		while (1) {
			if (isDrawing) { cv::circle(maskImage, currentPoint, 0, cv::Scalar(0, 0, 255), 16); }
			cv::addWeighted(inputImage, 0.5, maskImage, 0.5, 0, backImage);
			cv::imshow(windowTitle, backImage);
			char key = cv::waitKey(1);
			if (key == 27) break;
		}
	}

	void updateImage() {
		cv::Mat grayMask;
		// std::vector<std::vector<int>> seam_buffer;
		uint seamCount = 0;

		if (isVertical) {
			outputImage = RotateImage(outputImage);
			maskImage = RotateImage(maskImage);
		}
		cv::cvtColor(maskImage, grayMask, cv::COLOR_BGR2GRAY);
		while (cv::countNonZero(grayMask) > 0) {
			cv::Mat energyMap = getEnergyMap(outputImage, maskImage);
			std::vector<int> seam = getSeam(energyMap);
			// seam_buffer.push_back(seam);

			// ʵʱ��ʾ�ҵ���seam
			cv::Mat carvedImage = outputImage.clone();
			for (int i = 0; i < outputImage.rows; ++i) {
				for (int j = 0; j < 3; ++j) {
					carvedImage.at<cv::Vec3b>(i, seam[i])[j] = 233;
				}
			}
			cv::addWeighted(carvedImage, 0.5, maskImage, 0.5, 0, carvedImage);
			if (isVertical) carvedImage = RotateBack(carvedImage);
			cv::imshow("Processing", carvedImage);
			cv::waitKey(1);

			outputImage = RemoveSeam(outputImage, seam);
			maskImage = RemoveSeam(maskImage, seam);

			cv::cvtColor(maskImage, grayMask, cv::COLOR_BGR2GRAY);
			seamCount++;
		}
		if (resizeToOrigin) {
		cv::Mat zeroImage = cv::Mat(outputImage.size(), CV_8UC3);
		zeroImage = cv::Scalar(0, 0, 0);
			for (uint i = seamCount; i >= 1; --i) {
				// ʵʱ��ʾ�ҵ���seam
				cv::Mat energyMap = getEnergyMap(outputImage, zeroImage);
				std::vector<int> seam = getSeam(energyMap);
				cv::Mat carvedImage = outputImage.clone();
				for (int j = 0; j < outputImage.rows; ++j) {
					for (int k = 0; k < 3; ++k) {
						carvedImage.at<cv::Vec3b>(j, seam[j])[k] = 233;
					}
				}
				outputImage = AddSeam(outputImage, seam);
				zeroImage = AddSeam(zeroImage, seam);

				/*
					for (int j = 0; j < outputImage.rows; ++j) {
						for (int k = 0; k < 3; ++k) {
							carvedImage.at<cv::Vec3b>(j, seam_buffer[i-1][j])[k] = 233;
						}
					}
				*/
				
				if (isVertical) carvedImage = RotateBack(carvedImage);
				cv::imshow("Processing", carvedImage);
				cv::waitKey(1);
			}
		}
		if (isVertical) {
			outputImage = RotateBack(outputImage);
			maskImage = RotateBack(maskImage);
		}

		// ���
		cv::imshow("Output", outputImage);
		save_image("Output.png", outputImage);
		cv::waitKey(0);
	}

	// ����ͼ�������
	cv::Mat getEnergyMap(cv::Mat img, cv::Mat mask) {
		const int rate = -100;

		// �ȼ���Ҷ�ͼ
		cv::Mat gray_img, gray_mask, energy_map;
		cv::cvtColor(img, gray_img, cv::COLOR_BGR2GRAY);
		cv::cvtColor(mask, gray_mask, cv::COLOR_BGR2GRAY);
		// Sobel
		cv::Mat sobelX, sobelY;
		cv::Sobel(gray_img, sobelX, CV_16S, 1, 0);
		cv::Sobel(gray_img, sobelY, CV_16S, 0, 1);

		cv::convertScaleAbs(sobelX, sobelX);
		cv::convertScaleAbs(sobelY, sobelY);

		cv::addWeighted(sobelX, 1, sobelY, 1, 0, energy_map, CV_16S);

		// ��mask���������������ȥһ������
		cv::threshold(gray_mask, gray_mask, 1, 255, cv::THRESH_BINARY);
		cv::addWeighted(gray_mask, rate, energy_map, 1, 0, energy_map, CV_16S);
		return energy_map;
	}

	// �ҵ��������ٵ�һ��Seam
	std::vector<int> getSeam(const cv::Mat energy_map) {
		std::vector<int> seam;

		const int rows = energy_map.rows;
		const int cols = energy_map.cols;
		// Visual Studio do not support this haha
		// int map_data[rows][cols];
		int **map_data = new int*[rows];
		for (int i = 0; i < rows; i++) map_data[i] = new int[cols];

		int temp = 1919810;
		int temp_index = -1;
		seam.resize(rows, 0);

		for (int i = 0; i < cols; ++i) {
			map_data[0][i] = (int)energy_map.at<short>(0, i);
		}

		for (int i = 1; i < rows; ++i) {
			for (int j = 0; j < cols; ++j) {
				// �߽���
				if (j == 0)
					map_data[i][j] = std::min(map_data[i - 1][j + 1], map_data[i - 1][j]);
				else if (j == cols - 1)
					map_data[i][j] = std::min(map_data[i - 1][j - 1], map_data[i - 1][j]);
				// �Ǳ߽����
				else
					map_data[i][j] = std::min(map_data[i - 1][j - 1], std::min(map_data[i - 1][j], map_data[i - 1][j + 1]));
				map_data[i][j] += energy_map.at<short>(i, j);
			}
		}

		for (int i = 0; i < cols; ++i) {
			// Ѱ��������͵����
			if (map_data[rows - 1][i] < temp) {
				temp = map_data[rows - 1][i];
				temp_index = i;
			}
		}

		seam[rows - 1] = temp_index;
		for (int i = rows - 1; i > 0; --i) {
			auto energy = map_data[i][temp_index] - (int)energy_map.at<short>(i, temp_index);
			// �߽���
			if (temp_index == 0) {
				if (energy == map_data[i - 1][temp_index + 1])
					temp_index = temp_index + 1;
				else
					temp_index = temp_index;
			}
			else if (temp_index == cols - 1) {
				if (energy == map_data[i - 1][temp_index - 1])
					temp_index = temp_index - 1;
				else
					temp_index = temp_index;
			}
			// �Ǳ߽����
			else {
				if (energy == map_data[i - 1][temp_index - 1])
					temp_index = temp_index - 1;
				else if (energy == map_data[i - 1][temp_index + 1])
					temp_index = temp_index + 1;
				else
					temp_index = temp_index;
			}
			seam[i - 1] = temp_index;
		}

		return seam;
	}

	// ת90��
	cv::Mat RotateImage(const cv::Mat img) {
		cv::Mat rotated_img;
		cv::transpose(img, rotated_img);
		cv::flip(rotated_img, rotated_img, 1);
		return rotated_img;
	}

	// ת����
	cv::Mat RotateBack(const cv::Mat rotated_img) {
		cv::Mat img;
		cv::transpose(rotated_img, img);
		cv::flip(img, img, 0);
		return img;
	}

	// ɾ��Seam
	cv::Mat RemoveSeam(cv::Mat image, std::vector<int> seam) {
		cv::Mat new_img;
		new_img.create(image.rows, image.cols - 1, image.type());
		const int rows = image.rows;
		const int cols = image.cols;
		for (int i = 0; i < rows; ++i) {
			int temp = seam[i];
			// ����Ҫ�Ƴ���seam����ߵ����ر��ֲ���
			for (int j = 0; j < temp; ++j) {
				new_img.at<cv::Vec3b>(i, j) = image.at<cv::Vec3b>(i, j);
			}
			// �ұߵ�������������һ��
			for (int j = temp; j < cols - 1; ++j) {
				new_img.at<cv::Vec3b>(i, j) = image.at<cv::Vec3b>(i, j + 1);
			}
		}
		image = new_img.clone();
		return new_img;
	}

	// ����Seam
	cv::Mat AddSeam(cv::Mat image, std::vector<int> seam) {
		cv::Mat new_img;
		new_img.create(image.rows, image.cols + 1, image.type());
		const int rows = image.rows;
		const int cols = image.cols;
		for (int i = 0; i < rows; ++i) {
			int temp = seam[i];
			// ����Ҫ���ӵ�seam����ߵ����ر��ֲ���
			for (int j = 0; j < temp; ++j) {
				new_img.at<cv::Vec3b>(i, j) = image.at<cv::Vec3b>(i, j);
			}
			// �ұߵ�����������һ��
			for (int j = temp + 1; j < cols + 1; ++j) {
				new_img.at<cv::Vec3b>(i, j) = image.at<cv::Vec3b>(i, j - 1);
			}
			// 插入Seam
			// 边界检测
			if(temp == 0)
				new_img.at<cv::Vec3b>(i, seam[i]) = new_img.at<cv::Vec3b>(i, temp + 1);
			else if(temp == cols)
				new_img.at<cv::Vec3b>(i, seam[i]) = new_img.at<cv::Vec3b>(i, temp - 1);
			// 非边界，对出来的一行做平均值处理
			else 
				new_img.at<cv::Vec3b>(i, temp) = 
					new_img.at<cv::Vec3b>(i, temp - 1) * 0.5f + new_img.at<cv::Vec3b>(i, temp + 1) * 0.5f;
		}
		return new_img;
	}
};

bool ObjectRemoval::isDrawing = false;
cv::Point ObjectRemoval::currentPoint = cv::Point(0, 0);
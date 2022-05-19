#pragma once

#include "user_interface.h"

class SeamCarver {
    public:
        SeamCarver() {}
        SeamCarver(const char* filename) {
            load_image(filename, input_img);
            output_img = input_img.clone();
        }
        ~SeamCarver() {}

        // 更新图像
        void UpdateImg() {
            for(int i = 0; i < 100; ++i) {
                CalculateEnergyMap(output_img);
                FindSeam(energy_map, &seam);
                RemoveSeam(seam);

                cv::Mat test_image = output_img.clone();
                for(int j = 0; j < output_img.rows; ++j)
                {
                    test_image.at<cv::Vec3b>(j, seam[j])[0] = 0;
                    test_image.at<cv::Vec3b>(j, seam[j])[1] = 0;
                    test_image.at<cv::Vec3b>(j, seam[j])[2] = 233;
                }
                cv::imshow("test", test_image);
                cv::waitKey(1);
            }

            cv::imshow("output", output_img);
            cv::waitKey(0);
        }

        // 计算图像的能量
        void CalculateEnergyMap(const cv::Mat img) {
            // 先计算灰度图
            cv::Mat gray_img;
            cv::cvtColor(img, gray_img, cv::COLOR_BGR2GRAY);
            // Sobel
            cv::Mat sobelX, sobelY;
            cv::Sobel(gray_img, sobelX, CV_16S, 1, 0);
            cv::Sobel(gray_img, sobelY, CV_16S, 0, 1);

            cv::convertScaleAbs(sobelX, sobelX);
            cv::convertScaleAbs(sobelY, sobelY);

            cv::addWeighted(sobelX, 1, sobelY, 1, 0, energy_map);
        }

        // 初始化map
        void Initialize(const int rows, const int cols) {
            map_data.clear();
            map_path.clear();
            map_data.resize(rows, std::vector<float>(cols, 0));
            map_path.resize(rows, std::vector<int>(cols, 0));
        }

        // 找到能量最少的一条seam
        void FindSeam(const cv::Mat energy_map, std::vector<int> *seam) {
            assert(seam);

            const int rows = energy_map.rows;
            const int cols = energy_map.cols;
            float upleft, upright, up, min_energy;
            Initialize(rows, cols);
            seam->resize(rows, 0);
        }

        // 删除Seam
        void RemoveSeam(std::vector<int> seam) {
            cv::Mat new_img;
            new_img.create(output_img.rows, output_img.cols - 1, output_img.type());
            const int rows = output_img.rows;
            const int cols = output_img.cols;
            for(int i = 0; i < rows; ++i) {
                int temp = seam[i];
                for(int j = 0; j < temp; ++j) {
                    new_img.at<cv::Vec3b>(i, j)[0] = output_img.at<cv::Vec3b>(i, j)[0];
                    new_img.at<cv::Vec3b>(i, j)[1] = output_img.at<cv::Vec3b>(i, j)[1];
                    new_img.at<cv::Vec3b>(i, j)[2] = output_img.at<cv::Vec3b>(i, j)[2];
                }
                for(int j = temp; j < cols - 1; ++j) {
                    new_img.at<cv::Vec3b>(i, j)[0] = output_img.at<cv::Vec3b>(i, j + 1)[0];
                    new_img.at<cv::Vec3b>(i, j)[1] = output_img.at<cv::Vec3b>(i, j + 1)[1];
                    new_img.at<cv::Vec3b>(i, j)[2] = output_img.at<cv::Vec3b>(i, j + 1)[2];
                }
            }
            output_img = new_img.clone();
        }

        // 绘制界面
        void Draw() {
            std::vector<cv::Mat> images;
            images.push_back(input_img);
            images.push_back(energy_map);
            //show_image(images);
        }

    private:
        cv::Mat input_img;
        cv::Mat energy_map;
        cv::Mat output_img;

        std::vector<std::vector<float>> map_data;
        std::vector<std::vector<int>> map_path;
        std::vector<int> seam;
};

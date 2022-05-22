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

        // 实时更新图像
        void UpdateImg() {
            for(int i = 0; i < 100; ++i) {
                CalculateEnergyMap(output_img);
                FindSeam(energy_map);
                RemoveSeam(seam);

                // 实时显示找到的seam
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

            // 结果
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

        // 找到能量最少的一条seam
        void FindSeam(const cv::Mat energy_map) {
            const int rows = energy_map.rows;
            const int cols = energy_map.cols;
            int map_data[rows][cols];

            int temp = 1919810;
            int temp_index = -1;
            seam.resize(rows, 0);

            for(int i = 0; i < cols; ++i) {
                map_data[0][i] = (int)energy_map.at<uchar>(0, i);
            }

            for(int i = 1; i < rows; ++i) {
                for(int j = 0; j < cols; ++j) {
                    // 边界检测
                    if(j == 0)
                        map_data[i][j] = std::min(map_data[i-1][j+1], map_data[i-1][j]);
                    else if(j == cols-1)
                        map_data[i][j] = std::min(map_data[i-1][j-1], map_data[i-1][j]);
                    // 非边界情况
                    else
                        map_data[i][j] = std::min(map_data[i-1][j-1], std::min(map_data[i-1][j], map_data[i-1][j+1]));
                    map_data[i][j] += energy_map.at<uchar>(i, j);
                }
            }

            for(int i = 0; i < cols; ++i) {
                // 寻找能量最低的起点
                if(map_data[rows-1][i] < temp) {
                    temp = map_data[rows-1][i];
                    temp_index = i;
                }
            }

            cv::Point p(rows-1, temp_index);
            seam[p.x] = p.y;
            for(int i = p.x; i > 0; --i) {
                auto energy = map_data[i][p.y] - (int)energy_map.at<uchar>(i, p.y);
                // 边界检测
                if(p.y == 0) {
                    if(energy == map_data[i-1][p.y+1])
                        p = cv::Point(i-1, p.y+1);
                    else
                        p = cv::Point(i-1, p.y);
                }
                else if (p.y == cols - 1) {
                    if (energy == map_data[i - 1][p.y - 1])
                        p = cv::Point(i - 1, p.y - 1);
                    else
                        p = cv::Point(i - 1, p.y);
                }
                // 非边界情况
                else {
                    if (energy == map_data[i - 1][p.y - 1])
                        p = cv::Point(i - 1, p.y - 1);
                    else if (energy == map_data[i - 1][p.y + 1])
                        p = cv::Point(i - 1, p.y + 1);
                    else
                        p = cv::Point(i - 1, p.y);
                }
                seam[i] = p.y;
            }
        }

        // 删除Seam
        void RemoveSeam(std::vector<int> seam) {
            cv::Mat new_img;
            new_img.create(output_img.rows, output_img.cols - 1, output_img.type());
            const int rows = output_img.rows;
            const int cols = output_img.cols;
            for(int i = 0; i < rows; ++i) {
                int temp = seam[i];
                // 对于要移除的seam，左边的像素保持不动
                for(int j = 0; j < temp; ++j) {
                    new_img.at<cv::Vec3b>(i, j) = output_img.at<cv::Vec3b>(i, j);
                }
                // 右边的像素向左缩进一格
                for(int j = temp; j < cols - 1; ++j) {
                    new_img.at<cv::Vec3b>(i, j) = output_img.at<cv::Vec3b>(i, j + 1);
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

        std::vector<int> seam;
};

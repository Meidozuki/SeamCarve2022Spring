#pragma once

#include "cal_energy.h"
#include "user_interface.h"

enum Mod {
    VERTICAL_CARVE,
    HORIZONTAL_CARVE,
    VERTICAL_ENLARGE,
    HORIZONTAL_ENLARGE,
    CONTENT_ENHANCEMENT
};

class SeamCarver {
    private:
        cv::Mat input_img;
        cv::Mat energy_map;
        cv::Mat output_img;

        std::vector<int> seam;
        std::vector<std::vector<int>> seam_buffer;

    public:
        SeamCarver() {}
        SeamCarver(const char* filename) {
            load_image(filename, input_img);
            output_img = input_img.clone();
        }
        ~SeamCarver() {}

        // 实时更新图像
        void UpdateImg(int MOD, int size) {
            switch (MOD)
            {
                case Mod::VERTICAL_CARVE:
                    VerticalChangeAspectRatio(size);
                    break;

                case Mod::HORIZONTAL_CARVE:
                    HorizontalChangeAspectRatio(size);
                    break;

                case Mod::VERTICAL_ENLARGE:
                    VerticalImageEnlarging(size);
                    break;

                case Mod::HORIZONTAL_ENLARGE:
                    HorizontalImageEnlarging(size);
                    break;
                
                case Mod::CONTENT_ENHANCEMENT:
                    ContentEnhancement(size);
                    break;

                default:
                    std::cout << "Error: Mod not found!" << std::endl;
                    break;
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
            // cv::Sobel(gray_img, sobelX, CV_16S, 1, 0);
            // cv::Sobel(gray_img, sobelY, CV_16S, 0, 1);
            cal_gradient(gray_img, sobelX,  1, 0);
            cal_gradient(gray_img, sobelY,  0, 1);

            cv::convertScaleAbs(sobelX, sobelX);
            cv::convertScaleAbs(sobelY, sobelY);

            cv::addWeighted(sobelX, 1, sobelY, 1, 0, energy_map);
        }

        // 找到能量最少的一条Seam
        void FindSeam(const cv::Mat energy_map) {
            const int rows = energy_map.rows;
            const int cols = energy_map.cols;
            // int map_data[rows][cols];
            std::vector<std::vector<int>> map_data;
            map_data.resize(rows);
            for (int i = 0;i < rows;++i) {
                map_data[i].resize(cols);
            }

            int temp = 999999;
            int temp_index = -1;
            seam.resize(rows, 0);

            for(int i = 0; i < cols; ++i) {
                map_data[0][i] = (int)energy_map.at<uchar>(0, i);
            }

            for(int i = 1; i < rows; ++i) {
                for(int j = 0; j < cols; ++j) {
                    // 边界检测
                    if(j == 0)
                        map_data[i][j] = std::min(map_data[i - 1][j + 1], map_data[i - 1][j]);
                    else if(j == cols-1)
                        map_data[i][j] = std::min(map_data[i - 1][j - 1], map_data[i - 1][j]);
                    // 非边界情况
                    else
                        map_data[i][j] = std::min(map_data[i - 1][j - 1], std::min(map_data[i - 1][j], map_data[i - 1][j + 1]));
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

            seam[rows-1] = temp_index;
            for(int i = rows - 1; i > 0; --i) {
                auto energy = map_data[i][temp_index] - (int)energy_map.at<uchar>(i, temp_index);
                // 边界检测
                if(temp_index == 0) {
                    if(energy == map_data[i - 1][temp_index + 1])
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
                // 非边界情况
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
        }

        // 转90度
        cv::Mat RotateImage(const cv::Mat img) {
            cv::Mat rotated_img;
            cv::transpose(img, rotated_img);
            cv::flip(rotated_img, rotated_img, 1);
            return rotated_img;
        }

        // 转回来
        cv::Mat RotateBack(const cv::Mat rotated_img) {
            cv::Mat img;
            cv::transpose(rotated_img, img);
            cv::flip(img, img, 0);
            return img;
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

        // 插入Seam
        void AddSeam(std::vector<int> seam) {
            cv::Mat new_img;
            new_img.create(output_img.rows, output_img.cols + 1, output_img.type());
            const int rows = output_img.rows;
            const int cols = output_img.cols;
            for(int i = 0; i < rows; ++i) {
                int temp = seam[i];
                // 对于要移除的seam，左边的像素保持不动
                for(int j = 0; j < temp; ++j) {
                    new_img.at<cv::Vec3b>(i, j) = output_img.at<cv::Vec3b>(i, j);
                }
                // 右边的像素向右移一格
                for(int j = temp + 1; j < cols + 1; ++j) {
                    new_img.at<cv::Vec3b>(i, j) = output_img.at<cv::Vec3b>(i, j - 1);
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
            output_img = new_img.clone();
        }

        // 绘制界面
        void Draw() {
            std::vector<cv::Mat> images;
            images.push_back(input_img);
            images.push_back(energy_map);
            //show_image(images);
        }

        // Change Aspect Ratio
        void VerticalChangeAspectRatio(int size)
        {
            for(int i = 0; i < size; ++i) {
                // Vertical
                CalculateEnergyMap(output_img);
                FindSeam(energy_map);
                            
                // 实时显示找到的seam
                cv::Mat carved_image = output_img.clone();
                for(int j = 0; j < output_img.rows; ++j) {
                    for(int k = 0; k < 3; ++k) {
                        carved_image.at<cv::Vec3b>(j, seam[j])[k] = 233;
                    }
                }
                cv::imshow("Processing", carved_image);
                cv::waitKey(1);

                RemoveSeam(seam);
            }
        }

        void HorizontalChangeAspectRatio(int size)
        {
            output_img = RotateImage(output_img);
            for(int i = 0; i < size; ++i) {
                // Horizontal
                CalculateEnergyMap(output_img);
                FindSeam(energy_map);

                // 实时显示找到的seam
                cv::Mat carved_image = output_img.clone();
                for(int j = 0; j < output_img.rows; ++j) {
                    for(int k = 0; k < 3; ++k) {
                        carved_image.at<cv::Vec3b>(j, seam[j])[k] = 233;
                    }
                }
                carved_image = RotateBack(carved_image);
                cv::imshow("Processing", carved_image);
                cv::waitKey(1);

                RemoveSeam(seam);
            }   
            output_img = RotateBack(output_img);
        }

        // Image Enlarging
        void VerticalImageEnlarging(int size)
        {
            cv::Mat temp_img = output_img.clone();
            for(int i = 0; i < size; ++i) {
                CalculateEnergyMap(output_img);
                FindSeam(energy_map);
                seam_buffer.push_back(seam);
                RemoveSeam(seam);
            }
            output_img  = temp_img.clone();
            for(int i = 0; i < size; ++i) {
                // 实时显示找到的seam
                cv::Mat carved_image = output_img.clone();
                for(int j = 0; j < output_img.rows; ++j) {
                    for(int k = 0; k < 3; ++k) {
                        carved_image.at<cv::Vec3b>(j, seam_buffer[i][j])[k] = 233;
                    }
                }
                cv::imshow("Processing", carved_image);
                cv::waitKey(1);
                AddSeam(seam_buffer[i]);
            }
            seam_buffer.clear();
        }

        void HorizontalImageEnlarging(int size)
        {
            output_img = RotateImage(output_img);
            cv::Mat temp_img = output_img.clone();
            for(int i = 0; i < size; ++i) {
                CalculateEnergyMap(output_img);
                FindSeam(energy_map);
                seam_buffer.push_back(seam);
                RemoveSeam(seam);
            }
            output_img  = temp_img.clone();
            for(int i = 0; i < size; ++i) {
                // 实时显示找到的seam
                cv::Mat carved_image = output_img.clone();
                for(int j = 0; j < output_img.rows; ++j) {
                    for(int k = 0; k < 3; ++k) {
                        carved_image.at<cv::Vec3b>(j, seam_buffer[i][j])[k] = 233;
                    }
                }
                carved_image = RotateBack(carved_image);
                cv::imshow("Processing", carved_image);
                cv::waitKey(1);
                AddSeam(seam_buffer[i]);
            }
            output_img = RotateBack(output_img);
            seam_buffer.clear();
        }

        // Content Enhancement
        void ContentEnhancement(int size)
        {
            const int cols = output_img.cols + size;
            const int rows = output_img.rows + size;
            resize(output_img, output_img, cv::Size(cols, rows));

            VerticalChangeAspectRatio(size);
            HorizontalChangeAspectRatio(size);
        }

};

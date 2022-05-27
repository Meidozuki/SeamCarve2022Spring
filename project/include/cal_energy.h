//
// Created by Nikola on 2022/5/25.
//

#pragma once
#include <opencv2/opencv.hpp>
#include <cstdint>

typedef uint8_t dtype;

void cal_gradient(const cv::Mat &gray_img,cv::Mat &dst,bool dx,bool dy) {
    const int cols=gray_img.cols,rows=gray_img.rows;
    dst=gray_img.clone();

    int i,j;
    if (dx) {
        for (i=0;i < rows;++i) {
            for (j=1;j < cols;++j) {
                auto dif=gray_img.at<dtype>(i,j)-gray_img.at<dtype>(i,j-1);
                dst.at<dtype>(i,j)=dif;
            }
        }
    }
    else if (dy) {
        for (j=0;j < cols;++j) {
            for (i=1;i < rows;++i) {
                dst.at<dtype>(i,j)=gray_img.at<dtype>(i,j)-dst.at<dtype>(i-1,j);
            }
        }
    }
}

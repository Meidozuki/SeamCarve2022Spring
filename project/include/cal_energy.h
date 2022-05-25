//
// Created by Nikola on 2022/5/25.
//

#pragma once
#include <opencv2/core.hpp>

void cal_gradient(const cv::Mat &gray_img,cv::Mat &dst,bool dx,bool dy) {
//    const int cols=gray_img.cols,rows=gray_img.rows;
//    dst=cv::Mat(rows,cols,gray_img.type());
//
//    if (dx) {
//        for (int i=0;i < rows;++i) {
//            for (int j=1;j < cols;++j) {
//                dst.at<int>(i,j)=gray_img.at<int>(i,j)-gray_img.at<int>(i,j-1);
//            }
//        }
//    }
//    else if (dy) {
//        for (int j=0;j < cols;++j) {
//            for (int i=1;i < rows;++i) {
//                dst.at<int>(i,j)=gray_img.at<int>(i,j)-dst.at<int>(i-1,j);
//            }
//        }
//    }
}

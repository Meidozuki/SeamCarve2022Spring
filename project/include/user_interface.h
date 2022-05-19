# pragma once

#include "image_loader.h"
#include <vector>

// 绘制程序界面
void show_image(std::vector<cv::Mat> images)
{
	int nRows = 300;
	int nCols = 300;

	for (int i = 0; i < images.size(); i++)
	{
		resize(images[i], images[i], cv::Size(nCols, nRows));
	}

	// test
	cv::imshow("window", images[0]);
	cv::waitKey(0);
}
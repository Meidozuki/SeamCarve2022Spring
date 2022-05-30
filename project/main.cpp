

#include <iostream>
#include <cmath>
#include <memory>
#include <opencv2/core/utils/logger.hpp>
#include "seam_carver.h"
#include "object_removal.h"

#include "header_config.h"

// int main() {
// 	cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);

//     // std::string file_name = "", default_file = "test_image/HJoceanSmall.png";

// 	// std::cout << "Input the path of the image:";
// 	// std::getline(std::cin, file_name);

// 	// std::string file_path = "./";
// 	// if (file_name.length() > 0) {
// 	// 	file_path += file_name;
// 	// }
// 	// else {
// 	// 	file_path += default_file;
// 	// }

//     std::string src_dir(SRC_DIR);
//     std::string file_path = "/test_image/HJoceanSmall.png";
//     file_path = src_dir + file_path;

// 	/*
//     SeamCarver sc(file_path.c_str());
//     sc.UpdateImg(2, 200);
// 	*/
	
// 	// param 1: path of file
// 	// param 2: is vertical or not
// 	// param 3: whether to restore to the original size (still have bugs)
// 	ObjectRemoval OR(file_path.c_str(), false, false);
    
//     return 0;
// }

int main()
{
	std::cout << "The demo is used for presentation: " << std::endl;

	int mod;
	std::string src_dir(SRC_DIR);
	std::cout << "Please input the mod: " << std::endl;
	std::cin >> mod;

	switch(mod) {
		case 0: {
			std::cout << "Aspect Ratio Change: " << std::endl;
			std::string file_path = "/test_image/HJoceanSmall.png";
			file_path = src_dir + file_path;
			SeamCarver sc(file_path.c_str());
			sc.UpdateImg(Mod::VERTICAL_CARVE, 100);

			std::string save_path = "/test_image/Aspect Ratio Change.png";
			save_path = src_dir + save_path;
			save_image(save_path.c_str(), sc.output_img);
			break;
		}
		case 1: {
			std::cout << "Image Enlarging: " << std::endl;
			std::string file_path = "/test_image/HJoceanSmall.png";
			file_path = src_dir + file_path;
			SeamCarver sc(file_path.c_str());
			sc.UpdateImg(Mod::VERTICAL_ENLARGE, 100);

			std::string save_path = "/test_image/Image Enlarging.png";
			save_path = src_dir + save_path;
			save_image(save_path.c_str(), sc.output_img);
			break;
		}
		case 2: {
			std::cout << "Content Enhancement: " << std::endl;
			std::string file_path = "/test_image/test2.jpg";
			file_path = src_dir + file_path;
			SeamCarver sc(file_path.c_str());
			sc.UpdateImg(Mod::CONTENT_ENHANCEMENT, 500);
			
			std::string save_path = "/test_image/Content Enhancement.png";
			save_path = src_dir + save_path;
			save_image(save_path.c_str(), sc.output_img);
			break;
		}
		case 3: {
			std::cout << "Object Removal: " << std::endl;
			std::string file_path = "/test_image/shoes.jpg";
			file_path = src_dir + file_path;
			ObjectRemoval OR(file_path.c_str(), false, false);

			std::string save_path = "/test_image/Object Removal.png";
			save_path = src_dir + save_path;
			save_image(save_path.c_str(), OR.outputImage);
			break;
		}
		case 4: {
			std::cout << "Multi-size Images: " << std::endl;
			std::string file_path = "/test_image/HJoceanSmall.png";
			file_path = src_dir + file_path;
			SeamCarver sc(file_path.c_str());
			sc.UpdateImg(Mod::MULTI_SIZE_IMAGE, 100);
			break;
		}
		default: {
			std::cout << "Wrong input!" << std::endl;
			break;
		}
	}
}

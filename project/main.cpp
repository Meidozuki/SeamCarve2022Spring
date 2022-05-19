

#include <iostream>
#include <cmath>
#include <memory>
#include "seam_carver.h"

int main(void) {
    // 记得使用绝对路径
    std::string filename = "C:/Users/Hiyori/Documents/GitHub/SeamCarve2022Spring/project/test_image/test1.jpg";

    SeamCarver sc(filename.c_str());
    sc.UpdateImg();
    
    return 0;
}

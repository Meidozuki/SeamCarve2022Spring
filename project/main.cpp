

#include <iostream>
#include <cmath>
#include <memory>
#include "seam_carver.h"

#define MOD 0

int main(void) {
    // 记得使用绝对路径
    std::string filename = "C:/Users/Hiyori/Documents/GitHub/SeamCarve2022Spring/project/test_image/HJoceanSmall.png";

    SeamCarver sc(filename.c_str());
    sc.UpdateImg(MOD, 200);
    
    return 0;
}

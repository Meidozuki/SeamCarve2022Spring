

#include <iostream>
#include <cmath>
#include <memory>
#include "seam_carver.h"

#include "header_config.h"

#define MOD 2

int main(void) {
    // 记得使用绝对路径
    std::string src_dir(SRC_DIR);
    std::string filename = "/test_image/HJoceanSmall.png";
    filename=src_dir + filename;

    SeamCarver sc(filename.c_str());
    sc.UpdateImg(MOD, 200);
    
    return 0;
}

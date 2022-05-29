

#include <iostream>
#include <cmath>
#include <memory>
#include "seam_carver.h"

#include "header_config.h"

#define SEAM 200

int main(void) {

    std::string src_dir(SRC_DIR);
    std::string filename = "/test_image/HJoceanSmall.png";
    filename = src_dir + filename;

    SeamCarver sc(filename.c_str());
    sc.UpdateImg(Mod::HORIZONTAL_ENLARGE, SEAM);
    
    return 0;
}

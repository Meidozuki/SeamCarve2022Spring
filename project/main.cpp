

#include <iostream>
#include <cmath>
#include <memory>
#include "seam_carver.h"

#include "header_config.h"

#define MOD 3
#define SEAM 100

int main(void) {

    std::string src_dir(SRC_DIR);
    std::string filename = "/test_image/test3.jpg";
    filename=src_dir + filename;

    SeamCarver sc(filename.c_str());
    sc.UpdateImg(MOD, SEAM);
    
    return 0;
}

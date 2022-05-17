

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <cmath>
#include <memory>
using namespace std;


int main(void) {
//    cout << "Hello world! Project" << spa << PROJECT_NAME_s << spa << PROJECT_VERSION << endl;
//    cout << "cpp version " << __cplusplus << endl;

//    string keys="{@binary_thresh | 100 | }"
//                "{@binary_naxval | 192 | }";
//    CLParser parser(argc,argv,keys);

//    double x=INFINITY;
//    cout << x << endl;

    cout << "before read\n";
    string fname="D:/coding/OCSR/et.png";
    cv::Mat img=cv::imread(fname);
    cout << "after read\n";

    if (img.empty()) {
        cout << "ERROR: could not read the img";
        return 1;
    }
    else {
        cout << "img read.\n";
    }
    cv::imshow("window",img);
    cv::waitKey();


    return 0;
}

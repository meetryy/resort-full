 #ifndef H_PREPROC
 # define H_PREPROC

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <string>

class preprocessor_t{
public:

    int  marginLeft = 200;
    int  marginRight = 200;
    int  marginUp = 50;
    int  marginDown = 100;

    double  brightness = 1.1;
    double  contrast = 1.35;
    double  saturation = 3.5;
    int     sharpTimes = 1;


    cv::Mat     HUD;
    cv::Mat     out;
    cv::Size    matSize;
    cv::Mat Result(cv::Mat &input);
    cv::Rect    cropRect;
};

extern preprocessor_t preprocessor;

 #endif



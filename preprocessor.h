 #ifndef H_PREPROC
 # define H_PREPROC

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <string>

class preprocessor_t{
public:
    bool isOn = 0;
    int  marginLeft = 200;
    int  marginRight = 200;
    int  marginUp = 50;
    int  marginDown = 100;

    float  brightness = 1.1;
    float  contrast = 1.35;
    float  saturation = 3.5;
    int    sharpTimes = 1;
    int     rotation = -1;

    cv::Mat     HUD;
    cv::Mat     out;
    cv::Size    matSize;
    cv::Mat Result(cv::Mat &input);
    cv::Rect    cropRect;
};

extern preprocessor_t preprocessor;

 #endif



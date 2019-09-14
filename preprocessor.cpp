#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

#include "preprocessor.h"
#include "video_player.h"

using namespace cv;
using namespace std;

cv::Mat preprocessor_t::Result(cv::Mat &input){

    Mat in = input.clone();
    HUD = input.clone();
    //out;

    cropRect = Rect((int)marginLeft, (int)marginUp, (int)videoPlayer.frameW-marginRight-marginLeft, (int)videoPlayer.frameH-marginDown-marginUp);

    // divide and create mats
    out = input(cropRect).clone();
    matSize = Size(cropRect.width, cropRect.height);

    // adjust brightness and contrast
    out.convertTo(out, -1, contrast, brightness);
    Mat matHSV;
    cvtColor(out, matHSV, COLOR_BGR2HSV);

    // adjust saturation
    vector<Mat> spl;
    split(matHSV,spl);
    spl[1] *= saturation;
    cv::merge(spl, matHSV);
    cvtColor(matHSV, out, COLOR_HSV2BGR);

    // increase sharpness
    Mat testBlurMat = out.clone();
    for (int i=0; i<sharpTimes; i++){
        GaussianBlur(out, testBlurMat, cv::Size(0,0), 3);
        cv::addWeighted(out, 1.5, testBlurMat, -0.5, 0, out);
    }


    Mat dst_roi = HUD(Rect(marginLeft, marginUp, out.cols, out.rows));
    out.copyTo(dst_roi);
    rectangle(HUD, cropRect, Scalar(0.0, 0.0, 255.0), 2, LINE_8, 0);

    return out;


}

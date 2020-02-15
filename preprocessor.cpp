#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

#include "preprocessor.h"
#include "video_player.h"

using namespace cv;
using namespace std;

cv::Mat preprocessor_t::Result(cv::Mat &input){
    if (isOn){
        Mat in = input.clone();
        if (in.empty()) std::cout << "in.empty()" << std::endl;

        frameW = in.cols;
        frameH = in.rows;
        //out;

        //0 <= roi.x && 0 <= roi.width &&
        //roi.x + roi.width <= m.cols &&
        //0 <= roi.y && 0 <= roi.height &&
        //roi.y + roi.height <= m.rows

        int fixedML = marginLeft;
        int fixedMU = marginUp;

        cropRect = Rect(fixedML, fixedMU, in.cols-marginRight-marginLeft, in.rows-marginDown-marginUp);


        // divide and create mats
        out = in(cropRect).clone();
        matSize = Size(cropRect.width, cropRect.height);
        //std::cout << "cropRect = " << cropRect << std::endl;
        //std::cout << "matSize = " << matSize << std::endl;
        //if (out.empty()) std::cout << "out.empty()" << std::endl;

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
            medianBlur(out, testBlurMat, 3);
            cv::addWeighted(out, 1.5, testBlurMat, -0.5, 0, out);
        }

        Mat intHUD = in.clone();
        Mat dst_roi = intHUD(Rect(fixedML, fixedMU, out.cols, out.rows));
        out.copyTo(dst_roi);
        rectangle(intHUD, cropRect, Scalar(0.0, 0.0, 255.0), 2, LINE_8, 0);

        cv::Scalar letterColor = Scalar(255.0, 0.0, 0.0);
        cv::putText(intHUD, "L", Point(cropRect.x-20, cropRect.y+cropRect.height/2+10), FONT_HERSHEY_SIMPLEX, 0.5, letterColor, 2, LINE_8, 0);
        cv::putText(intHUD, "R", Point(cropRect.x+cropRect.width+10, cropRect.y+cropRect.height/2+15), FONT_HERSHEY_SIMPLEX, 0.5, letterColor, 2, LINE_8, 0);

        cv::putText(intHUD, "U", Point(cropRect.x+cropRect.width/2, cropRect.y-10), FONT_HERSHEY_SIMPLEX, 0.5, letterColor, 2, LINE_8, 0);
        cv::putText(intHUD, "D", Point(cropRect.x+cropRect.width/2, cropRect.y+cropRect.height+20), FONT_HERSHEY_SIMPLEX, 0.5, letterColor, 2, LINE_8, 0);

        if (rotation >= 0) cv::rotate(intHUD, intHUD, rotation);
        if (rotation >= 0) cv::rotate(out, out, rotation);

        HUD = intHUD.clone();

    }

    else out = input.clone();

    return out;
}



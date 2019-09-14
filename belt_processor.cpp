#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <omp.h>

#include "belt_processor.h"
#include "video_player.h"
#include "preprocessor.h"

using namespace cv;
using namespace std;

void imageProcessor_t::shiftMat(Mat &in, Mat &out, int pixels, bool dirLeft){
    if (dirLeft) in(cv::Rect(pixels, 0, in.cols - pixels, in.rows)).copyTo(out(cv::Rect(0,0,in.cols-pixels,in.rows)));
    else in(cv::Rect(0, 0, in.cols - pixels, in.rows)).copyTo(out(cv::Rect(pixels,0,in.cols-pixels,in.rows)));

}

void imageProcessor_t::Init(void){
    matAlphaMask =          cv::Mat(preprocessor.out.rows, preprocessor.out.cols, CV_8UC4);
    matAlphaMaskShifted =   cv::Mat(preprocessor.out.rows, preprocessor.out.cols, CV_8UC4);
    matAlphaMaskAccum =     cv::Mat(preprocessor.out.rows, preprocessor.out.cols, CV_8UC4);

    matAlphaMask = Scalar{0.0, 0.0, 0.0, 0.0};
    matAlphaMaskShifted = Scalar{0.0, 0.0, 0.0, 0.0};
    matAlphaMaskAccum = Scalar{0.0, 0.0, 0.0, 0.0};
}


void imageProcessor_t::DrawHUD(Mat &mat){

    //int pixPerZone = mat.rows/Ejector.numZones;
    for (int zone=0; zone<Ejector.numZones; zone++){
        float fraction = 255.0 / (float)Ejector.numZones * (float)zone;
        line(mat,   Point(0, Ejector.posMm[zone]*pixPerMm),
                    Point(mat.cols, Ejector.posMm[zone]*pixPerMm),
                    colorsByName[zone],1,LINE_8,(int)0);
        line(mat,   Point(0,        Ejector.posMm[zone]*pixPerMm+Ejector.widthMm[zone]*pixPerMm/2.0),
                    Point(mat.cols, Ejector.posMm[zone]*pixPerMm+Ejector.widthMm[zone]*pixPerMm/2.0),
                    colorsByName[zone],2,LINE_8,(int)0);
        line(mat,   Point(0,        Ejector.posMm[zone]*pixPerMm-Ejector.widthMm[zone]*pixPerMm/2.0),
                    Point(mat.cols, Ejector.posMm[zone]*pixPerMm-Ejector.widthMm[zone]*pixPerMm/2.0),
                    colorsByName[zone],2,LINE_8,(int)0);
    }
}

void imageProcessor_t::ProcessTest(cv::Mat &input,  cv::Mat &output){
    output = input.clone();
}

//void imageProcessor_t::Process(cv::Mat &input,  cv::Mat &output){/
cv::Mat imageProcessor_t::Result(cv::Mat Input){
    if (!initDone){
        matAlphaMask =          cv::Mat(Input.rows, Input.cols, CV_8UC4);
        matAlphaMaskShifted =   cv::Mat(Input.rows, Input.cols, CV_8UC4);
        matAlphaMaskAccum =     cv::Mat(Input.rows, Input.cols, CV_8UC4);
        initDone = 1;
    }

    if (initDone){
        Mat output;
        Mat input;
        //============================= color particles detection =====================================

        input = Input.clone();
        input.copyTo(mainMatHUD);
        cvtColor(input, mainMatHSV, COLOR_BGR2HSV);
        blur(mainMatHSV, mainMatHSVblur, Size(blurSize,blurSize));
        inRange(mainMatHSVblur, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S, high_V), mainMatHSVRanged);
        cv::Mat element = cv::getStructuringElement(morphShape, cv::Size(morphArea + 1, morphArea+1 ), cv::Point( morphArea, morphArea ));
        cv::morphologyEx(mainMatHSVRanged, mainMatHSVmorphed, morphType, element);
        //====================================================================================================================================
        cv::findContours(mainMatHSVmorphed, contoursHSV, hierarchyHSV, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        int  pixShiftPerFrame = mmPerFrame * pixPerMm; // pix per frame
        matAlphaMask = cv::Mat(Input.rows, Input.cols, CV_8UC4);//Scalar{0.0, 0.0, 0.0, 0.0};
        //#pragma omp parallel
        for(unsigned long i = 0; i < contoursHSV.size(); i++ ){
            if ((contourArea(contoursHSV[i]) > minArea)&&(contourArea(contoursHSV[i]) < maxArea)){
                Rect r = boundingRect(contoursHSV[i]);
                //std::cout << "r = " << r  << std::endl;
                rectangle(matAlphaMask, r, Scalar(255.0, 255.0, 255.0, 0.1), FILLED, LINE_8, 0);
                //rectangle(mainMatHUD, Rect(boundingRect(contoursHSV[i]).tl(), boundingRect(contoursHSV[i]).br()), Scalar(0,0,255.0), 2, LINE_8, 0);
                //drawContours(matAlphaMask, contoursHSV, i, Scalar(255.0, 255.0, 255.0, 0.1), FILLED, LINE_8, hierarchyHSV);
            }
        }

        matAlphaMaskShifted = cv::Mat(Input.rows, Input.cols, CV_8UC4);//Scalar{0.0, 0.0, 0.0, 0.0};
        shiftMat(matAlphaMask, matAlphaMaskShifted, pixShiftPerFrame, 0);

        //matAlphaMaskAccum = cv::Mat(Input.rows, Input.cols, CV_8UC4);
        cv::addWeighted(matAlphaMaskShifted, saturationWeight, matAlphaMaskAccum, 1.0-saturationWeight, 0, matAlphaMaskAccum);
        shiftMat(matAlphaMaskAccum, matAlphaMaskAccum, (int)pixShiftPerFrame, 0);


        cvtColor(matAlphaMaskAccum, matSatRendered,COLOR_BGRA2GRAY);
        inRange(matSatRendered, Scalar(satRangeMin), Scalar(satRangeMax), matSatRanged);
        vector<vector<Point> >  contoursSat;
        vector<Vec4i>           hierarchySat;
        cv::findContours(matSatRanged, contoursSat, hierarchySat, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        //#pragma omp parallel
        for(unsigned long i = 0; i < contoursSat.size(); i++ ){
            //rectangle(mainMatHUD, Rect(boundingRect(contoursSat[i]).tl(), boundingRect(contoursSat[i]).br()), Scalar(0,0,255.0), 2, LINE_8, 0);
            drawContours(mainMatHUD, contoursSat, i, Scalar(0,0,255.0), 2, LINE_8, hierarchySat);
        }
        //putText(mainMatHSVmorphed, "HSV", Point(5, 20), FONT_HERSHEY_PLAIN, 1.5, Scalar(255), 2);


        //DrawHUD(mainMatHUD);
        mainMatHUD.copyTo(output);

        // =============================================================================================================================================

        //ejectorDecZoneW = 1;
        //ejectorDecZoneX = 20;
        //Rect decZoneRect = Rect(ejectorDecZoneX, 0, ejectorDecZoneW, matSatRanged.rows);
        //ejectorDecZoneMask = matSatRanged(decZoneRect).clone();

        //rotate(output, output, ROTATE_180);
        return output;
    }
}


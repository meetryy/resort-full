#include <Windows.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <thread>

#include "belt_processor.h"
#include "video_player.h"
#include "preprocessor.h"
#include "COM_port.h"
#include "hardware.h"

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
    ejHUD =                 cv::Mat(preprocessor.out.rows, 500, CV_8UC3);

    matAlphaMask = Scalar{0.0, 0.0, 0.0, 0.0};
    matAlphaMaskShifted = Scalar{0.0, 0.0, 0.0, 0.0};
    matAlphaMaskAccum = Scalar{0.0, 0.0, 0.0, 0.0};
    ejHUD = Scalar{0.0, 0.0, 0.0, 0.0};
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


cv::Mat imageProcessor_t::Result(cv::Mat &Input){
    if (!initDone){
        matAlphaMask =          cv::Mat(Input.rows, Input.cols, CV_8UC4);
        matAlphaMaskShifted =   cv::Mat(Input.rows, Input.cols, CV_8UC4);
        matAlphaMaskAccum =     cv::Mat(Input.rows, Input.cols, CV_8UC4);
        ejHUD =                 cv::Mat(Input.rows, 500, CV_8UC3);

        startTimeThread();
        initDone = 1;
    }

    if (initDone){
        Mat output;
        Mat input;
        input = Input.clone();

        //============================= particles color detection =====================================
        input.copyTo(mainMatHUD);

        cvtColor(input, mainMatHSV, COLOR_BGR2HSV);
        blur(mainMatHSV, mainMatHSVblur, Size(blurSize,blurSize));
        inRange(mainMatHSVblur, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S, high_V), mainMatHSVRanged);

        cv::Mat element = cv::getStructuringElement(morphShape, cv::Size(morphArea + 1, morphArea+1 ), cv::Point( morphArea, morphArea ));
        cv::morphologyEx(mainMatHSVRanged, mainMatHSVmorphed, morphType, element);

        //====================================================================================================================================
        cv::findContours(mainMatHSVmorphed, contoursHSV, hierarchyHSV, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        int  pixShiftPerFrame = mmPerFrame * pixPerMm; // pix per frame
        matAlphaMask = Scalar{0.0, 0.0, 0.0, 0.0};

        //#pragma omp parallel
        {
        //#pragma omp for
            for(unsigned long i = 0; i < contoursHSV.size(); i++ ){
                if ((contourArea(contoursHSV[i]) > minArea)&&(contourArea(contoursHSV[i]) < maxArea)){
                    Rect r = boundingRect(contoursHSV[i]);
                    rectangle(matAlphaMask, r, Scalar(255.0, 255.0, 255.0, 0.1), FILLED, LINE_8, 0);
                    //rectangle(mainMatHUD, Rect(boundingRect(contoursHSV[i]).tl(), boundingRect(contoursHSV[i]).br()), Scalar(0,0,255.0), 2, LINE_8, 0);
                    //drawContours(matAlphaMask, contoursHSV, i, Scalar(255.0, 255.0, 255.0, 0.1), FILLED, LINE_8, hierarchyHSV);
                }
            }
        }

        shiftMat(matAlphaMask, matAlphaMaskShifted, pixShiftPerFrame, 0);
        cv::addWeighted(matAlphaMaskShifted, saturationWeight, matAlphaMaskAccum, 1.0-saturationWeight, 0, matAlphaMaskAccum);
        shiftMat(matAlphaMaskAccum, matAlphaMaskAccum, (int)pixShiftPerFrame, 0);

        cvtColor(matAlphaMaskAccum, matSatRendered,COLOR_BGRA2GRAY);
        inRange(matSatRendered, Scalar(satRangeMin), Scalar(satRangeMax), matSatRanged);

        vector<vector<Point> >  contoursSat;
        vector<Vec4i>           hierarchySat;
        cv::findContours(matSatRanged, contoursSat, hierarchySat, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        //#pragma omp parallel
            {
            //#pragma omp for
            for(unsigned long i = 0; i < contoursSat.size(); i++ ){
                //rectangle(mainMatHUD, Rect(boundingRect(contoursSat[i]).tl(), boundingRect(contoursSat[i]).br()), Scalar(0,0,255.0), 2, LINE_8, 0);
                drawContours(mainMatHUD, contoursSat, i, Scalar(0,0,255.0), 2, LINE_8, hierarchySat);
            }
        }
        mainMatHUD.copyTo(output);

        // ====================================================

        Rect decZoneRect = Rect(ejectorDecZoneX, 0, ejectorDecZoneW, matSatRanged.rows);
        ejectorDecZoneMask = matSatRanged(decZoneRect).clone();

        for (int zone = 0; zone<Ejector.numZones; zone++){
            float imgFractionY = ejectorDecZoneMask.rows / (float)Ejector.numZones * (float)zone;

            Rect meanRect = Rect(Point(0, imgFractionY), Point(1, imgFractionY + ejectorDecZoneMask.rows / (float)Ejector.numZones));
            Scalar average = mean(ejectorDecZoneMask(meanRect));
            Ejector.Zone[zone].avgLoad = average[0] / 255.0;
            Ejector.Zone[zone].ejCrit = (average[0] > (255.0 * Ejector.loadToEject));

            if (Ejector.Zone[zone].ejCrit != Ejector.Zone[zone].oldEjCrit){
                //Ejector.Zone[zone].timeUntil =
                float decZoneToEjectorTimeMs =
                    (((input.cols - ejectorDecZoneX) * mmPerPix) // mm from dec zone to edge
                    + Ejector.Zone[zone].mmFromCamEdge) // mm from edge to ej
                    / (beltSpeedMm / 1000.0);

                    auto timeNow = chrono::high_resolution_clock::now();
                    auto timeDiff = chrono::microseconds((int)(1000.0 * decZoneToEjectorTimeMs));
                    Ejector.Zone[zone].timeTarget = timeNow + timeDiff;

                    struct ejTask_t newTask;
                    newTask.done = 0;
                    newTask.newState = Ejector.Zone[zone].ejCrit;
                    newTask.time = Ejector.Zone[zone].timeTarget;
                    Ejector.Zone[zone].tasks.push_back(newTask);
                    //std::cout << " pushing back @ zone " << zone << " = " << Ejector.Zone[zone].ejCrit << std::endl;

                    //if (Ejector.Zone[zone].ejCrit == 0) Ejector.Zone[zone].listOFF.push_back(Ejector.Zone[zone].timeTarget);
                    //else Ejector.Zone[zone].listON.push_back(Ejector.Zone[zone].timeTarget);

                Ejector.Zone[zone].taskDone = 0;
                Ejector.Zone[zone].oldEjCrit = Ejector.Zone[zone].ejCrit;
            }
        }

        if (drawHUD){
            // =========== hud ===========
            ejHUD = Scalar(0,0,0);
            int HudPartW  = 100;
            int squareWidth = 30;
            int decMaskW = 30;
            cv::resize(ejectorDecZoneMask, ejectorDecZoneMask, Size(), decMaskW, 1, INTER_LINEAR);
            int zoneH = ejHUD.rows / (float)Ejector.numZones;

            Mat ejInColor = cv::Mat(ejectorDecZoneMask.rows, ejectorDecZoneMask.cols, CV_8UC3);
            cvtColor(ejectorDecZoneMask, ejInColor, COLOR_GRAY2BGR);

            input(Rect(ejectorDecZoneX - HudPartW, 0, HudPartW, input.rows)).copyTo(ejHUD(Rect(0, 0, HudPartW, input.rows)));
            ejInColor(Rect(0, 0, ejInColor.cols, ejInColor.rows)).copyTo(ejHUD(Rect(HudPartW, 0, ejInColor.cols, ejInColor.rows)));

            for (int zone = 0; zone<Ejector.numZones; zone++){
                int zoneY = zoneH * zone;
                Rect zoneRect = Rect(HudPartW + decMaskW, zoneY, squareWidth, zoneY + zoneH);
                float load = (Ejector.Zone[zone].avgLoad*255.0);
                rectangle(ejHUD, zoneRect, Scalar(load, load, load), FILLED, LINE_4, 0);

                zoneRect = Rect(HudPartW + decMaskW +  squareWidth, zoneY, squareWidth, zoneY + zoneH);
                load = (Ejector.Zone[zone].ejCrit*255.0);
                rectangle(ejHUD, zoneRect, Scalar(load, load, load), FILLED, LINE_4, 0);

                zoneRect = Rect(HudPartW + decMaskW +  squareWidth*2, zoneY, squareWidth, zoneY + zoneH);
                load = (Ejector.Zone[zone].State*255.0);
                rectangle(ejHUD, zoneRect, Scalar(load, load, load), FILLED, LINE_4, 0);

                line(ejHUD,    Point(0, zoneY),
                                            Point(ejHUD.cols, zoneY),
                                            Scalar(0,0,255) , 1, LINE_8,(int)0);


                if (!Ejector.Zone[zone].tasks.empty()){
                    char text[16] = {0};
                    sprintf (text, "%.2f ms", Ejector.Zone[zone].tasks[Ejector.Zone[zone].tasks.size()].timeLeftMs);
                    putText(ejHUD, text, Point(260, zoneY + ejHUD.rows/Ejector.numZones/2 + 5), FONT_HERSHEY_PLAIN, 1.0, Scalar(255,255,255), 1, 4, 0);
                }

                char buf[16] = {0};
                sprintf (buf, "%u", zone);
                putText(ejHUD, buf, Point(220, zoneY + ejHUD.rows/Ejector.numZones/2 + 5), FONT_HERSHEY_PLAIN, 1.0, Scalar(255,255,255), 2, 4, 0);

            }

            line(ejHUD, Point(HudPartW, 0), Point(HudPartW, ejHUD.rows), Scalar(0,0,255) , 1, LINE_8,(int)0);
            line(ejHUD, Point(HudPartW+decMaskW, 0), Point(HudPartW+decMaskW, ejHUD.rows), Scalar(0,0,255) , 1, LINE_8,(int)0);
            line(ejHUD, Point(HudPartW+decMaskW+squareWidth, 0), Point(HudPartW+decMaskW+squareWidth, ejHUD.rows), Scalar(0,0,255) , 1, LINE_8,(int)0);
            line(ejHUD, Point(HudPartW+decMaskW+squareWidth*2, 0), Point(HudPartW+decMaskW+squareWidth*2, ejHUD.rows), Scalar(0,0,255) , 1, LINE_8,(int)0);
            }

        return output;
    }
}

#include <chrono>
#include <thread>

using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds


void imageProcessor_t::ejTaskExc(void){
    std::cout << "ejTaskExc running..."<< std::endl;
    while (1){
        if (timerTheradRun){
            for (int zone = 0; zone<Ejector.numZones; zone++){
                if (Ejector.Zone[zone].tasks.size() > 0){
                    for (unsigned i = 0; i < Ejector.Zone[zone].tasks.size(); i++) {
                        if (!Ejector.Zone[zone].tasks[i].done){
                            auto timeNow = chrono::high_resolution_clock::now();
                            auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(Ejector.Zone[zone].tasks[i].time - timeNow);
                            float msToTask = (float)milliseconds.count();
                            Ejector.Zone[zone].tasks[i].timeLeftMs = msToTask;
                            //std::cout << i << " = " << msToTask << " ms" << std::endl;
                            if (msToTask < 0.2f) {
                                msToTask = 0;
                                // TODO: finish overlap!
                                if ((Ejector.Zone[zone].State == 1) && (Ejector.Zone[zone].tasks[i].newState == 1))
                                    Ejector.Zone[zone].overlap = 1;

                                Ejector.Zone[zone].State = Ejector.Zone[zone].tasks[i].newState;
                                hw.State.nozzles.state[zone] = Ejector.Zone[zone].State;
                                if (COM.connectionOk) COM.sendZoneState();
                                Ejector.Zone[zone].tasks[i].done = 1;
                            }
                        }
                        else {
                                //std::cout << i << " to be removed" << std::endl;
                                Ejector.Zone[zone].taskLock.lock();
                                Ejector.Zone[zone].tasks.erase(Ejector.Zone[zone].tasks.begin() + i);
                                Ejector.Zone[zone].taskLock.unlock();

                        }
                    }
                }
            }
        sleep_for(milliseconds(1));
    }
    else sleep_for(milliseconds(100));
    }
}



void imageProcessor_t::startTimeThread(void){
    timerThread = std::thread(&imageProcessor_t::ejTaskExc, this);
    timerTheradRun = 1;
    timerThread.detach();

}

void imageProcessor_t::stopTimeThread(void){
    timerTheradRun = 0;
}



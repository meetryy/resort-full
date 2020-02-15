#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#include <vector>
#include <iostream>

#include "variables1.h"
#include "img.h"
#include "calibration.h"
#include "gui_tools.h"
#include "newGUI.h"

Calibration_t Calib;

void Calibration_t::Init(void){

}

void serializeMatbin(cv::Mat& mat, std::string filename){
    if (!mat.isContinuous()) {
        std::cout << "Not implemented yet" << std::endl;
        exit(1);
    }

    int elemSizeInBytes = (int)mat.elemSize();
    int elemType        = (int)mat.type();
    int dataSize        = (int)(mat.cols * mat.rows * mat.elemSize());

    FILE* FP = fopen(filename.c_str(), "wb");
    int sizeImg[4] = {mat.cols, mat.rows, elemSizeInBytes, elemType };
    fwrite(/* buffer */ sizeImg, /* how many elements */ 4, /* size of each element */ sizeof(int), /* file */ FP);
    fwrite(mat.data, mat.cols * mat.rows, elemSizeInBytes, FP);
    fclose(FP);
}

cv::Mat deserializeMatbin(std::string filename){
    FILE* fp = fopen(filename.c_str(), "rb");
    int header[4];
    fread(header, sizeof(int), 4, fp);
    int cols            = header[0];
    int rows            = header[1];
    int elemSizeInBytes = header[2];
    int elemType        = header[3];

    cv::Mat outputMat = cv::Mat::ones(rows, cols, elemType);

    size_t result = fread(outputMat.data, elemSizeInBytes, (size_t)(cols * rows), fp);

    if (result != (size_t)(cols * rows)) {
        fputs ("Reading error", stderr);
    }

    fclose(fp);
    return outputMat;
}

float tempResX, tempResY;

void Calibration_t::processFrame(cv::Mat &in){
    imgSize = in.size();
    numSquares = calibData.numCornersHor * calibData.numCornersVer;
    cv::Size boardSize = cv::Size(calibData.numCornersHor, calibData.numCornersVer);

    matHUD = in.clone();
    std::vector<cv::Point3f> obj;
    //isSuccess = 0;
    cv::Mat matGray;

    for(int j = 0;j < numSquares;j++)
        obj.push_back(cv::Point3f(j/calibData.numCornersHor, j%calibData.numCornersHor, 0.0f));
        if(successes < numBoards){
            //while(!isSuccess){
                cv::cvtColor(in, matGray, cv::COLOR_BGR2GRAY);
                chessFound = cv::findChessboardCorners(matHUD,
                                                       boardSize,
                                                       calibData.corners,
                                                       cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_FILTER_QUADS | cv::CALIB_CB_NORMALIZE_IMAGE);

                if (chessFound) {
                    cornerSubPix(matGray,
                                 calibData.corners,
                                 cv::Size(11, 11),
                                 cv::Size(-1, -1),
                                 cv::TermCriteria(cv::TermCriteria::EPS | cv::TermCriteria::MAX_ITER, 30, 0.1));
                    drawChessboardCorners(matHUD, boardSize, calibData.corners, chessFound);
                }

                if (!calibData.corners.empty()){

                        cv::Point pt1 = calibData.corners[0];
                        cv::Point ptX2 = calibData.corners[calibData.numCornersHor - 1];
                        cv::Point ptY2 = calibData.corners[calibData.numCornersHor*(calibData.numCornersVer-1)];

                        cv::line(matHUD, calibData.corners[0], ptX2, cv::Scalar(255,0,0), 4, cv::LINE_8, 0);
                        cv::line(matHUD, calibData.corners[0], ptY2, cv::Scalar(255,0,0), 4, cv::LINE_8, 0);

                        double diffY = cv::norm(ptY2 - pt1);
                        double diffX = cv::norm(pt1 - ptX2);
                        double diffHypo = cv::norm(ptY2 - ptX2);

                        tempResX =  (calibData.boardXmm) / diffX;
                        tempResY =  (calibData.boardYmm) / diffY;

                        char text[16] = {0};
                        sprintf (text, "X = %.3f, %.3f mm/pix", diffX, calibData.resolutionX);
                        putText(matHUD, text, cv::Point(0, 20), cv::FONT_HERSHEY_PLAIN, 1.0, cv::Scalar(0,255,0), 1, 4, 0);

                        sprintf (text, "Y = %.3f, %.3f mm/pix", diffY, calibData.resolutionY);
                        putText(matHUD, text, cv::Point(0, 45), cv::FONT_HERSHEY_PLAIN, 1.0, cv::Scalar(0,255,0), 1, 4, 0);

                        sprintf (text, "X / Y = %.3f, hypo = %.3f mm", calibData.resolutionX/ calibData.resolutionY, diffHypo*((calibData.resolutionX+calibData.resolutionY)/2));
                        putText(matHUD, text, cv::Point(0, 70), cv::FONT_HERSHEY_PLAIN, 1.0, cv::Scalar(0,255,0), 1, 4, 0);

                }

                 if(chessFound && calibNext){//calibStop){
                    calibData.image_points.push_back(calibData.corners);
                    calibData.object_points.push_back(obj);
                    GUI.ConsoleOut(u8"Снимок %u из %u сохранен", successes, numBoards);

                    if (successes == 0){
                        calibData.resolutionX = tempResX;
                        calibData.resolutionY = tempResY;
                        GUI.ConsoleOut(u8"Новое разрешение: X = %.3f мм/пикс, Y = %.3f мм/пикс", calibData.resolutionX, calibData.resolutionY);
                    }

                    successes++;

                    if(successes >= numBoards){
                        saveData();
                        GUI.popupError(u8"Калибровка заверщена");
                        successes = 0;
                    }
                    calibNext = 0;

                 }

        }

}

cv::Mat Calibration_t::getCorrectedFrame(cv::Mat &in){
    cv::Mat toReturn;
    cv::remap(in, toReturn, calibData.map1, calibData.map2, cv::INTER_LINEAR);
    return toReturn;
}

void Calibration_t::stopCalib(void){
    calibStop = 1;
}

#include <stdio.h>

void Calibration_t::readData(void){
    //::Mat map1, map2;
    //map1 = cv::imread("calibData/map1.txt");
    //map2 = cv::imread("calibData/map2.txt");
    /*
    int filesExist = 0;
        FILE * pFile;
        pFile = fopen ("calibData/map1.bin","w");
        if (pFile!=NULL){
            filesExist++;
            fclose (pFile);
        }

        pFile = fopen ("calibData/map2.bin","w");
        if (pFile!=NULL){
            filesExist++;
            fclose (pFile);
        }

    if (filesExist == 2){
        */
        cv::Mat map1 = deserializeMatbin("calibData/map1.bin");
        cv::Mat map2 = deserializeMatbin("calibData/map2.bin");
    //}

    if (map1.empty() || map2.empty()) {
        dataReady = 0;
        GUI.popupError(u8"Ошибка чтения файла калибровки!");
        GUI.ConsoleOut(u8"Файлы калибровки не прочитаны!");
    }

    else {
        dataReady = 1;
        convertMaps(map1, map2, calibData.map1, calibData.map2, CV_32FC1 , 1);
        calibData.map1 = map1;
        calibData.map2 = map2;


        GUI.ConsoleOut(u8"Чтение файлов калибровки завершено");
    }
}

void Calibration_t::saveData(void){
    std::vector<cv::Mat> rvecs;
    std::vector<cv::Mat> tvecs;

    calibData.intrinsic.ptr<float>(0)[0] = 1;
    calibData.intrinsic.ptr<float>(1)[1] = 1;

    cv::calibrateCamera(calibData.object_points,
                        calibData.image_points,
                        imgSize,
                        calibData.intrinsic,
                        calibData.distCoeffs,
                        rvecs,
                        tvecs);

    cv::Mat newCam;
    dataReady = 0;
    cv::initUndistortRectifyMap(calibData.intrinsic,
                                calibData.distCoeffs,
                                cv::Mat(),
                                newCam,
                                imgSize,
                                CV_32FC1,
                                calibData.map1,
                                calibData.map2);

    dataReady = 1;

    serializeMatbin(calibData.map1, "calibData/map1.bin");
    serializeMatbin(calibData.map2, "calibData/map2.bin");

    GUI.ConsoleOut(u8"Файлы калибровки записаны");
    calibStop = 0;
}



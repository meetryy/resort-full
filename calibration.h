#include <iostream>
#include <fstream>
#include <Windows.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>


struct Calibration_t{
    struct {
        int numCornersHor = 9;
        int numCornersVer = 6;
        std::vector<std::vector<cv::Point3f>> object_points;
        std::vector<std::vector<cv::Point2f>> image_points;
        std::vector<cv::Point2f>         corners;
        cv::Mat intrinsic = cv::Mat(3, 3, CV_32FC1);
        cv::Mat distCoeffs;

        cv::Mat map1, map2;

        float boardXmm = 74.4;
        float boardYmm = 31.9;
        float resolutionX = 0;
        float resolutionY = 0;

    } calibData;

    int numSquares = 0;
    cv::Size boardSize;
    cv::Mat matHUD;
    bool isSuccess = 0;
    bool calibStop = 0;
    bool chessFound = 0;
    cv::Size imgSize;
    std::vector<cv::Point3f> obj;
    bool dataReady = 1;
    int numBoards = 4;
    int successes = 0;
    bool calibNext = 0;

    bool undistIsOn = 0;

    void Init(void);
    void processFrame(cv::Mat &in);
    cv::Mat getCorrectedFrame(cv::Mat &in);
    void saveData(void);
    void stopCalib(void);
    void readData(void);

};

extern struct Calibration_t Calib;

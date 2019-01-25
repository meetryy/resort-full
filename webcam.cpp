#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <thread>

#include "variables1.h"
#include "UI.h"
#include "main.h"
#include "file.h"
#include "img.h"
#include "rs232.h"
#include "COM_port.h"
#include "webcam.h"


cv::Mat webcam_out;


void webcam_init(void){
    //int apiBackend = cv::CAP_DSHOW;
    //cv::VideoCapture cap(0+apiBackend);

    cap.open(0);

    cap.set(cv::CAP_PROP_FOURCC ,cv::VideoWriter::fourcc('M', 'J', 'P', 'G') );
    //cap.set(CAP_PROP_EXPOSURE , 1);
    //cap.set(CAP_PROP_GAIN , 10);

    cap.set(cv::CAP_PROP_FPS, 30);
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 960  );
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 720);
}

void webcam_get(void){
   cap >> img;
}










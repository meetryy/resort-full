#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

#include "video_player.h"

using namespace cv;
using namespace std;

void  videoPlayer_t::Start(std::string FlieName, int StartFrame, int EndFrame, int Rotate){
    cap.open(FlieName);
    cout << "opening " << FlieName << endl;
    if(!cap.isOpened()) {cout << "Error opening video stream or file" << endl;}
    if (EndFrame < 0) endFrame = cap.get(CAP_PROP_FRAME_COUNT);
    else endFrame = EndFrame;
    startFrame = StartFrame;
    Rotation = Rotate;
    partLength = EndFrame - StartFrame;

    cap.set(CAP_PROP_POS_FRAMES, StartFrame);
    frameH = cap.get(CAP_PROP_FRAME_HEIGHT);
    frameW = cap.get(CAP_PROP_FRAME_WIDTH);
    playbackMarker = cap.get(CAP_PROP_POS_FRAMES);
    playbackMs = cap.get(CAP_PROP_POS_MSEC);

    videoFPS = cap.get(CAP_PROP_FPS);
    fileLengthFrames =  cap.get(CAP_PROP_FRAME_COUNT);
    fileLengthMs = fileLengthFrames / videoFPS;

    cout << "frameH = " << frameH << " frameW = " << frameW << "marker = " <<playbackMarker<< endl;
}

cv::Mat videoPlayer_t::getFrame(void){
    Mat tempMat;
    playbackMarker = cap.get(CAP_PROP_POS_FRAMES);
    playbackMs = cap.get(CAP_PROP_POS_MSEC);

    partLength = endFrame - startFrame;
    playbackPortion = (float)playbackMarker / (float)partLength;
    if (playbackMarker >= endFrame) cap.set(CAP_PROP_POS_FRAMES, startFrame);

    if (!pause) {
            cap >> tempMat;
            lastMat = tempMat.clone();
    }
    else {tempMat = lastMat.clone();}
    if (Rotation >= 0) cv::rotate(tempMat, tempMat, Rotation);

    return tempMat;
}

void videoPlayer_t::setMarker(double Position){
    playbackMarker = Position * (double)(endFrame - startFrame);

//    cout << "playbackMarker = " <<playbackMarker<< " Position = "<<Position<< endl;
    cap.set(CAP_PROP_POS_FRAMES, playbackMarker);
}

void videoPlayer_t::setMarker(int frames){
    bool success = cap.set(CAP_PROP_POS_FRAMES, frames);
    //cout << "success = " <<success << endl;
}

void videoPlayer_t::fpsStart(void){
    startTime = cv::getTickCount(); // fps routine
}
#include <ctime>
#include <unistd.h>

void videoPlayer_t::fpsStop(void){
    fpsAccum += cv::getTickFrequency() / (cv::getTickCount() - startTime);
        fpsAvgCounter++;
        if (fpsAvgCounter >= fpsAvg){
            fps = fpsAccum / (double)fpsAvgCounter;
            fpsAccum =0 ;
            fpsAvgCounter = 0;
        }
        if (fps >= fpsLimiter) {
            #warning FIXME this works wrong
            int fpsDiff = (int)(fps - fpsLimiter);
            float targerFrameTime = 1000.0 / fpsLimiter;
            int thisFrameTime = 1000.0 / fps;
            int delay = (int)(targerFrameTime - thisFrameTime);
            //float diffMs = 1000.0 / (float)fpsDiff;
            usleep(1000 * delay);
        }
}

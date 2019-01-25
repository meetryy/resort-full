#include "KalmanFilterTracker.h"

#include <iostream>
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;


KalmanFilterTracker::KalmanFilterTracker(){
    //ctor
    KF = cv::KalmanFilter(4, 2, 0);
    state(4, 1);
    precessNoise = Mat(4, 1, CV_32F);
    measurement(2, 1);
    measurement.setTo(Scalar(0));
    init = true;

}

KalmanFilterTracker::~KalmanFilterTracker() {}

void KalmanFilterTracker::initilizeKF(int x, int y) {

    KF.statePre.at<float>(0) = x;
    KF.statePre.at<float>(1) = y;
    KF.statePre.at<float>(2) = 0;
    KF.statePre.at<float>(3) = 0;
//    KF.transitionMatrix = *(Mat_<float>(4, 4) << 1,0,0,0,   0,1,0,0,   0,0,1,0,   0,0,0,1 );
   KF.transitionMatrix = (Mat_<float>(2, 2) << 1, 1, 0, 1);

    setIdentity(KF.measurementMatrix);
    setIdentity(KF.processNoiseCov, Scalar::all(1e-4));
    setIdentity(KF.measurementNoiseCov, Scalar::all(1e-4));
    setIdentity(KF.errorCovPost, Scalar::all(.1));

    points_vector.clear();
    kalman_vector.clear();
    init = false;
}

void KalmanFilterTracker::track(int x, int y) {

    if ( init )
        initilizeKF(x, y);

    Mat prediction = KF.predict();
    Point predictPt(prediction.at<float>(0), prediction.at<float>(1));

    measurement(0) = x;
    measurement(1) = y;

    Point measPt(measurement(0), measurement(1));
    points_vector.push_back(measPt);

    Mat estimated = KF.correct(measurement);
    Point statePt( estimated.at<float>(0), estimated.at<float>(1) );
    kalman_vector.push_back(statePt);

}


void KalmanFilterTracker::draw(Mat img) {

    for (int i=0; i<kalman_vector.size()-1; i++) {
        line(img, kalman_vector[i], kalman_vector[i+1], Scalar(0,255,0), 1);
    }

}

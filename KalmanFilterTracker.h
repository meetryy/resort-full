#ifndef KALMANFILTERTRACKER_H
#define KALMANFILTERTRACKER_H

#include <iostream>
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

class KalmanFilterTracker
{
    public:
        KalmanFilter KF;
        Mat_<float> state;
        Mat precessNoise;
        Mat_<float> measurement;
        vector<Point>points_vector, kalman_vector;
        bool init;

        KalmanFilterTracker();
        virtual ~KalmanFilterTracker();
        void track(int x, int y);
        void initilizeKF(int x, int y);
        void draw(Mat img);
    protected:
    private:
};

#endif // KALMANFILTERTRACKER_H

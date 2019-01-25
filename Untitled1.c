#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgcodecs.hpp"
#include <opencv2/core/utility.hpp>
#include <opencv2/bgsegm.hpp>
#include "bgsegm.hpp"
#include <opencv2/videoio.hpp>

#include <iostream>
#include <thread>
#include <windows.h>
#include <chrono>

#include "main.h"
#include "img.h"
#include "variables1.h"
#include "mog.h"
#include "real_dimensions.h"
#include "KalmanFilterTracker.h"
#include "bgsubcnt.h"
#include "omp.h"
#include "newGUI.h"

using namespace cv;
using namespace std;

Scalar avg_color_hsv;

char str[200];
float avg_time = 0;
int ScalarInRange (Scalar input, Scalar compare_to, Scalar delta, bool useHSV);
cv::Scalar BGR2HSV(cv::Scalar inBGR);

int     info_shown = 0;
int     bs_cnt_min_pix_stability;
int     bs_cnt_max_pix_stability;
bool    bs_cnt_use_history;
bool    bs_cnt_isparallel;
int     bs_cnt_fps;
int     bs_mog2_history;
float   bs_mog2_thresh;
bool    bs_mog2_shadows;
bool    bs_mog2_learning;
int     bs_knn_history;
float   bs_knn_thresh;
bool    bs_knn_shadows;
bool    bs_knn_learning;
int     bs_gsoc_mc;
int     bs_gsoc_samples;
float   bs_gsoc_reprate;
float   bs_gsoc_proprate;
int     bs_gsoc_hits_thresh;
float   bs_gsoc_alpha;
float   bs_gsoc_beta;
float   bs_gsoc_bs_decay;
float   bs_gsoc_bs_mul;
float   bs_gsoc_noise_bg;
float   bs_gsoc_noise_fg;
bool    bs_gsoc_learning;
int     mog_history;
int     mog_mixtures;
float   mog_backratio;
float   mog_noisesigma ;
bool    mog_learning;
float   bs_cnt_lrate;
float   bs_knn_lrate;
float   bs_mog2_lrate;
float   bs_mog_lrate;
float   bs_gsoc_lrate;
bool    bs_cnt_learning;
bool    ColorOK =0;
bool    video_recording = 0;
bool    draw_ruler =0;
int     morpho_type=0;

int     cam_number;
long    cam_width;
long    cam_height;
int     cam_gain;
int     cam_contrast;
int     cam_brightness;
int     cam_exposure;
int     cam_saturation;
int     cam_hue;
int     input_source = SOURCE_CAM;

double  cut_up;
double  cut_down;
double  cut_left;
double  cut_right;

bool    capture_file = 0;
bool    show_quad = 0;
bool    show_particle_number = 0;

int     bs_current_algo = BS_MOG;
int     contour_current_algo;
int     blur_before_mog = 3;

cv::Ptr<cv::BackgroundSubtractor> BackSubtractor;
cv::VideoCapture camera(cv::CAP_DSHOW);
cv::VideoCapture videocapture("test_video.avi");
VideoWriter oVideoWriter;

void BS_Init(int bs_algo)
{
    if (bs_algo==BS_MOG)
    {
        BackSubtractor = cv::bgsegm::createBackgroundSubtractorMOG(mog_history,mog_mixtures,mog_backratio,mog_noisesigma);
    }
    if (bs_algo==BS_MOG2)
    {
        BackSubtractor = cv::createBackgroundSubtractorMOG2(bs_mog2_history,bs_mog2_thresh,bs_mog2_shadows);
    }
    if (bs_algo==BS_CNT)
    {
        BackSubtractor = cv::bgsubcnt::createBackgroundSubtractorCNT(bs_cnt_min_pix_stability, bs_cnt_use_history, bs_cnt_max_pix_stability*bs_cnt_fps, bs_cnt_isparallel);
    }
    if (bs_algo==BS_KNN)
    {
        BackSubtractor = createBackgroundSubtractorKNN(bs_knn_history,bs_knn_thresh,bs_knn_shadows);
    }
    if (bs_algo==BS_GSOC)
    {
        BackSubtractor = cv::bgsegm::createBackgroundSubtractorGSOC(bs_gsoc_mc,
                         bs_gsoc_samples,
                         bs_gsoc_reprate,
                         bs_gsoc_proprate,
                         bs_gsoc_hits_thresh,
                         bs_gsoc_alpha,
                         bs_gsoc_beta,
                         bs_gsoc_bs_decay,
                         bs_gsoc_bs_mul,
                         bs_gsoc_noise_bg,
                         bs_gsoc_noise_fg);
    }

    bs_current_algo = bs_algo;
}

/*
void ProcessImg(void){

    img_wholemask = Mat::zeros(img.size(), CV_8UC3);

    // capture from cam
    if (capture_run && !freeze_frame && !capture_file) {camera >> img_in;}

    // capture from file
    if (capture_file) {
            camera.release();
            videocapture >> img_in;
            if (img_in.empty()) {videocapture.set(CAP_PROP_POS_FRAMES, 0); videocapture >> img_in;}
    }

    img = img_in;
    img_output = img.clone();

    cv::Mat img_gray;
    cv::cvtColor(img, img_gray, cv::COLOR_BGR2GRAY);
    blur(img_gray, img_gray, Size(blur_before_mog,blur_before_mog));

    if (bs_current_algo==BS_MOG) {BackSubtractor->apply(img_gray, img_mog_output, bs_mog_lrate*mog_learning);}
    if (bs_current_algo==BS_MOG2){BackSubtractor->apply(img_gray, img_mog_output, bs_mog2_lrate*bs_mog2_learning);}
    if (bs_current_algo==BS_CNT) {BackSubtractor->apply(img_gray, img_mog_output, bs_cnt_lrate*bs_cnt_learning);}
    if (bs_current_algo==BS_KNN) {BackSubtractor->apply(img_gray, img_mog_output, bs_knn_learning*bs_knn_lrate);}
    if (bs_current_algo==BS_GSOC) {BackSubtractor->apply(img_gray,img_mog_output, bs_gsoc_lrate);}

    blur(img_mog_output, img_mog_output, Size((int)canny_blur_value,(int)canny_blur_value) );

    if (contour_current_algo){
        Mat dx, dy;
        Scharr(img_mog_output,dx,CV_16S,1,0);
        Scharr(img_mog_output,dy,CV_16S,0,1);
        Canny( dx, dy, img_canny_output, canny_thresh_value, canny_thresh_value*3);
    }

    else{Canny(img_mog_output, img_canny_output, canny_thresh_value, canny_thresh_value*2 );}

    static int type;
    if (morpho_type==MORPH_CURRENT_RECT){type=cv::MORPH_RECT;}
    else if (morpho_type==MORPH_CURRENT_CROSS){type=cv::MORPH_CROSS;}
    else if (morpho_type==MORPH_CURRENT_ELLIPSE){type=cv::MORPH_ELLIPSE;}

    cv::Mat element = cv::getStructuringElement(type, cv::Size( morph_size + 1, morph_size+1 ), cv::Point( morph_size, morph_size ) );
    cv::morphologyEx(img_canny_output, img_morph_out, cv::MORPH_CLOSE, element);
    cv::findContours(img_morph_out, contours, hierarchy,cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    good_contours = 0;
    info_total_contours = 0;

    omp_set_num_threads(2);

    #pragma omp parallel for
    for( int i = 0; i < contours.size(); i++ ){

        int id = omp_get_thread_num();

        printf("thread (%d)\n", id );

        // draw each contour filled at img_wholemask
        cv::drawContours(img_wholemask, contours, i, cv::Scalar(255,255,255), cv::FILLED);
        info_total_contours++;

        vector<Point2f> mc(contours.size());
        vector<Moments> mu(contours.size());
        vector<Point2f> centers( contours.size() );

        if ((contourArea(contours[i]) > min_bbox_area)&&(contourArea(contours[i]) < max_bbox_area)){
        useful_contours++;

        mu[i] = moments(contours[i], false);
        mc[i] = Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 );
        float X = mu[i].m10/mu[i].m00;
        float Y = mu[i].m01/mu[i].m00;

        Rect roi = boundingRect(contours[i]);
        diameter = roi.width;

        img(roi).copyTo(img_roi, img_wholemask(roi));

        double PartOfColor = ((contourArea(contours[i])) / boundingRect(contours[i]).area());

        Scalar avg_color = Scalar(mean(img_roi)) / PartOfColor;
/*
        int ColorsMatch;
        if (good_colorspace == BGR){ColorsMatch = ScalarInRange(avg_color, good_color_rgb, color_tolerance_rgb, 0);}
        else{
            Mat img_roi_hsv;
            cvtColor(img_roi,img_roi_hsv,COLOR_BGR2HSV);
            cv::Mat tempBGR(1, 1, CV_8UC3, avg_color);
            cv::Mat tempHSV; //(1, 1, CV_8UC3, avg_color);

            cvtColor(tempBGR, tempHSV,COLOR_BGR2HSV);
            Vec3b hsvout=tempHSV.at<Vec3b>(0,0);

            Scalar avg_color_hsv = hsvout;
            ColorsMatch = ScalarInRange(avg_color_hsv, good_color_hsv, color_tolerance_hsv ,1);
        }

    ColorOK = (ColorsMatch >= 2 );
    good_contours += ColorOK;


    ColorOK = 1;
    Scalar contourcolor = Scalar(0, (0+255*ColorOK), 255-(255*ColorOK)) ;
    Scalar textcolor = Scalar(0, (0+255*!ColorOK), 255-(255*!ColorOK)) ;

    if (show_particle_number){sprintf(str,"%u", (int)i); putText(img_output, str, Point(X,Y), FONT_HERSHEY_PLAIN  , 1.2, textcolor, 1);}
    if (show_area){ sprintf(str,"%.0f mm2", pix2mm((long)(contourArea(contours[i])))); putText(img_output, str, Point(X,Y+14), FONT_HERSHEY_PLAIN  , 1.2, textcolor, 1);}
    if (show_avgcolor){ sprintf(str,"%.0f;%.0f;%.0f", avg_color[2], avg_color[1],avg_color[0]); putText(img_output, str, Point(X,Y+28), FONT_HERSHEY_PLAIN  , 1.2, textcolor, 1);}
    if (show_diameter){ sprintf(str,"d=%.1f mm", pix2mm(diameter)); putText(img_output, str, Point(X,Y+42), FONT_HERSHEY_PLAIN  , 1.2, textcolor, 1);}
    if (show_bboxes){rectangle(img_output,boundingRect(contours[i]).tl(), boundingRect(contours[i]).br(), Scalar(255,0,0), 1, 4, 0 );}
    if (show_centers){circle(img_output, mc[i], 2, 255, -1, 8, 0 );}
    if (show_contours){drawContours(img_output, contours, (int)i, contourcolor, 2, LINE_4, hierarchy, 0);}
    if (show_fill_avgcolor){ cv::drawContours(img_output, contours, i, avg_color, cv::FILLED);}
    if (show_fill_contcolor){ cv::drawContours(img_output, contours, i, contourcolor, cv::FILLED);}

    }

}
}

*/

Mat img_in;
Mat img_mog_output;
Mat img_canny;
Mat img_roi;
Mat img_roi_hsv;
Mat img_wholemask;
Mat img_output;
Mat img_bs_back;
Mat img_morph_out;
Mat img_canny_output;
Mat img_contours;

void ImgProcessor (void)
{
    while (1)
    {
        if (capture_run)
        {
            if (capture_run && !freeze_frame && !capture_file)
            {
                camera >> img_in;
            }
            if (capture_file)
            {
                //camera.release();
                videocapture >> img_in;
                if (img_in.empty())
                {
                    videocapture.set(CAP_PROP_POS_FRAMES, 0);
                    videocapture >> img_in;
                }
            }

            img_wholemask = Mat::zeros(img_in.size(), CV_8UC3);
            img_output = img_in.clone();

            cv::Mat img_gray;
            cv::cvtColor(img_in, img_gray, cv::COLOR_BGR2GRAY);
            blur(img_gray, img_gray, Size(blur_before_mog,blur_before_mog));

            switch (bs_current_algo)
            {
            case BS_MOG:
            {
                BackSubtractor->apply(img_gray, img_mog_output, bs_mog_lrate*mog_learning);
                break;
            }
            case BS_MOG2:
            {
                BackSubtractor->apply(img_gray, img_mog_output, bs_mog2_lrate*bs_mog2_learning);
                break;
            }
            case BS_CNT:
            {
                BackSubtractor->apply(img_gray, img_mog_output, bs_cnt_lrate*bs_cnt_learning);
                break;
            }
            case BS_KNN:
            {
                BackSubtractor->apply(img_gray, img_mog_output, bs_knn_learning*bs_knn_lrate);
                break;
            }
            case BS_GSOC:
            {
                BackSubtractor->apply(img_gray, img_mog_output, bs_gsoc_lrate);
                break;
            }

            }

            blur(img_mog_output, img_mog_output, Size((int)canny_blur_value,(int)canny_blur_value) );

            if (contour_current_algo)
            {
                Mat dx, dy;
                Scharr(img_mog_output,dx,CV_16S,1,0);
                Scharr(img_mog_output,dy,CV_16S,0,1);
                Canny( dx, dy, img_canny_output, canny_thresh_value, canny_thresh_value*3);
            }

            else
            {
                Canny(img_mog_output, img_canny_output, canny_thresh_value, canny_thresh_value*2 );
            }

            static int type;
            if (morpho_type==MORPH_CURRENT_RECT)
            {
                type=cv::MORPH_RECT;
            }
            else if (morpho_type==MORPH_CURRENT_CROSS)
            {
                type=cv::MORPH_CROSS;
            }
            else if (morpho_type==MORPH_CURRENT_ELLIPSE)
            {
                type=cv::MORPH_ELLIPSE;
            }

            vector<vector<Point> >  contours;
            vector<Vec4i>           hierarchy;
            vector<float>           radius(contours.size());
            vector<vector<Point> >  contours_poly(contours.size());
            vector<Rect>            boundRect(contours.size() );

            cv::Mat element = cv::getStructuringElement(type, cv::Size( morph_size + 1, morph_size+1 ), cv::Point( morph_size, morph_size ) );
            cv::morphologyEx(img_canny_output, img_morph_out, cv::MORPH_CLOSE, element);
            cv::findContours(img_morph_out, contours, hierarchy,cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

            good_contours = 0;
            info_total_contours = 0;

            //omp_set_num_threads(8);
            //#pragma omp parallel for
            for( int i = 0; i < contours.size(); i++ )
            {
                cv::drawContours(img_wholemask, contours, i, cv::Scalar(255,255,255), cv::FILLED);

                info_total_contours++;
                vector<Point2f> mc(contours.size());
                vector<Moments> mu(contours.size());
                vector<Point2f> centers( contours.size() );

                if ((contourArea(contours[i]) > min_bbox_area)&&(contourArea(contours[i]) < max_bbox_area))
                {
                    useful_contours++;

                    mu[i] = moments(contours[i], false);
                    mc[i] = Point2f( mu[i].m10/mu[i].m00, mu[i].m01/mu[i].m00 );
                    float X = mu[i].m10/mu[i].m00;
                    float Y = mu[i].m01/mu[i].m00;

                    Rect roi = boundingRect(contours[i]);
                    long diameter = roi.width;

                    img_in(roi).copyTo(img_roi, img_wholemask(roi));

                    double PartOfColor = ((contourArea(contours[i])) / boundingRect(contours[i]).area());

                    Scalar avg_color = Scalar(mean(img_roi)) / PartOfColor;

                    int ColorsMatch;
                    if (good_colorspace == BGR)
                    {
                        ColorsMatch = ScalarInRange(avg_color, good_color_rgb, color_tolerance_rgb, 0);
                    }
                    else
                    {
                        ColorsMatch = ScalarInRange(BGR2HSV(avg_color), good_color_hsv, color_tolerance_hsv, 1);
                    }


                    ColorOK = (ColorsMatch >= 3 );
                    good_contours += ColorOK;

                    Scalar contourcolor = Scalar(0, (0+255*ColorOK), 255-(255*ColorOK)) ;
                    Scalar textcolor = Scalar(0, (0+255*!ColorOK), 255-(255*!ColorOK)) ;

                    if (show_particle_number)
                    {
                        sprintf(str,"%u", (int)i);
                        putText(img_output, str, Point(X,Y), FONT_HERSHEY_PLAIN, 1.2, textcolor, 1);
                    }
                    if (show_area)
                    {
                        sprintf(str,"%.0f mm2", pix2mm((long)(contourArea(contours[i]))));
                        putText(img_output, str, Point(X,Y+14), FONT_HERSHEY_PLAIN, 1.2, textcolor, 1);
                    }
                    if (show_avgcolor)
                    {
                        sprintf(str,"%.0f;%.0f;%.0f", avg_color[2], avg_color[1],avg_color[0]);
                        putText(img_output, str, Point(X,Y+28), FONT_HERSHEY_PLAIN, 1.2, textcolor, 1);
                    }
                    if (show_diameter)
                    {
                        sprintf(str,"d=%.1f mm", pix2mm(diameter));
                        putText(img_output, str, Point(X,Y+42), FONT_HERSHEY_PLAIN, 1.2, textcolor, 1);
                    }
                    if (show_bboxes)
                    {
                        rectangle(img_output,boundingRect(contours[i]).tl(), boundingRect(contours[i]).br(), Scalar(255,0,0), 1, 4, 0 );
                    }
                    if (show_centers)
                    {
                        circle(img_output, mc[i], 2, 255, -1, 8, 0 );
                    }
                    if (show_fill_avgcolor)
                    {
                        cv::drawContours(img_output, contours, i, avg_color, cv::FILLED);
                    }
                    if (show_fill_contcolor)
                    {
                        cv::drawContours(img_output, contours, i, contourcolor, cv::FILLED);
                    }
                    if (show_contours)
                    {
                        drawContours(img_output, contours, (int)i, contourcolor, 2, LINE_4, hierarchy, 0);
                    }
                }
            }

            for (int i=0; i<W_MAT_NR; i++)
            {
                if (MatWin[i].show)
                {
                    MatWin[i].write.lock();
                    MatWin[i].mat->copyTo(MatWin[i].mat_show);
                    MatWin[i].write.unlock();
                }
            }
        }

        else
        {
            Sleep(100);
            cout << "i shleep" << endl;
        }

    }
}

int ScalarInRange_old (Scalar input, Scalar compare_to, Scalar delta, bool useHSV)
{
    int to_return = 0;
    if (useHSV)
    {
        double Hoverrun = (Scalar(compare_to + delta).val[0] - 180.0);

        to_return +=    ((Hoverrun > 0.0) *(input.val[0] <= Hoverrun)) * (input.val[0] >= (compare_to - delta).val[0]) +
                        ((Hoverrun <= 0)*(input.val[0] <= (compare_to + delta).val[0])) * (input.val[0] >= (compare_to - delta).val[0]);
        for (int k = 1; k < 3; k++)
        {
            if  (
                ((compare_to - delta).val[k] <= input.val[k]) &&
                ((compare_to + delta).val[k] >= input.val[k]) //||
                //((-1.0*180.0 + (compare_to + delta).val[0]) >= input.val[0])
            )
            {
                to_return++;
            }
        }
        //cout << "input: " << input << endl;
        //cout << "compare_to: " << compare_to << endl;
        //cout << "delta: " << delta << endl;
        //cout << "compare_to - delta: " << compare_to - delta << endl;
        //cout << "compare_to + delta: " << compare_to + delta << endl;
        //cout << "Hoverrun: " << Scalar(compare_to + delta).val[0] - 180.0 << endl;
        //cout << "(input.val[0] <= Hoverrun): " << (input.val[0] <= Hoverrun) << endl;
        //cout << "(input.val[0] >= (compare_to - delta).val[0]): " << (input.val[0] >= (compare_to - delta).val[0]) << endl;
        //cout << "(input.val[0] <= (compare_to + delta).val[0]): " << (input.val[0] <= (compare_to + delta).val[0]) << endl;
        //cout << "(input.val[0] >= (compare_to - delta).val[0]): " << (input.val[0] >= (compare_to - delta).val[0]) << endl;
        //cout << "=========================" << endl;
    }
    else
    {
        for (int k = 0; k < 3; k++)
        {
            if  (
                ((compare_to - delta).val[k] <= input.val[k]) &&
                ((compare_to + delta).val[k] >= input.val[k]) //||
                //((-1.0*180.0 + (compare_to + delta).val[0]) >= input.val[0])
            )
            {
                to_return++;
            }
        }
    }
    return to_return;
}

inline bool ValueInRange(double value, double lower, double upper)
{
    return((lower < value) && (value < upper));
}

int ScalarInRange (Scalar input, Scalar compare_to, Scalar delta, bool useHSV)
{
    int to_return = 0;
    // HSV compare
    if (useHSV)
    {
        double H_add_to_min = (Scalar(compare_to + delta).val[0] - 180.0);  //overlap at the end, adds to min
        double H_add_to_max = (180.0 + Scalar(compare_to - delta).val[0]);  //overlap at min, adds to max

        if (H_add_to_min >= 0)
        {
            to_return+= ValueInRange(input.val[0], 0,                                 H_add_to_min) ||
                        ValueInRange(input.val[0], Scalar(compare_to - delta).val[0], 180.0);
        }
        else if (H_add_to_max <= 180)
        {
            to_return+= ValueInRange(input.val[0], 0,                                 Scalar(compare_to + delta).val[0]) ||
                        ValueInRange(input.val[0], H_add_to_max,                      180.0);
        }

        else
        {
            to_return+= ValueInRange(input.val[0], Scalar(compare_to - delta).val[0], Scalar(compare_to + delta).val[0]);
        }


        for (int k = 1; k < 3; k++)
        {
            if  (ValueInRange(input.val[k], (compare_to - delta).val[k], (compare_to + delta).val[k]))
            {
                to_return++;
            }
        }
    }

    // RGB compare
    else
    {
        for (int k = 0; k < 3; k++)
        {
            if  (ValueInRange(input.val[k], (compare_to - delta).val[k], (compare_to + delta).val[k]))
                to_return++;

        }

        return to_return;
    }
}

cv::Scalar BGR2HSV(cv::Scalar inBGR)
{
    cv::Mat tempBGR(1, 1, CV_8UC3, inBGR);
    cv::Mat tempHSV; //(1, 1, CV_8UC3, avg_color);
    cvtColor(tempBGR, tempHSV,COLOR_BGR2HSV);
    Vec3b hsvout=tempHSV.at<Vec3b>(0,0);
    Scalar avg_color_hsv = hsvout;
    return hsvout;
}

void cam_open(void)
{
    camera.open(cam_number);
    camera.set(cv::CAP_PROP_FOURCC,cv::VideoWriter::fourcc('M', 'J', 'P', 'G') );
    camera.set(cv::CAP_PROP_FPS, 50);
    camera.set(cv::CAP_PROP_FRAME_WIDTH, cam_width);
    camera.set(cv::CAP_PROP_FRAME_HEIGHT, cam_height);

    cam_update();

    std::cout << "CAP_PROP_CONTRAST " << camera.get(CAP_PROP_CONTRAST) << endl;
    std::cout << "CAP_PROP_GAIN " << camera.get(CAP_PROP_GAIN) << endl;
    std::cout << "CAP_PROP_BRIGHTNESS " << camera.get(CAP_PROP_BRIGHTNESS) << endl;
    std::cout << "CAP_PROP_EXPOSURE " << camera.get(CAP_PROP_EXPOSURE) << endl;
    std::cout << "CAP_PROP_SATURATION " << camera.get(CAP_PROP_SATURATION) << endl;
    std::cout << "CAP_PROP_FPS " << camera.get(CAP_PROP_FPS) << endl;

    camera >> img_in;
    if (camera.isOpened())
    {
        capture_run =1;
    }
}

void cam_close(void)
{
    camera.release();
    if (!camera.isOpened())
    {
        capture_run = 0;
    }
}

void cam_update(void)
{
    camera.set(cv::CAP_PROP_CONTRAST, cam_contrast);
    camera.set(cv::CAP_PROP_GAIN, cam_gain);
    camera.set(cv::CAP_PROP_BRIGHTNESS,cam_brightness);
    camera.set(cv::CAP_PROP_SATURATION,cam_saturation);
    camera.set(cv::CAP_PROP_EXPOSURE,cam_exposure);
    //camera.set(cv::CAP_PROP_GAMMA   ,cam_hue);
}

void start_video_rec(void)
{
    time_t now;
    char the_date[32];
    the_date[0] = '\0';
    now = time(NULL);
    strftime(the_date, 32, "%d-%m-%Y %H-%M-%S", gmtime(&now));
    char buf[64];
    sprintf(buf, "video/%s.avi", the_date);

    double dWidth = camera.get(CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
    double dHeight = camera.get(CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video
    Size frameSize(static_cast<int>(dWidth), static_cast<int>(dHeight));
    oVideoWriter.open(buf, cv::VideoWriter::fourcc('M','J','P','G'), 50, frameSize, true); //initialize the VideoWriter object

    if ( !oVideoWriter.isOpened() )
    {
        cout << "ERROR: Failed to write the video" << endl;
    }
    else
    {
        video_recording = 1;
    }
}

void stop_video_rec(void)
{
    if (oVideoWriter.isOpened() )
    {
        oVideoWriter.release();
        video_recording = 0;
    }
}


//================================================================

/*
    int ruler_cm = 16;
    if ((show_frame == SHOW_OUTPUT) && draw_ruler) {
        long pix_per_10mm = pix_per_100mm/20;
        long rulerX = (img_output.cols / 2) - ruler_cm/2 * pix_per_10mm;
        long rulerY = img_output.rows - 20;

        for(int i = 0; (i<ruler_cm); i++){
            cv::Rect ruler(rulerX+i*pix_per_10mm, rulerY, pix_per_10mm, 10);
            rectangle(img_output,ruler,Scalar(255,255,255),2-(3*(i%2==0)),8,0);

        }
    }


    if (show_frame==SHOW_QUAD){
        Mat img_mog_output_mini;
        Mat img_canny_output_mini;
        Mat morph_out_mini;
        Mat img_wholemask_mini;

        cv::resize(img_canny_output, img_canny_output_mini, img_canny_output_mini.size(), 0.5, 0.5, INTER_LINEAR);
        cv::resize(img_morph_out, morph_out_mini, morph_out_mini.size(), 0.5, 0.5, INTER_LINEAR);
        cv::resize(img_mog_output, img_mog_output_mini, img_mog_output_mini.size(), 0.5, 0.5, INTER_LINEAR);
        cv::resize(img_wholemask, img_wholemask_mini, img_wholemask_mini.size(), 0.5, 0.5, INTER_LINEAR);

        cv::Mat grayscaleMat (img_wholemask_mini.size(), CV_8U);
        cv::cvtColor( img_wholemask_mini, grayscaleMat, COLOR_BGR2GRAY );
        cv::Mat binaryMat(grayscaleMat.size(), grayscaleMat.type());
        cv::threshold(grayscaleMat, binaryMat, 1, 255, cv::THRESH_BINARY);

        img_quad.create(img_mog_output_mini.rows*2, img_mog_output_mini.cols*2);

        //process img_wholemask
        img_mog_output_mini.copyTo(img_quad       (Rect(0,    0,      img_mog_output_mini.cols,   img_mog_output_mini.rows)));
        img_canny_output_mini.copyTo(img_quad(Rect(img_mog_output_mini.cols, 0,      img_mog_output_mini.cols,   img_mog_output_mini.rows)));
        morph_out_mini.copyTo(img_quad       (Rect(0,    img_mog_output_mini.rows,   img_mog_output_mini.cols,   img_mog_output_mini.rows)));
        binaryMat.copyTo(img_quad (Rect (img_mog_output_mini.cols, img_mog_output_mini.rows,   img_mog_output_mini.cols,   img_mog_output_mini.rows)));

        putText(img_quad, "MOG OUTPUT",      Point(10, 20), FONT_HERSHEY_SIMPLEX  , 0.5, Scalar(255), 1);
        putText(img_quad, "CANNY OUTPUT",    Point(img_mog_output_mini.cols+10, 20), FONT_HERSHEY_SIMPLEX  , 0.5, Scalar(255), 1);
        putText(img_quad, "MORPH OUTPUT",    Point(10, img_mog_output_mini.rows+20), FONT_HERSHEY_SIMPLEX  , 0.5, Scalar(255), 1);
        putText(img_quad, "MASK",            Point(img_mog_output_mini.cols+10, img_mog_output_mini.rows+20), FONT_HERSHEY_SIMPLEX  , 0.5, Scalar(255), 1);
    }


    switch (show_frame){
        case SHOW_INPUT:    {if(!img_in.empty()){imshow("Image",img_in);}; break;}
        case SHOW_CANNY:    {if(!img_canny_output.empty()){imshow("Image",img_canny_output);}; break;}
        case SHOW_MASK:     {if(!img_wholemask.empty()){imshow("Image",img_wholemask);}; break;}
        case SHOW_OUTPUT:   {   sprintf(str,"%2.1f FPS", info_fps);
                                putText(img_output, str, Point(20,30), FONT_HERSHEY_SIMPLEX  , 0.4, Scalar(255,255,255), 1);
                                sprintf(str,"%d contours", info_total_contours);
                                putText(img_output, str, Point(20,45), FONT_HERSHEY_SIMPLEX  , 0.4, Scalar(255,255,255), 1);
                                sprintf(str,"%d good", good_contours);
                                putText(img_output, str, Point(20,60), FONT_HERSHEY_SIMPLEX  , 0.4, Scalar(255,255,255), 1);
                                if(!img_output.empty()){imshow("Image",img_output);}; break;
                            }
        case SHOW_QUAD:   {if(!img_quad.empty()){imshow("Image",img_quad);}; break;}
    }
    */


#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgcodecs.hpp"
#include <opencv2/core/utility.hpp>
#include <opencv2/bgsegm.hpp>
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
#include "bgsubcnt.h"
#include "omp.h"
#include "newGUI.h"
#include "video_player.h"
#include "preprocessor.h"
#include "belt_processor.h"

using namespace cv;
using namespace std;

Image_class Img;
cv::Ptr<cv::BackgroundSubtractor> BackSubtractor;
cv::VideoCapture    camera(cv::CAP_DSHOW);
cv::VideoCapture    videocapture;
cv::VideoWriter     oVideoWriter;

videoPlayer_t       videoPlayer;
preprocessor_t      preprocessor;
imageProcessor_t    BeltProcessor;

void Image_class::FPS_Routine(void){
    sf::Time elapsed1 = FPS_Clock.getElapsedTime();
    FPS_Clock.restart();

    fps_accumulated += 1.f / elapsed1.asSeconds();
    V.Info.fps_avg_counter++;
    if (V.Info.fps_avg_counter >= fps_average){
        V.Info.FPS = fps_accumulated / fps_average;
        fps_accumulated = 0;
        V.Info.fps_avg_counter = 0;
    }
}

void Image_class::BS_Init(int bs_algo){
    if (bs_algo==BS_MOG){BackSubtractor = cv::bgsegm::createBackgroundSubtractorMOG(V.BS.MOG.History,V.BS.MOG.Mixtures,V.BS.MOG.BackRatio,V.BS.MOG.NoiseSigma);}
    if (bs_algo==BS_MOG2){BackSubtractor = cv::createBackgroundSubtractorMOG2(V.BS.MOG2.History,V.BS.MOG2.Thresh,V.BS.MOG2.DetectShadows);}
    if (bs_algo==BS_CNT){
            BackSubtractor = cv::bgsubcnt::createBackgroundSubtractorCNT(V.BS.CNT.MinPixStability, V.BS.CNT.UseHistory, V.BS.CNT.MaxPixStability*V.BS.CNT.FPS, V.BS.CNT.IsParallel);}
    if (bs_algo==BS_KNN){BackSubtractor = createBackgroundSubtractorKNN(bs_knn_history,bs_knn_thresh,bs_knn_shadows);}
    /*
    if (bs_algo==BS_GSOC){BackSubtractor = cv::bgsegm::createBackgroundSubtractorGSOC(bs_gsoc_mc,
                                                                                      bs_gsoc_samples,
                                                                                      bs_gsoc_reprate,
                                                                                      bs_gsoc_proprate,
                                                                                      bs_gsoc_hits_thresh,
                                                                                      bs_gsoc_alpha,
                                                                                      bs_gsoc_beta,
                                                                                      bs_gsoc_bs_decay,
                                                                                      bs_gsoc_bs_mul,
                                                                                      bs_gsoc_noise_bg,
                                                                                      bs_gsoc_noise_fg);}
    */

    V.BS.CurrentAlgo = bs_algo;
    GUI.ConsoleOut(u8"ИЗОБРАЖЕНИЕ: Алгоритм вычитания фона инициализирован");
}

void Image_class::WaterfallProcessor(cv::Mat img_in){
    if (!img_in.empty()){
        img_wholemask = Mat::zeros(img_in.size(), CV_8UC3);
        Mat img_output_temp = img_in.clone();
        cv::Mat img_gray;
        cv::cvtColor(img_in, img_gray, cv::COLOR_BGR2GRAY);
        blur(img_gray, img_gray, Size(V.BS.BlurBeforeMog,V.BS.BlurBeforeMog));

        switch (V.BS.CurrentAlgo){
            case BS_MOG:    {BackSubtractor->apply(img_gray, img_mog_output, V.BS.MOG.LRate*V.BS.MOG.Learning); break;}
            case BS_MOG2:   {BackSubtractor->apply(img_gray, img_mog_output, V.BS.MOG2.LRate*V.BS.MOG2.Learning); break;}
            case BS_CNT:    {BackSubtractor->apply(img_gray, img_mog_output, V.BS.CNT.LRate*V.BS.CNT.Learning); break;}
            case BS_KNN:    {BackSubtractor->apply(img_gray, img_mog_output, bs_knn_learning * bs_knn_lrate); break;}
            //case BS_GSOC:   {BackSubtractor->apply(img_gray, img_mog_output, bs_gsoc_lrate); break;}
        }

        blur(img_mog_output, img_mog_output, Size((int)V.Edge.BlurValue,(int)V.Edge.BlurValue) );

        if (V.Contours.CurrentAlgo){
            Mat dx, dy;
            Scharr(img_mog_output,dx,CV_16S,1,0);
            Scharr(img_mog_output,dy,CV_16S,0,1);
            Canny( dx, dy, img_canny_output, V.Edge.CannyThresh1, V.Edge.CannyThresh1*3);
        }

        else {Canny(img_mog_output, img_canny_output, V.Edge.CannyThresh1, V.Edge.CannyThresh2 );}

        static int type;
        if (V.Morph.Type==MORPH_CURRENT_RECT){type=cv::MORPH_RECT;}
        else if (V.Morph.Type==MORPH_CURRENT_CROSS){type=cv::MORPH_CROSS;}
        else if (V.Morph.Type==MORPH_CURRENT_ELLIPSE){type=cv::MORPH_ELLIPSE;}

        vector<vector<Point> >  contours;
        vector<Vec4i>           hierarchy;
        vector<float>           radius(contours.size());
        vector<vector<Point> >  contours_poly(contours.size());
        vector<Rect>            boundRect(contours.size() );

        cv::Mat element = cv::getStructuringElement(type, cv::Size( V.Morph.Size + 1, V.Morph.Size+1 ), cv::Point( V.Morph.Size, V.Morph.Size ) );
        cv::morphologyEx(img_canny_output, img_morph_out, cv::MORPH_CLOSE, element);
        cv::findContours(img_morph_out, contours, hierarchy,cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        good_contours = 0;
        info_total_contours = 0;

        //omp_set_num_threads(16);
        //#pragma omp parallel for schedule(dynamic)

        for(unsigned long i = 0; i < contours.size(); i++ ){
            cv::drawContours(img_wholemask, contours, i, cv::Scalar(255,255,255), cv::FILLED);

            //info_total_contours++;
            vector<Point2f> mc(contours.size());
            vector<Moments> mu(contours.size());
            vector<Point2f> centers( contours.size() );

            if ((contourArea(contours[i]) > V.Contours.MinBBoxArea)&&(contourArea(contours[i]) < V.Contours.MaxBBoxArea)){
                //useful_contours++;

                mu[i] = moments(contours[i], false);
                mc[i] = Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 );
                float X = mu[i].m10/mu[i].m00;
                float Y = mu[i].m01/mu[i].m00;

                Rect roi = boundingRect(contours[i]);
                long diameter = roi.width;

                img_in(roi).copyTo(img_roi, img_wholemask(roi));

                double PartOfColor = ((contourArea(contours[i])) / boundingRect(contours[i]).area());

                Scalar avg_color = Scalar(mean(img_roi)) / PartOfColor;

                int ColorsMatch;
                if (V.Color.GoodSpace == BGR){ColorsMatch = ScalarInRange(avg_color, V.Color.GoodRGB, V.Color.ToleranceRGB, 0);}
                else{ColorsMatch = ScalarInRange(BGR2HSV(avg_color), V.Color.GoodHSV, V.Color.ToleranceHSV, 1);}

                ColorOK = (ColorsMatch >= 3 );
                good_contours += ColorOK;

                Scalar contourcolor = Scalar(0, (0+255*ColorOK), 255-(255*ColorOK)) ;
                Scalar textcolor = Scalar(0, (0+255*!ColorOK), 255-(255*!ColorOK)) ;

                if (V.Show.FillAvg&&!V.Show.FilContour){cv::drawContours(img_output_temp, contours, i, avg_color, cv::FILLED);}
                if (V.Show.FilContour&&!V.Show.FillAvg){cv::drawContours(img_output_temp, contours, i, contourcolor, cv::FILLED);}
                if (V.Show.Contours){drawContours(img_output_temp, contours, (int)i, contourcolor, 2, LINE_4, hierarchy, 0);}
                //if (show_particle_number){sprintf(str,"%u", (int)i); putText(img_output_temp, str, Point(X,Y), FONT_HERSHEY_PLAIN  , 1.2, textcolor, 1);}
                if (V.Show.Area){ sprintf(str,"%.0f mm2", pix2mm((long)(contourArea(contours[i])))); putText(img_output_temp, str, Point(X,Y+14), FONT_HERSHEY_PLAIN  , 1.2, textcolor, 1);}
                if (V.Show.AvgColor){ sprintf(str,"%.0f;%.0f;%.0f", avg_color[2], avg_color[1],avg_color[0]); putText(img_output_temp, str, Point(X,Y+28), FONT_HERSHEY_PLAIN  , 1.2, textcolor, 1);}
                if (V.Show.Diameter){ sprintf(str,"d=%.1f mm", pix2mm(diameter)); putText(img_output_temp, str, Point(X,Y+42), FONT_HERSHEY_PLAIN  , 1.2, textcolor, 1);}
                if (V.Show.BBoxes){rectangle(img_output_temp,boundingRect(contours[i]).tl(), boundingRect(contours[i]).br(), Scalar(255,0,0), 1, 4, 0 );}
                if (V.Show.Centers){circle(img_output_temp, mc[i], 2, 255, -1, 8, 0 );}
            }
        }
        img_output_temp.copyTo(img_output);
}
else  Sleep(100);
}
Mat videofileFrame;

void Image_class::limitMatWinFPS(void){
    matLimiterCounter++;
    if (matLimiterCounter >= matLimiterTarget){
        for (int i=0; i<GUI.NR_CAT; i++){
            if ((GUI.setCats[i].matID != -1) && !GUI.setCats[i].mat->empty()){
                GUI.setCats[i].write.lock();
                    GUI.setCats[i].mat -> copyTo(GUI.setCats[i].matToShow);
                GUI.setCats[i].write.unlock();
            }
        }
    matLimiterCounter = 0;
    }
}


void Image_class::initProcessor(int newProcType){
    switch (newProcType){
        case PROC_B: {
                BeltProcessor.initDone = 0;
            break;}

        case PROC_WF: {

            break;}

        case PROC_CALIB: {
                //Calib.Init();
            break;}
    }
};

void Image_class::startProcessing(void){
    initProcessor(V.procType);
    procRun = 1;
};

void Image_class::stopProcessing(void){
    procRun = 0;
};

#include "calibration.h"

void Image_class::ImgProcessor(void){
    GUI.ConsoleOut(u8"ИЗОБРАЖЕНИЕ: Запуск процессора изображения");
    while (1){
        if (V.Input.CaptureRun){
            FPS_Routine();
            switch (V.Input.Source){
                case SOURCE_CAM: { // camera
                    Mat camFrame;
                    //if (!V.Input.FreezeFrame)
                        camera >> camFrame;
                    if (preprocessor.isOn) img_in = preprocessor.Result(camFrame);
                    else img_in = camFrame;
                    break;}

                case SOURCE_VIDEO: { // video
                    videoPlayer.fpsStart();
                    Mat videoFrame = videoPlayer.getFrame();
                    //std::cout << "frame get "<< std::endl:
                    if (preprocessor.isOn) img_in = preprocessor.Result(videoFrame);

                    else img_in = videoFrame;
                    //std::cout << "frame get "<< std::endl:
                    break;}
            }


            if (Calib.undistIsOn && Calib.dataReady) {
                    Mat matDistorted = img_in.clone();
                    cv::remap(matDistorted, img_in, Calib.calibData.map1, Calib.calibData.map2, INTER_LINEAR);
            }

            if (procRun){
                switch (V.procType){
                    case PROC_WF:   {WaterfallProcessor(img_in);    break;}
                    case PROC_B:    {BeltProcessor.Result(img_in);  break;}
                    case PROC_CALIB:{Calib.processFrame(img_in);    break;}
                }
            }

            limitMatWinFPS();
        }
        videoPlayer.fpsStop();
    }
}

void Image_class::RunProcessor(void){
    std::thread img_t(&Image_class::ImgProcessor, this);
    img_t.detach();
}

int ScalarInRange_old (Scalar input, Scalar compare_to, Scalar delta, bool useHSV){
    int to_return = 0;
    if (useHSV) {
        double Hoverrun = (Scalar(compare_to + delta).val[0] - 180.0);

        to_return +=    ((Hoverrun > 0.0) *(input.val[0] <= Hoverrun)) * (input.val[0] >= (compare_to - delta).val[0]) +
                        ((Hoverrun <= 0)*(input.val[0] <= (compare_to + delta).val[0])) * (input.val[0] >= (compare_to - delta).val[0]);
        for (int k = 1; k < 3; k++){
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
    else{
        for (int k = 0; k < 3; k++){
            if  (
                 ((compare_to - delta).val[k] <= input.val[k]) &&
                 ((compare_to + delta).val[k] >= input.val[k]) //||
                 //((-1.0*180.0 + (compare_to + delta).val[0]) >= input.val[0])
                )
            //{
            to_return++;
            //}
        }
    }
    return to_return;
}

inline bool ValueInRange(double value, double lower, double upper) {return((lower < value) && (value < upper));}
int Image_class::ScalarInRange (Scalar input, Scalar compare_to, Scalar delta, bool useHSV){
    int to_return = 0;
    // HSV compare
    if (useHSV) {
        double H_add_to_min = (Scalar(compare_to + delta).val[0] - 180.0);  //overlap at the end, adds to min
        double H_add_to_max = (180.0 + Scalar(compare_to - delta).val[0]);  //overlap at min, adds to max

        if (H_add_to_min >= 0){
            to_return+= ValueInRange(input.val[0], 0,                                 H_add_to_min) ||
                        ValueInRange(input.val[0], Scalar(compare_to - delta).val[0], 180.0);
        }
        else if (H_add_to_max <= 180){
            to_return+= ValueInRange(input.val[0], 0,                                 Scalar(compare_to + delta).val[0]) ||
                        ValueInRange(input.val[0], H_add_to_max,                      180.0);
        }

        else{
            to_return+= ValueInRange(input.val[0], Scalar(compare_to - delta).val[0], Scalar(compare_to + delta).val[0]);
        }


        for (int k = 1; k < 3; k++){
            if  (ValueInRange(input.val[k], (compare_to - delta).val[k], (compare_to + delta).val[k])){
                to_return++;
                }
        }
    }

    // RGB compare
    else {
        for (int k = 0; k < 3; k++){
            if  (ValueInRange(input.val[k], (compare_to - delta).val[k], (compare_to + delta).val[k]))
                to_return++;

    }
    }
return to_return;
}

cv::Scalar Image_class::BGR2HSV(cv::Scalar inBGR){
    cv::Mat tempBGR(1, 1, CV_8UC3, inBGR);
    cv::Mat tempHSV; //(1, 1, CV_8UC3, avg_color);
    cvtColor(tempBGR, tempHSV,COLOR_BGR2HSV);
    Vec3b hsvout=tempHSV.at<Vec3b>(0,0);
    return hsvout;
}

void Image_class::cameraOpen(void){
    camera.open(V.Cam.Number);
    if (camera.isOpened()){
        camera.set(cv::CAP_PROP_FOURCC ,cv::VideoWriter::fourcc('M', 'J', 'P', 'G') );
        //camera.set(cv::CAP_PROP_FPS, V.Cam.FPS);
        camera.set(cv::CAP_PROP_FRAME_WIDTH, V.Cam.Width);
        camera.set(cv::CAP_PROP_FRAME_HEIGHT, V.Cam.Height);
        //cameraUpdSettings();
        GUI.ConsoleOut(u8"ИЗОБРАЖЕНИЕ: Камера открыта");

        //std::cout << "CAP_PROP_CONTRAST" << camera.get(cv::CAP_PROP_CONTRAST) << std::endl;
        //std::cout << "CAP_PROP_GAIN" << camera.get(cv::CAP_PROP_GAIN) << std::endl;
        //std::cout << "CAP_PROP_BRIGHTNESS" << camera.get(cv::CAP_PROP_BRIGHTNESS) << std::endl;
        //std::cout << "CAP_PROP_SATURATION" << camera.get(cv::CAP_PROP_SATURATION) << std::endl;
        //std::cout << "CAP_PROP_EXPOSURE" << camera.get(cv::CAP_PROP_EXPOSURE) << std::endl;

        V.Input.CaptureRun = 1;
        camera >> img_in;
    }
    else{
         GUI.ConsoleOut(u8"ОШИБКА: Не могу открыть камеру!");
         GUI.popupError(u8"Ошибка открытия камеры");
    }
}

void Image_class::cameraClose(void){
    if (camera.isOpened()) camera.release();
    GUI.ConsoleOut(u8"ИЗОБРАЖЕНИЕ: Камера закрыта");
}

/*
void Image_class::videoClose(void){
    V.Input.CaptureRun = 0;
    videocapture.release();
}

void Image_class::videoOpen(std::string fileName){
    videocapture.open(fileName);
    if (videocapture.isOpened()) {
            GUI.ConsoleOut(u8"ИЗОБРАЖЕНИЕ: Видео открыто");
            V.Input.CaptureRun = 1;
    }
    else{
         GUI.ConsoleOut(u8"ОШИБКА: Не могу открыть видео!");
    }
}
*/


void Image_class::cameraUpdSettings(void){
    if (camera.isOpened()){
        camera.set(cv::CAP_PROP_CONTRAST , V.Cam.Contrast);
        camera.set(cv::CAP_PROP_GAIN, V.Cam.Gain);
        camera.set(cv::CAP_PROP_BRIGHTNESS ,V.Cam.Brightness);
        camera.set(cv::CAP_PROP_SATURATION ,V.Cam.Saturation);
        camera.set(cv::CAP_PROP_EXPOSURE ,V.Cam.Exposure);
        GUI.ConsoleOut(u8"ИЗОБРАЖЕНИЕ: Параметры камеры обновлены");
    }
    else{
        GUI.ConsoleOut(u8"ОШИБКА: Камера закрыта!");
        GUI.popupError(u8"Ошибка обновления параметров камеры\nЗначения не поддерживаются, или камера закрыта");

    }

}

/*
void Image_class::start_video_rec(void){
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

    if ( !oVideoWriter.isOpened() ) {
        GUI.ConsoleOut(u8"ОШИБКА: Невозможно открыть поток для записи");
   }
   else{GUI.ConsoleOut(u8"ЗАПИСЬ: Запись видео начата");
        video_recording = 1;}
}

void Image_class::stop_video_rec(void){
    if (oVideoWriter.isOpened() ){
    oVideoWriter.release();
    GUI.ConsoleOut(u8"ЗАПИСЬ: Запись видео остановлена");
    video_recording = 0;
   }
}

*/

void Image_class::startCapture(void){
    GUI.ConsoleOut(u8"ИЗОБРАЖЕНИЕ: Начинаю захват...");
    if (V.Input.Source == SOURCE_CAM)
        Img.cameraOpen();


    if (V.Input.Source == SOURCE_VIDEO) {
            if (videoPlayer.Start(Img.videoFileName, videoPlayer.startFrame, -1, -1) == RESULT_OK)
                V.Input.CaptureRun = 1;

            else {
                GUI.ConsoleOut(u8"ОШИБКА: Невозможно открыть файл");
                GUI.popupError(u8"Ошибка открытия файла!");
            }
    }


}

void Image_class::stopCapture(void){
    if (V.Input.CaptureRun){
        V.Input.CaptureRun = 0;

        if (V.Input.Source == SOURCE_CAM) Img.cameraClose();
        if (V.Input.Source == SOURCE_VIDEO){
                videoPlayer.Stop();
        }

        GUI.ConsoleOut(u8"ИЗОБРАЖЕНИЕ: Захват остановлен");

    }
}


#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#define CVUI_IMPLEMENTATION
#include "cvui.h"

#include "variables1.h"
#include "UI.h"
#include "main.h"
#include "rs232.h"
#include "COM_port.h"
#include "file.h"
#include "img.h"



//long info_total_contours;
//long useful_contours;

using namespace cv;

cv::Scalar SclHSV2BGR(Scalar HSV);
cv::Scalar SclBGR2HSV(Scalar BGR);

void CallBackFunc(int event, int x, int y, int flags, void* userdata);
int isWindowOpen(const cv::String &name);
void openWindow(const cv::String &name);
void closeWindow(const cv::String &name);
void ShowError(std::string message);
const long  window_width = 150;
const long  window_height = 500;
const int   window_number = 2;
int window_padding = 2;
int         padding;

bool small_GUI = 1;

bool err;
std::string err_text;
std::string random_string( size_t length );

cv::Point       CvuiMousePoint;
cv::Point       MousePoint;

cv::Mat frame;
cv::Mat error_frame;

long cat_window_width = 130;
long main_window_width = 250;
bool wide_area_range;

void save_image(cv::Mat input);

enum CatAlias {CAT_CAM, CAT_CUT, CAT_MOG, CAT_INPUT, CAT_EDGE, CAT_CONT, CAT_COLOR, CAT_OUT, CAT_COM_PORT, CAT_INFO, CAT_HARDWARE, CAT_FILES};
int show_category;// = CAT_INPUT;

void UI_init(void)
{

    if (small_GUI){frame = cv::Mat(window_height,(cat_window_width+main_window_width+window_padding*2),CV_8UC3);}
    else{frame = cv::Mat(window_height,(window_width+window_padding)*window_number,CV_8UC3);}
    error_frame = cv::Mat(100, 500, CV_8UC3);

    cv::namedWindow(WINDOW1_NAME);
    //cv::namedWindow(ERROR_WINDOW_NAME);

	cvui::init(WINDOW1_NAME);
	//cvui::watch(ERROR_WINDOW_NAME);

	int padding = 10;
	err = 0;
	show_category == CAT_CAM;
}

void UI_routine_new(void){

    cvui::context(WINDOW1_NAME);
	// Fill the frame with a nice color
    frame = cv::Scalar(49, 52, 49);
    cvui::beginRow(frame, 0, 0, 100, 50, window_padding);
	cvui::window(cat_window_width ,window_height,"CATEGORY");
	cvui::window(main_window_width ,window_height,"CONTENTS");
    cvui::endRow();

        // ======== ROWS ======
    int row_padding = 2;
    int window_current = 1;

    cvui::beginColumn(frame, (0+window_padding), 25, 100, 200, row_padding);
    if (cvui::button(cat_window_width-5, 25, "CAMERA")) {show_category = CAT_CAM;}
    //if (cvui::button(cat_window_width-5, 25, "INPUT")) {show_category = CAT_INPUT;}
    if (cvui::button(cat_window_width-5, 25, "CUT")) {show_category = CAT_CUT;}
    if (cvui::button(cat_window_width-5, 25, "BACK SUBSTRACT")) {show_category = CAT_MOG;}
    if (cvui::button(cat_window_width-5, 25, "EDGE DETECTION")) {show_category = CAT_EDGE;}
    if (cvui::button(cat_window_width-5, 25, "MASK")) {show_category = CAT_CONT;}
    if (cvui::button(cat_window_width-5, 25, "COLOR")) {show_category = CAT_COLOR;}
    if (cvui::button(cat_window_width-5, 25, "OUTPUT")) {show_category = CAT_OUT;}
    if (cvui::button(cat_window_width-5, 25, "COM PORT")) {show_category = CAT_COM_PORT;}
    if (cvui::button(cat_window_width-5, 25, "INFO")) {show_category = CAT_INFO;}
    if (cvui::button(cat_window_width-5, 25, "HARDWARE")) {show_category = CAT_HARDWARE;}
    //if (cvui::button(cat_window_width-5, 25, "LOAD / SAVE")) {show_category = CAT_FILES;}


cvui::endColumn();

if (cvui::button(frame, 2, window_height-55, cat_window_width-5, 25, "SAVE SETTINGS")) {SaveConfig("settings.ini");};
if (cvui::button(frame, 2, window_height-27, cat_window_width-5, 25, "EXIT")) {exit_program = 1;};

if (show_category == CAT_CAM){

        //camera
		cvui::beginColumn(frame, window_current*(cat_window_width+window_padding)+2, 25, 100, 200, row_padding);
		if (!capture_run) { cvui::text("capturing closed");
                            cvui::space(5);
                            if (cvui::button(main_window_width-5, 25, "open capture")) {cam_open();}
                          }
		else {  cvui::text("capturing running");
                cvui::space(5);
                if (cvui::button(main_window_width-5, 25, "show capture")) {show_frame = SHOW_INPUT;}
                if (cvui::button(main_window_width-5, 25, "close capture")) {cam_close();}
            }

        cvui::space(5);
        cvui::checkbox("freeze frame", &freeze_frame);
        cvui::checkbox("read test_file.avi", &capture_file);
        cvui::space(5);
        cvui::printf("camera #%d", cam_number);
        cvui::printf("width: %d pix", cam_width);
        cvui::printf("height: %d pix", cam_height);
		cvui::space(5);

		/*
		if (!capture_run) { cvui::text("gain");
                            cvui::trackbar(main_window_width-5, &cam_gain, (int)1, (int)255, 1, "%3.2Lf", cvui::TRACKBAR_DISCRETE, (int)1);}
		if (!capture_run) { cvui::text("exposure");
                            cvui::trackbar(main_window_width-5, &cam_contrast, (int)1, (int)255, 1, "%3.2Lf", cvui::TRACKBAR_DISCRETE, (int)1);}

        */
        cvui::text("gain");
        if (cvui::trackbar(main_window_width-5, &cam_gain, (int)0, (int)127, 1, "%3.2Lf", cvui::TRACKBAR_DISCRETE, (int)1)){
            cam_update();
        }

        cvui::text("contrast");
        if (cvui::trackbar(main_window_width-5, &cam_contrast, (int)0, (int)127, 1, "%3.2Lf", cvui::TRACKBAR_DISCRETE, (int)1)){
            cam_update();
        }

         cvui::text("brightness");
        if (cvui::trackbar(main_window_width-5, &cam_brightness, (int)1, (int)255, 1, "%3.2Lf", cvui::TRACKBAR_DISCRETE| cvui::TRACKBAR_HIDE_STEP_SCALE, (int)1)){
            cam_update();
        }

          cvui::text("exposure");
        if (cvui::trackbar(main_window_width-5, &cam_exposure, (int)-5, (int)0, 1, "%3.2Lf", cvui::TRACKBAR_DISCRETE, (int)1)){
            cam_update();
        }

        cvui::text("saturation");
        if (cvui::trackbar(main_window_width-5, &cam_saturation, (int)0, (int)127, 1, "%3.2Lf", cvui::TRACKBAR_DISCRETE, (int)1)){
            cam_update();
        }


		cvui::endColumn();

}

if (show_category == CAT_CUT){
 cvui::beginColumn(frame, window_current*(cat_window_width+window_padding)+2, 25, 100, 200, row_padding);
 cvui::text("cut up");
 (cvui::trackbar(main_window_width-5, &cut_up, 0.0, (double)cam_height/2.0-1.0, 1, "%.0Lf", cvui::TRACKBAR_DISCRETE| cvui::TRACKBAR_HIDE_STEP_SCALE, 1.0));
  cvui::text("cut down");
 (cvui::trackbar(main_window_width-5, &cut_down,0.0, (double)cam_height/2.0-1.0, 1, "%.0Lf", cvui::TRACKBAR_DISCRETE| cvui::TRACKBAR_HIDE_STEP_SCALE, 1.0));
  cvui::text("cut left");
 (cvui::trackbar(main_window_width-5, &cut_left, 0.0, (double)cam_width/2.0-1.0, 1, "%.0Lf", cvui::TRACKBAR_DISCRETE| cvui::TRACKBAR_HIDE_STEP_SCALE, 1.0));
  cvui::text("cut right");
 (cvui::trackbar(main_window_width-5, &cut_right, 0.0, (double)cam_width/2.0-1.0, 1, "%.0Lf", cvui::TRACKBAR_DISCRETE| cvui::TRACKBAR_HIDE_STEP_SCALE, 1.0));

 cvui::endColumn();

}

if (show_category == CAT_INPUT){

        //input window

		cvui::beginColumn(frame, window_current*(cat_window_width+window_padding)+2, 25, 100, 200, row_padding);

        if (cvui::button(main_window_width-5, 25, "show input")) {show_frame = SHOW_INPUT;}

		cvui::space(5);
		if (!capture_run) {if (cvui::button(main_window_width-5, 25, "start capture")) {capture_run = 1;};}
		if (capture_run) {if (cvui::button(main_window_width-5, 25, "stop capture")) {capture_run = 0; show_frame = SHOW_NOTHING;};}

		cvui::space(20);
        cvui::checkbox("HSV input correction", &input_correction_on);
		cvui::trackbar(main_window_width-5, &hsv_input_correction.val[0], -90., 90., 1, "%3.2Lf", cvui::TRACKBAR_DISCRETE, 0.1);
		cvui::trackbar(main_window_width-5, &hsv_input_correction.val[1], -127., 127., 1, "%3.2Lf", cvui::TRACKBAR_DISCRETE| cvui::TRACKBAR_HIDE_STEP_SCALE , 0.1);
		cvui::trackbar(main_window_width-5, &hsv_input_correction.val[2], -127., 127., 1, "%3.2Lf", cvui::TRACKBAR_DISCRETE| cvui::TRACKBAR_HIDE_STEP_SCALE , 0.1);
		if (cvui::button(main_window_width-5, 25, "reset corr values")) {
            hsv_input_correction.val[0] = 0.;
            hsv_input_correction.val[1] = 0;
            hsv_input_correction.val[2] = 0;
		};
		//cvui::printf(0.35, 0xcccccc, "   HSV: %3.02lf,%3.02lf,%3.02lf", test[0], test[1], test[2]);
		cvui::endColumn();

}

if (show_category == CAT_MOG){

		cvui::beginColumn(frame, window_current*(cat_window_width+window_padding)+2, 25, 100, 200, row_padding);
		//if (!capture_run) {if (cvui::button(main_window_width-5, 25, "start MOG")) {show_frame = SHOW_INPUT;}}
		//else {if (cvui::button(main_window_width-5, 25, "stop MOG")) {show_frame = SHOW_INPUT;}}
		cvui::text("back subtract settings:");
		cvui::space(5);
        cvui::checkbox("background learning", &bs_mog_learning);
        //cvui::space(5);
		cvui::text("history");
        cvui::trackbar(main_window_width-5, &bs_mog_history, (int)1, (int)250, 1, "%.0Lf", cvui::TRACKBAR_DISCRETE| cvui::TRACKBAR_HIDE_STEP_SCALE, (int)1);
		cvui::text("mixtures");
        cvui::trackbar(main_window_width-5, &bs_mog_mixtures, (int)1, (int)10, 1, "%.0Lf", cvui::TRACKBAR_DISCRETE, (int)1);
		cvui::text("learning rate");
        //cvui::trackbar(main_window_width-5, &mog_backratio, 0.0001, 0.99, 1, "%3.2Lf", cvui::TRACKBAR_DISCRETE | cvui::TRACKBAR_HIDE_STEP_SCALE , 0.01);
		cvui::text("noise sigma");
        //cvui::trackbar(main_window_width-5, &mog_noisesigma, 0.0, 0.99, 1, "%3.2Lf", cvui::TRACKBAR_DISCRETE | cvui::TRACKBAR_HIDE_STEP_SCALE, 0.01);

		cvui::space(5);
		if (cvui::button(main_window_width-5, 25, "apply settings")) { }

		cvui::endColumn();

}


if (show_category == CAT_EDGE){
        //canny filter
        //window_current++;
		cvui::beginColumn(frame, window_current*(cat_window_width+window_padding)+2, 25, 100, 200, row_padding);
		if (cvui::button(main_window_width-5, 25, "show canny result")) {show_frame = SHOW_CANNY;};
		cvui::space(5);
		cvui::text("blur value");
		cvui::trackbar(main_window_width-5, &canny_blur_value, 1, 5, 1, "%3.0Lf", cvui::TRACKBAR_DISCRETE||cvui::TRACKBAR_HIDE_STEP_SCALE, 1);
        cvui::text("canny threshold");
		//cvui::trackbar(main_window_width-5, &canny_thresh_value, (long)5, (long)500, 1, "%.0Lf", cvui::TRACKBAR_DISCRETE | cvui::TRACKBAR_HIDE_STEP_SCALE, (long)1);
		cvui::checkbox("use scharr filter", &use_scharr);
		//cvui::printf(0.35, 0xcccccc, "   HSV: %3.02lf,%3.02lf,%3.02lf", test[0], test[1], test[2]);
		//cvui::endColumn();



        //scharr filter
        //window_current++;
		//cvui::beginColumn(frame, window_current*(main_window_width+window_padding)+2, 25, 100, 200, row_padding);
		//cvui::space(5);
		//if (cvui::button(main_window_width-5, 25, "show scharr result")) {show_frame = SHOW_SCHARR;};
		//cvui::space(20);
		//cvui::checkbox("use scharr filter", &use_scharr);
		//cvui::text("threshold");
		//cvui::trackbar(main_window_width-5, &scharr_thresh_value, (long)5, (long)500, 1, "%.0Lf", cvui::TRACKBAR_DISCRETE | cvui::TRACKBAR_HIDE_STEP_SCALE, (long)1);
		//cvui::printf(0.35, 0xcccccc, "   HSV: %3.02lf,%3.02lf,%3.02lf", test[0], test[1], test[2]);
		cvui::endColumn();
}


if (show_category == CAT_CONT){
        //contours
        //window_current++;
		cvui::beginColumn(frame, window_current*(cat_window_width+window_padding)+2, 25, 100, 200, row_padding);
		if (cvui::button(main_window_width-5, 25, "show mask")) {show_frame = SHOW_MASK;};
		//if (cvui::button(main_window_width-5, 25, "show masked image")) {show_frame = SHOW_MASKED;};
		cvui::checkbox("wide range", &wide_area_range);

		cvui::text("min bbox area");
		//cvui::trackbar(main_window_width-5, &min_bbox_area, 0.0, (100.0 + 1000.0 * wide_area_range) , 1, "%.0Lf", cvui::TRACKBAR_DISCRETE| cvui::TRACKBAR_HIDE_STEP_SCALE, 1.0);
		cvui::text("max bbox area");
		//cvui::trackbar(main_window_width-5, &max_bbox_area, min_bbox_area+1.0, (10000.0 + 90000.0 * wide_area_range), 1.0, "%.0Lf", cvui::TRACKBAR_DISCRETE| cvui::TRACKBAR_HIDE_STEP_SCALE, 1.0);

        cvui::text("morph size");
		cvui::trackbar(main_window_width-5, &morph_size, 0, 20, 1, "%.0Lf", cvui::TRACKBAR_DISCRETE, 1);

		//cvui::space(20);
		//cvui::checkbox("morph mask", &morph_mask);

		//cvui::printf(0.35, 0xcccccc, "   HSV: %3.02lf,%3.02lf,%3.02lf", test[0], test[1], test[2]);
		cvui::endColumn();
}

if (show_category == CAT_COLOR){
        //good color
        //window_current++;
		cvui::beginColumn(frame, window_current*(cat_window_width+window_padding)+2, 25, 100, 200, row_padding);
		//if (cvui::button(main_window_width-5, 25, "sample good color")) {err = 1; openWindow(ERROR_WINDOW_NAME);};

		double hex_goodcolor;
		if (good_colorspace==HSV){
		    good_color_rgb = SclHSV2BGR(good_color_hsv);
            hex_goodcolor = ((long)good_color_rgb.val[2]*65536) + ((long)good_color_rgb.val[1]*256) + ((long)good_color_rgb.val[0]);
        }
        else{
            good_color_hsv = SclBGR2HSV(good_color_rgb);
            hex_goodcolor = ((long)good_color_rgb.val[2]*65536) + ((long)good_color_rgb.val[1]*256) + ((long)good_color_rgb.val[0]);
        }

		cvui::rect(main_window_width-5, 50, 0x727272, hex_goodcolor);
		cvui::checkbox("use HSV colorspace", &good_colorspace);
		cvui::space(5);
		if (good_colorspace==HSV){
            cvui::text("HSV good color:");
            cvui::trackbar(main_window_width-5, &good_color_hsv.val[0], 0., 180., 1, "%3.2Lf", cvui::TRACKBAR_DISCRETE| cvui::TRACKBAR_HIDE_STEP_SCALE, 0.5);
            cvui::trackbar(main_window_width-5, &good_color_hsv.val[1], 0., 255., 1, "%3.2Lf", cvui::TRACKBAR_DISCRETE| cvui::TRACKBAR_HIDE_STEP_SCALE, 0.5);
            cvui::trackbar(main_window_width-5, &good_color_hsv.val[2], 0., 255., 1, "%3.2Lf", cvui::TRACKBAR_DISCRETE| cvui::TRACKBAR_HIDE_STEP_SCALE, 0.5);
		}
		else{
            cvui::text("RGB good color:");
            cvui::trackbar(main_window_width-5, &good_color_rgb.val[2], 0., 255., 1, "%3.2Lf", cvui::TRACKBAR_DISCRETE| cvui::TRACKBAR_HIDE_STEP_SCALE, 0.5);
            cvui::trackbar(main_window_width-5, &good_color_rgb.val[1], 0., 255., 1, "%3.2Lf", cvui::TRACKBAR_DISCRETE| cvui::TRACKBAR_HIDE_STEP_SCALE, 0.5);
            cvui::trackbar(main_window_width-5, &good_color_rgb.val[0], 0., 255., 1, "%3.2Lf", cvui::TRACKBAR_DISCRETE| cvui::TRACKBAR_HIDE_STEP_SCALE, 0.5);
		}
        //cvui::endColumn();

        //color tolerance
        //window_current++;
		//cvui::beginColumn(frame, window_current*(window_width+window_padding)+2, 25, 100, 200, row_padding);
		//if (cvui::button(main_window_width-5, 25, "sample good color")) {/**/};
		//cvui::rect(main_window_width-5, 50, 0x00ff00, 0xff0000);
		cvui::space(5);
		if (good_colorspace==BGR){
            cvui::text("RGB color tolerance:");
            cvui::trackbar(main_window_width-5, &color_tolerance_rgb.val[2], 0., 255., 1, "%3.2Lf", cvui::TRACKBAR_DISCRETE| cvui::TRACKBAR_HIDE_STEP_SCALE, 0.25);
            cvui::trackbar(main_window_width-5, &color_tolerance_rgb.val[1], 0., 255., 1, "%3.2Lf", cvui::TRACKBAR_DISCRETE| cvui::TRACKBAR_HIDE_STEP_SCALE, 0.25);
            cvui::trackbar(main_window_width-5, &color_tolerance_rgb.val[0], 0., 255., 1, "%3.2Lf", cvui::TRACKBAR_DISCRETE| cvui::TRACKBAR_HIDE_STEP_SCALE, 0.25);
		}
		else{
            cvui::text("HSV color tolerance:");
            cvui::trackbar(main_window_width-5, &color_tolerance_hsv.val[0], 0., 180., 1, "%3.2Lf", cvui::TRACKBAR_DISCRETE| cvui::TRACKBAR_HIDE_STEP_SCALE, 0.25);
            cvui::trackbar(main_window_width-5, &color_tolerance_hsv.val[1], 0., 255., 1, "%3.2Lf", cvui::TRACKBAR_DISCRETE| cvui::TRACKBAR_HIDE_STEP_SCALE, 0.25);
            cvui::trackbar(main_window_width-5, &color_tolerance_hsv.val[2], 0., 255., 1, "%3.2Lf", cvui::TRACKBAR_DISCRETE, 0.25);
		}
        cvui::endColumn();
}


if (show_category == CAT_OUT){
        //output
        //window_current++;
		cvui::beginColumn(frame, window_current*(cat_window_width+window_padding)+2, 25, 100, 200, row_padding);
		if (cvui::button(main_window_width-5, 25, "show output")) {show_frame = SHOW_OUTPUT;};
		if (cvui::button(main_window_width-5, 25, "show quad")) {show_frame = SHOW_QUAD;};

		if (cvui::button(main_window_width-5, 25, "save screenshot")) {save_image(img_output); };
		if (!video_recording) {if (cvui::button(main_window_width-5, 25, "record video")) {start_video_rec(); }}
		else {if (cvui::button(main_window_width-5, 25, "stop recording video")) {stop_video_rec(); }}
		cvui::text("show:");
		cvui::checkbox("contours", &show_contours);
		cvui::checkbox("centers", &show_centers);
		cvui::checkbox("bounding boxes", &show_bboxes);
		cvui::checkbox("mean color", &show_avgcolor);
		cvui::checkbox("area", &show_area);
		cvui::checkbox("diameter", &show_diameter);
		if (!show_fill_avgcolor) {cvui::checkbox("fill w/ contour color", &show_fill_contcolor);}
		if (!show_fill_contcolor) {cvui::checkbox("fill w/ avg color", &show_fill_avgcolor);}
		cvui::space(10);
		cvui::checkbox("draw ruler", &draw_ruler);
		//cvui::checkbox("fill w/ avg color", &show_fill_avgcolor);
		//cvui::checkbox("fill w/ contour color", &show_fill_contcolor);

		cvui::checkbox("show quad output", &show_quad);

		cvui::endColumn();

		}

if (show_category == CAT_COM_PORT){
		//com port
        //window_current++;
		cvui::beginColumn(frame, window_current*(cat_window_width+window_padding)+2, 25, 100, 200, row_padding);

		if (com_connected) {cvui::text("status: connected");}
		else {cvui::text("status: disconnected");}

		cvui::space(5);

		if (com_connected == 0) {if (cvui::button(main_window_width-5, 25, "list COM ports")) {list_COM();
		//err = 0;
		//closeWindow(ERROR_WINDOW_NAME);}
		}}
		cvui::space(10+30*com_connected);

        cvui::text("available ports:");
		for (int k = 0; k < 16; k++){
            if (COM_present[k]){
                cvui::printf(0.4, 0xcccccc,"#%d: COM%d", k, (k+1));
            }
        }

        cvui::space(5);
        cvui::text("port number:");
		cvui::counter(&com_port_number);
		cvui::printf(0.35, 0xcccccc,"COM speed: %.0f", (double)com_speed);

        if (com_connected) {if (cvui::button(main_window_width-5, 25, "disconnect")) {close_COM(com_port_number);};}
		else {if (cvui::button(main_window_width-5, 25, "connect")) {open_COM(com_port_number); handshake_COM();};};
		if (com_connected) {if (cvui::button(main_window_width-5, 25, "manual handshake")) {handshake_COM();};}

		cvui::endColumn();
}

if (show_category == CAT_INFO){
		//info
        //window_current++;
		cvui::beginColumn(frame, window_current*(cat_window_width+window_padding)+2, 25, 100, 200, row_padding);
		cvui::printf("ms/frame: %0.5f", info_ms_frame);
		cvui::printf("average FPS: %.0f", info_fps);
		cvui::space(20);
		cvui::printf("contours: %d", info_total_contours);
		cvui::printf("useful contours: %d", useful_contours);
		cvui::printf("good contours: %d", good_contours);
		info_total_contours = 0;
		useful_contours = 0;
		cvui::space(20);
		cvui::printf("pixels per 100mm: %d", pix_per_100mm);
		cvui::endColumn();
}

if (show_category == CAT_HARDWARE){
		//hardware
        //window_current++;
		cvui::beginColumn(frame, window_current*(cat_window_width+window_padding)+2, 25, 100, 200, row_padding);
		cvui::checkbox("front light", &show_contours);
		cvui::trackbar(main_window_width-5, &color_tolerance_hsv.val[0], 0., 90., 1, "%3.2Lf", cvui::TRACKBAR_DISCRETE| cvui::TRACKBAR_HIDE_STEP_SCALE, 0.25);
		cvui::checkbox("back light", &show_contours);
		cvui::trackbar(main_window_width-5, &color_tolerance_hsv.val[0], 0., 90., 1, "%3.2Lf", cvui::TRACKBAR_DISCRETE| cvui::TRACKBAR_HIDE_STEP_SCALE, 0.25);
		cvui::space(20);
		cvui::checkbox("motor", &show_contours);
		cvui::trackbar(main_window_width-5, &color_tolerance_hsv.val[0], 0., 90., 1, "%3.2Lf", cvui::TRACKBAR_DISCRETE| cvui::TRACKBAR_HIDE_STEP_SCALE, 0.25);
        cvui::space(20);
        if (cvui::button(main_window_width-5, 25, "check valves")) {/**/};

		cvui::endColumn();

}

int cfg_file_number;

if (show_category == CAT_FILES){
        cvui::beginColumn(frame, window_current*(cat_window_width+window_padding)+2, 25, 100, 200, row_padding);
        cvui::text("config file number:");
        cvui::counter(&cfg_file_number);
        //if (cvui::button(main_window_width-5, 25, "check settings")) {CfgCheck();};
        if (cvui::button(main_window_width-5, 25, "load settings")) {CfgLoad(cfg_file_number);};
        if (cvui::button(main_window_width-5, 25, "save settings")) {CfgSave(cfg_file_number);};
		cvui::endColumn();
}

		if (cvui::mouse(cvui::DOWN)) {
			// Position the anchor at the mouse pointer.
			CvuiMousePoint.x = cvui::mouse().x;
			CvuiMousePoint.y = cvui::mouse().y;
            std::cout << "x: " << CvuiMousePoint.x << " y: " << CvuiMousePoint.y << std::endl;
		}


		if (err && isWindowOpen(ERROR_WINDOW_NAME)) {
			// Inform cvui that all subsequent component calls and events are
			// related to the error window from now on
			cvui::context(ERROR_WINDOW_NAME);

			// Fill the error window if a vibrant color
			error_frame = cv::Scalar(10, 10, 49);

			cvui::text(error_frame, 70, 20, err_text, 0.5, 0xff0000);

			if (cvui::button(error_frame, 220, 50, "OK")) {
				err = false;
			}

			if (err) {
				// We still have an active error.
				// Update all components of the error window, e.g. mouse clicks, and show it.
				cvui::update(ERROR_WINDOW_NAME);
				cv::imshow(ERROR_WINDOW_NAME, error_frame);
			} else {
				// No more active error. Let's close the error window.
				closeWindow(ERROR_WINDOW_NAME);
			}
		}

        if (show_GUI){
        // This function must be called *AFTER* all UI components. It does
		// all the behind the scenes magic to handle mouse clicks, etc.

		cvui::update(WINDOW1_NAME);

		// Show everything on the screen
		cv::imshow(WINDOW1_NAME, frame);
        }
}

void CallBackFunc(int event, int x, int y, int flags, void* userdata){
    if (event == (EVENT_LBUTTONDOWN)){ //&& (event == EVENT_MOUSEMOVE ) ) {
        MousePoint.x = x;
        MousePoint.y = y;
        std::cout << "x: " << MousePoint.x << " y: " << MousePoint.y << std::endl;

        Vec3b intensity = img_output.at<Vec3b>(MousePoint.y, MousePoint.x);
        uchar blue = intensity.val[0];
        uchar green = intensity.val[1];
        uchar red = intensity.val[2];
        std::cout << intensity << std::endl;

        good_color_rgb = intensity;
        good_color_hsv = SclBGR2HSV(good_color_rgb);
    }
}

Scalar SclHSV2BGR(Scalar HSV){
    Mat bgr;
    Mat hsv(1,1, CV_8UC3, HSV);
    cvtColor(hsv, bgr, COLOR_HSV2BGR);
    return Scalar(bgr.data[0], bgr.data[1], bgr.data[2]);
}

Scalar SclBGR2HSV(Scalar BGR){
    Mat bgr(1,1, CV_8UC3, BGR);
    Mat hsv;
    cvtColor(bgr, hsv, COLOR_BGR2HSV);
    return Scalar(hsv.data[0], hsv.data[1], hsv.data[2]);
}

int isWindowOpen(const cv::String &name) {
	return cv::getWindowProperty(name, cv::WND_PROP_AUTOSIZE) != -1;
}

void openWindow(const cv::String &name) {
	cv::namedWindow(name);
	cvui::watch(name);
}

void closeWindow(const cv::String &name) {
	cv::destroyWindow(name);
	cv::waitKey(1);
}

void ShowError(std::string message){
    err_text = message;
    err = 1;
    openWindow(ERROR_WINDOW_NAME);
}

void save_image(cv::Mat input){
    time_t now;
    char the_date[32];
    the_date[0] = '\0';
    now = time(NULL);
    strftime(the_date, 32, "%d-%m-%Y %H-%M-%S", gmtime(&now));
    char buf[64];
    sprintf(buf, "save/%s.png", the_date);
    imwrite(buf,input);
}

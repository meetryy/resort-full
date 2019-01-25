#include <iostream>
#include <fstream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "variables1.h"
#include "file.h"
#include "main.h"
#include "UI.h"

using namespace cv;
using namespace std;

void ReadConfig(void);
int ProcessLineLong(string str, long *parameter, string paramname);
int ProcessLineBool(string str, bool *parameter, string paramname);
int ProcessLineScalar(string str, Scalar *parameter, string paramname);
int ProcessLineInt(string str, int *parameter, string paramname);
int ProcessLineDouble(string str, double *parameter, string paramname);
int ProcessLineFloat(string str, float *parameter, string paramname);
void LineList(void);

bool ParseSuccess = 0;

enum ResultAlias {RESULT_OK, RESULT_FAIL};


int nthOccurrence(const std::string& str, const std::string& findMe, int nth)
{
    size_t  pos = 0;
    int     cnt = 0;

    while( cnt != nth )
    {
        pos+=1;
        pos = str.find(findMe, pos);
        if ( pos == std::string::npos )
            return -1;
        cnt++;
    }
    return pos;
}

template <typename T>
void ProcessLine(string input, T&value, string paramname){
    if (std::is_same<T, float>::value) {ProcessLineFloat(input, &value, paramname);}
    else if (std::is_same<T, int>::value) {ProcessLineInt(input, &value, paramname);}
    else if (std::is_same<T, bool>::value) {ProcessLineBool(input, &value, paramname);}
    else if (std::is_same<T, double>::value)  {ProcessLineDouble(input, &value, paramname);}
    else if (std::is_same<T, long>::value)  {ProcessLineLong(input, &value, paramname);}
    else if (std::is_same<T, cv::Scalar>::value) {ProcessLineScalar(input, &value, paramname);}
}

void ReadConfig(string filename){
    ifstream in;            // Create an input file stream.
    in.open(filename);
    if (!in) {
        cout << "settings file not found\n";
        //ShowError("settings file not found!");
        //err = 1; //openWindow(ERROR_WINDOW_NAME);
    }
    //else {closeWindow(ERROR_WINDOW_NAME);}
    string str;
    getline(in, str);  // Get the frist line from the file, if any.
    while ( in ) {  // Continue if the line was sucessfully read.
        if (str.find("#") == string::npos){
            //input
            //ProcessLineBool(str, &capture_run, "capture_run");
            //ProcessLineBool(str, &input_correction_on, "input_correction_on");
            //ProcessLineScalar(str, &hsv_input_correction, "hsv_input_correction");

            //ProcessLineDouble(str, &cut_up, "cut_up");
            //ProcessLineDouble(str, &cut_down, "cut_down");
            //ProcessLineDouble(str, &cut_left, "cut_left");
            //ProcessLineDouble(str, &cut_right, "cut_right");
            ProcessLineFloat(str, &cam_fps, "cam_fps");

            //edge detection
            ProcessLineInt(str, &canny_blur_value, "canny_blur_value");
            ProcessLineInt(str, &canny_thresh_value1, "canny_thresh_value1");
            ProcessLineInt(str, &canny_thresh_value2, "canny_thresh_value2");
            ProcessLineBool(str, &use_scharr, "use_scharr");
            ProcessLineLong(str, &scharr_thresh_value, "scharr_thresh_value");
            ProcessLineInt(str, &contour_current_algo, "contour_current_algo");

            //contours
            ProcessLineFloat(str, &min_bbox_area, "min_bbox_area");
            ProcessLineFloat(str, &max_bbox_area, "max_bbox_area");
            ProcessLineBool(str, &morph_mask, "morph_mask");
            ProcessLineInt(str, &morph_size, "morph_size");


            //color
            ProcessLineScalar(str, &good_color_rgb, "good_color_rgb");
            ProcessLineScalar(str, &color_tolerance_rgb, "color_tolerance_rgb");
            ProcessLineScalar(str, &good_color_hsv, "good_color_hsv");
            ProcessLineScalar(str, &color_tolerance_hsv, "color_tolerance_hsv");
            ProcessLineBool(str, &good_colorspace, "good_colorspace");

            //output
            ProcessLineBool(str, &show_contours, "show_contours");
            ProcessLineBool(str, &show_centers, "show_centers");
            ProcessLineBool(str, &show_bboxes, "show_bboxes");
            ProcessLineBool(str, &show_avgcolor, "show_avgcolor");
            ProcessLineBool(str, &show_area, "show_area");
            ProcessLineBool(str, &show_fill_avgcolor, "show_fill_avgcolor");
            ProcessLineBool(str, &show_fill_contcolor, "show_fill_contcolor");

            //com port
            ProcessLineBool(str, &com_connected, "com_connected");
            ProcessLineInt(str, &com_port_number, "com_port_number");
            ProcessLineLong(str, &com_speed, "com_speed");

            //mog
            ProcessLineInt(str, &bs_mog_history, "bs_mog_history");
            ProcessLineInt(str, &bs_mog_mixtures, "bs_mog_mixtures");
            ProcessLineFloat(str, &bs_mog_backratio, "bs_mog_backratio");
            ProcessLineFloat(str, &bs_mog_noisesigma, "bs_mog_noisesigma");
            ProcessLineFloat(str, &bs_mog_lrate, "bs_mog_lrate");
            ProcessLineBool(str, &bs_mog_learning, "bs_mog_learning");

            //mog2
            ProcessLineInt(str, &bs_mog2_history, "bs_mog2_history");
            ProcessLineFloat(str, &bs_mog2_thresh, "bs_mog2_thresh");
            ProcessLineBool(str, &bs_mog2_shadows, "bs_mog2_shadows");
            ProcessLineBool(str, &bs_mog2_learning, "bs_mog2_learning");

            //knn
            ProcessLineInt(str, &bs_knn_history, "bs_knn_history");
            ProcessLineFloat(str, &bs_knn_thresh, "bs_knn_thresh");
            ProcessLineBool(str, &bs_knn_shadows, "bs_knn_shadows");
            ProcessLineBool(str, &bs_knn_learning, "bs_knn_learning");

            //cnt
            ProcessLineInt(str, &bs_cnt_min_pix_stability, "bs_cnt_min_pix_stability");
            ProcessLineInt(str, &bs_cnt_max_pix_stability, "bs_cnt_max_pix_stability");
            ProcessLineBool(str, &bs_cnt_use_history, "bs_cnt_use_history");
            ProcessLineBool(str, &bs_cnt_isparallel, "bs_cnt_isparallel");
            ProcessLineInt(str, &bs_cnt_fps, "bs_cnt_fps");

            //gsoc
            ProcessLineInt(str, &bs_gsoc_mc, "bs_gsoc_mc");
            ProcessLineInt(str, &bs_gsoc_samples, "bs_gsoc_samples");
            ProcessLineFloat(str, &bs_gsoc_reprate, "bs_gsoc_reprate");
            ProcessLineFloat(str, &bs_gsoc_proprate, "bs_gsoc_proprate");
            ProcessLineInt(str, &bs_gsoc_hits_thresh, "bs_gsoc_hits_thresh");
            ProcessLineFloat(str, &bs_gsoc_alpha, "bs_gsoc_alpha");
            ProcessLineFloat(str, &bs_gsoc_beta, "bs_gsoc_beta");
            ProcessLineFloat(str, &bs_gsoc_bs_decay, "bs_gsoc_bs_decay");
            ProcessLineFloat(str, &bs_gsoc_bs_mul, "bs_gsoc_bs_mul");
            ProcessLineFloat(str, &bs_gsoc_noise_bg, "bs_gsoc_noise_bg");
            ProcessLineFloat(str, &bs_gsoc_noise_fg, "bs_gsoc_noise_fg");
            ProcessLineBool(str, &bs_gsoc_learning, "bs_gsoc_learning");

            //com camera
            ProcessLineInt(str, &cam_number, "cam_number");
            ProcessLineLong(str, &cam_width, "cam_width");
            ProcessLineLong(str, &cam_height, "cam_height");
            ProcessLineInt(str, &cam_gain, "cam_gain");
            ProcessLineInt(str, &cam_contrast, "cam_contrast");
            ProcessLineInt(str, &cam_brightness, "cam_brightness");
            ProcessLineInt(str, &cam_exposure, "cam_exposure");
            ProcessLineInt(str, &cam_saturation, "cam_saturation");
            ProcessLineInt(str, &cam_hue, "cam_hue");

            //misc
            ProcessLineLong(str, &pix_per_100mm, "pix_per_100mm");
            ProcessLineInt(str, &morpho_type, "morpho_type");
            ProcessLineInt(str, &contour_current_algo, "contour_current_algo");
            ProcessLineInt(str, &bs_current_algo, "bs_current_algo");
            ProcessLineInt(str, &blur_before_mog, "blur_before_mog");
            ProcessLineBool(str, &fullscreen, "fullscreen");

        }
        getline(in, str);   // Try to get another line.
    }
}

int ProcessLineLong(string str, long *parameter, string paramname){
    int to_return = RESULT_FAIL;
    int position = str.find(paramname);         //find a variable name
    if (position != string::npos) {             //if exist
        string value = str.substr(str.find("=")+1, str.find(";"));  //save value as string
        std::string::size_type sz;     // alias of size_t
        *parameter = std::stoi (value,&sz);  //string to int
        //cout << paramname << " = " << *parameter << endl;
        to_return = RESULT_OK;
        //cout << paramname << " found in line" << endl;
    }
    //else {cout << paramname << " NOT found in line" << endl;}
    return to_return;
}

int ProcessLineFloat(string str, float *parameter, string paramname){
    int to_return = RESULT_FAIL;
    int position = str.find(paramname);         //find a variable name
    if (position != string::npos) {             //if exist
        string value = str.substr(str.find("=")+1, str.find(";"));  //save value as string
        std::string::size_type sz;     // alias of size_t
        *parameter = std::stof (value,&sz);  //string to int
        //cout << paramname << " = " << *parameter << endl;
        to_return = RESULT_OK;
        //cout << paramname << " found in line" << endl;
    }
    //else {cout << paramname << " NOT found in line" << endl;}
    return to_return;
}

int ProcessLineDouble(string str, double *parameter, string paramname){
    int to_return = RESULT_FAIL;
    int position = str.find(paramname);         //find a variable name
    if (position != string::npos) {             //if exist
        string value = str.substr(str.find("=")+1, str.find(";"));  //save value as string
        std::string::size_type sz;     // alias of size_t
        *parameter = std::stof (value,&sz);  //string to int
        //cout << paramname << " = " << *parameter << endl;
        to_return = RESULT_OK;
        //cout << paramname << " found in line" << endl;
    }
    //else {cout << paramname << " NOT found in line" << endl;}
    return to_return;
}

int ProcessLineInt(string str, int *parameter, string paramname){
    int to_return = RESULT_FAIL;
    int position = str.find(paramname);         //find a variable name
    if (position != string::npos) {             //if exist
        string value = str.substr(str.find("=")+1, str.find(";"));  //save value as string
        std::string::size_type sz;     // alias of size_t
        *parameter = std::stoi (value,&sz);  //string to int
        //cout << paramname << " = " << *parameter << endl;
        to_return = RESULT_OK;
    }
    return to_return;
}

int ProcessLineBool(string str, bool *parameter, string paramname){
    int to_return = RESULT_FAIL;
    int position = str.find(paramname);         //find a variable name
    if (position != string::npos) {             //if exist
        string value = str.substr(str.find("=")+1, str.find(";"));  //save value as string
        //cout << "bool value = " << value << endl;
        std::string::size_type sz;     // alias of size_t
        *parameter = std::stoi (value,nullptr,2);;  //string to int
        //cout << paramname << " = " << *parameter << endl;
        //cout << paramname << " FOUND in line" << endl;
        to_return = RESULT_OK;
    }
    //else{cout << paramname << " NOT found in line" << endl;}
    return to_return;
}

int ProcessLineScalar(string str, Scalar *parameter, string paramname){
    int to_return = RESULT_FAIL;
    //hsv_input_correction=(50.0,100.0,150.0);
    int position = str.find(paramname);         //find a variable name
    if (position != string::npos) {             //if exist
        string value = str.substr(str.find("=")+1, str.find(";"));  //save value as string

        int comma1 = nthOccurrence(value, ",", 1);
        int comma2 = nthOccurrence(value, ",", 2);
        int comma3 = nthOccurrence(value, ",", 3);

        int from = value.find("[")+1;
        int len =  comma1 - from;
        string subvalue1 = value.substr(from, len);  //save value as string
        std::string::size_type sz;     // alias of size_t
        double val0 = std::stof (subvalue1,&sz);  //string to int

        from = comma1 +1;
        len = comma2 - from;
        string subvalue2 = value.substr(from, len);  //save value as string
        double val1 = std::stof (subvalue2,&sz);  //string to int

        from = comma2 +1;
        len = comma3 - from;
        string subvalue3 = value.substr(from, len);  //save value as string
        double val2 = std::stof (subvalue3,&sz);  //string to in

        //string subvalue4 = value.substr(value.find_last_of(",")+1, value.find(",0]")-2);  //save value as string
        //cout << subvalue3 << endl;
        double val3 = 0;//std::stof (subvalue3,&sz);  //string to int

        *parameter = Scalar(val0,val1,val2,val3);

        to_return = RESULT_OK;


    }
    //else {cout << paramname << " NOT found in line" << endl;}
    return to_return;
}


void SaveConfig(string filename){
    ofstream file;
    file.open(filename,  fstream::out);

       cout<<"settings.ini file not found!" << endl;

    file << "# input\n";
    file << "capture_run=" << capture_run <<";\n";

    file << "input_correction_on=" << input_correction_on <<";\n";
    file << "hsv_input_correction=" << hsv_input_correction <<";\n";
    //file << "cut_up=" << cut_up <<";\n";
    //file << "cut_down=" << cut_down <<";\n";
    //file << "cut_left=" << cut_left <<";\n";
    //file << "cut_right=" << cut_right <<";\n";

    file << "\n";
    file << "# edge detection\n";
    file << "canny_blur_value=" << canny_blur_value <<";\n";
    file << "canny_thresh_value1=" << canny_thresh_value1 <<";\n";
    file << "canny_thresh_value2=" << canny_thresh_value2 <<";\n";
    file << "contour_current_algo=" << contour_current_algo <<";\n";
    file << "use_scharr=" << use_scharr <<";\n";
    file << "scharr_thresh_value=" << scharr_thresh_value <<";\n";

    file << "\n";
    file << "# contours\n";
    file << "min_bbox_area=" << min_bbox_area <<";\n";
    file << "max_bbox_area=" << max_bbox_area <<";\n";
    file << "morph_mask=" << morph_mask <<";\n";
    file << "input_correction_on=" << input_correction_on <<";\n";
    file << "morph_size=" << morph_size <<";\n";

    file << "\n";
    file << "# color\n";
    file << "good_color_rgb=" << good_color_rgb <<";\n";
    file << "color_tolerance_rgb=" << color_tolerance_rgb <<";\n";
    file << "good_color_hsv=" << good_color_hsv <<";\n";
    file << "color_tolerance_hsv=" << color_tolerance_hsv <<";\n";
    file << "good_colorspace=" << good_colorspace <<";\n";

    file << "\n";
    file << "# output\n";
    file << "show_contours=" << show_contours <<";\n";
    file << "show_centers=" << show_centers <<";\n";
    file << "show_bboxes=" << show_bboxes <<";\n";
    file << "show_avgcolor=" << show_avgcolor <<";\n";
    file << "show_area=" << show_area <<";\n";
    file << "show_bboxes=" << show_bboxes <<";\n";
    file << "show_fill_avgcolor=" << show_fill_avgcolor <<";\n";
    file << "show_fill_contcolor=" << show_fill_contcolor <<";\n";

    file << "\n";
    file << "# com port\n";
    file << "com_connected=" << com_connected <<";\n";
    file << "com_port_number=" << com_port_number <<";\n";
    file << "com_speed=" << com_speed <<";\n";

    file << "\n";
    file << "# mog\n";
    file << "bs_current_algo=" << bs_current_algo <<";\n";

    file << "bs_mog_lrate=" << bs_mog_lrate <<";\n";
    file << "bs_mog_lrate=" << bs_mog_lrate <<";\n";
    file << "bs_mog_lrate=" << bs_mog_lrate <<";\n";
    file << "bs_mog_lrate=" << bs_mog_lrate <<";\n";
    file << "bs_mog_lrate=" << bs_mog_lrate <<";\n";
    file << "bs_mog_lrate=" << bs_mog_lrate <<";\n";


    file << "\n";
    file << "# mog2\n";
    file << "bs_mog2_history=" << bs_mog2_history <<";\n";
    file << "bs_mog2_thresh=" << bs_mog2_thresh <<";\n";
    file << "bs_mog2_shadows=" << bs_mog2_shadows <<";\n";
    file << "bs_mog2_learning=" << bs_mog2_learning <<";\n";
    file << "bs_mog2_lrate=" << bs_mog2_lrate <<";\n";

    file << "\n";
    file << "# knn\n";
    file << "bs_knn_history=" << bs_knn_history <<";\n";
    file << "bs_knn_thresh=" << bs_knn_thresh <<";\n";
    file << "bs_knn_shadows=" << bs_knn_shadows <<";\n";
    file << "bs_knn_learning=" << bs_knn_learning <<";\n";
    file << "bs_knn_lrate=" << bs_knn_lrate <<";\n";

    file << "\n";
    file << "# cnt\n";
    file << "bs_cnt_min_pix_stability=" << bs_cnt_min_pix_stability <<";\n";
    file << "bs_cnt_max_pix_stability=" << bs_cnt_max_pix_stability <<";\n";
    file << "bs_cnt_use_history=" << bs_cnt_use_history <<";\n";
    file << "bs_cnt_isparallel=" << bs_cnt_isparallel <<";\n";
    file << "bs_cnt_fps=" << bs_cnt_fps <<";\n";
    file << "bs_cnt_learning=" << bs_cnt_learning <<";\n";
    file << "bs_cnt_lrate=" << bs_cnt_lrate <<";\n";


    file << "\n";
    file << "# gsoc\n";
    file << "bs_gsoc_mc=" << bs_gsoc_mc <<";\n";
    file << "bs_gsoc_samples=" << bs_gsoc_samples <<";\n";
    file << "bs_gsoc_reprate=" << bs_gsoc_reprate <<";\n";
    file << "bs_gsoc_proprate=" << bs_gsoc_proprate <<";\n";
    file << "bs_gsoc_hits_thresh=" << bs_gsoc_hits_thresh <<";\n";
    file << "bs_gsoc_alpha=" << bs_gsoc_alpha <<";\n";
    file << "bs_gsoc_beta=" << bs_gsoc_beta <<";\n";
    file << "bs_gsoc_bs_decay=" << bs_gsoc_bs_decay <<";\n";
    file << "bs_gsoc_bs_mul=" << bs_gsoc_bs_mul <<";\n";
    file << "bs_gsoc_noise_bg=" << bs_gsoc_noise_bg <<";\n";
    file << "bs_gsoc_noise_fg=" << bs_gsoc_noise_fg <<";\n";
    file << "bs_gsoc_learning=" << bs_gsoc_learning <<";\n";

    file << "\n";
    file << "# camera\n";
    file << "cam_number=" << cam_number <<";\n";
    file << "cam_width=" << cam_width <<";\n";
    file << "cam_height=" << cam_height <<";\n";
    file << "cam_gain=" << cam_gain <<";\n";
    file << "cam_contrast=" << cam_contrast <<";\n";
    file << "cam_brightness=" << cam_brightness <<";\n";
    file << "cam_saturation=" << cam_saturation <<";\n";
    file << "cam_exposure=" << cam_exposure <<";\n";
    file << "cam_hue=" << cam_hue <<";\n";
    file << "cam_fps=" << cam_fps <<";\n";


    file << "\n";
    file << "# misc\n";
    file << "pix_per_100mm=" << pix_per_100mm <<";\n";
    file << "morpho_type=" << morpho_type <<";\n";
    file << "contour_current_algo=" << contour_current_algo <<";\n";
    file << "bs_current_algo=" << bs_current_algo <<";\n";
    file << "blur_before_mog=" << blur_before_mog <<";\n";
    file << "fullscreen=" << fullscreen <<";\n";
    file.close();
}

void CfgLoad(int cfg_file_number){
    char buff[32];
    std::string str(buff);

    sprintf(buff, "settings%d.ini", cfg_file_number);
    std::cout << buff  <<std::endl;
    ReadConfig(str);
}

void CfgSave(int cfg_file_number){
    char buff[32];
    std::string str(buff);

    sprintf(buff, "settings%d.ini", cfg_file_number);
    std::cout << buff  <<std::endl;
    SaveConfig(str);
}






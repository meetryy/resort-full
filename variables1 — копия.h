#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>


//using namespace cv;

enum ColorspaceAlias {BGR, HSV};
enum FrameShowAlias {SHOW_NOTHING, SHOW_INPUT, SHOW_CANNY, SHOW_SCHARR, SHOW_MASK, SHOW_OUTPUT, SHOW_MASKED};

extern bool err;
extern std::string err_text;
extern float time1;
extern float time2;

extern int show_frame;

//input
extern bool capture_run;
extern bool input_correction_on;
extern cv::Scalar hsv_input_correction;

//edge detection
extern int canny_blur_value;
extern long canny_thresh_value;
extern bool use_scharr;
extern long scharr_thresh_value;

//contours
extern long min_bbox_area;
extern long max_bbox_area;
extern bool morph_mask;
extern int morph_size;

//color
extern cv::Scalar good_color_rgb;
extern cv::Scalar color_tolerance_rgb;
extern cv::Scalar good_color_hsv;
extern cv::Scalar color_tolerance_hsv;
extern bool good_colorspace;

//output
extern bool show_contours;
extern bool show_centers;
extern bool show_bboxes;
extern bool show_avgcolor;
extern bool show_area;
extern bool show_diameter;
extern bool show_fill_avgcolor;
extern bool show_fill_contcolor;

//com port
extern bool com_connected;
extern int  com_port_number;
extern long com_speed;

//info
extern float info_ms_frame;
extern float info_fps;
extern long info_total_contours;
extern long useful_contours;














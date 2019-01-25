#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

//using namespace cv;

enum ColorspaceAlias {BGR, HSV};
enum FrameShowAlias {SHOW_NOTHING, SHOW_INPUT, SHOW_CANNY, SHOW_SCHARR, SHOW_MASK, SHOW_OUTPUT, SHOW_QUAD};

extern bool err;
extern std::string err_text;
extern float time1;
extern float time2;

extern int show_frame;

struct V_t{
    struct Input_t{
        bool CaptureRun;
        bool FreezeFrame;
        int  Source;
        bool CaptureFile;
    };
    Input_t Input;

    struct Cam_t{
        int     Number;
        long    Width;
        long    Height;
        int     Gain;
        int     Contrast;
        int     Brightness;
        int     Exposure;
        int     Hue;
        int     Saturation;
        float   FPS;
    };
    Cam_t Cam;

    struct Edge_t{
        int BlurValue;
        int CannyThresh1;
        int CannyThresh2;
        bool UseScharr;
        long ScharrThresh;
    };
    Edge_t Edge;

    struct Contours_t{
        float MinBBoxArea;
        float MaxBBoxArea;
        bool morph_mask;
        bool WideAreaRange;
        int   CurrentAlgo;
    };
    Contours_t Contours;

    struct Morph_t{
        int Size;
        int Type;
    };
    Morph_t Morph;

    struct Color_t{
        cv::Scalar GoodRGB;
        cv::Scalar ToleranceRGB;
        cv::Scalar GoodHSV;
        cv::Scalar ToleranceHSV;
        bool GoodSpace;
    };
    Color_t Color;

    struct Show_t{
        bool Contours;
        bool Centers;
        bool BBoxes;
        bool AvgColor;
        bool Area;
        bool Diameter;
        bool FillAvg;
        bool FilContour;
        int     MatUpdCounter;
        int     MatUpdTarget;
    };
    Show_t Show;

    struct COM_t{
        bool Connected;
        int  Number;
        long Speed;
    };
    COM_t ComPort;

    struct BS_t{
        int CurrentAlgo;
        int BlurBeforeMog;
        struct MOG_t{
            int     History;
            int     Mixtures;
            float   BackRatio;
            float   NoiseSigma;
            bool    Learning;
            float   LRate;
        };
        MOG_t MOG;

        struct MOG2_t{
            int     Mixtures;
            float   Thresh;
            bool    DetectShadows;
            bool    Learning;
            float   LRate;
        };
        MOG2_t MOG2;

        struct CNT_t{
            int     MinPixStability;
            int     MaxPixStability;
            int     FPS;
            bool    UseHistory;
            bool    IsParallel;
            float   LRate;
            bool    Learning;
        };
        CNT_t CNT;
    };
    BS_t BS;

    struct Info_t{
        float   time1;
        float   time2;
        float   s_per_frame;
        float   s_per_frame_accumulated;
        float   FPS;
        long    fps_avg_counter;
        long    TotalContours;
        long    UsefulContours;
        int     FPSAverage;
    };
    Info_t Info;

    struct UI_t{
        bool Fullscreen;
    };
    UI_t UI;

};
extern V_t V;


//input
extern bool capture_run;
extern bool freeze_frame;
extern int  input_source;
extern bool input_correction_on;
extern  cv::Scalar hsv_input_correction;
extern  int     cam_number;
extern  long    cam_width;
extern  long    cam_height;
extern  int     cam_gain;
extern  int     cam_contrast;
extern  int     cam_brightness;
extern  int     cam_exposure;
extern  int     cam_hue;
extern  int     cam_saturation;
extern float    cam_fps;

//edge detection
extern int canny_blur_value;
extern int canny_thresh_value1;
extern int canny_thresh_value2;
extern bool use_scharr;
extern long scharr_thresh_value;

//contours
extern float min_bbox_area;
extern float max_bbox_area;
extern bool morph_mask;
extern int morph_size;
extern bool wide_area_range;

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
extern float   time1;
extern float   time2;
extern float   info_s_per_frame;
extern float   info_s_per_frame_accumulated;
extern float   info_fps;
extern long    fps_avg_counter;

extern long info_total_contours;
extern long useful_contours;


///////////////////////

extern  int     bs_mog_history;
extern  int     bs_mog_mixtures;
extern  float   bs_mog_backratio;
extern  float   bs_mog_noisesigma;
extern  bool    bs_mog_learning;
extern  float   bs_mog_lrate;

extern  int     bs_mog2_history;
extern  float   bs_mog2_thresh;
extern  bool    bs_mog2_shadows;
extern  bool    bs_mog2_learning;
extern  float   bs_mog2_lrate;

extern  int     bs_knn_history;
extern  float   bs_knn_thresh;
extern  bool    bs_knn_shadows;
extern  bool    bs_knn_learning;
extern  float   bs_knn_lrate;

extern  int     bs_cnt_min_pix_stability;
extern  int     bs_cnt_max_pix_stability;
extern  int     bs_cnt_fps;
extern  bool    bs_cnt_use_history;
extern  bool    bs_cnt_isparallel;
extern  float   bs_cnt_lrate;
extern  bool    bs_cnt_learning;

extern  int     bs_gsoc_mc;
extern  int     bs_gsoc_samples;
extern  float   bs_gsoc_reprate;
extern  float   bs_gsoc_proprate;
extern  int     bs_gsoc_hits_thresh;
extern  float   bs_gsoc_alpha;
extern  float   bs_gsoc_beta;
extern  float   bs_gsoc_bs_decay;
extern  float   bs_gsoc_bs_mul;
extern  float   bs_gsoc_noise_bg;
extern  float   bs_gsoc_noise_fg;
extern  bool    bs_gsoc_learning;
extern  float   bs_gsoc_lrate;

extern  int     bs_current_algo;
extern  int     morpho_type;
extern  int     contour_current_algo;

extern  int     blur_before_mog;
extern bool     fullscreen;
extern  long    pix_per_100mm;

extern bool     video_recording;

extern bool     show_GUI;

extern  double    cut_up;
extern  double    cut_down;
extern  double    cut_left;
extern  double    cut_right;

extern bool     draw_ruler;
extern bool     capture_file;
extern bool     show_quad;
extern bool     show_particle_number;


extern bool fullscreen;

extern int show_mat_upd_counter;
extern int show_mat_upd_target;
extern int fps_average;

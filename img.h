#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgcodecs.hpp"
#include <opencv2/core/utility.hpp>
#include <opencv2/bgsegm.hpp>
#include <opencv2/videoio.hpp>

#include <SFML/System/Clock.hpp>

//extern cv::Mat img_output;
//extern cv::Mat img_in;
//extern cv::Mat img_mog_output;
extern cv::Mat img_gray;
extern cv::Mat img_canny;
//extern cv::Mat img_wholemask;
extern cv::Mat img_output;
extern cv::Mat1b img_quad;
//extern cv::Mat img_bs_back;

//extern cv::Mat img_morph_out;
//extern cv::Mat img_canny_output;
extern cv::Mat img_contours;
extern cv::Mat img_mask;

//extern long good_contours;

enum morphAlias {MORPH_CURRENT_RECT, MORPH_CURRENT_ELLIPSE, MORPH_CURRENT_CROSS};

#include "omp.h"

class Image_class{
private:
    bool    ColorOK =0;
    bool    video_recording = 0;

public:
    int fps_average = 20;
    cv::Scalar avg_color_hsv;
    char str[200];
    float avg_time = 0;
    int ScalarInRange (cv::Scalar input, cv::Scalar compare_to, cv::Scalar delta, bool useHSV);
    cv::Scalar BGR2HSV(cv::Scalar inBGR);


    bool input_correction_on;
    cv::Scalar hsv_input_correction;
    int show_frame;


    float   time1;
    float   time2;
    //V.Info.fps_avg_counter;

    sf::Clock FPS_Clock;
    float fps_accumulated = 0;

    long    info_total_contours;
    long    useful_contours;
    long    good_contours;

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
    float   bs_knn_lrate;
    float   bs_gsoc_lrate;

    bool    capture_file = 0;
    bool    show_particle_number = 0;
    int     show_mat_upd_counter = 0;
    int     show_mat_upd_target = 2;

    cv::Mat img_in;
    cv::Mat img_mog_output;
    cv::Mat img_canny;
    cv::Mat img_roi;
    cv::Mat img_roi_hsv;
    cv::Mat img_wholemask;
    cv::Mat img_output;
    cv::Mat img_bs_back;
    cv::Mat img_morph_out;
    cv::Mat img_canny_output;
    cv::Mat img_contours;

    omp_lock_t img_roi_lock;
    omp_lock_t img_output_lock;
    omp_lock_t max_cont_lock;
    omp_lock_t img_wholemask_lock;

    void ProcessImg(void);
    void MOG_Init(void);
    void cam_open(void);
    void cam_close(void);
    void cam_update(void);
    void start_video_rec(void);
    void stop_video_rec(void);
    void video_open(void);
    void video_close(void);

    void ImgProcessor (void);
    void BS_Init(int bs_algo);
    void FPS_Routine(void);
    void RunProcessor(void);

};

extern Image_class Img;

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgcodecs.hpp"
#include <opencv2/core/utility.hpp>
#include <opencv2/bgsegm.hpp>
#include <opencv2/videoio.hpp>

#include <SFML/System/Clock.hpp>

//extern cv::Mat img_gray;
//extern cv::Mat img_canny;
//extern cv::Mat img_output;
//extern cv::Mat1b img_quad;
//extern cv::Mat img_contours;
//extern cv::Mat img_mask;

enum morphAlias {MORPH_CURRENT_RECT, MORPH_CURRENT_ELLIPSE, MORPH_CURRENT_CROSS};
enum procTypes {PROC_WF, PROC_B};
enum {PMODE_INIT, PMODE_BELT_STOP, PMODE_BELT_RUN, PMODE_WF_STOP, PMODE_WF_RUN, PMODE_CALIB_DIST, PMODE_CALIB_DIM};

class Image_class{
private:
    bool    ColorOK = 0;
    bool    video_recording = 0;

public:
    int fps_average = 20;
    cv::Scalar avg_color_hsv;
    char str[200];
    float avg_time = 0;

    bool input_correction_on;
    cv::Scalar hsv_input_correction;
    int show_frame;

    float   time1;
    float   time2;

    sf::Clock   FPS_Clock;
    float       fps_accumulated = 0;

    long    info_total_contours;
    long    useful_contours;
    long    good_contours;

    int     bs_knn_history;
    float   bs_knn_thresh;
    bool    bs_knn_shadows;
    bool    bs_knn_learning;
    float   bs_knn_lrate;

    //bool    capture_file = 0;
    //bool    show_particle_number = 0;

    int     matLimiterCounter = 0;
    int     matLimiterTarget = 2;

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
    cv::Mat img_debug;

    int ScalarInRange (cv::Scalar input, cv::Scalar compare_to, cv::Scalar delta, bool useHSV);
    cv::Scalar BGR2HSV(cv::Scalar inBGR);

    void BS_Init(int bs_algo);
    void ProcessImg(void);

    void cameraOpen(void);
    void cameraClose(void);
    void cameraUpdSettings(void);

    void stopCapture();
    void startCapture();

    void start_video_rec(void);
    void stop_video_rec(void);


    void initProcessor(int newProcType);
    void startProcessing(void);
    void stopProcessing(void);

    std::string videoFileName;

    //void videoOpen(std::string fileName);
    //void videoClose(void);

    void ImgProcessor (void);

    void FPS_Routine(void);
    void RunProcessor(void);
    void WaterfallProcessor(cv::Mat img_in);
    void limitMatWinFPS(void);

    int procState = PMODE_INIT;
    int procRun = 1;
};

extern Image_class Img;

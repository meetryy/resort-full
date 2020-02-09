#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

enum ColorspaceAlias {BGR, HSV};

extern float time1;
extern float time2;

struct V_t{
    struct {
        bool CaptureRun;
        bool FreezeFrame;
        int  Source;
        bool CaptureFile;
    } Input;

    int procType = 0;

    struct {
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
    } Cam;

    struct{
        int BlurValue;
        int CannyThresh1;
        int CannyThresh2;
        bool UseScharr;
        long ScharrThresh;
    } Edge;

    struct {
        float MinBBoxArea;
        float MaxBBoxArea;
        bool morph_mask;
        bool WideAreaRange;
        int   CurrentAlgo;
    } Contours;

    struct {
        int Size;
        int Type;
    } Morph;

    struct {
        cv::Scalar GoodRGB;
        cv::Scalar ToleranceRGB;
        cv::Scalar GoodHSV;
        cv::Scalar ToleranceHSV;
        bool GoodSpace;
    } Color;

    struct {
        bool Contours;
        bool Centers;
        bool BBoxes;
        bool AvgColor;
        bool Area;
        bool Diameter;
        bool FillAvg;
        bool FilContour;
        int  MatUpdCounter;
        int  MatUpdTarget;
    } Show;

    struct {
        int     Number = 0;
        long    Speed = 9600;
        int     shakeTimeout = 500;
        std::string shakeQuery;
        std::string shakeAnswer;
    } ComPort;

    struct {
        int CurrentAlgo;
        int BlurBeforeMog;
        struct {
            int     History;
            int     Mixtures;
            float   BackRatio;
            float   NoiseSigma;
            bool    Learning;
            float   LRate;
        } MOG;

        struct {
            int     History;
            float   Thresh;
            bool    DetectShadows;
            bool    Learning;
            float   LRate;
        } MOG2;

        struct {
            int     MinPixStability;
            int     MaxPixStability;
            int     FPS;
            bool    UseHistory;
            bool    IsParallel;
            float   LRate;
            bool    Learning;
        }CNT;
    } BS;

    struct {
        float   time1;
        float   time2;
        float   s_per_frame;
        float   s_per_frame_accumulated;
        float   FPS;
        long    fps_avg_counter;
        long    TotalContours;
        long    UsefulContours;
        int     FPSAverage;
        bool    ConsoleDestination;
    } Info;

    struct {
        bool Fullscreen = 0;
        int guiFPS = 30;
    } UI;

    bool comTest = 0;

};

extern struct V_t V;

extern  long    pix_per_100mm;
extern bool     video_recording;
extern bool     show_GUI;

extern  double    cut_up;
extern  double    cut_down;
extern  double    cut_left;
extern  double    cut_right;

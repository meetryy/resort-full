 #ifndef H_BELT
 # define H_BELT

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <vector>
#include <mutex>
#include <thread>
//#include <variant>

enum ProcessorParams{   PARAM_MARKERH, PARAM_MARKERUPPERMARGIN, PARAM_FRAMELOWERMARGIN,
                        PARAM_NR};

struct ProcessorParameter_t{
    std::string Name;
    std::string Category;
    //std::variant<float, double, cv::Scalar, b> f, d, s, b;
    union Number{
        float f;
        double d;
        cv::Scalar s;
        bool b;
    };
    Number *data;
};

class imageProcessor_t{
public:
        cv::Mat inputMat;
    cv::Mat outputMat;
    cv::Mat mat;

    const int max_H = 360/2;
    const int max_V = 255;
    const int max_L = max_V;
    const int max_S = max_V;
    int     low_H = 0,      low_S = 33,      low_V = 0,      low_L = 21;
    int     high_H = max_H, high_S = max_S, high_V = max_V, high_L = max_L;

    float   minArea = 50;
    float   maxArea = 5000;
    int     morphShape = 0;
    int     morphType = 3;
    int     morphArea = 1;

    double  test1, test2;
    int     blurSize = 3;

    float   saturationWeight = 0.1;
    int     satRangeMin = 10;
    int     satRangeMax = 255;

    int     cropW = 600;

    float mmPerFrame = 1.17;
    float beltSpeedMm = 100.0;

    int ejectorDecZoneW = 1;
    int ejectorDecZoneX = 200;

    bool drawHUD = 1;
    std::vector<std::vector<cv::Point>> contoursHSV;
    std::vector<cv::Vec4i>              hierarchyHSV;

    cv::Mat matAlphaMask;
    cv::Mat matAlphaMaskShifted;
    cv::Mat matAlphaMaskAccum;
    cv::Mat matSatRanged;



    cv::Mat outMat;
    cv::Rect cropRect, mainRect;
    cv::Mat mainMatHUD;

    cv::Mat mainMatHSVblur;
    cv::Mat mainMatHSV;
    cv::Mat mainMatHSVRanged;
    cv::Mat mainMatHSVmorphed;
    cv::Mat matSatRendered;

    cv::Mat ejHUD;
    cv::Mat ejectorDecZoneMask;

    bool initDone = 0;

    const static int numShowableMats = 3;
    struct namedMat_t{
        cv::Mat mat;
        std::string name;
    } showableMats[numShowableMats];

    cv::Scalar colorsByName[16] = { cv::Scalar(255,0,0),    cv::Scalar(0,255,0),    cv::Scalar(0,0,255),    cv::Scalar(100,255,255),
                                cv::Scalar(255,150,50), cv::Scalar(255,100,255),cv::Scalar(128,128,128),cv::Scalar(150,70,0),
                                cv::Scalar(0,128,255), cv::Scalar(50,255,150), cv::Scalar(0,160,160), cv::Scalar(0,0,0), cv::Scalar(0,0,0),
                                cv::Scalar(0,0,0), cv::Scalar(0,0,0), cv::Scalar(0,0,0)};


    struct ejTask_t{
        bool done = 1;
        bool newState = 0;
        std::chrono::time_point<std::chrono::high_resolution_clock>  time;
        float timeLeftMs = 0;
    };

    struct Ejector_t{
        static const int numZones = 12;
        float posMm[numZones] = {6,12,18,24,30,36,42,48,54,60};
        float widthMm[numZones] = {8,8,8,8,8,8,8,8,8,8};

        float loadToEject = 0.05;
        struct{
            float   avgLoad = 0;
            bool    ejCrit = 0;
            bool    State = 0;
            bool    oldEjCrit = 0;
            float   mmFromCamEdge = 80.0;

            bool    taskNewState = 0;
            bool    taskDone = 1;
            std::chrono::time_point<std::chrono::high_resolution_clock>  timeTarget;
            float    msToTask = 0;
            bool    overlap = 0;

            std::vector<ejTask_t> tasks;
            //std::vector<std::chrono::time_point<std::chrono::high_resolution_clock>> listON;
            std::mutex taskLock;
            //std::vector<std::chrono::time_point<std::chrono::high_resolution_clock>> listOFF;

        } Zone[numZones];
    } Ejector;

    int pixPerFrame, pixPerFrameAccum, pixPerFrameCounter;
    float mmPerPix = 150.0 / 1280.0;
    float pixPerMm = 1.0 / mmPerPix;


    void Init(void);
    cv::Mat Result(cv::Mat &input);

    void DrawHUD(cv::Mat &mat);

    void startTimeThread(void);
    void stopTimeThread(void);


private:
    std::thread timerThread;
    void ejTaskExc(void);
    void shiftMat(cv::Mat &in, cv::Mat &out, int pixels, bool dirLeft);
    bool timerTheradRun = 0;
};

extern imageProcessor_t BeltProcessor;


 #endif



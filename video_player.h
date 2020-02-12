 #ifndef H_PLAYER
 # define H_PLAYER

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <string>

class videoPlayer_t{
public:
    cv::Mat output;
    cv::Mat lastMat;
    int startFrame;
    int endFrame;
    float frameW, frameH;
    int playbackMarker;
    float playbackMs;
    std::string flieName;
    cv::VideoCapture cap;
    int Rotation;
    float fpsLimiter = 10.0;
    double fps = 0;
    double fpsAccum = 0;
    int fpsAvgCounter = 0;
    int fpsAvg = 2;
    uint64_t startTime;

    float playbackPortion = 0;
    int partLength = 0;
    bool pause = 0;

    float videoFPS;// = cap.get(CAP_PROP_FPS);
    int fileLengthFrames;// =  cap.get(CAP_PROP_FRAME_COUNT);
    float fileLengthMs;// = (float)fileLengthFrames / videoFPS;

    int Start(std::string FlieName, int StartFrame, int EndFrame, int Rotate);
    void Stop(void);
    void Pause(void);

    cv::Mat getFrame(void);
    void setMarker(double Position);
    void setMarker(int frames);
    void fpsStart(void);
    void fpsStop(void);

};

extern videoPlayer_t videoPlayer;

 #endif



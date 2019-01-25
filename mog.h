#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
enum sources {SOURCE_CAM, SOURCE_VIDEO};
enum bsAlias {BS_MOG, BS_MOG2, BS_CNT,BS_KNN,BS_GSOC};

extern cv::Mat mog_output;

extern int mog_history;
extern int mog_mixtures;
extern float mog_backratio;
extern float mog_noisesigma;



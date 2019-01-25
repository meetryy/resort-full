extern cv::Mat img_output;
extern cv::Mat img_in;
extern cv::Mat img_mog_output;
extern cv::Mat img_gray;
extern cv::Mat img_canny;
extern cv::Mat img_wholemask;
extern cv::Mat img_output;
extern cv::Mat1b img_quad;
extern cv::Mat img_bs_back;

extern cv::Mat img_morph_out;
extern cv::Mat img_canny_output;
extern cv::Mat img_contours;
extern cv::Mat img_mask;

extern long good_contours;
void ProcessImg(void);
void MOG_Init(void);
void cam_open(void);
void cam_close(void);
void cam_update(void);
void start_video_rec(void);
void stop_video_rec(void);
void video_open(void);

void ImgProcessor (void);
void BS_Init(int bs_algo);

enum morphAlias {MORPH_CURRENT_RECT, MORPH_CURRENT_ELLIPSE, MORPH_CURRENT_CROSS};

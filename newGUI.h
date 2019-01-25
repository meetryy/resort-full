enum MatWinAlias {W_MAT_IN, W_MAT_BG, W_MAT_CONTOUR, W_MAT_MOG, W_MAT_MORPH, W_MAT_OUT, W_MAT_MASK, W_MAT_NR};
enum SetWinAlias {  W_SET_IN, W_SET_BG, W_SET_CONTOUR, W_SET_MASK, W_SET_COLOR, W_SET_COM, W_SET_HW, W_SET_INFO,W_SET_OUT,
                    W_SELF_COLORS, W_SELF_LOG, W_SET_FILE,
                    W_SET_NR};


void Draw_ImGui(void);
void GUI_Fill_Textures(void);
void GUI_VarInit(void);

struct mat_window_t{
    bool show;
    std::mutex write;
    std::string title;
    cv::Mat *mat;
    bool out_ready;
    cv::Mat mat_show;
};

struct set_window_t{
    bool show;
    std::string title;
};



extern struct mat_window_t MatWin[W_MAT_NR];
extern struct set_window_t SetWin[W_SET_NR];

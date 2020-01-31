#include <string>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

class GUI_class{
private:
    int a = 0;
    sf::Event event;
    bool    FullscreenChanged = 0;
    int test_int = 0;
    float col[3]={0.0};
    float test_color[4]={0.0};

    struct mat_window_t{
        bool show;
        std::mutex write;
        std::string title;
        cv::Mat *mat;
        bool out_ready;
        int cat = 0;
        cv::Mat mat_show;
    };

    struct set_window_t{
        bool show;
        std::string title;
    };



public:
    enum MatWinAlias {  W_MAT_IN,
                        W_MAT_WF_BG, W_MAT_WF_CONTOUR, W_MAT_WF_MOG, W_MAT_WF_MORPH, W_MAT_WF_OUT, W_MAT_WF_MASK,
                        W_MAT_PP_IN, W_MAT_PP_HUD,
                        W_MAT_B_HUD, W_MAT_B_ACCUM, W_MAT_B_RANGED,
                        W_MAT_DEBUG, W_MAT_NR};
    enum SetWinAlias {  W_SET_IN, W_SET_BG, W_SET_CONTOUR, W_SET_MASK, W_SET_COLOR, W_SET_COM, W_SET_HW, W_SET_INFO,W_SET_OUT,
                        W_SELF_COLORS, W_SELF_LOG, W_SET_FILE,
                        W_SET_NR};

    enum matwins {W_CAT_NONE, W_CAT_INPUT, W_CAT_PP, W_CAT_WF, W_CAT_BELT, NR_W_CAT};

    const std::string matWinCats[NR_W_CAT] = {u8"Без категории", u8"Вход", u8"Препроцессор", u8"Водопад",u8"Ремень"};

    cv::Mat frameRGB[W_MAT_NR], frameRGBA[W_MAT_NR];
    sf::Image image[W_MAT_NR];
    sf::Sprite sprite[W_MAT_NR];
    sf::Texture texture[W_MAT_NR];

    enum BrowseAlias {BROWSE_SAVE, BROWSE_LOAD};
    bool BrowseMode = 0;

    struct mat_window_t MatWin[W_MAT_NR];
    struct set_window_t SetWin[W_SET_NR];

    sf::RenderWindow window;
    sf::Clock deltaClock;
    std::string WindowName = "ReSort v0.25";
    long ScreenW = 0;
    long ScreenH = 0;

    void StartWorker(void);
    void Worker(void);
    void VarInit(void);
    void FileDialogue(void);
    void Fill_Textures(void);
    static void ShowHelpMarker(const char* desc);
    void ConsoleOut (std::string InString);
    void Draw(void);
    void Init(void);
    void LoadFont(void);
    void MenuBarStateList(void);
    void StatedText(std::string Input, bool state);

    void drawMenuBar(void);
    void drawMatWindows(void);

    //void drawSettingsWindowOld(void);

    void drawSettingsWindow(void);
    void drawMatBar(void);
    void drawSettingsBar(void);
    void drawSettingsBlock(void);

    //void drawWaterfallSetingsWindow(void);
    enum settingsCats { CAT_INPUT, CAT_PROCESSING,
                        CAT_WF_EDGE, CAT_WF_CONTOURS, CAT_WF_MORPHO, CAT_WF_COLOR,
                        CAT_WF_BS,
                        //CAT_WF_BS_MOG, CAT_WF_BS_MOG2, CAT_WF_BS_CNT,
                        CAT_WF_SHOW,
                        CAT_B_COLOR, CAT_B_SIZE, CAT_B_MORPH, CAT_B_BLUR, CAT_B_ACCUM, CAT_B_INFO,
                        CAT_COM, CAT_UI,
                        CAT_STATS,
                        CAT_DEBUG,
                        NR_CAT};
    std::string settingsCatNames[NR_CAT];
};

extern GUI_class GUI;

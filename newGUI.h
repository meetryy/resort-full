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
        cv::Mat mat_show;
    };

    struct set_window_t{
        bool show;
        std::string title;
    };



public:
    enum MatWinAlias {W_MAT_IN, W_MAT_BG, W_MAT_CONTOUR, W_MAT_MOG, W_MAT_MORPH, W_MAT_OUT, W_MAT_MASK, W_MAT_NR};
    enum SetWinAlias {  W_SET_IN, W_SET_BG, W_SET_CONTOUR, W_SET_MASK, W_SET_COLOR, W_SET_COM, W_SET_HW, W_SET_INFO,W_SET_OUT,
                        W_SELF_COLORS, W_SELF_LOG, W_SET_FILE,
                        W_SET_NR};


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
    const std::string WindowName = "ReSort v0.2 06.2019";
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

};

extern GUI_class GUI;

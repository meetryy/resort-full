#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/bgsegm.hpp>
#include <iostream>
#include <fstream>
#include <ctime>
#include <sstream>
#include <cstdlib>
#include <thread>

#include "imgui.h"
#include "imgui-SFML.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include "variables1.h"
#include "UI.h"
#include "main.h"
#include "file.h"
#include "img.h"
#include "rs232.h"
#include "COM_port.h"
#include "mog.h"
#include "real_dimensions.h"
#include "newGUI.h"
#include "ImGuiFileDialog.h"
#include "omp.h"
#define FPS_AVERAGE 50

using namespace cv;
using namespace std;

void    comport_thread(void);
void    image_thread(void);
void    ProcessImg(void);


bool    FullscreenChanged =0;
sf::Window window;

V_t V;

const string WindowName = "ReSort v0.1 01.2019";

void LoadFont(void);
int main(int argc, const char *argv[])
{
    //========= GUI INITIALIZATION ==============
    long ScreenW=sf::VideoMode::getDesktopMode().width;
    long ScreenH=sf::VideoMode::getDesktopMode().height;
    sf::RenderWindow window(sf::VideoMode(ScreenW, ScreenH), WindowName);//,sf::Style::Fullscreen);
    window.setFramerateLimit(50);
    V.Info.ConsoleDestination = 1;
    V.UI.Fullscreen = 0;
    SetWin[W_SELF_LOG].show = 1;

    ImGui::SFML::Init(window, false);
    LoadFont();
    sf::Clock deltaClock;
    sf::Event event;

    sf::RectangleShape rect_back(sf::Vector2f(ScreenW, ScreenH));
    rect_back.setFillColor(sf::Color(50, 50, 50));

    //========= CONFIGURATION =================
    ReadConfig("settings.ini");
    SaveConfig("settings.ini");

    //cv::setUseOptimized(true);
    //cv::setNumThreads(4);

    list_COM();

    V.Input.CaptureRun = 0;

    //thread t1(comport_thread);

    BS_Init(BS_MOG);
    GUI_VarInit();

    omp_set_num_threads(32);

    thread ImgProcessor_t(ImgProcessor);
    ImgProcessor_t.detach();

    while (window.isOpen()) {
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);
            if (event.type == sf::Event::Closed) {window.close();}
            if (event.type == sf::Event::KeyPressed){
                if (event.key.code == sf::Keyboard::F12) {V.UI.Fullscreen = !V.UI.Fullscreen; FullscreenChanged = 1;}
            }
         }

        if(FullscreenChanged){
                window.close();
                if (V.UI.Fullscreen) window.create(sf::VideoMode(ScreenW, ScreenH), WindowName,sf::Style::Fullscreen);
                        else    window.create(sf::VideoMode(ScreenW, ScreenH), WindowName);
                window.setFramerateLimit(60);
                FullscreenChanged=0;
                ConsoleOut(u8"ИНТЕРФЕЙС: Полноэкранный режим переключен");
            }

        ImGui::SFML::Update(window, deltaClock.restart());
        Draw_ImGui();

        window.clear();
        window.draw(rect_back);
        ImGui::SFML::Render(window);
        window.display();
    }
    ImGui::SFML::Shutdown();
}

void comport_thread(void){
    while(1){
        //std::cout << "task1 says:";
        //cv::waitKey(100);
        //if (V.ComPort.Connected) {handshake_COM();}
    }
}



void LoadFont(void){
    ImFontConfig font_config;
    font_config.OversampleH = 1; //or 2 is the same
    font_config.OversampleV = 1;
    font_config.PixelSnapH = 1;

    static const ImWchar ranges[] =
    {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0x0400, 0x044F, // Cyrillic
        0,
    };

    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->Clear(); // clear fonts if you loaded some before (even if only default one was loaded)
    //io.Fonts->AddFontDefault(); // this will load default font as well
    io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Tahoma.ttf", 16.0f, &font_config, ranges);
    io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Verdana.ttf", 16.0f, &font_config, ranges);
    io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\lucon.ttf", 14.0f, &font_config, ranges);
    io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\comic.ttf", 18.0f, &font_config, ranges);

    ImGui::SFML::UpdateFontTexture(); // important call: updates font texture
    ImGui::GetIO().Fonts->Fonts[0]->AddRemapChar(0xCF, 0x043F);
    ConsoleOut(u8"ИНТЕРФЕЙС: Закгрузка шрифтов завершена");
}












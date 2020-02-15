#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <fstream>
#include <ctime>
#include <string>
#include <thread>
#include <mutex>

#include "imgui.h"
#include "imgui-SFML.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/CircleShape.hpp>

#include "variables1.h"
#include "main.h"
#include "file.h"
#include "img.h"
#include "rs232.h"
#include "COM_port.h"
#include "mog.h"
#include "real_dimensions.h"
#include "newGUI.h"
#include "gui_tools.h"
#include "gui/imguifilesystem.h"
#include "video_player.h"
#include "preprocessor.h"
#include "belt_processor.h"
#include "calibration.h"
#include "hardware.h"


using namespace std;
using namespace ImGui;

std::string FilePath(void);

bool* p_open;
void GUI_t::ShowHelpMarker(const char* desc){
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

void GUI_t::Init(void){
    ScreenW = sf::VideoMode::getDesktopMode().width;
    ScreenH = sf::VideoMode::getDesktopMode().height;

    GUI.WindowName.append(" (");
    GUI.WindowName.append(__DATE__);
    GUI.WindowName.append(")");

    window.create(sf::VideoMode(GUI.ScreenW, GUI.ScreenH), GUI.WindowName);//,sf::Style::Fullscreen);
    window.setFramerateLimit(V.UI.guiFPS);

    ImGui::SFML::Init(window, false);
    LoadFont();
    /*
    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
    colors[ImGuiCol_ChildBg]                = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
    colors[ImGuiCol_PopupBg]                = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    colors[ImGuiCol_Border]                 = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]                = ImVec4(0.20f, 0.21f, 0.22f, 0.54f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.40f, 0.40f, 0.40f, 0.40f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.18f, 0.18f, 0.18f, 0.67f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.29f, 0.29f, 0.29f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    colors[ImGuiCol_CheckMark]              = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
    colors[ImGuiCol_Button]                 = ImVec4(0.44f, 0.44f, 0.44f, 0.40f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.46f, 0.47f, 0.48f, 1.00f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.42f, 0.42f, 0.42f, 1.00f);
    colors[ImGuiCol_Header]                 = ImVec4(0.70f, 0.70f, 0.70f, 0.31f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.70f, 0.70f, 0.70f, 0.80f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.48f, 0.50f, 0.52f, 1.00f);
    colors[ImGuiCol_Separator]              = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.72f, 0.72f, 0.72f, 0.78f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.91f, 0.91f, 0.91f, 0.25f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.81f, 0.81f, 0.81f, 0.67f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.46f, 0.46f, 0.46f, 0.95f);
    colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(0.73f, 0.60f, 0.15f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.87f, 0.87f, 0.87f, 0.35f);
    colors[ImGuiCol_ModalWindowDarkening]   = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
    colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight]           = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    */
}


void GUI_t::Worker(void){
    sf::Clock deltaClock;
    sf::Event event;

    FullscreenChanged = 1;

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
                window.setFramerateLimit(30);
                FullscreenChanged=0;
                GUI.ConsoleOut(u8"ИНТЕРФЕЙС: Полноэкранный режим переключен");
            }

        ImGui::SFML::Update(window, deltaClock.restart());
        GUI.Draw();

        window.clear();
        sf::RectangleShape rect_back(sf::Vector2f(ScreenW, ScreenH));
        rect_back.setFillColor(sf::Color(50, 50, 50));
        window.draw(rect_back);
        ImGui::SFML::Render(window);
        window.display();
    }
    ImGui::SFML::Shutdown();
}


void GUI_t::LoadFont(void){
    ImFontConfig font_config;
    font_config.OversampleH = 1; //or 2 is the same
    font_config.OversampleV = 1;
    font_config.PixelSnapH = 1;

    static const ImWchar ranges[] ={
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

    ImGui::SFML::UpdateFontTexture(); // important call: updates font texture
    ImGui::GetIO().Fonts->Fonts[0]->AddRemapChar(0xCF, 0x043F);
    ConsoleOut(u8"ИНТЕРФЕЙС: Закгрузка шрифтов завершена");
}



ImVec4 ColorOn = ImColor(0.0f,1.0f,0.0f,1.0f);
ImVec4 ColorOff = ImColor(1.0f,0.0f,0.0f,1.0f);

void GUI_t::StatedText(string Input, bool state){
    if (state) ImGui::PushStyleColor(ImGuiCol_Text, ColorOn);
    else    ImGui::PushStyleColor(ImGuiCol_Text, ColorOff);

    ImGui::BulletText(Input.c_str());
    ImGui::PopStyleColor();
}


void GUI_t::drawMenuBar(void){
    if (ImGui::BeginMainMenuBar()){

        ImGui::Text(WindowName.c_str());
        ImGui::Separator();

        static char fileMenuName[64] = {0};
        sprintf(fileMenuName, u8"Конфигурация: %s", V.settingsFileName.c_str());

        if (ImGui::BeginMenu(fileMenuName)){
                if (ImGui::MenuItem(u8"Загрузить")) openFileBrowser(BROWSE_LOAD_CFG);
                if (ImGui::MenuItem(u8"Сохранить")) openFileBrowser(BROWSE_SAVE_CFG);
            ImGui::EndMenu();
        };

        ImGui::Separator();

        if(ImGui::MenuItem(u8"Полный экран", NULL, &V.UI.Fullscreen)) {FullscreenChanged = 1;}

        ImGui::Separator();
        ImGui::Text("GUI %.1f FPS",ImGui::GetIO().Framerate);

        ImGui::Separator();
        ImGui::Text(u8"Обработка %.1f FPS", V.Info.FPS);

        static time_t now;
        now = time(NULL);
        static char time_buff[64];
        strftime(time_buff, 64, "%H:%M:%S %d.%m.%Y ", gmtime(&now));

        ImGui::Separator();
        ImGui::Text("%s", time_buff);

        //MenuBarStateList();

        SameLine(GetWindowWidth() - 57);
        if (ImGui::MenuItem(u8"Выход")){openPopUp(PU_EXIT);};

    ImGui::EndMainMenuBar();
    }
}

int catSelected = 0;

void GUI_t::drawMatWindows(void){
    for(int i=0; i < NR_CAT; i++){
            if (setCats[i].p_open){
                if((!setCats[i].matToShow.empty()) && (setCats[i].matID != -1)){
                    setCats[i].write.lock();
                    switch (setCats[i].matToShow.type()) {
                        case 0:  {cv::cvtColor(setCats[i].matToShow, frameRGBA[i], cv::COLOR_GRAY2RGBA); break;}
                        case 16: {cv::cvtColor(setCats[i].matToShow, frameRGBA[i], cv::COLOR_BGR2RGBA); break;}
                        case 24: {cv::cvtColor(setCats[i].matToShow, frameRGBA[i], cv::COLOR_BGRA2RGBA); break;}
                    }
                    setCats[i].write.unlock();

                    image[i].create(frameRGBA[i].cols, frameRGBA[i].rows, frameRGBA[i].ptr());
                    texture[i].setSmooth(true);
                    texture[i].loadFromImage(image[i]);

                    float initSizeX = texture[i].getSize().x;
                    float initSizeY = texture[i].getSize().y;
                    float aspRatio =  initSizeX / initSizeY;

                    float videoW, videoH;

                    //ImGui::SetNextWindowSizeConstraints()
                    ImGui::Begin(setCats[i].name.c_str(), &setCats[i].p_open , 0);
                        float winAspRatio = ImGui::GetWindowContentRegionMax().x / ImGui::GetWindowContentRegionMax().y;

                        if (winAspRatio < aspRatio){
                            videoW = ImGui::GetWindowContentRegionMax().x - ImGui::GetStyle().ItemSpacing.x*2;
                            videoH = videoW / aspRatio;
                        }

                        else {
                            videoH = ImGui::GetWindowContentRegionMax().y - ImGui::GetStyle().ItemSpacing.y*2;
                            videoW = videoH * aspRatio;
                        }

                        ImGui::Image(texture[i], ImVec2(videoW, videoH));

                    ImGui::End();


            }

                else {
                    ImGui::Begin(setCats[i].name.c_str(), &setCats[i].p_open, ImGuiWindowFlags_AlwaysAutoResize);
                        ImGui::Text(u8"Изображение отсутстует!");
                        ImGui::Text(u8"(обоработка не начата)");
                    ImGui::End();
                }
            }


    }

    ImGui::Text("empty = %u, id = %i, open = %u", setCats[catSelected].matToShow.empty(), setCats[catSelected].matID, setCats[catSelected].p_open);

}

int thismarker = 0;


ImVec4 colByFrac(int pos, int max){
    float part = float(pos) / (float)max * 1.0f;
    ImVec4 ret = (ImVec4)ImColor::HSV(part, 0.8f, 0.5f);
    return ret;
}

int barWinH = 500;

cv::Mat *matPtr;
cv::Mat smallMat, smallMatRGBA;
sf::Image smallImage;
sf::Sprite smallSprite;
sf::Texture smallTexture;
std::mutex smallMutex;

void alignCenter(float width){
    ImGui::Dummy(ImVec2(0.0f, 0.0f));
    ImGui::SameLine(ImGui::GetWindowContentRegionMax().x / 2 - width / 2);
}


void GUI_t::drawMatBar(void){
    ImGuiStyle& style = ImGui::GetStyle();
    int i = catSelected;

    if((!setCats[i].matToShow.empty()) && (setCats[i].matID != -1)){
        setCats[i].write.lock();
        switch (setCats[i].matToShow.type()) {
            case 0:  {cv::cvtColor(setCats[i].matToShow, frameRGBA[i], cv::COLOR_GRAY2RGBA); break;}
            case 16: {cv::cvtColor(setCats[i].matToShow, frameRGBA[i], cv::COLOR_BGR2RGBA); break;}
            case 24: {cv::cvtColor(setCats[i].matToShow, frameRGBA[i], cv::COLOR_BGRA2RGBA); break;}
        }
        setCats[i].write.unlock();

        image[i].create(frameRGBA[i].cols, frameRGBA[i].rows, frameRGBA[i].ptr());
        texture[i].setSmooth(true);
        texture[i].loadFromImage(image[i]);

        float initSizeX = texture[i].getSize().x;
        float initSizeY = texture[i].getSize().y;
        float aspRatio =  initSizeX / initSizeY;

        float videoH = ImGui::GetWindowContentRegionMax().y - style.ItemSpacing.y*2;
        float videoW = videoH * aspRatio;

        if (videoW > ImGui::GetWindowContentRegionMax().x - style.ItemSpacing.x*2){
            float videoW = ImGui::GetWindowContentRegionMax().x - style.ItemSpacing.x * 2;
            float videoH = videoW * aspRatio;
        }

        alignCenter(videoW);
        ImGui::Image(texture[i], ImVec2(videoW, videoH));//}
    }

}


#include "imgui_internal.h"

void pushNextDisabledIf(bool isOff){
    if (isOff){
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
    }
}

void popNextDisabledIf(bool isOff){
    if (isOff){
        ImGui::PopItemFlag();
        ImGui::PopStyleVar();
    }
}


void GUI_t::shortcutCapture(void){
    if  (!V.Input.CaptureRun) Img.startCapture();
    else openPopUp(PU_SOURCE_CLOSE);
}

void GUI_t::shortcutConnect(void){
    if (!COM.isOpen && !COM.connectionOk) COM.tryConnect();
    if (COM.isOpen && !COM.connectionOk) COM.Shake();
    if (COM.isOpen && COM.connectionOk) openPopUp(PU_COMM_STOP);

}

void GUI_t::shortcutProcessing(void){
    if (V.Input.CaptureRun){
        if (!Img.procRun) Img.startProcessing();
        else openPopUp(PU_PROC_STOP);
    }
}

void GUI_t::shortcutSeparation(void){

}


void GUI_t::drawSettingsBlock(void){
    ImGui::BeginGroup();
            ImGui::BeginChild("##wfitems", ImVec2(0, 0), 1, 0);
            switch (catSelected) {
                case CAT_INPUT: {
                    ImGui::Text(u8"Настройки источника изображения");
                    ImGui::Separator();
                        if  (!V.Input.CaptureRun) {
                                if (ImGui::Button(u8"Открыть", ImVec2(60,22))) {
                                    Img.startCapture();
                                }
                            }

                            else {
                                if (ImGui::Button(u8"Закрыть", ImVec2(60,22))) {
                                    openPopUp(PU_SOURCE_CLOSE);
                                    //stopCapture();
                                }
                            }
                    //ImGui::EndChild();

                    ImGui::SameLine();

                    //ImGui::BeginChild("##startbtnch2", ImVec2(0,50), 0, ImGuiWindowFlags_None);
                        pushNextDisabledIf(V.Input.CaptureRun);
                            ImGui::Combo(u8"Источник картинки", &V.Input.Source, u8"Камера\0Видео\0\0");
                            if (V.Input.Source == SOURCE_VIDEO){
                                if (ImGui::Button(u8"Обзор...", ImVec2(60,22))) openFileBrowser(BROWSE_LOAD_VIDEO);
                            }
                        popNextDisabledIf(V.Input.CaptureRun);



                        GUI.ShowHelpMarker(u8"Видео с камеры или из файла \"test_video.avi\"");

                        ImGui::SliderInt(u8"Выводить каждый n-ный кадр", &Img.matLimiterTarget,  0, 127);
                        GUI.ShowHelpMarker(u8"Может ускорить быстродействие");
                    //ImGui::EndChild();



                    ImGui::Separator();

                    if (V.Input.Source == 0){
                        ImGui::Text(u8"Настройки камеры:");
                        ImGui::InputInt(u8"Номер камеры", &V.Cam.Number);
                        GUI.ShowHelpMarker(u8"По умолчанию 0");

                        //const char* reso[] = { "", "", "800x600", "1024x768", "1280x720"};
                        const long  resol[] = {320,240,640,480,800,600,1024,768,1280,720,1920,1080};
                        static int reso_curr;

                        pushNextDisabledIf(V.Input.CaptureRun);
                            if(ImGui::Combo(u8"Разрешение", &reso_curr, " 320x240\0 640x480\0 800x600\0 1024x768\0 1280x720\0 1920x1080 \0\0")){
                                V.Cam.Width  = resol[reso_curr*2];
                                V.Cam.Height = resol[reso_curr*2+1];
                                //Img.cameraOpen();
                            }
                        popNextDisabledIf(V.Input.CaptureRun);

                        GUI.ShowHelpMarker(u8"Чем меньше, тем быстрее");

                        if (ImGui::SliderFloat(u8"Частота кадров", &V.Cam.FPS,  5.0, 200.0,"%.0f")) Img.cameraUpdSettings();
                        GUI.ShowHelpMarker(u8"Поддерживаются не все скорости\nВозможная скорость зависит от экспозиции");

                        ImGui::Text(u8"Коррекция цвета:");
                        if (ImGui::SliderInt(u8"Усиление", &V.Cam.Gain,  0, 127)) Img.cameraUpdSettings();
                        GUI.ShowHelpMarker(u8"Следует устанавливать наименьшее значение во избежание шума");

                        if (ImGui::SliderInt(u8"Контраст", &V.Cam.Contrast,  0, 127)) Img.cameraUpdSettings();

                        if (ImGui::SliderInt(u8"Экспозиция", &V.Cam.Exposure,  -5, 0)) Img.cameraUpdSettings();
                        GUI.ShowHelpMarker(u8"Обратно пропорциональна к скорости");

                        if (ImGui::SliderInt(u8"Насыщенность", &V.Cam.Saturation,  0, 127)) Img.cameraUpdSettings();

                        ImGui::Checkbox(u8"Стоп-кадр", &V.Input.FreezeFrame);


                    }

                    if (V.Input.Source == SOURCE_VIDEO){
                        //Img.capture_file = V.Input.Source;

                        #warning TODO add video file list here
                        #warning TODO open video file by name

                        ImGui::Text(u8"Свойства видео файла [%s]:", Img.videoFileName.c_str());
                        ImGui::Text(u8"Разрешение: %.0f x %.0f", videoPlayer.frameW, videoPlayer.frameH);
                        ImGui::Text(u8"Разрешение: %.0f x %.0f", videoPlayer.frameW, videoPlayer.frameH);
                        //ImGui::Text(u8"с %u до %u", videoPlayer.startFrame, videoPlayer.endFrame);
                        //ImGui::Text(u8"%u, %.2f ms", videoPlayer.playbackMarker, videoPlayer.playbackMs);
                        ImGui::Text(u8"Всего %u кадров, %.1f FPS, %.1f FPS now", videoPlayer.fileLengthFrames, videoPlayer.videoFPS, videoPlayer.fps);

                        ImGui::BeginChild("##player", ImVec2(0,0), 1,0);
                        //alignCenter(100.0f);
                        ImGui::Text(u8"Плеер");
                        char pBarBuf[64];
                        sprintf(pBarBuf, u8"Кадры: %i / %i", videoPlayer.playbackMarker - videoPlayer.startFrame, videoPlayer.partLength);

                        ImGui::ProgressBar(videoPlayer.playbackPortion, ImVec2(0.f,0.f), pBarBuf);
                        ImGui::SliderInt(u8"Начало, кадр", &videoPlayer.startFrame, 0, videoPlayer.fileLengthFrames, "%u");
                        ImGui::SliderInt(u8"Конец, кадр", &videoPlayer.endFrame, videoPlayer.startFrame, videoPlayer.fileLengthFrames, "%u");
                        ImGui::Checkbox(u8"Пауза", &videoPlayer.pause);

                        ImGui::SliderFloat(u8"Ограничитель скорости", &videoPlayer.fpsLimiter, 1.0f, 200.0f, "%.1f ms");
                        if (videoPlayer.pause){
                            if (ImGui::SliderInt(u8"Позиция", &videoPlayer.playbackMarker, videoPlayer.startFrame, videoPlayer.endFrame, "%u"))
                                videoPlayer.setMarker(videoPlayer.playbackMarker);
                        }
                        ImGui::EndChild();
                    }

                    break;}

                case CAT_PROCESSING: {
                    ImGui::Text(u8"Настройки обработки изображения");
                    ImGui::Separator();

                    ImGui::Combo(u8"Основной алгоритм работы", &V.procType, u8"Водопад\0Ремень\0Калибровка\0\0");
                    ImGui::Separator();

                    ImGui::Text(u8"Настройки препроцессора изображения:");
                    pushNextDisabledIf(Img.procRun);
                            if (ImGui::Checkbox("on", &preprocessor.isOn)) Img.initProcessor(V.procType);

                        const char* items[] = { u8"0", u8"90 ЧС", u8"180", u8"90 ПЧС"};
                        static int item_current = 0;
                        ImGui::Combo(u8"Поворот", &item_current, items, IM_ARRAYSIZE(items));
                        preprocessor.rotation = item_current - 1;
                    popNextDisabledIf(Img.procRun);

                    ImGui::SliderInt("marginLeft", &preprocessor.marginLeft, 0, preprocessor.frameW/2-1, "%u pix");
                    ImGui::SliderInt("marginRight", &preprocessor.marginRight, 0, preprocessor.frameW/2-1, "%u pix");
                    ImGui::SliderInt("marginUp", &preprocessor.marginUp, 0, preprocessor.frameH/2-1, "%u pix");
                    ImGui::SliderInt("marginDown", &preprocessor.marginDown, 0, preprocessor.frameH/2-1, "%u pix");

                    ImGui::SliderFloat("brightness", &preprocessor.brightness, 0.0f, 5.0f, "%.2f");
                    ImGui::SliderFloat("contrast", &preprocessor.contrast, 0.0f, 5.0f, "%.2f");
                    ImGui::SliderFloat("saturation", &preprocessor.saturation, 0.0f, 5.0f, "%.2f");
                    ImGui::SliderInt("sharpTimes", &preprocessor.sharpTimes, 1, 10, "%u");

                    break;}

                case CAT_WF_CONTOURS: {
                    ImGui::Text(u8"Водопад: настройки размеров частиц");
                    ImGui::Separator();

                    ImGui::DragFloat(u8"min площадь рамки", &V.Contours.MinBBoxArea, 1.0, 1.0, V.Contours.MaxBBoxArea, "%.0f");
                    //ImGui::SliderFloat(, ,  0.0f, 100.0f+V.Contours.WideAreaRange*10000.0f, "%.0f");
                    GUI.ShowHelpMarker(u8"(pix^2)");
                    //ImGui::SliderFloat(u8"max площадь рамки", &V.Contours.MaxBBoxArea,  0.0f, 10000.0f+V.Contours.WideAreaRange*20000.0f, "%.0f");
                    ImGui::DragFloat(u8"max площадь рамки", &V.Contours.MaxBBoxArea, 1.0, V.Contours.MinBBoxArea, 500000000.0, "%.0f");
                    GUI.ShowHelpMarker(u8"(pix^2)");
                break;}

                case CAT_WF_EDGE: {
                    ImGui::Text(u8"Водопад: настройки поиска контуров");
                    ImGui::Separator();

                    ImGui::Combo(u8"Алгоритм поиска", &V.Contours.CurrentAlgo, u8"Canny\0Scharr\0\0");
                    ImGui::Separator();
                    if (V.Contours.CurrentAlgo == 0){
                        V.Edge.UseScharr=0;
                        ImGui::SliderInt(u8"Размер размытия", &V.Edge.BlurValue,  1, 5);
                        GUI.ShowHelpMarker(u8"Предварительное размытие помогает избавиться от мусора\nНечётные значения работают лучше");
                        ImGui::SliderInt(u8"Порог 1", &V.Edge.CannyThresh1, 1, 500);
                        ImGui::SliderInt(u8"Порог 2", &V.Edge.CannyThresh2, 1, 500);
                        GUI.ShowHelpMarker(u8"По умолчанию порог 2 примерно в два раза больше порога 1");
                    }

                    if (V.Contours.CurrentAlgo == 1){
                        ImGui::Text(u8"(добавить сюда настройки Щарра!)");
                        V.Edge.UseScharr=1;
                        //ImGui::SliderInt(u8"Размытие", &test_int,  1, 2);
                        //ImGui::InputFloat(u8"Порог", &f, 1,2, .1f);
                    }
                    break;}

                case CAT_WF_MORPHO: {
                    ImGui::Text(u8"Водопад: настройки морфологического преобразования");
                    ImGui::Separator();

                    ImGui::Combo(u8"Тип морфо преобразования", &V.Morph.Type, u8"RECT\0ELLIPSE\0CROSS\0\0");
                    GUI.ShowHelpMarker(u8"Вид корневого элемента морфологического преобразования\nRECT самый быстрый, ELLIPSE немного медленнее, но более реалистичный");
                    ImGui::SliderInt(u8"Размер зерна", &V.Morph.Size,  1, 10);
                    GUI.ShowHelpMarker(u8"Размер корневого элемента морфологического преобразования\nМаленький работает более тонко, большой /""залепляет/"" маску грубее");
                    break;}

                case CAT_WF_COLOR: {
                    ImGui::Text(u8"Водопад: настройки цвета частиц");
                    ImGui::Separator();

                    static float tolh = (float)V.Color.ToleranceHSV.val[0];
                    static float tols = (float)V.Color.ToleranceHSV.val[1];
                    static float tolv = (float)V.Color.ToleranceHSV.val[2];

                    static float tolr = (float)V.Color.ToleranceRGB.val[2];
                    static float tolg = (float)V.Color.ToleranceRGB.val[1];
                    static float tolb = (float)V.Color.ToleranceRGB.val[0];

                    if(ImGui::ColorPicker3(u8"Правильный цвет", (float*)&test_color)){
                        float hue, saturation, value;
                        ImGui::ColorConvertRGBtoHSV(test_color[0], test_color[1], test_color[2], hue, saturation, value);
                        V.Color.GoodHSV.val[0] = hue*180.0;
                        V.Color.GoodHSV.val[1] = saturation*255.0;
                        V.Color.GoodHSV.val[2] = value*255.0;
                        V.Color.GoodRGB.val[2] = test_color[0]*255.0;
                        V.Color.GoodRGB.val[1] = test_color[1]*255.0;
                        V.Color.GoodRGB.val[0] = test_color[2]*255.0;
                    }

                    ImGui::Checkbox(u8"Цветовое пространство допуска", &V.Color.GoodSpace);
                    GUI.ShowHelpMarker(u8"Использовать HSV или RGB допуск");
                    if(V.Color.GoodSpace){
                        if (ImGui::SliderFloat(u8"Допуск H", &tolh, 0.0f, 180.0f, "%3.1f"))   V.Color.ToleranceHSV.val[0] = (double)tolh;
                        if (ImGui::SliderFloat(u8"Допуск S", &tols, 0.0f, 255.0f, "%3.1f"))   V.Color.ToleranceHSV.val[1] = (double)tols;
                        if (ImGui::SliderFloat(u8"Допуск V", &tolv, 0.0f, 255.0f, "%3.1f"))   V.Color.ToleranceHSV.val[2] = (double)tolv;
                    }
                    else {
                        if (ImGui::SliderFloat(u8"Допуск R", &tolr, 0.0f, 180.0f, "%3.1f"))   V.Color.ToleranceRGB.val[2] = (double)tolr;
                        if (ImGui::SliderFloat(u8"Допуск G", &tolg, 0.0f, 255.0f, "%3.1f"))   V.Color.ToleranceRGB.val[1] = (double)tolg;
                        if (ImGui::SliderFloat(u8"Допуск B", &tolb, 0.0f, 255.0f, "%3.1f"))   V.Color.ToleranceRGB.val[0] = (double)tolb;
                    }
                    break;}

                case CAT_WF_BS: {
                    ImGui::Text(u8"Водопад: настройки алгоритма отделения фона");
                    ImGui::Separator();

                    if(ImGui::Combo(u8"Алгоритм", &V.BS.CurrentAlgo, u8"MOG\0MOG2\0CNT\0\0")){Img.BS_Init(V.BS.CurrentAlgo);}
                    GUI.ShowHelpMarker(u8"MOG и MOG2 не нагружают процессор, но не очень точны\nCNT наиболее быстрый, но имеет низкое быстродействие");
                    ImGui::SliderInt(u8"Предварительное размытие", &V.BS.BlurBeforeMog,  1, 10);
                    GUI.ShowHelpMarker(u8"Помогает избавиться от шелухи в маске вычитания фона");
                    ImGui::Separator();

                    if (V.BS.CurrentAlgo==BS_MOG){
                        ImGui::Checkbox(u8"Обучение", &V.BS.MOG.Learning);
                        GUI.ShowHelpMarker(u8"Создавать модель фона");
                        ImGui::SliderInt(u8"История", &V.BS.MOG.History,  1, 300);
                        ImGui::SliderInt(u8"Преобразований", &V.BS.MOG.Mixtures,  1, 10);
                        ImGui::SliderFloat(u8"Background ratio", &V.BS.MOG.BackRatio, 0.01f, 0.99f, "%1.2f");
                        ImGui::SliderFloat(u8"Скорость обучения", &V.BS.MOG.LRate, 0.01f, 0.99f, "%1.2f");
                        ImGui::SliderFloat(u8"Noise sigma", &V.BS.MOG.NoiseSigma, 0.0f, 1.0f, "%1.2f");
                    }

                    if (V.BS.CurrentAlgo==BS_MOG2){
                        ImGui::Checkbox(u8"Обучение", &V.BS.MOG2.Learning);
                        GUI.ShowHelpMarker(u8"Создавать модель фона");
                        ImGui::SliderInt(u8"История", &V.BS.MOG2.History,  1, 300);
                        ImGui::SliderFloat(u8"Порог", &V.BS.MOG2.Thresh, 0.1f, 200.0f, "%1.1f");
                        ImGui::SliderFloat(u8"Скорость обучения", &V.BS.MOG2.LRate, 0.01f, 0.99f, "%1.2f");
                        ImGui::Checkbox(u8"Искать тени", &V.BS.MOG2.DetectShadows);
                    }

                    if (V.BS.CurrentAlgo==BS_CNT){
                        ImGui::Checkbox(u8"Обучение", &V.BS.CNT.Learning);
                        GUI.ShowHelpMarker(u8"Должно всегда быть включено");
                        ImGui::SliderInt(u8"Мин. стабильных кадров", &V.BS.CNT.MinPixStability,  1, 200);
                        GUI.ShowHelpMarker(u8"Через сколько кадров пиксель считается фоновым");
                        ImGui::SliderInt(u8"Макс. стабильных кадров", &V.BS.CNT.MaxPixStability,  1, 200);
                        GUI.ShowHelpMarker(u8"Максимальный рейтинг пикселя в истории");
                        ImGui::SliderInt(u8"Целевая частота кадров", &V.BS.CNT.FPS,  1, 100);
                        ImGui::Checkbox(u8"Использовать историю", &V.BS.CNT.UseHistory);
                        ImGui::Checkbox(u8"Работать параллельно", &V.BS.CNT.IsParallel);
                        GUI.ShowHelpMarker(u8"Иногда ускоряет, иногда бесполезно");
                        ImGui::SliderFloat(u8"Скорость обучения", &V.BS.CNT.LRate, 0.01f, 0.99f, "%1.2f");
                        GUI.ShowHelpMarker(u8"Скорость создания модели фона");
                    }
                    /*
                    if (V.BS.CurrentAlgo==BS_KNN){
                        ImGui::Checkbox(u8"Обучение", &Img.bs_knn_learning);
                        ImGui::SliderInt(u8"История", &Img.bs_knn_history,  1, 300);
                        ImGui::SliderFloat(u8"Порог", &Img.bs_knn_thresh, 0.0f, 1.0f, "%1.2f");
                        ImGui::SliderFloat(u8"Скорость обучения", &Img.bs_knn_lrate, 0.01f, 0.99f, "%1.2f");
                        ImGui::Checkbox(u8"Искать тени", &Img.bs_knn_shadows);

                    }

                    if (V.BS.CurrentAlgo==BS_GSOC){
                        ImGui::Checkbox(u8"Обучение", &Img.bs_gsoc_learning);
                        ImGui::SliderFloat(u8"Скорость обучения", &Img.bs_gsoc_lrate, 0.01f, 0.99f, "%1.2f");
                        ImGui::SliderInt(u8"Компенсация движения камеры", &Img.bs_gsoc_mc,  0, 1);
                        ImGui::SliderInt(u8"Образцов", &Img.bs_gsoc_samples,  1, 200);
                        ImGui::SliderFloat(u8"Propagation rate", &Img.bs_gsoc_proprate, 0.0f, 1.0f, "%1.3f");
                        ImGui::SliderFloat(u8"Replace rate", &Img.bs_gsoc_reprate, 0.0f, 1.0f, "%1.3f");
                        ImGui::SliderInt(u8"Hits threshold", &Img.bs_gsoc_hits_thresh,  1, 200);
                        ImGui::SliderFloat(u8"alpha", &Img.bs_gsoc_alpha, 0.0f, 1.0f, "%1.3f");
                        ImGui::SliderFloat(u8"beta", &Img.bs_gsoc_beta, 0.0f, 1.0f, "%1.3f");
                        ImGui::SliderFloat(u8"blinkingSupressionDecay ", &Img.bs_gsoc_bs_decay, 0.0f, 1.0f, "%1.3f");
                        ImGui::SliderFloat(u8"blinkingSupressionMultiplier ", &Img.bs_gsoc_bs_mul, 0.0f, 1.0f, "%1.3f");
                        ImGui::SliderFloat(u8"noiseRemovalThresholdFacBG ", &Img.bs_gsoc_noise_bg, 0.0f, 1.0f, "%1.3f");
                        ImGui::SliderFloat(u8"noiseRemovalThresholdFacFG ", &Img.bs_gsoc_noise_fg, 0.0f, 1.0f, "%1.3f");
                    */

                    if (ImGui::Button(u8"Применить настройки")) {Img.BS_Init(V.BS.CurrentAlgo);}
                    GUI.ShowHelpMarker(u8"Некоторые настройки не применяются автоматически");
                break;}

                //case CAT_WF_BS_MOG: {break;}
                //case CAT_WF_BS_MOG2: {break;}
                //case CAT_WF_BS_CNT: {break;}

                case CAT_WF_SHOW: {
                    ImGui::Text(u8"Водопад: настройки HUD");
                    ImGui::Separator();

                    ImGui::Checkbox(u8"Контуры", &V.Show.Contours);
                    ImGui::Checkbox(u8"Центры", &V.Show.Centers);
                    ImGui::Checkbox(u8"Коробки", &V.Show.BBoxes);
                    ImGui::Checkbox(u8"Площадь", &V.Show.Area);
                    ImGui::Checkbox(u8"Диаметр", &V.Show.Diameter);
                    ImGui::Checkbox(u8"Залить контуры средним цветом", &V.Show.FillAvg);
                    ImGui::Checkbox(u8"Залить контуры результатом", &V.Show.FilContour);
                    break;}

                case CAT_B_COLOR: {
                    ImGui::Text(u8"Ремень: настройки цвета");
                    ImGui::Separator();

                    ImGui::Text(u8"Оттенок (H):");
                        ImGui::SliderInt(u8"H min", &BeltProcessor.low_H, 0, BeltProcessor.max_H, "%u");
                        ImGui::SliderInt(u8"H max", &BeltProcessor.high_H, BeltProcessor.low_H, BeltProcessor.max_H, "%u");

                    ImGui::Text(u8"Насыщенность (S):");
                        ImGui::SliderInt(u8"S min", &BeltProcessor.low_S, 0, BeltProcessor.max_S, "%u");
                        ImGui::SliderInt(u8"S max", &BeltProcessor.high_S, BeltProcessor.low_S, BeltProcessor.max_S, "%u");

                    ImGui::Text(u8"Значение (V):");
                        ImGui::SliderInt(u8"V min", &BeltProcessor.low_V, 0, BeltProcessor.max_V, "%u");
                        ImGui::SliderInt(u8"V max", &BeltProcessor.high_V, BeltProcessor.low_V, BeltProcessor.max_V, "%u");

                    break;}

                case CAT_B_SIZE: {
                    ImGui::Text(u8"Ремень: настройки размеров частиц");
                    ImGui::Separator();
                    ImGui::DragFloat(u8"min площадь коробки", &BeltProcessor.minArea, 1.0, 1.0, 100000.0, "%.0f");
                    ImGui::DragFloat(u8"max площадь коробки", &BeltProcessor.maxArea, 1.0, BeltProcessor.minArea, 100000.0, "%.0f");
                    break;}

                case CAT_B_MORPH: {
                    ImGui::Text(u8"Ремень: настройки морфологического преобразования");
                    ImGui::Separator();
                    ImGui::Combo(u8"Тип морфо преобразования", &BeltProcessor.morphType,
                                 u8"MORPH_ERODE\0MORPH_DILATE\0MORPH_OPEN\0MORPH_CLOSE\0MORPH_GRADIENT\0MORPH_TOPHAT\0MORPH_BLACKHAT\0MORPH_HITMISS\0\0");
                    ImGui::Combo(u8"Тип корневого элемента", &BeltProcessor.morphShape, u8"RECT\0ELLIPSE\0CROSS\0\0");
                    GUI.ShowHelpMarker(u8"Вид корневого элемента морфологического преобразования\nRECT самый быстрый, ELLIPSE немного медленнее, но более реалистичный");
                    ImGui::SliderInt(u8"Размер зерна", &BeltProcessor.morphArea,  1, 10);
                    GUI.ShowHelpMarker(u8"Размер корневого элемента морфологического преобразования\nМаленький работает более тонко, большой /""залепляет/"" маску грубее");
                    break;}

                case CAT_B_BLUR: {
                    ImGui::Text(u8"Ремень: настройки размытия");
                    ImGui::Separator();
                    ImGui::SliderInt(u8"Размер размытия", &BeltProcessor.blurSize,  1, 10);
                    break;}

                case CAT_B_ACCUM: {
                    ImGui::Text(u8"Ремень: настройки аккумулятора");
                    ImGui::Separator();
                    ImGui::SliderFloat(u8"saturationWeight", &BeltProcessor.saturationWeight, 0.0, 1.0, "%.2f");
                    ImGui::SliderInt(u8"satRangeMin", &BeltProcessor.satRangeMin,  0, 255);
                    ImGui::SliderInt(u8"satRangeMax", &BeltProcessor.satRangeMax,  0, 255);

                break;}

                case CAT_B_INFO: {
                    ImGui::DragFloat(u8"mmPerFrame", &BeltProcessor.mmPerFrame, 0.01, 0.0, 10.0, "%.2f");
                    break;}

                case CAT_COM: {

                    ImGui::BeginChild("##commain", ImVec2(ImGui::GetWindowContentRegionWidth()*0.8, -1), true);

                    static int com_selected = 0;

                    ImGui::Text(u8"Настройки коммуникации с аппартным обеспечением");
                    ImGui::Separator();

                    if (COM.isOpen) ImGui::Text(u8"Порт: открыт");
                    else ImGui::Text(u8"Порт: закрыт");

                    if (COM.connectionOk) ImGui::Text(u8"Соединение с блоком управления: есть");
                    else ImGui::Text(u8"Соединение с блоком управления: нет");
                    ImGui::Separator();

                    pushNextDisabledIf(COM.isOpen);
                            if (ImGui::Button(u8"Сканировать порты")) {COM.List();}
                            GUI.ShowHelpMarker(u8"Вывести список первых 16 доступных портов");

                            static char lastPortName[16];
                            sprintf(lastPortName, "COM%d", V.ComPort.Number+1);

                            if (ImGui::BeginCombo(u8"Порт", lastPortName, 0)){
                                for (int n = 0; n < 16; n++){
                                        if (COM.IsPresent[n]){
                                            char label[32];
                                            sprintf(label, "COM%d", n+1);
                                            if (ImGui::Selectable(label, com_selected == n)){
                                                com_selected = n;
                                                V.ComPort.Number = com_selected;
                                            }
                                        }
                                }
                                ImGui::EndCombo();
                            }

                        const char* comspeeds[] = { "9 600", "57 600", "115 200", "250 000", "1M", "2M"};
                        const long  com_speeds[] = { 9600 ,57600, 115200, 250000, 1000000, 2000000};
                        static int speed_current = 0;

                            if (ImGui::Combo(u8"Скорость", &speed_current, comspeeds, IM_ARRAYSIZE(com_speeds)))
                                V.ComPort.Speed = com_speeds[speed_current];
                        popNextDisabledIf(COM.isOpen);


                        if(!COM.isOpen)     {if (ImGui::Button(u8"Подключиться")){COM.tryConnect();}}
                        else                {if (ImGui::Button(u8"Отключиться")){openPopUp(PU_COMM_STOP);}}

                    ImGui::EndChild();

                    ImGui::SameLine();

                    ImGui::BeginChild("##comsonsole", ImVec2(-1, -1), true);
                        drawComConsole();
                    ImGui::EndChild();
                    break;}

                case CAT_UI: {break;}
                case CAT_STATS: {
                    ImGui::Text(u8"Статистика обработки");
                    ImGui::Separator();

                    static float fps_values[100] = {0};
                    static float contours_values[100] = {0};
                    static float good_contours_values[100] = {0};

                        static int values_offset = 0;
                        static double refresh_time = 0.0;
                        if (refresh_time == 0.0f) refresh_time = ImGui::GetTime();
                        while (refresh_time < ImGui::GetTime()) // Create dummy data at fixed 60 hz rate for the demo
                        {
                            fps_values[values_offset] = V.Info.FPS;
                            contours_values[values_offset] = (float)Img.info_total_contours;
                            good_contours_values[values_offset] = (float)Img.good_contours;

                            values_offset = (values_offset+1) % IM_ARRAYSIZE(fps_values);
                            refresh_time += 1.0f/5.0f;
                        }
                    ImGui::Spacing();
                        char fps_buf[8];
                        sprintf(fps_buf,"%2.1f",  V.Info.FPS);
                        ImGui::PlotLines("FPS", fps_values, IM_ARRAYSIZE(fps_values), values_offset, fps_buf, 0.0f, *std::max_element(std::begin(fps_values), std::end(fps_values)), ImVec2(0,80));
                    ImGui::Spacing();
                        char cont_buf[8];
                        sprintf(cont_buf,"%lu",  Img.info_total_contours);
                        ImGui::PlotLines(u8"Контуров", contours_values, IM_ARRAYSIZE(contours_values), values_offset, cont_buf, 0.0f, *std::max_element(std::begin(contours_values), std::end(contours_values)), ImVec2(0,80));
                    ImGui::Spacing();
                        char useful_cont_buf[8];
                        sprintf(useful_cont_buf,"%lu",Img.good_contours);
                        ImGui::PlotLines(u8"Полезных контуров", good_contours_values, IM_ARRAYSIZE(good_contours_values), values_offset, useful_cont_buf, 0.0f, 70.0f, ImVec2(0,80));
                    ImGui::Spacing();
                    ImGui::SliderInt(u8"Усреднять FPS обработки", &Img.fps_average,  1, 127);
                    //ImGui::Checkbox(u8"Стоп-кадр", &V.Input.FreezeFrame);

                    break;}

                case CAT_CALIB: {
                    ImGui::Text(u8"Калибровка камеры");
                    ImGui::Separator();

                    pushNextDisabledIf(!Calib.dataReady);
                        ImGui::Checkbox(u8"Включить коррекцию искажений", &Calib.undistIsOn);
                    popNextDisabledIf(!Calib.dataReady);

                    ImGui::Separator();

                    ImGui::Text(u8"Параметры калибровочного поля:");
                    ImGui::InputInt(u8"Углов по горизонтали", &Calib.calibData.numCornersHor);
                    ImGui::InputInt(u8"Углов по вертикали", &Calib.calibData.numCornersHor);
                    ImGui::InputFloat(u8"Размер по горизонтали, мм", &Calib.calibData.boardXmm, 0.1f, .1f, "%.1f", 0);
                    ImGui::InputFloat(u8"Размер по вертикали, мм", &Calib.calibData.boardYmm, 0.1f, .1f, "%.1f", 0);
                    ImGui::InputInt(u8"numBoards", &Calib.numBoards);

                    ImGui::Separator();


                    ImGui::Text(u8"Инструкция по калибровке:\n 1. Для первого снимка размещать табличку в середине кадра\n 2. Для дальнейших снимков размещать табличку во всех углах кадра");
                    ImGui::Separator();

                    ImGui::Text(u8"Результаты калибровки:");
                    ImGui::Text(u8"Разрашение:\nX = %.3f mm/pix\nY = %.3f mm/pix\nX\Y = %.3f%%",     Calib.calibData.resolutionX,
                                                                                        Calib.calibData.resolutionY,
                                                                                        (Calib.calibData.resolutionX / Calib.calibData.resolutionY)*100.0f
                    );

                    //char textBuf[32];
                    //sprintf(textBuf, u8"Записано изображений: %u из %u", Calib.successes, Calib.numBoards);
                    ImGui::ProgressBar(((float)Calib.successes/(float)Calib.numBoards));

                    //if (ImGui::Button(u8"Начать калибровку")) Calib.

                    if (Calib.chessFound){
                        if (ImGui::Button(u8"Следующая точка")) Calib.calibNext = 1;
                    }


                    break;}

                case CAT_HW: {
                    //ImVec2 diagramSize = ImVec2(ImGui::GetWindowContentRegionMax().x - 10, ImGui::GetWindowContentRegionMax().y - 10);


                    /*
                    ImDrawList* dList = ImGui::GetWindowDrawList();
                    ImVec2 p = ImGui::GetCursorScreenPos();
                    float x = p.x + 4.0f, y = p.y + 4.0f;
                    const ImU32 col = ImColor(ImVec4(0.3f, 0.3f, 0.3f, 1.0f));

                    ImVec2 diagramBox[2] = {ImVec2(p.x, p.y), ImVec2(p.x + diagramSize.x, p.y + diagramSize.y)};

                    float hopperW = (diagramSize.x - padding*4) / 3;
                    float hopperH = (diagramSize.y - padding*6) * 0.3;
                    ImVec2 hopperBox[2] = {ImVec2(p.x +padding , p.y+padding), ImVec2(p.x + hopperW + padding, p.y + hopperH+padding)};

                    //static float feederW = 100;
                    float feederH = (diagramSize.y - padding*6) * 0.1;
                    ImVec2 feederBox[2] = {ImVec2(hopperBox[0].x, hopperBox[1].y+padding), ImVec2(hopperBox[1].x, hopperBox[1].y+padding + feederH)};

                    float vertCamW = hopperW;
                    ImVec2 vertCamBox[2] = {ImVec2(feederBox[1].x + padding, hopperBox[0].y), ImVec2(feederBox[1].x + padding + vertCamW, feederBox[1].y)};



                    float ejH = (diagramSize.y - padding*6) * 0.1;
                    float ejW = hopperW;
                    ImVec2 ejectorBox[2] = {ImVec2(vertCamBox[1].x+padding, vertCamBox[1].y - ejH), ImVec2(vertCamBox[1].x + padding + ejW, vertCamBox[1].y)};

                    float transpH = (diagramSize.y - padding*6) * 0.2;
                    ImVec2 transpBox[2] = { ImVec2(feederBox[1].x + padding - (feederBox[1].x - feederBox[0].x)/2, feederBox[1].y+padding),
                                            ImVec2(ejectorBox[1].x,ejectorBox[1].y+padding + transpH)};



                    float horScreenW= 20.0f;
                    float horCamH = (diagramSize.y - padding*6) * 0.3;

                    ImVec2 vertScreenBox[2] = { ImVec2(vertCamBox[0].x, transpBox[0].y + padding),
                                                ImVec2(vertCamBox[1].x, transpBox[0].y + padding + horScreenW)};

                    ImVec2 horCamBox[2] = {  ImVec2(ejectorBox[0].x,                           transpBox[1].y + padding),
                                                    ImVec2(transpBox[1].x - horScreenW - padding,   transpBox[1].y+padding + horCamH)};

                    ImVec2 horScreenBox[2] = {ImVec2(transpBox[1].x - horScreenW, horCamBox[0].y), ImVec2(transpBox[1].x,horCamBox[1].y)};

                    float nozW = ejW;
                    float nozH = (diagramSize.y - padding*6) * 0.1;
                    ImVec2 nozzleBox[2] = {  ImVec2(horScreenBox[1].x-nozW, horScreenBox[1].y + padding),
                                                    ImVec2(horCamBox[1].x, horScreenBox[1].y + padding + nozH)};


                    dList->AddRect(diagramBox[0], diagramBox[1], col, 0.0f, ImDrawCornerFlags_All, 1);
                    dList->AddRect(hopperBox[0], hopperBox[1], col, 0.0f, ImDrawCornerFlags_All, 1);
                        const ImU32 hopperCol = ImColor(ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
                        dList->AddRect(hopperBox[0], ImVec2(hopperBox[1].x, hopperBox[1].y*0.9), hopperCol, 0.0f,  ImDrawCornerFlags_All, 2);

                        dList->AddLine(ImVec2(hopperBox[0].x,  hopperBox[1].y * 0.9),
                                       ImVec2(hopperBox[0].x+(hopperBox[1].x-hopperBox[0].x)/2, hopperBox[1].y), hopperCol, 2);

                        dList->AddLine(ImVec2(hopperBox[0].x+(hopperBox[1].x-hopperBox[0].x)/2, hopperBox[1].y),
                                       ImVec2(hopperBox[1].x, hopperBox[1].y*0.9), hopperCol, 2);

                    dList->AddRect(feederBox[0], feederBox[1], col, 0.0f, ImDrawCornerFlags_All, 1);
                        const ImU32 feederCol = ImColor(ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
                        dList->AddLine(feederBox[0], ImVec2(feederBox[0].x, feederBox[1].y), feederCol, 2);
                        dList->AddLine(ImVec2(feederBox[0].x, feederBox[1].y), feederBox[1], feederCol, 2);
                        dList->AddLine(feederBox[0], ImVec2(feederBox[1].x, feederBox[1].y*0.9), feederCol, 2);
                        dList->AddLine(ImVec2(feederBox[1].x, feederBox[1].y*0.9), feederBox[1], feederCol, 2);
                        //ImGui::SetCursorPos(ImVec2(feederBox[0].x - p.x, feederBox[0].y - p.y));
                        //ImGui::Button ("testing");

                    dList->AddRect(vertCamBox[0], vertCamBox[1], col, 0.0f, ImDrawCornerFlags_All, 1);
                        const ImU32 camCol = ImColor(ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
                        ImVec2 camStartPt = ImVec2(vertCamBox[0].x + (vertCamBox[1].x - vertCamBox[0].x)/2, vertCamBox[0].y);

                        dList->AddLine(camStartPt, ImVec2(camStartPt.x - 20, camStartPt.y+50), camCol, 2);
                        dList->AddLine(camStartPt, ImVec2(camStartPt.x + 20, camStartPt.y+50), camCol, 2);
                        dList->AddLine(ImVec2(camStartPt.x - 20, camStartPt.y+50), ImVec2(camStartPt.x + 20, camStartPt.y+50), camCol, 2);

                    dList->AddRect(ejectorBox[0], ejectorBox[1], col, 0.0f, ImDrawCornerFlags_All, 1);

                    dList->AddRect(vertScreenBox[0], vertScreenBox[1], col, 0.0f, ImDrawCornerFlags_All, 1);
                    dList->AddRect(transpBox[0], transpBox[1], col, 0.0f, ImDrawCornerFlags_All, 1);
                        const ImU32 transpCol = ImColor(ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
                        float circRadius = (transpBox[1].y - transpBox[0].y) / 2;
                        dList->AddCircle(ImVec2(transpBox[0].x + circRadius, transpBox[0].y + circRadius), circRadius, transpCol, 32, 3);
                        dList->AddCircle(ImVec2(transpBox[1].x - circRadius - horScreenW - padding, transpBox[1].y - circRadius), circRadius, transpCol, 32, 3);
                        dList->AddLine( ImVec2(transpBox[0].x + circRadius, transpBox[0].y),
                                        ImVec2(transpBox[1].x - circRadius -  horScreenW - padding, transpBox[0].y), camCol, 2);

                        dList->AddLine( ImVec2(transpBox[0].x + circRadius, transpBox[1].y),
                                        ImVec2(transpBox[1].x - circRadius  - horScreenW - padding, transpBox[1].y), camCol, 2);

                    dList->AddRect(horCamBox[0], horCamBox[1], col, 0.0f, ImDrawCornerFlags_All, 1);
                        camStartPt = ImVec2(horCamBox[0].x, horCamBox[0].y + (horCamBox[1].y - horCamBox[0].y)/2);

                        dList->AddLine(camStartPt, ImVec2(camStartPt.x + 50, camStartPt.y+20), camCol, 2);
                        dList->AddLine(camStartPt, ImVec2(camStartPt.x + 50, camStartPt.y-20), camCol, 2);
                        dList->AddLine(ImVec2(camStartPt.x + 50, camStartPt.y-20),
                                       ImVec2(camStartPt.x + 50, camStartPt.y+20), camCol, 2);

                    dList->AddRect(horScreenBox[0], horScreenBox[1], col, 0.0f, ImDrawCornerFlags_All, 1);

                    dList->AddRect(nozzleBox[0], nozzleBox[1], col, 0.0f, ImDrawCornerFlags_All, 1);


                    */


                    static float padding = ImGui::GetStyle().ItemSpacing.y;

                    float groupW = (ImGui::GetWindowContentRegionMax().x - 10 - padding*5) / 5;
                    static float testF;

                    ImGui::BeginChild("##hopper", ImVec2(groupW, -1), true);
                        ImGui::Text("hopper");

                        if (ImGui::SliderFloat("##speed1", &hw.State.hopper.speedRevMin, 0.0f, 100.0f, "%.1f rev/min", 1.0)){
                            hw.State.hopper.speedSteps =    hw.State.hopper.microstepDivider *
                                                            ((hw.State.hopper.speedRevMin / 60.0f) * 200.0f);
                        }
                        if (ImGui::SliderInt("##speed2", &hw.State.hopper.speedSteps, 0, 1000, "%u steps/sec")){
                            hw.State.hopper.speedRevMin =   (float)hw.State.hopper.speedSteps /
                                                            (float)hw.State.hopper.microstepDivider / 200.0f * 60.0f;
                        }

                        static int hopperMicro;
                        if(ImGui::Combo("microstepping", &hopperMicro, "1\0 2\0 4\0 8\0 16\0 32\0\0")){
                            hw.State.hopper.microstepDivider = pow(2, hopperMicro);

                            hw.State.hopper.speedSteps =    hw.State.hopper.microstepDivider *
                                                            ((hw.State.hopper.speedRevMin / 60.0f) * 200.0f);

                            hw.State.hopper.speedRevMin =   (float)hw.State.hopper.speedSteps /
                                                            (float)hw.State.hopper.microstepDivider / 200.0f * 60.0f;

                        }

                        if (ImGui::Checkbox("run", &hw.State.hopper.run)){

                        }
                        if (ImGui::Checkbox("dir", &hw.State.hopper.dir)){

                        }

                        /*
                        const long  resol[] = {320,240,640,480,800,600,1024,768,1280,720,1920,1080};
                        static int reso_curr;

                        pushNextDisabledIf(V.Input.CaptureRun);
                            if(ImGui::Combo(u8"Разрешение", &reso_curr, " 320x240\0 640x480\0 800x600\0 1024x768\0 1280x720\0 1920x1080 \0\0")){
                        */

                    ImGui::EndChild();
                    ImGui::SameLine();

                    ImGui::BeginChild("##feeder", ImVec2(groupW, -1), true);
                        ImGui::Text("feeder");



                    ImGui::EndChild();
                    ImGui::SameLine();

                    ImGui::BeginChild("##transp", ImVec2(groupW, -1), true);
                        ImGui::Text("transp");

                        ImGui::SliderFloat("pulleyDiam", &hw.State.transp.pulleyDiamMm , 0.0f, 100.0f, "%.1f mm", 1.0);

                        if (ImGui::SliderFloat("##speedT1", &hw.State.transp.speedMmS, 0.0f, 300.0f, "%.1f mm/sec", 1.0)){
                            hw.State.transp.speedSteps = hw.State.transp.speedMmS / (3.14159f * hw.State.transp.pulleyDiamMm) *
                                                         200 * hw.State.transp.microstepDivider;
                        }


                        if (ImGui::SliderInt("##speedT2", &hw.State.transp.speedSteps, 0, 1000, "%u steps/sec")){
                            hw.State.transp.speedMmS =   (float)hw.State.transp.speedSteps /
                                                        (float)hw.State.transp.microstepDivider / 200.0f * (3.14159f * hw.State.transp.pulleyDiamMm);
                        }

                        static int transpMicro;
                        if(ImGui::Combo("microstepping", &transpMicro, "1\0 2\0 4\0 8\0 16\0 32\0\0")){
                            hw.State.transp.microstepDivider = pow(2, transpMicro);

                            hw.State.transp.speedMmS =   (float)hw.State.transp.speedSteps /
                                                        (float)hw.State.transp.microstepDivider / 200.0f * (3.14159f * hw.State.transp.pulleyDiamMm);
                            hw.State.transp.speedSteps = hw.State.transp.speedMmS / (3.14159f * hw.State.transp.pulleyDiamMm) *
                                                         200 * hw.State.transp.microstepDivider;

                        }

                        if (ImGui::Checkbox("run", &hw.State.transp.run)){

                        }


                    ImGui::EndChild();
                    ImGui::SameLine();

                    ImGui::BeginChild("##nozzles", ImVec2(groupW, -1), true);
                        ImGui::Text("nozzles");

                        ImVec2 btnSize = {  (ImGui::GetWindowContentRegionWidth() - ImGui::GetStyle().ItemSpacing.x * 2),
                                            (ImGui::GetWindowContentRegionMax().y - 35 - ImGui::GetStyle().ItemSpacing.y * 17) / 16};

                        ImGui::BeginChild("##zones", ImVec2(ImGui::GetWindowContentRegionWidth()*0.3f, -1), true);
                            char buf[16] = {0};

                            for (int i=0; i < 16; i++){

                                ImGui::Text("%02u", i);
                                ImGui::SameLine();

                                if (hw.State.nozzles.state[i])
                                    ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);

                                    if (ImGui::Button(buf, btnSize)) {}

                                if (hw.State.nozzles.state[i])
                                    ImGui::PopStyleColor();
                            }
                        ImGui::EndChild();

                        ImGui::SameLine();
                        ImGui::BeginGroup();
                            if (ImGui::Button("all on")) {}
                            if (ImGui::Button("all off")) {}
                        ImGui::EndGroup();

                    ImGui::EndChild();
                    ImGui::SameLine();

                    break;}



                case CAT_DEBUG: {
                    ImGui::Checkbox("show debug mat", &MatWin[W_MAT_DEBUG].show);


                    static int hwTypeMaskHere =0;
                    static int hwActMaskHere =0;
                    static int hwIDhere = 0;
                    static int paramHere = 0;

                    ImGui::Combo(u8"hwTypeMask", &hwTypeMaskHere, u8"MASK_HWTYPE_MOTOR\0MASK_HWTYPE_LAMP\0MASK_HWTYPE_LED\0MASK_HWTYPE_MISC\0\0");

                    if (hwTypeMaskHere == MASK_HWTYPE_MOTOR)
                        ImGui::Combo(u8"hwActMask", &hwActMaskHere, u8"MASK_ACT_MOTORRUN\0MASK_ACT_MOTORACCEL\0MASK_ACT_MOTORSPD\0MASK_ACT_MOTORACCEL\0\0");

                    if (hwTypeMaskHere == MASK_HWTYPE_LAMP)
                        ImGui::Combo(u8"hwActMask", &hwActMaskHere, u8"MASK_ACT_LAMPPWM\0MASK_ACT_LAMPONOFF\0\0");

                    if (hwTypeMaskHere == MASK_HWTYPE_LED)
                        ImGui::Combo(u8"hwActMask", &hwActMaskHere, u8"MASK_ACT_LEDR\0MASK_ACT_LEDG\0MASK_ACT_LEDB\0MASK_ACT_LEDONOFF\0\0");

                    if (hwTypeMaskHere == MASK_HWTYPE_MISC)
                        ImGui::Combo(u8"hwActMask", &hwActMaskHere, u8"MASK_ACT_PING\0\0");


                    //ImGui::SliderInt("hw ID", &hwIDhere, 0, 4, "%u");
                    //if(ImGui::SliderInt("parameter", &paramHere, 0, 255, "%u")) COM.setHwState(hwTypeMaskHere, hwIDhere, hwActMaskHere, paramHere);

                    //if (ImGui::Button("send")) COM.setHwState(hwTypeMaskHere, hwIDhere, hwActMaskHere, paramHere);
                    if (ImGui::Button("shake")) COM.tryShake = 1;//COM.Shake();


                    break;}
            }
            ImGui::EndChild();
        ImGui::EndGroup();
}

void btnStyleInactive(void){
    ImGui::PushStyleColor(ImGuiCol_Button,          (ImVec4)ImColor::HSV(0.5f, 0.0f, 0.5f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,   (ImVec4)ImColor::HSV(0.7f, 0.0f, 0.7f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,    (ImVec4)ImColor::HSV(0.9f, 0.0f, 0.9f, 1.0f));
};

void btnStyleFrac(float frac){
    ImGui::PushStyleColor(ImGuiCol_Button,          (ImVec4)ImColor::HSV(frac, 0.7f, 0.7f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,   (ImVec4)ImColor::HSV(frac, 0.5f, 0.7f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,    (ImVec4)ImColor::HSV(frac, 0.3f, 0.7f, 1.0f));
};

void btnStyleOnOff(bool on){
    ImGui::PushStyleColor(ImGuiCol_Button,          (ImVec4)ImColor::HSV(0.3f*on + 0.0f * !on, 0.7f, 0.7f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,   (ImVec4)ImColor::HSV(0.3f*on + 0.0f * !on, 0.5f, 0.7f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,    (ImVec4)ImColor::HSV(0.3f*on + 0.0f * !on, 0.3f, 0.7f, 1.0f));

};

void btnStyleColorEnd(void){};
void btnStylePop(void){ImGui::PopStyleColor(3);};

bool testBool = 0;

void GUI_t::drawSettingsBar(void){
    ImGuiStyle& style = ImGui::GetStyle();
     ImGui::BeginChild("##wfsettingsleft", ImVec2(180/*ImGui::GetWindowWidth()/3.8f*/, 0), true);
        for (int i = 0; i < NR_CAT; i++){
            if (!   (   (V.procType == PROC_WF)     && (i >= CAT_B_COLOR) && (i <= CAT_B_INFO)  ||
                        (V.procType == PROC_B)      && (i >= CAT_WF_EDGE) && (i <= CAT_WF_SHOW) ||
                        (V.procType == PROC_CALIB)  && (i >= CAT_WF_EDGE) && (i <= CAT_B_INFO) )) {

                if (setCats[i].matID != -1) {
                    btnStyleFrac((float)i/NR_CAT);
                        pushNextDisabledIf(setCats[i].matToShow.empty());
                            char txt[16];
                            sprintf(txt, "##btnI%u", i);
                            if (ImGui::Button(txt, ImVec2(10.0f, 22.0f))) setCats[i].p_open = !setCats[i].p_open;
                        popNextDisabledIf(setCats[i].matToShow.empty());
                    btnStylePop();
                    ImGui::SameLine();
                }

                else ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (20 + style.ItemSpacing.x));

                ImGui::PushStyleColor(ImGuiCol_HeaderHovered, (ImVec4)ImColor::HSV((float)i/NR_CAT, 0.7f, 0.7f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_HeaderActive, (ImVec4)ImColor::HSV((float)i/NR_CAT, 0.7f, 0.5f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_Header, (ImVec4)ImColor::HSV((float)i/NR_CAT, 0.7f, 0.3f, 1.0f));
                if (ImGui::Selectable(setCats[i].name.c_str(), catSelected == i)){
                    catSelected = i;
                    setCats[i].show = 1;
                }
                ImGui::PopStyleColor(3);

                //ImGui::SameLine();
                //ImGui::SetCursorPosX(ImGui::GetWindowContentRegionMax().x - 25);
                //ImGui::Checkbox("##a", &testBool);

            }

        }
        ImGui::EndChild();
}

//ImVec2 mainWinSize = ImVec2(1920, 1060);
ImVec2 mainWinSize = ImVec2(1280, 1024 - 25);

//ImVec2 mainWinSize = ImVec2(GUI.ScreenW, GUI.ScreenH - 25);

void GUI_t::drawSettingsWindow(void){
    ImGuiStyle& style = ImGui::GetStyle();
    int spacing = style.ItemSpacing.x;
    ImGui::SetNextWindowSize(mainWinSize, ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(0, 20), ImGuiCond_Always);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
    if (ImGui::Begin(u8"Настройки", p_open, ImGuiWindowFlags_NoResize|
                                            ImGuiWindowFlags_NoCollapse|
                                            ImGuiWindowFlags_NoTitleBar|
                                            ImGuiWindowFlags_NoBringToFrontOnFocus)){

        ImGui::BeginChild("##fast", ImVec2(0, 38), 1, 0);
            char btnNameBuf[64] = {0};
            std::string sourceType;
            if (V.Input.Source == SOURCE_CAM) sourceType = u8"камера";
            if (V.Input.Source == SOURCE_VIDEO) sourceType = u8"файл";

            std::string processorType;
            if (V.procType == PROC_B) processorType = u8"лента";
            if (V.procType == PROC_WF) processorType = u8"водопад";
            if (V.procType == PROC_CALIB) processorType = u8"калибровка";

            btnStyleOnOff(V.Input.CaptureRun);
                sprintf(btnNameBuf, u8"Изображение (%s)", sourceType.c_str());
                if (ImGui::Button(btnNameBuf, ImVec2( ImGui::GetWindowContentRegionWidth()/4,0))) {shortcutCapture();} ImGui::SameLine();
            btnStylePop();

            btnStyleOnOff(COM.connectionOk);
                sprintf(btnNameBuf, u8"Коммуникация (COM%u, %u)", V.ComPort.Number+1, V.ComPort.Speed);
                if (ImGui::Button(btnNameBuf, ImVec2(ImGui::GetWindowContentRegionWidth()/4,0))) {shortcutConnect();} ImGui::SameLine();
            btnStylePop();

            pushNextDisabledIf(!V.Input.CaptureRun);
                btnStyleOnOff(Img.procRun);
                    sprintf(btnNameBuf, u8"Обработка (%s)", processorType.c_str());
                    if (ImGui::Button(btnNameBuf, ImVec2(ImGui::GetWindowContentRegionWidth()/4,0))) {shortcutProcessing();} ImGui::SameLine();
                btnStylePop();
            popNextDisabledIf(!V.Input.CaptureRun);

            pushNextDisabledIf(!Img.procRun);
                btnStyleOnOff(COM.connectionOk);
                    if (ImGui::Button(u8"Сепарация", ImVec2(ImGui::GetWindowContentRegionWidth()/4,0))) {shortcutSeparation();}
                btnStylePop();
            popNextDisabledIf(!Img.procRun);

        ImGui::EndChild();

        ImGui::BeginChild("##upper", ImVec2(0, mainWinSize.y/2 - 80));
            drawSettingsBar();
            ImGui::SameLine();
            drawSettingsBlock();
        ImGui::EndChild();

        ImGui::BeginChild("##lower_img", ImVec2(ImGui::GetWindowContentRegionWidth() - 400, 0), 1, 0);
            drawMatBar();
        ImGui::EndChild();

        ImGui::SameLine();

        ImGui::BeginChild("##lower_console", ImVec2(400 - spacing, 0), 1, 0);
           drawConsole();
        ImGui::EndChild();

    }
    ImGui::End();
    ImGui::PopStyleVar();


}

std::string errorText = "";
void GUI_t::popupError(std::string text){
    errorText = text;
    openPopUpFlags[PU_ERROR] = 1;
}


#include <chrono>
#include <mutex>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

std::mutex listAccess;

typedef struct logRecord{
    std::string text;
    std::string cat;
    char timeStr[32];
    //std::chrono::time_point<std::chrono::system_clock> time;
    //time_t  rawTime;
    //struct tm *time;
};

vector <logRecord> logContent;

void logAdd(std::string category, std::string inText){
        struct logRecord newRec;
            newRec.text = inText;
            newRec.cat = category;

            time_t  rawTime;
            struct tm *infoTime;
            time(&rawTime);
            infoTime = localtime(&rawTime);
            strftime(newRec.timeStr, sizeof(newRec.timeStr), "%H:%M:%S", infoTime);

            //newRec.time = std::chrono::system_clock::now();

    listAccess.lock();
        logContent.push_back(newRec);
    listAccess.unlock();
};

void logPrint(std::string category, const char *fmt, ...){
    char buffer[512];
    va_list args;
    va_start(args, fmt);
    int rc = vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    struct logRecord newRec;
        std::string tmp(buffer);
        newRec.text = tmp;
        newRec.cat = category;

        time_t  rawTime;
        struct tm *infoTime;
        time(&rawTime);
        infoTime = localtime(&rawTime);
        strftime(newRec.timeStr, sizeof(newRec.timeStr), "%H:%M:%S", infoTime);

    listAccess.lock();
        logContent.push_back(newRec);
    listAccess.unlock();
};

void testLogAdd(void){
    logAdd("erroz", "some fckn erro");
}

void testLogPrint(void){
    logPrint("randomz", "new random is %u", (rand() % 100));
}

void drawNewLog(void){
    static int selected;
    if (!logContent.empty()){
        for (size_t i = 0; i < logContent.size(); i++){
            char buf[512];
            sprintf(buf, "%s [%s] %s", logContent[i].timeStr, logContent[i].cat.c_str(), logContent[i].text.c_str());

            if (ImGui::Selectable(buf, selected == i))
                selected = i;
        }
    }
}


void GUI_t::Draw(void){
    drawMenuBar();
    drawMatWindows();
    drawSettingsWindow();
    ImGui::ShowDemoWindow();
    drawFileBrowser();
    drawPopUps();

    ImGui::Begin(u8"Изображения", p_open, ImGuiWindowFlags_None);


            for (int win=0; win<NR_CAT; win++){
               // if (setCats[win].name == cat){
                    ImGui::Checkbox(setCats[win].name.c_str(), &setCats[win].show);
                //}
            }



    ImGui::End();




    if (ImGui::Button("testLogPrint")){
        testLogPrint();
    }
    if (ImGui::Button("testLogAdd")){
        testLogAdd();
    }

        if (ImGui::TreeNode("drawNewLog")){
    drawNewLog();
       ImGui::TreePop();
    }
}

void GUI_t::openPopUp(int id){
    openPopUpFlags[id] = 1;
}

struct Separator_t{
    void stopSeparation(void);
};

Separator_t Sep;


void Separator_t::stopSeparation(void){
};

void GUI_t::drawPopUps(void){
    if (openPopUpFlags[PU_COMM_STOP]) {ImGui::OpenPopup("popupCommStop"); openPopUpFlags[PU_COMM_STOP] = 0;}
    ImGui::SetNextWindowSize(ImVec2(300, 150));
    if (ImGui::BeginPopupModal("popupCommStop", NULL, ImGuiWindowFlags_AlwaysAutoResize|ImGuiWindowFlags_NoTitleBar)){
        ImGui::BeginChild("##popupper2", ImVec2(0, 100), 0, 0);
            if (COM.connectionOk) ImGui::Text(u8"Блок правления подключен");
            ImGui::Text(u8"Преравть коммуникацию?\nСепарация также будет остановлена!");
        ImGui::EndChild();

        if (ImGui::Button(u8"Да", ImVec2(ImGui::GetWindowContentRegionWidth()/2-4, 25))) {
            Sep.stopSeparation();
            COM.tryGoodbye();
            ImGui::CloseCurrentPopup();}
        ImGui::SameLine();

        if (ImGui::Button(u8"Нет", ImVec2(ImGui::GetWindowContentRegionWidth()/2-4, 25))) {ImGui::CloseCurrentPopup();}
        ImGui::EndPopup();
    }

    if (openPopUpFlags[PU_SOURCE_CLOSE]) {ImGui::OpenPopup("popupSourceClose"); openPopUpFlags[PU_SOURCE_CLOSE] = 0;}
    ImGui::SetNextWindowSize(ImVec2(300, 150));
    if (ImGui::BeginPopupModal("popupSourceClose", NULL, ImGuiWindowFlags_AlwaysAutoResize|ImGuiWindowFlags_NoTitleBar)){
        ImGui::BeginChild("##popupper2", ImVec2(0, 100), 0, 0);
            ImGui::Text(u8"Закрыть источник?\nОбработка и сепарация будут остановлены!");
        ImGui::EndChild();

        if (ImGui::Button(u8"Да", ImVec2(ImGui::GetWindowContentRegionWidth()/2-4, 25))) {
            // stop processing here

            Sep.stopSeparation();
            Img.stopProcessing();

            Img.stopCapture();

            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button(u8"Нет", ImVec2(ImGui::GetWindowContentRegionWidth()/2-4, 25))) {ImGui::CloseCurrentPopup();}
        ImGui::EndPopup();
    }

    if (openPopUpFlags[PU_EXIT]) {ImGui::OpenPopup("popupExit"); openPopUpFlags[PU_EXIT] = 0;}
    ImGui::SetNextWindowSize(ImVec2(300, 150));
    if (ImGui::BeginPopupModal("popupExit", NULL, ImGuiWindowFlags_AlwaysAutoResize|ImGuiWindowFlags_NoTitleBar)){
        ImGui::BeginChild("##popupper2", ImVec2(0, 100), 0, 0);
            ImGui::Text(u8"Выйти из программы?");
        ImGui::EndChild();

        if (ImGui::Button(u8"Да", ImVec2(ImGui::GetWindowContentRegionWidth()/2-4, 25))) {
            // stop processing here
            exitApplication();
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button(u8"Нет", ImVec2(ImGui::GetWindowContentRegionWidth()/2-4, 25))) {ImGui::CloseCurrentPopup();}
        ImGui::EndPopup();
    }



    if (openPopUpFlags[PU_ERROR]) {ImGui::OpenPopup("popupEror"); openPopUpFlags[PU_ERROR] = 0;}
    ImGui::SetNextWindowSize(ImVec2(300, 150));
    if (ImGui::BeginPopupModal("popupEror", NULL, ImGuiWindowFlags_AlwaysAutoResize|ImGuiWindowFlags_NoTitleBar)){
        ImGui::BeginChild("##popupper2", ImVec2(0, 100), 0, 0);
            ImGui::Text(errorText.c_str());
        ImGui::EndChild();

        if (ImGui::Button("OK", ImVec2(ImGui::GetWindowContentRegionWidth(), 25))) {ImGui::CloseCurrentPopup();}
        ImGui::EndPopup();
    }

    if (openPopUpFlags[PU_PROC_STOP]) {ImGui::OpenPopup("popupProcStop"); openPopUpFlags[PU_PROC_STOP] = 0;}
    ImGui::SetNextWindowSize(ImVec2(300, 150));
    if (ImGui::BeginPopupModal("popupProcStop", NULL, ImGuiWindowFlags_AlwaysAutoResize|ImGuiWindowFlags_NoTitleBar)){
        ImGui::BeginChild("##popupper2", ImVec2(0, 100), 0, 0);
            ImGui::Text(u8"Остановить обработку?\nСепарация также будет остановлена!");
        ImGui::EndChild();

        if (ImGui::Button(u8"Да", ImVec2(ImGui::GetWindowContentRegionWidth()/2-4, 25))) {
            // stop processing here
            Img.stopProcessing();
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button(u8"Нет", ImVec2(ImGui::GetWindowContentRegionWidth()/2-4, 25))) {ImGui::CloseCurrentPopup();}
        ImGui::EndPopup();
    }

    if (openPopUpFlags[PU_SEPAR_STOP]) {ImGui::OpenPopup("popupSepStop"); openPopUpFlags[PU_SEPAR_STOP] = 0;}
    ImGui::SetNextWindowSize(ImVec2(300, 150));
    if (ImGui::BeginPopupModal("popupSepStop", NULL, ImGuiWindowFlags_AlwaysAutoResize|ImGuiWindowFlags_NoTitleBar)){
        ImGui::BeginChild("##popupper2", ImVec2(0, 100), 0, 0);
            ImGui::Text(u8"Остановить сепарацию?");
        ImGui::EndChild();

        if (ImGui::Button(u8"Да", ImVec2(ImGui::GetWindowContentRegionWidth()/2-4, 25))) {
            // stop processing here
            //Img.stopProcessing();
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button(u8"Нет", ImVec2(ImGui::GetWindowContentRegionWidth()/2-4, 25))) {ImGui::CloseCurrentPopup();}
        ImGui::EndPopup();
    }

}

void GUI_t::VarInit(void){
    int i = CAT_INPUT;
    setCats[i].name = u8"Вход изображения";
    setCats[i].matID = W_MAT_IN;
    setCats[i].mat = &Img.img_in;

    i = CAT_PROCESSING;
    setCats[i].name = u8"Обработка";
    setCats[i].matID = W_MAT_PP_HUD;
    setCats[i].mat = &Img.img_in;

    i = CAT_WF_EDGE;
    setCats[i].name = u8"- Водопад: Поиск границ";
    setCats[i].mat = &Img.img_canny_output;

    i = CAT_WF_CONTOURS;
    setCats[i].name = u8"- Водопад: Контуры";
    setCats[i].mat = &Img.img_wholemask;

    i = CAT_WF_MORPHO;
    setCats[i].name = u8"- Водопад: Морфология";
   setCats[i].mat = &Img.img_morph_out;

    i = CAT_WF_COLOR;
    setCats[i].name = u8"- Водопад: Цвет";
    setCats[i].matID = -1;

    i = CAT_WF_BS;
    setCats[i].name = u8"- Водопад: отделение фона";
    setCats[i].mat = &Img.img_mog_output;

    i = CAT_WF_SHOW;
    setCats[i].name = u8"- Водопад: HUD";
    setCats[i].mat = &Img.img_output;

    i = CAT_COM;
    setCats[i].name = u8"Коммуникация";
    setCats[i].matID = -1;

    i = CAT_HW;
    setCats[i].name = u8"Оборудование";
    setCats[i].matID = -1;

    i = CAT_UI;
    setCats[i].name = u8"Интерфейс";
    setCats[i].matID = -1;

    i = CAT_DEBUG;
    setCats[i].name = u8"Debug";
    setCats[i].matID = -1;

    i = CAT_STATS;
    setCats[i].name = u8"Статистика";
    setCats[i].matID = -1;

    i = CAT_B_COLOR;
    setCats[i].name = u8"- Ремень: Цвет";
    setCats[i].matID = -1;

    i = CAT_B_SIZE;
    setCats[i].name = u8"- Ремень: Размеры";
    setCats[i].mat = &BeltProcessor.matSatRanged;

    i = CAT_B_MORPH;
    setCats[i].name = u8"- Ремень: Морфология";
    setCats[i].matID = -1;

    i = CAT_B_BLUR;
    setCats[i].name = u8"- Ремень: Размытие";
    setCats[i].matID = -1;

    i = CAT_B_ACCUM;
    setCats[i].name = u8"- Ремень: Аккумулятор";
    setCats[i].mat = &BeltProcessor.matSatRendered;

    i = CAT_B_INFO;
    setCats[i].name = u8"- Ремень: HUD";
    setCats[i].mat = &BeltProcessor.ejHUD;

    i = CAT_CALIB;
    setCats[i].name = u8"Калибровка";
    setCats[i].mat = &Calib.matHUD;
}


/*
void GUI_t::Fill_Textures(void){
    for(int i=0; i<W_MAT_NR; i++){
        //if (MatWin[i].show){
            MatWin[i].write.lock();
            if(!MatWin[i].mat->empty()){
                if (MatWin[i].mat->type() == 16){cv::cvtColor(*MatWin[i].mat, frameRGBA[i], cv::COLOR_BGR2RGBA);}
                else if (MatWin[i].mat->type() == 0){cv::cvtColor(*MatWin[i].mat, frameRGBA[i], cv::COLOR_GRAY2RGBA);}
                //else if (MatWin[i].mat->type() == 0){cv::cvtColor(*MatWin[i].mat, frameRGBA[i], cv::COLOR_GRAY2RGBA);}
                std::cout << "type = " << MatWin[i].mat->type() << std::endl;
                image[i].create(frameRGBA[i].cols, frameRGBA[i].rows, frameRGBA[i].ptr());
                texture[i].loadFromImage(image[i]);
                sprite[i].setTexture(texture[i]);
            }
            MatWin[i].write.unlock();
        //}
    }
}

*/




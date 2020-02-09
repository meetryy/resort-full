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

using namespace std;
using namespace ImGui;

std::string FilePath(void);

bool* p_open;
void GUI_class::ShowHelpMarker(const char* desc){
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

void GUI_class::Init(void){
    ScreenW = sf::VideoMode::getDesktopMode().width;
    ScreenH = sf::VideoMode::getDesktopMode().height;

    GUI.WindowName.append(" (");
    GUI.WindowName.append(__DATE__);
    GUI.WindowName.append(")");

    window.create(sf::VideoMode(GUI.ScreenW, GUI.ScreenH), GUI.WindowName);//,sf::Style::Fullscreen);
    window.setFramerateLimit(V.UI.guiFPS);

    ImGui::SFML::Init(window, false);
    LoadFont();
}


void GUI_class::Worker(void){
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
                GUI.ConsoleOut(u8"���������: ������������� ����� ����������");
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


void GUI_class::LoadFont(void){
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
    ConsoleOut(u8"���������: ��������� ������� ���������");
}



ImVec4 ColorOn = ImColor(0.0f,1.0f,0.0f,1.0f);
ImVec4 ColorOff = ImColor(1.0f,0.0f,0.0f,1.0f);

void GUI_class::StatedText(string Input, bool state){
    if (state) ImGui::PushStyleColor(ImGuiCol_Text, ColorOn);
    else    ImGui::PushStyleColor(ImGuiCol_Text, ColorOff);

    ImGui::BulletText(Input.c_str());
    ImGui::PopStyleColor();
}


void GUI_class::drawMenuBar(void){
    if (ImGui::BeginMainMenuBar()){

        ImGui::Text(WindowName.c_str());
        ImGui::Separator();

        ImGui::Text(u8"������������: settings.ini");
        if (ImGui::MenuItem(u8"���������")) openFileBrowser(BROWSE_LOAD_CFG);
        if (ImGui::MenuItem(u8"���������")) openFileBrowser(BROWSE_SAVE_CFG);
        ImGui::Separator();

        if(ImGui::MenuItem(u8"������ �����", NULL, &V.UI.Fullscreen)) {FullscreenChanged=1;}

        ImGui::Separator();
        ImGui::Text("GUI %.1f FPS",ImGui::GetIO().Framerate);

        ImGui::Separator();
        ImGui::Text(u8"��������� %.1f FPS", V.Info.FPS);


        static time_t now;
        now = time(NULL);
        static char time_buff[64];
        strftime(time_buff, 64, "%H:%M:%S %d.%m.%Y ", gmtime(&now));

        ImGui::Separator();
        ImGui::Text("%s", time_buff);

        //MenuBarStateList();

        SameLine(GetWindowWidth() - 57);
        if (ImGui::MenuItem(u8"�����")){openPopUp(PU_EXIT);};

    ImGui::EndMainMenuBar();
    }
}

void GUI_class::drawMatWindows(void){
    for(int i=0; i<W_MAT_NR; i++){
        if (MatWin[i].show){
            if(!MatWin[i].mat_show.empty()){
                    MatWin[i].write.lock();
                    if (MatWin[i].mat_show.type() == 16){cv::cvtColor(MatWin[i].mat_show, frameRGBA[i], cv::COLOR_BGR2RGBA);}
                    else if (MatWin[i].mat_show.type() == 0){cv::cvtColor(MatWin[i].mat_show, frameRGBA[i], cv::COLOR_GRAY2RGBA);}
                    MatWin[i].write.unlock();
                    image[i].create(frameRGBA[i].cols, frameRGBA[i].rows, frameRGBA[i].ptr());
                    texture[i].loadFromImage(image[i]);
                    sprite[i].setTexture(texture[i]);
                ImGuiWindowFlags MatWinFlags = ImGuiWindowFlags_None;//ImGuiWindowFlags_AlwaysAutoResize;

                float initSizeX = texture[i].getSize().x;
                float initSizeY = texture[i].getSize().y;
                float aspRatio =  initSizeX / initSizeY;

                ImGui::Begin(MatWin[i].title.c_str(), p_open, MatWinFlags);
                    //ImGui::Image(texture[i], ImVec2(ImGui::GetWindowContentRegionMax().x, ImGui::GetWindowContentRegionMax().y-40));//}
                    ImGui::Image(texture[i], ImVec2(ImGui::GetWindowContentRegionMax().y*aspRatio, ImGui::GetWindowContentRegionMax().y-40));//}
                ImGui::End();
            }
            else{
                ImGui::Begin(MatWin[i].title.c_str(), p_open, /*ImGuiWindowFlags_MenuBar|*/ImGuiWindowFlags_AlwaysAutoResize);
                    ImGui::Text(u8"����������� ����������!");
                    ImGui::Text(u8"(���������� �� ������)");
                ImGui::End();
            }
        }
    }

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

int catSelected = 0;

void GUI_class::drawMatBar(void){
    ImGuiStyle& style = ImGui::GetStyle();

    int i = setCats[catSelected].matID;
    MatWin[i].show = 1;

    if((!MatWin[i].mat_show.empty()) && (i != -1)){
        MatWin[i].write.lock();
        switch (MatWin[i].mat_show.type()) {
            case 0:  {cv::cvtColor(MatWin[i].mat_show, frameRGBA[i], cv::COLOR_GRAY2RGBA); break;}
            case 16: {cv::cvtColor(MatWin[i].mat_show, frameRGBA[i], cv::COLOR_BGR2RGBA); break;}
            case 24: {cv::cvtColor(MatWin[i].mat_show, frameRGBA[i], cv::COLOR_BGRA2RGBA); break;}
        }
        MatWin[i].write.unlock();

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

void GUI_class::drawSettingsBlock(void){
    ImGui::BeginGroup();
            ImGui::BeginChild("##wfitems", ImVec2(0, 0), 1, 0);
            switch (catSelected) {
                case CAT_INPUT: {
                    ImGui::Text(u8"��������� ��������� �����������");
                    ImGui::Separator();

                        if  (!V.Input.CaptureRun) {
                                if (ImGui::Button(u8"�������", ImVec2(60,22))) {
                                    Img.startCapture();
                                }
                            }

                            else {
                                if (ImGui::Button(u8"�������", ImVec2(60,22))) {
                                    openPopUp(PU_SOURCE_CLOSE);
                                    //stopCapture();
                                }
                            }
                    //ImGui::EndChild();

                    ImGui::SameLine();

                    //ImGui::BeginChild("##startbtnch2", ImVec2(0,50), 0, ImGuiWindowFlags_None);
                        pushNextDisabledIf(V.Input.CaptureRun);
                            ImGui::Combo(u8"�������� ��������", &V.Input.Source, u8"������\0�����\0\0");
                            if (V.Input.Source == SOURCE_VIDEO){
                                if (ImGui::Button(u8"�����...", ImVec2(60,22))) openFileBrowser(BROWSE_LOAD_VIDEO);
                            }
                        popNextDisabledIf(V.Input.CaptureRun);



                        GUI.ShowHelpMarker(u8"����� � ������ ��� �� ����� \"test_video.avi\"");

                        ImGui::SliderInt(u8"�������� ������ n-��� ����", &Img.matLimiterTarget,  0, 127);
                        GUI.ShowHelpMarker(u8"����� �������� ��������������");
                    //ImGui::EndChild();



                    ImGui::Separator();

                    if (V.Input.Source == 0){
                        ImGui::Text(u8"��������� ������:");
                        ImGui::InputInt(u8"����� ������", &V.Cam.Number);
                        GUI.ShowHelpMarker(u8"�� ��������� 0");

                        //const char* reso[] = { "", "", "800x600", "1024x768", "1280x720"};
                        const long  resol[] = {320,240,640,480,800,600,1024,768,1280,720,1920,1080};
                        static int reso_curr;

                        pushNextDisabledIf(V.Input.CaptureRun);
                            if(ImGui::Combo(u8"����������", &reso_curr, " 320x240\0 640x480\0 800x600\0 1024x768\0 1280x720\0 1920x1080 \0\0")){
                                V.Cam.Width  = resol[reso_curr*2];
                                V.Cam.Height = resol[reso_curr*2+1];
                                //Img.cameraOpen();
                            }
                        popNextDisabledIf(V.Input.CaptureRun);

                        GUI.ShowHelpMarker(u8"��� ������, ��� �������");

                        if (ImGui::SliderFloat(u8"������� ������", &V.Cam.FPS,  5.0, 200.0,"%.0f")) Img.cameraUpdSettings();
                        GUI.ShowHelpMarker(u8"�������������� �� ��� ��������\n��������� �������� ������� �� ����������");

                        ImGui::Text(u8"��������� �����:");
                        if (ImGui::SliderInt(u8"��������", &V.Cam.Gain,  0, 127)) Img.cameraUpdSettings();
                        GUI.ShowHelpMarker(u8"������� ������������� ���������� �������� �� ��������� ����");

                        if (ImGui::SliderInt(u8"��������", &V.Cam.Contrast,  0, 127)) Img.cameraUpdSettings();

                        if (ImGui::SliderInt(u8"����������", &V.Cam.Exposure,  -5, 0)) Img.cameraUpdSettings();
                        GUI.ShowHelpMarker(u8"������� ��������������� � ��������");

                        if (ImGui::SliderInt(u8"������������", &V.Cam.Saturation,  0, 127)) Img.cameraUpdSettings();

                        ImGui::Checkbox(u8"����-����", &V.Input.FreezeFrame);


                    }

                    if (V.Input.Source == SOURCE_VIDEO){
                        //Img.capture_file = V.Input.Source;

                        #warning TODO add video file list here
                        #warning TODO open video file by name

                        ImGui::Text(u8"�������� ����� ����� [%s]:", Img.videoFileName.c_str());
                        ImGui::Text(u8"����������: %.0f x %.0f", videoPlayer.frameW, videoPlayer.frameH);
                        ImGui::Text(u8"����������: %.0f x %.0f", videoPlayer.frameW, videoPlayer.frameH);
                        //ImGui::Text(u8"� %u �� %u", videoPlayer.startFrame, videoPlayer.endFrame);
                        //ImGui::Text(u8"%u, %.2f ms", videoPlayer.playbackMarker, videoPlayer.playbackMs);
                        ImGui::Text(u8"����� %u ������, %.1f FPS, %.1f FPS now", videoPlayer.fileLengthFrames, videoPlayer.videoFPS, videoPlayer.fps);

                        ImGui::BeginChild("##player", ImVec2(0,0), 1,0);
                        //alignCenter(100.0f);
                        ImGui::Text(u8"�����");
                        char pBarBuf[64];
                        sprintf(pBarBuf, u8"�����: %i / %i", videoPlayer.playbackMarker - videoPlayer.startFrame, videoPlayer.partLength);

                        ImGui::ProgressBar(videoPlayer.playbackPortion, ImVec2(0.f,0.f), pBarBuf);
                        ImGui::SliderInt(u8"������, ����", &videoPlayer.startFrame, 0, videoPlayer.fileLengthFrames, "%u");
                        ImGui::SliderInt(u8"�����, ����", &videoPlayer.endFrame, videoPlayer.startFrame, videoPlayer.fileLengthFrames, "%u");
                        ImGui::Checkbox(u8"�����", &videoPlayer.pause);

                        ImGui::SliderFloat(u8"������������ ��������", &videoPlayer.fpsLimiter, 1.0f, 200.0f, "%.1f ms");
                        if (videoPlayer.pause){
                            if (ImGui::SliderInt(u8"�������", &videoPlayer.playbackMarker, videoPlayer.startFrame, videoPlayer.endFrame, "%u"))
                                videoPlayer.setMarker(videoPlayer.playbackMarker);
                        }
                        ImGui::EndChild();
                    }

                    break;}

                case CAT_PROCESSING: {
                    ImGui::Text(u8"��������� ��������� �����������");
                    ImGui::Separator();

                    ImGui::Combo(u8"�������� �������� ������", &V.procType, u8"�������\0������\0\0");
                    ImGui::Separator();

                    ImGui::Text(u8"��������� ������������� �����������:");
                    ImGui::Checkbox("on", &preprocessor.isOn);

                    const char* items[] = { u8"0", u8"90 ��", u8"180", u8"90 ���"};
                    static int item_current = 0;
                    ImGui::Combo(u8"�������", &item_current, items, IM_ARRAYSIZE(items));
                    preprocessor.rotation = item_current - 1;

                    ImGui::SliderInt("marginLeft", &preprocessor.marginLeft, 0, (int)videoPlayer.frameW/2-1, "%u pix");
                    ImGui::SliderInt("marginRight", &preprocessor.marginRight, 0, (int)videoPlayer.frameW/2-1, "%u pix");
                    ImGui::SliderInt("marginUp", &preprocessor.marginUp, 0, (int)videoPlayer.frameH/2-1, "%u pix");
                    ImGui::SliderInt("marginDown", &preprocessor.marginDown, 0, (int)videoPlayer.frameH/2-1, "%u pix");

                    ImGui::SliderFloat("brightness", &preprocessor.brightness, 0.0f, 5.0f, "%.2f");
                    ImGui::SliderFloat("contrast", &preprocessor.contrast, 0.0f, 5.0f, "%.2f");
                    ImGui::SliderFloat("saturation", &preprocessor.saturation, 0.0f, 5.0f, "%.2f");
                    ImGui::SliderInt("sharpTimes", &preprocessor.sharpTimes, 1, 10, "%u");

                    break;}

                case CAT_WF_CONTOURS: {
                    ImGui::Text(u8"�������: ��������� �������� ������");
                    ImGui::Separator();

                    ImGui::DragFloat(u8"min ������� �����", &V.Contours.MinBBoxArea, 1.0, 1.0, V.Contours.MaxBBoxArea, "%.0f");
                    //ImGui::SliderFloat(, ,  0.0f, 100.0f+V.Contours.WideAreaRange*10000.0f, "%.0f");
                    GUI.ShowHelpMarker(u8"(pix^2)");
                    //ImGui::SliderFloat(u8"max ������� �����", &V.Contours.MaxBBoxArea,  0.0f, 10000.0f+V.Contours.WideAreaRange*20000.0f, "%.0f");
                    ImGui::DragFloat(u8"max ������� �����", &V.Contours.MaxBBoxArea, 1.0, V.Contours.MinBBoxArea, 500000000.0, "%.0f");
                    GUI.ShowHelpMarker(u8"(pix^2)");
                break;}

                case CAT_WF_EDGE: {
                    ImGui::Text(u8"�������: ��������� ������ ��������");
                    ImGui::Separator();

                    ImGui::Combo(u8"�������� ������", &V.Contours.CurrentAlgo, u8"Canny\0Scharr\0\0");
                    ImGui::Separator();
                    if (V.Contours.CurrentAlgo == 0){
                        V.Edge.UseScharr=0;
                        ImGui::SliderInt(u8"������ ��������", &V.Edge.BlurValue,  1, 5);
                        GUI.ShowHelpMarker(u8"��������������� �������� �������� ���������� �� ������\n�������� �������� �������� �����");
                        ImGui::SliderInt(u8"����� 1", &V.Edge.CannyThresh1, 1, 500);
                        ImGui::SliderInt(u8"����� 2", &V.Edge.CannyThresh2, 1, 500);
                        GUI.ShowHelpMarker(u8"�� ��������� ����� 2 �������� � ��� ���� ������ ������ 1");
                    }

                    if (V.Contours.CurrentAlgo == 1){
                        ImGui::Text(u8"(�������� ���� ��������� �����!)");
                        V.Edge.UseScharr=1;
                        //ImGui::SliderInt(u8"��������", &test_int,  1, 2);
                        //ImGui::InputFloat(u8"�����", &f, 1,2, .1f);
                    }
                    break;}

                case CAT_WF_MORPHO: {
                    ImGui::Text(u8"�������: ��������� ���������������� ��������������");
                    ImGui::Separator();

                    ImGui::Combo(u8"��� ����� ��������������", &V.Morph.Type, u8"RECT\0ELLIPSE\0CROSS\0\0");
                    GUI.ShowHelpMarker(u8"��� ��������� �������� ���������������� ��������������\nRECT ����� �������, ELLIPSE ������� ���������, �� ����� ������������");
                    ImGui::SliderInt(u8"������ �����", &V.Morph.Size,  1, 10);
                    GUI.ShowHelpMarker(u8"������ ��������� �������� ���������������� ��������������\n��������� �������� ����� �����, ������� /""���������/"" ����� ������");
                    break;}

                case CAT_WF_COLOR: {
                    ImGui::Text(u8"�������: ��������� ����� ������");
                    ImGui::Separator();

                    static float tolh = (float)V.Color.ToleranceHSV.val[0];
                    static float tols = (float)V.Color.ToleranceHSV.val[1];
                    static float tolv = (float)V.Color.ToleranceHSV.val[2];

                    static float tolr = (float)V.Color.ToleranceRGB.val[2];
                    static float tolg = (float)V.Color.ToleranceRGB.val[1];
                    static float tolb = (float)V.Color.ToleranceRGB.val[0];

                    if(ImGui::ColorPicker3(u8"���������� ����", (float*)&test_color)){
                        float hue, saturation, value;
                        ImGui::ColorConvertRGBtoHSV(test_color[0], test_color[1], test_color[2], hue, saturation, value);
                        V.Color.GoodHSV.val[0] = hue*180.0;
                        V.Color.GoodHSV.val[1] = saturation*255.0;
                        V.Color.GoodHSV.val[2] = value*255.0;
                        V.Color.GoodRGB.val[2] = test_color[0]*255.0;
                        V.Color.GoodRGB.val[1] = test_color[1]*255.0;
                        V.Color.GoodRGB.val[0] = test_color[2]*255.0;
                    }

                    ImGui::Checkbox(u8"�������� ������������ �������", &V.Color.GoodSpace);
                    GUI.ShowHelpMarker(u8"������������ HSV ��� RGB ������");
                    if(V.Color.GoodSpace){
                        if (ImGui::SliderFloat(u8"������ H", &tolh, 0.0f, 180.0f, "%3.1f"))   V.Color.ToleranceHSV.val[0] = (double)tolh;
                        if (ImGui::SliderFloat(u8"������ S", &tols, 0.0f, 255.0f, "%3.1f"))   V.Color.ToleranceHSV.val[1] = (double)tols;
                        if (ImGui::SliderFloat(u8"������ V", &tolv, 0.0f, 255.0f, "%3.1f"))   V.Color.ToleranceHSV.val[2] = (double)tolv;
                    }
                    else {
                        if (ImGui::SliderFloat(u8"������ R", &tolr, 0.0f, 180.0f, "%3.1f"))   V.Color.ToleranceRGB.val[2] = (double)tolr;
                        if (ImGui::SliderFloat(u8"������ G", &tolg, 0.0f, 255.0f, "%3.1f"))   V.Color.ToleranceRGB.val[1] = (double)tolg;
                        if (ImGui::SliderFloat(u8"������ B", &tolb, 0.0f, 255.0f, "%3.1f"))   V.Color.ToleranceRGB.val[0] = (double)tolb;
                    }
                    break;}

                case CAT_WF_BS: {
                    ImGui::Text(u8"�������: ��������� ��������� ��������� ����");
                    ImGui::Separator();

                    if(ImGui::Combo(u8"��������", &V.BS.CurrentAlgo, u8"MOG\0MOG2\0CNT\0\0")){Img.BS_Init(V.BS.CurrentAlgo);}
                    GUI.ShowHelpMarker(u8"MOG � MOG2 �� ��������� ���������, �� �� ����� �����\nCNT �������� �������, �� ����� ������ ��������������");
                    ImGui::SliderInt(u8"��������������� ��������", &V.BS.BlurBeforeMog,  1, 10);
                    GUI.ShowHelpMarker(u8"�������� ���������� �� ������ � ����� ��������� ����");
                    ImGui::Separator();

                    if (V.BS.CurrentAlgo==BS_MOG){
                        ImGui::Checkbox(u8"��������", &V.BS.MOG.Learning);
                        GUI.ShowHelpMarker(u8"��������� ������ ����");
                        ImGui::SliderInt(u8"�������", &V.BS.MOG.History,  1, 300);
                        ImGui::SliderInt(u8"��������������", &V.BS.MOG.Mixtures,  1, 10);
                        ImGui::SliderFloat(u8"Background ratio", &V.BS.MOG.BackRatio, 0.01f, 0.99f, "%1.2f");
                        ImGui::SliderFloat(u8"�������� ��������", &V.BS.MOG.LRate, 0.01f, 0.99f, "%1.2f");
                        ImGui::SliderFloat(u8"Noise sigma", &V.BS.MOG.NoiseSigma, 0.0f, 1.0f, "%1.2f");
                    }

                    if (V.BS.CurrentAlgo==BS_MOG2){
                        ImGui::Checkbox(u8"��������", &V.BS.MOG2.Learning);
                        GUI.ShowHelpMarker(u8"��������� ������ ����");
                        ImGui::SliderInt(u8"�������", &V.BS.MOG2.History,  1, 300);
                        ImGui::SliderFloat(u8"�����", &V.BS.MOG2.Thresh, 0.1f, 200.0f, "%1.1f");
                        ImGui::SliderFloat(u8"�������� ��������", &V.BS.MOG2.LRate, 0.01f, 0.99f, "%1.2f");
                        ImGui::Checkbox(u8"������ ����", &V.BS.MOG2.DetectShadows);
                    }

                    if (V.BS.CurrentAlgo==BS_CNT){
                        ImGui::Checkbox(u8"��������", &V.BS.CNT.Learning);
                        GUI.ShowHelpMarker(u8"������ ������ ���� ��������");
                        ImGui::SliderInt(u8"���. ���������� ������", &V.BS.CNT.MinPixStability,  1, 200);
                        GUI.ShowHelpMarker(u8"����� ������� ������ ������� ��������� �������");
                        ImGui::SliderInt(u8"����. ���������� ������", &V.BS.CNT.MaxPixStability,  1, 200);
                        GUI.ShowHelpMarker(u8"������������ ������� ������� � �������");
                        ImGui::SliderInt(u8"������� ������� ������", &V.BS.CNT.FPS,  1, 100);
                        ImGui::Checkbox(u8"������������ �������", &V.BS.CNT.UseHistory);
                        ImGui::Checkbox(u8"�������� �����������", &V.BS.CNT.IsParallel);
                        GUI.ShowHelpMarker(u8"������ ��������, ������ ����������");
                        ImGui::SliderFloat(u8"�������� ��������", &V.BS.CNT.LRate, 0.01f, 0.99f, "%1.2f");
                        GUI.ShowHelpMarker(u8"�������� �������� ������ ����");
                    }
                    /*
                    if (V.BS.CurrentAlgo==BS_KNN){
                        ImGui::Checkbox(u8"��������", &Img.bs_knn_learning);
                        ImGui::SliderInt(u8"�������", &Img.bs_knn_history,  1, 300);
                        ImGui::SliderFloat(u8"�����", &Img.bs_knn_thresh, 0.0f, 1.0f, "%1.2f");
                        ImGui::SliderFloat(u8"�������� ��������", &Img.bs_knn_lrate, 0.01f, 0.99f, "%1.2f");
                        ImGui::Checkbox(u8"������ ����", &Img.bs_knn_shadows);

                    }

                    if (V.BS.CurrentAlgo==BS_GSOC){
                        ImGui::Checkbox(u8"��������", &Img.bs_gsoc_learning);
                        ImGui::SliderFloat(u8"�������� ��������", &Img.bs_gsoc_lrate, 0.01f, 0.99f, "%1.2f");
                        ImGui::SliderInt(u8"����������� �������� ������", &Img.bs_gsoc_mc,  0, 1);
                        ImGui::SliderInt(u8"��������", &Img.bs_gsoc_samples,  1, 200);
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

                    if (ImGui::Button(u8"��������� ���������")) {Img.BS_Init(V.BS.CurrentAlgo);}
                    GUI.ShowHelpMarker(u8"��������� ��������� �� ����������� �������������");
                break;}

                //case CAT_WF_BS_MOG: {break;}
                //case CAT_WF_BS_MOG2: {break;}
                //case CAT_WF_BS_CNT: {break;}

                case CAT_WF_SHOW: {
                    ImGui::Text(u8"�������: ��������� HUD");
                    ImGui::Separator();

                    ImGui::Checkbox(u8"�������", &V.Show.Contours);
                    ImGui::Checkbox(u8"������", &V.Show.Centers);
                    ImGui::Checkbox(u8"�������", &V.Show.BBoxes);
                    ImGui::Checkbox(u8"�������", &V.Show.Area);
                    ImGui::Checkbox(u8"�������", &V.Show.Diameter);
                    ImGui::Checkbox(u8"������ ������� ������� ������", &V.Show.FillAvg);
                    ImGui::Checkbox(u8"������ ������� �����������", &V.Show.FilContour);
                    break;}

                case CAT_B_COLOR: {
                    ImGui::Text(u8"������: ��������� �����");
                    ImGui::Separator();

                    ImGui::Text(u8"������� (H):");
                        ImGui::SliderInt(u8"H min", &BeltProcessor.low_H, 0, BeltProcessor.max_H, "%u");
                        ImGui::SliderInt(u8"H max", &BeltProcessor.high_H, BeltProcessor.low_H, BeltProcessor.max_H, "%u");

                    ImGui::Text(u8"������������ (S):");
                        ImGui::SliderInt(u8"S min", &BeltProcessor.low_S, 0, BeltProcessor.max_S, "%u");
                        ImGui::SliderInt(u8"S max", &BeltProcessor.high_S, BeltProcessor.low_S, BeltProcessor.max_S, "%u");

                    ImGui::Text(u8"�������� (V):");
                        ImGui::SliderInt(u8"V min", &BeltProcessor.low_V, 0, BeltProcessor.max_V, "%u");
                        ImGui::SliderInt(u8"V max", &BeltProcessor.high_V, BeltProcessor.low_V, BeltProcessor.max_V, "%u");

                    break;}

                case CAT_B_SIZE: {
                    ImGui::Text(u8"������: ��������� �������� ������");
                    ImGui::Separator();
                    ImGui::DragFloat(u8"min ������� �������", &BeltProcessor.minArea, 1.0, 1.0, 100000.0, "%.0f");
                    ImGui::DragFloat(u8"max ������� �������", &BeltProcessor.maxArea, 1.0, BeltProcessor.minArea, 100000.0, "%.0f");
                    break;}

                case CAT_B_MORPH: {
                    ImGui::Text(u8"������: ��������� ���������������� ��������������");
                    ImGui::Separator();
                    ImGui::Combo(u8"��� ����� ��������������", &BeltProcessor.morphType,
                                 u8"MORPH_ERODE\0MORPH_DILATE\0MORPH_OPEN\0MORPH_CLOSE\0MORPH_GRADIENT\0MORPH_TOPHAT\0MORPH_BLACKHAT\0MORPH_HITMISS\0\0");
                    ImGui::Combo(u8"��� ��������� ��������", &BeltProcessor.morphShape, u8"RECT\0ELLIPSE\0CROSS\0\0");
                    GUI.ShowHelpMarker(u8"��� ��������� �������� ���������������� ��������������\nRECT ����� �������, ELLIPSE ������� ���������, �� ����� ������������");
                    ImGui::SliderInt(u8"������ �����", &BeltProcessor.morphArea,  1, 10);
                    GUI.ShowHelpMarker(u8"������ ��������� �������� ���������������� ��������������\n��������� �������� ����� �����, ������� /""���������/"" ����� ������");
                    break;}

                case CAT_B_BLUR: {
                    ImGui::Text(u8"������: ��������� ��������");
                    ImGui::Separator();
                    ImGui::SliderInt(u8"������ ��������", &BeltProcessor.blurSize,  1, 10);
                    break;}

                case CAT_B_ACCUM: {
                    ImGui::Text(u8"������: ��������� ������������");
                    ImGui::Separator();
                    ImGui::SliderFloat(u8"saturationWeight", &BeltProcessor.saturationWeight, 0.0, 1.0, "%.2f");
                    ImGui::SliderInt(u8"satRangeMin", &BeltProcessor.satRangeMin,  0, 255);
                    ImGui::SliderInt(u8"satRangeMax", &BeltProcessor.satRangeMax,  0, 255);

                break;}

                case CAT_B_INFO: {
                    ImGui::DragFloat(u8"mmPerFrame", &BeltProcessor.mmPerFrame, 0.01, 0.0, 10.0, "%.2f");
                    break;}

                case CAT_COM: {

                    static int com_selected = 0;
                    const char* comspeeds[] = { "9 600", "57 600", "115 200", "250 000", "1M", "2M"};
                    const long  com_speeds[] = { 9600 ,57600, 115200, 250000, 1000000, 2000000};
                    static int speed_current = 0;

                    ImGui::Text(u8"��������� ������������ � ��������� ������������");
                    ImGui::Separator();

                    ImGui::BeginChild("##compnelleft", ImVec2(180,250), 0, 0);
                        if (ImGui::Button(u8"����������� �����")) {COM.List();}
                        GUI.ShowHelpMarker(u8"������� ������ ������ 16 ��������� ������");


                        ImGui::Text(u8"��������� �����:");

                        ImGui::BeginChild("##coms", ImVec2(150, 200), true);
                            for (int i = 0; i < 32; i++){
                                if (COM.IsPresent[i]){
                                    char label[32];
                                    sprintf(label, "COM%d", i+1);
                                    if (ImGui::Selectable(label, com_selected == i))
                                    com_selected = i;
                                }
                            }
                        ImGui::EndChild();
                    ImGui::EndChild();

                    ImGui::SameLine(0);

                    ImGui::BeginChild("##compnelright", ImVec2(800,0), 0, 0);
                        //ImGui::SetNextItemWidth(500);
                        ImGui::Combo(u8"��������", &speed_current, comspeeds, IM_ARRAYSIZE(com_speeds));
                        V.ComPort.Speed = com_speeds[speed_current];
                        if(!COM.isOpen)     {if (ImGui::Button(u8"������������")){COM.Open(com_selected);}}
                        else                {if (ImGui::Button(u8"�����������")){COM.closeCurrent();}}
                    ImGui::EndChild();

                    break;}

                case CAT_UI: {break;}
                case CAT_STATS: {
                    ImGui::Text(u8"���������� ���������");
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
                        ImGui::PlotLines(u8"��������", contours_values, IM_ARRAYSIZE(contours_values), values_offset, cont_buf, 0.0f, *std::max_element(std::begin(contours_values), std::end(contours_values)), ImVec2(0,80));
                    ImGui::Spacing();
                        char useful_cont_buf[8];
                        sprintf(useful_cont_buf,"%lu",Img.good_contours);
                        ImGui::PlotLines(u8"�������� ��������", good_contours_values, IM_ARRAYSIZE(good_contours_values), values_offset, useful_cont_buf, 0.0f, 70.0f, ImVec2(0,80));
                    ImGui::Spacing();
                    ImGui::SliderInt(u8"��������� FPS ���������", &Img.fps_average,  1, 127);
                    //ImGui::Checkbox(u8"����-����", &V.Input.FreezeFrame);

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

void GUI_class::drawSettingsBar(void){
    ImGuiStyle& style = ImGui::GetStyle();
     ImGui::BeginChild("##wfsettingsleft", ImVec2(210/*ImGui::GetWindowWidth()/3.8f*/, 0), true);
        for (int i = 0; i < NR_CAT; i++){
            if (!((V.procType == 0) && (i >= CAT_B_COLOR) && (i <= CAT_B_INFO) ||
                (V.procType == 1) && (i >= CAT_WF_EDGE) && (i <= CAT_WF_SHOW))) {
                if (setCats[i].matID != -1) {
                    btnStyleFrac((float)i/NR_CAT);
                        if (ImGui::Button(" ", ImVec2(10.0f, 22.0f))) {}
                    btnStylePop();

                   // chkColorSet();
                   //     ImGui::Checkbox("##a", &testBool);
                   // chkColorPop();

                    ImGui::SameLine();
                }

                else ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (20 + style.ItemSpacing.x));

                ImGui::PushStyleColor(ImGuiCol_HeaderHovered, (ImVec4)ImColor::HSV((float)i/NR_CAT, 0.7f, 0.7f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_HeaderActive, (ImVec4)ImColor::HSV((float)i/NR_CAT, 0.7f, 0.5f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_Header, (ImVec4)ImColor::HSV((float)i/NR_CAT, 0.7f, 0.3f, 1.0f));
                if (ImGui::Selectable(setCats[i].name.c_str(), catSelected == i))
                    catSelected = i;
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

void GUI_class::drawSettingsWindow(void){
    ImGuiStyle& style = ImGui::GetStyle();
    int spacing = style.ItemSpacing.x;
    ImGui::SetNextWindowSize(mainWinSize, ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(0, 20), ImGuiCond_Always);
    if (ImGui::Begin(u8"���������", p_open, ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoTitleBar)){

        ImGui::BeginChild("##fast", ImVec2(0, 38), 1, 0);
            char btnNameBuf[64] = {0};
            std::string sourceType;
            if (V.Input.Source == SOURCE_CAM) sourceType = u8"������";
            if (V.Input.Source == SOURCE_VIDEO) sourceType = u8"����";

            std::string processorType;
            if (V.procType == PROC_B) processorType = u8"�����";
            if (V.procType == PROC_WF) processorType = u8"�������";

            btnStyleOnOff(V.Input.CaptureRun);
                sprintf(btnNameBuf, u8"����������� (%s)", sourceType.c_str());
                if (ImGui::Button(btnNameBuf, ImVec2( ImGui::GetWindowContentRegionWidth()/4,0))) {Img.startCapture();} ImGui::SameLine();
            btnStylePop();

            btnStyleOnOff(COM.connectionOk);
                sprintf(btnNameBuf, u8"������������ (COM%u, %u)", V.ComPort.Number+1, V.ComPort.Speed);
                if (ImGui::Button(btnNameBuf, ImVec2(ImGui::GetWindowContentRegionWidth()/4,0))) {COM.Open(V.ComPort.Number);} ImGui::SameLine();
            btnStylePop();

            btnStyleOnOff(Img.procRun);
                sprintf(btnNameBuf, u8"��������� (%s)", processorType.c_str());
                if (ImGui::Button(btnNameBuf, ImVec2(ImGui::GetWindowContentRegionWidth()/4,0))) {} ImGui::SameLine();
            btnStylePop();

            btnStyleOnOff(COM.connectionOk);
                if (ImGui::Button(u8"���������", ImVec2(ImGui::GetWindowContentRegionWidth()/4,0))) {}
            btnStylePop();

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


}

std::string errorText = "";
void GUI_class::popupError(std::string text){
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


void GUI_class::Draw(void){
    drawMenuBar();
    drawMatWindows();
    drawSettingsWindow();
    ImGui::ShowDemoWindow();
    drawFileBrowser();
    drawPopUps();

    ImGui::Begin(u8"�����������", p_open, ImGuiWindowFlags_None);
        for (int cat=0; cat<NR_W_CAT; cat++){
            if (ImGui::TreeNode(matWinCats[cat].c_str())){
                for (int win=0; win<W_MAT_NR; win++){
                    if (MatWin[win].cat == cat){
                        ImGui::Checkbox(MatWin[win].title.c_str(), &MatWin[win].show);
                    }
                }
                ImGui::TreePop();
            }
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

void GUI_class::openPopUp(int id){
    openPopUpFlags[id] = 1;
}


void GUI_class::drawPopUps(void){
    if (openPopUpFlags[PU_COMM_STOP]) {ImGui::OpenPopup("popupCommStop"); openPopUpFlags[PU_COMM_STOP] = 0;}
    ImGui::SetNextWindowSize(ImVec2(300, 150));
    if (ImGui::BeginPopupModal("popupCommStop", NULL, ImGuiWindowFlags_AlwaysAutoResize|ImGuiWindowFlags_NoTitleBar)){
        ImGui::BeginChild("##popupper2", ImVec2(0, 100), 0, 0);
            ImGui::Text(u8"���� ��������� ���������. �������� ������������?");
        ImGui::EndChild();

        if (ImGui::Button(u8"��", ImVec2(ImGui::GetWindowContentRegionWidth()/2-4, 25))) {COM.tryGoodbye(); ImGui::CloseCurrentPopup();}
        ImGui::SameLine();

        if (ImGui::Button(u8"���", ImVec2(ImGui::GetWindowContentRegionWidth()/2-4, 25))) {ImGui::CloseCurrentPopup();}
        ImGui::EndPopup();
    }

    if (openPopUpFlags[PU_SOURCE_CLOSE]) {ImGui::OpenPopup("popupSourceClose"); openPopUpFlags[PU_SOURCE_CLOSE] = 0;}
    ImGui::SetNextWindowSize(ImVec2(300, 150));
    if (ImGui::BeginPopupModal("popupSourceClose", NULL, ImGuiWindowFlags_AlwaysAutoResize|ImGuiWindowFlags_NoTitleBar)){
        ImGui::BeginChild("##popupper2", ImVec2(0, 100), 0, 0);
            ImGui::Text(u8"������� ��������?");
        ImGui::EndChild();

        if (ImGui::Button(u8"��", ImVec2(ImGui::GetWindowContentRegionWidth()/2-4, 25))) {
            // stop processing here
            Img.stopCapture();
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button(u8"���", ImVec2(ImGui::GetWindowContentRegionWidth()/2-4, 25))) {ImGui::CloseCurrentPopup();}
        ImGui::EndPopup();
    }

    if (openPopUpFlags[PU_EXIT]) {ImGui::OpenPopup("popupExit"); openPopUpFlags[PU_EXIT] = 0;}
    ImGui::SetNextWindowSize(ImVec2(300, 150));
    if (ImGui::BeginPopupModal("popupExit", NULL, ImGuiWindowFlags_AlwaysAutoResize|ImGuiWindowFlags_NoTitleBar)){
        ImGui::BeginChild("##popupper2", ImVec2(0, 100), 0, 0);
            ImGui::Text(u8"����� �� ���������?");
        ImGui::EndChild();

        if (ImGui::Button(u8"��", ImVec2(ImGui::GetWindowContentRegionWidth()/2-4, 25))) {
            // stop processing here
            exitApplication();
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button(u8"���", ImVec2(ImGui::GetWindowContentRegionWidth()/2-4, 25))) {ImGui::CloseCurrentPopup();}
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

}

void GUI_class::VarInit(void){
    SetWin[W_SET_IN].title=u8"���������: ����";
    SetWin[W_SET_BG].title=u8"���������: ��������� ����";
    SetWin[W_SET_CONTOUR].title=u8"���������: �������";
    SetWin[W_SET_MASK].title=u8"���������: �����";
    SetWin[W_SET_COLOR].title=u8"���������: �����";
    SetWin[W_SET_OUT].title=u8"���������: �����";
    SetWin[W_SET_COM].title=u8"���������: ������������";
    SetWin[W_SET_INFO].title=u8"���������: ����������";
    SetWin[W_SET_HW].title=u8"���������� �������������";
    SetWin[W_SELF_COLORS].title=u8"��������� ���������: ���������";

    MatWin[W_MAT_IN].title=u8"����";
    MatWin[W_MAT_IN].mat = &Img.img_in;
    MatWin[W_MAT_IN].cat = W_CAT_INPUT;

    MatWin[W_MAT_WF_CONTOUR].title=u8"�������";
    MatWin[W_MAT_WF_CONTOUR].mat = &Img.img_canny_output;
    MatWin[W_MAT_WF_CONTOUR].cat = W_CAT_WF;

    MatWin[W_MAT_WF_MASK].title=u8"�����";
    MatWin[W_MAT_WF_MASK].mat = &Img.img_wholemask;
    MatWin[W_MAT_WF_MASK].cat = W_CAT_WF;

    MatWin[W_MAT_WF_BG].title=u8"������ ����";
    MatWin[W_MAT_WF_BG].mat = &Img.img_bs_back;
    MatWin[W_MAT_WF_BG].cat = W_CAT_WF;

    MatWin[W_MAT_WF_MOG].title=u8"��������� ��������� ����";
    MatWin[W_MAT_WF_MOG].mat = &Img.img_mog_output;
    MatWin[W_MAT_WF_MOG].cat = W_CAT_WF;

    MatWin[W_MAT_WF_MORPH].title=u8"����������";
    MatWin[W_MAT_WF_MORPH].mat = &Img.img_morph_out;
    MatWin[W_MAT_WF_MORPH].cat = W_CAT_WF;

    MatWin[W_MAT_WF_OUT].title=u8"�����";
    MatWin[W_MAT_WF_OUT].mat = &Img.img_output;
    MatWin[W_MAT_WF_OUT].cat = W_CAT_WF;

    MatWin[W_MAT_DEBUG].mat = &Img.img_debug;
    MatWin[W_MAT_DEBUG].title=u8"W_MAT_DEBUG";
    MatWin[W_MAT_DEBUG].cat = W_CAT_NONE;
    MatWin[W_MAT_DEBUG].show = 1;

    MatWin[W_MAT_PP_HUD].title=u8"HUD";
    MatWin[W_MAT_PP_HUD].mat = &preprocessor.HUD;
    MatWin[W_MAT_PP_HUD].cat = W_CAT_PP;

    MatWin[W_MAT_B_HUD].title=u8"HUD";
    MatWin[W_MAT_B_HUD].mat = &BeltProcessor.ejHUD;
    MatWin[W_MAT_B_HUD].cat = W_CAT_BELT;

    MatWin[W_MAT_B_ACCUM].title=u8"W_MAT_B_ACCUM";
    MatWin[W_MAT_B_ACCUM].mat = &BeltProcessor.matSatRendered;
    MatWin[W_MAT_B_ACCUM].cat = W_CAT_BELT;

    MatWin[W_MAT_B_RANGED].title=u8"matSatRanged";
    MatWin[W_MAT_B_RANGED].mat = &BeltProcessor.matSatRanged;
    MatWin[W_MAT_B_RANGED].cat = W_CAT_BELT;

    setCats[CAT_INPUT].name = u8"���� �����������";
    setCats[CAT_INPUT].matID = W_MAT_IN;

    setCats[CAT_PROCESSING].name = u8"���������";
    setCats[CAT_PROCESSING].matID = W_MAT_PP_HUD;

    setCats[CAT_WF_EDGE].name = u8"- �������: ����� ������";
    setCats[CAT_WF_EDGE].matID = W_MAT_WF_CONTOUR;

    setCats[CAT_WF_CONTOURS].name = u8"- �������: �������";
    setCats[CAT_WF_CONTOURS].matID = W_MAT_WF_MASK;

    setCats[CAT_WF_MORPHO].name = u8"- �������: ����������";
    setCats[CAT_WF_MORPHO].matID = W_MAT_WF_MORPH;

    setCats[CAT_WF_COLOR].name = u8"- �������: ����";
    setCats[CAT_WF_COLOR].matID = -1;

    setCats[CAT_WF_BS].name = u8"- �������: ��������� ����";
    setCats[CAT_WF_BS].matID = W_MAT_WF_MOG;

    setCats[CAT_WF_SHOW].name = u8"- �������: HUD";
    setCats[CAT_WF_SHOW].matID = W_MAT_WF_OUT;

    setCats[CAT_COM].name = u8"������������";
    setCats[CAT_COM].matID = -1;

    setCats[CAT_UI].name = u8"���������";
    setCats[CAT_UI].matID = -1;

    setCats[CAT_DEBUG].name = u8"Debug";
    setCats[CAT_DEBUG].matID = -1;

    setCats[CAT_STATS].name = u8"����������";
    setCats[CAT_STATS].matID = -1;

    setCats[CAT_B_COLOR].name = u8"- ������: ����";
    setCats[CAT_B_COLOR].matID = -1;

    setCats[CAT_B_SIZE].name = u8"- ������: �������";
    setCats[CAT_B_SIZE].matID = W_MAT_B_RANGED;

    setCats[CAT_B_MORPH].name = u8"- ������: ����������";
    setCats[CAT_B_MORPH].matID = -1;

    setCats[CAT_B_BLUR].name = u8"- ������: ��������";
    setCats[CAT_B_BLUR].matID = -1;

    setCats[CAT_B_ACCUM].name = u8"- ������: �����������";
    setCats[CAT_B_ACCUM].matID = W_MAT_B_ACCUM;

    setCats[CAT_B_INFO].name = u8"- ������: HUD";
    setCats[CAT_B_INFO].matID = W_MAT_B_HUD;
}

void GUI_class::Fill_Textures(void){
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




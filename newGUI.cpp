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
#include "gui/imguifilesystem.h"
#include "video_player.h"

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

    window.create(sf::VideoMode(GUI.ScreenW, GUI.ScreenH), GUI.WindowName);//,sf::Style::Fullscreen);

    window.setFramerateLimit(50);
    V.Info.ConsoleDestination = 1;
    V.UI.Fullscreen = 0;
    SetWin[W_SELF_LOG].show = 1;

    ImGui::SFML::Init(window, false);
    LoadFont();
}


void GUI_class::Worker(void){
    sf::Clock deltaClock;
    sf::Event event;

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

    ImGui::SFML::UpdateFontTexture(); // important call: updates font texture
    ImGui::GetIO().Fonts->Fonts[0]->AddRemapChar(0xCF, 0x043F);
    ConsoleOut(u8"���������: ��������� ������� ���������");
}

struct LogWidget_t
{
    ImGuiTextBuffer     Buf;
    ImGuiTextFilter     Filter;
    ImVector<int>       LineOffsets;        // Index to lines offset
    bool                ScrollToBottom;

    void    Clear()     { Buf.clear(); LineOffsets.clear(); }

    void    AddLog(const char* fmt, ...) IM_FMTARGS(2)
    {
        int old_size = Buf.size();
        va_list args;
        va_start(args, fmt);
        Buf.appendfv(fmt, args);
        va_end(args);
        for (int new_size = Buf.size(); old_size < new_size; old_size++)
            if (Buf[old_size] == '\n')
                LineOffsets.push_back(old_size);
        ScrollToBottom = true;
    }

    void    Draw(const char* title, bool* p_open = NULL)
    {
        ImGui::SetNextWindowSize(ImVec2(500,400), ImGuiCond_FirstUseEver);
        if (!ImGui::Begin(title, p_open))
        {
            ImGui::End();
            return;
        }
        if (ImGui::Button(u8"��������")) Clear();
        ImGui::SameLine();
        bool copy = ImGui::Button(u8"����������");
        ImGui::SameLine();
        Filter.Draw(u8"������", -100.0f);
        ImGui::Checkbox(u8"����� ���� (����� � cout)", &V.Info.ConsoleDestination);
        ImGui::Separator();
        ImGui::BeginChild("scrolling", ImVec2(0,0), false, ImGuiWindowFlags_HorizontalScrollbar);
        if (copy) ImGui::LogToClipboard();

        if (Filter.IsActive())
        {
            const char* buf_begin = Buf.begin();
            const char* line = buf_begin;
            for (int line_no = 0; line != NULL; line_no++)
            {
                const char* line_end = (line_no < LineOffsets.Size) ? buf_begin + LineOffsets[line_no] : NULL;
                if (Filter.PassFilter(line, line_end))
                    ImGui::TextUnformatted(line, line_end);
                line = line_end && line_end[1] ? line_end + 1 : NULL;
            }
        }
        else
        {
            ImGui::TextUnformatted(Buf.begin());
        }

        if (ScrollToBottom)
            ImGui::SetScrollHereY(1.0f);
        ScrollToBottom = false;
        ImGui::EndChild();
        ImGui::End();
    }
};

static LogWidget_t GUI_Log;

void GUI_class::ConsoleOut (string InString){
    time_t now;
    char the_date[32];
    the_date[0] = '\0';
    now = time(NULL);
    strftime(the_date, 32, "%H-%M-%S", gmtime(&now));
    if (V.Info.ConsoleDestination){
    GUI_Log.AddLog("%s: %s\n", the_date, InString.c_str());
    }
    else{
        cout << the_date << " " << InString <<endl;
    }
}

static bool browseButtonPressed;
ImVec4 ColorOn = ImColor(0.0f,1.0f,0.0f,1.0f);
ImVec4 ColorOff = ImColor(1.0f,0.0f,0.0f,1.0f);

void GUI_class::StatedText(string Input, bool state){
    if (state) {
        ImGui::PushStyleColor(ImGuiCol_Text, ColorOn);
    }
    else{
        ImGui::PushStyleColor(ImGuiCol_Text, ColorOff);
    }
    ImGui::BulletText(Input.c_str());
    ImGui::PopStyleColor();
}

void GUI_class::MenuBarStateList(void){
        StatedText(u8"������", V.Input.CaptureRun);
        ImGui::Separator();
        StatedText(u8"�����������", V.Input.CaptureRun);
        ImGui::Separator();
        StatedText(u8"���������", V.Input.CaptureRun);
        ImGui::Separator();

}

void drawSettingsWindow(void){
    ImGui::SetNextWindowSize(ImVec2(500, 440), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Example: Layout", p_open, ImGuiWindowFlags_MenuBar))
    {
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Close")) *p_open = false;
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        // left
        static int selected = 0;
        ImGui::BeginChild("left pane", ImVec2(150, 0), true);
        for (int i = 0; i < 100; i++)
        {
            char label[128];
            sprintf(label, "MyObject %d", i);
            if (ImGui::Selectable(label, selected == i))
                selected = i;
        }
        ImGui::EndChild();
        ImGui::SameLine();

        // right
        ImGui::BeginGroup();
            ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
                ImGui::Text("MyObject: %d", selected);
                ImGui::Separator();
                ImGui::TextWrapped("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. ");
            ImGui::EndChild();
            if (ImGui::Button("Revert")) {}
            ImGui::SameLine();
            if (ImGui::Button("Save")) {}
        ImGui::EndGroup();
    }
    ImGui::End();
}

void GUI_class::drawMenuBar(void){
    if (ImGui::BeginMainMenuBar()){
        if (ImGui::BeginMenu(u8"����")){
                if (ImGui::MenuItem(u8"��������� ������������")){
                    BrowseMode = BROWSE_LOAD;
                    browseButtonPressed = 1;
                    SetWin[W_SET_FILE].show = 1;
                }

                if (ImGui::MenuItem(u8"��������� ������������")){
                    BrowseMode = BROWSE_SAVE;
                    browseButtonPressed = 1;
                    SetWin[W_SET_FILE].show = 1;
                    };
                if (ImGui::MenuItem(u8"����� �� ���������")){ImGui::SFML::Shutdown();};
            ImGui::EndMenu();
        }
        ImGui::Separator();
        if (ImGui::BeginMenu(u8"��������")){
                ImGui::MenuItem(u8"����", NULL, &MatWin[W_MAT_IN].show);
                ImGui::MenuItem(u8"��������� ����: ���������", NULL, &MatWin[W_MAT_MOG].show);
                ImGui::MenuItem(u8"��������� ����: ������ ����", NULL, &MatWin[W_MAT_BG].show);
                ImGui::MenuItem(u8"�������", NULL, &MatWin[W_MAT_CONTOUR].show);
                ImGui::MenuItem(u8"����������", NULL, &MatWin[W_MAT_MORPH].show);
                ImGui::MenuItem(u8"�����", NULL, &MatWin[W_MAT_MASK].show);
                ImGui::MenuItem(u8"�����", NULL, &MatWin[W_MAT_OUT].show);
            ImGui::EndMenu();
        }
        ImGui::Separator();
         if (ImGui::BeginMenu(u8"��������� ���������")){
                ImGui::MenuItem(u8"����", "", &SetWin[W_SET_IN].show);
                ImGui::MenuItem(u8"��������� ����", "", &SetWin[W_SET_BG].show);
                ImGui::MenuItem(u8"����� ��������", "", &SetWin[W_SET_CONTOUR].show);
                ImGui::MenuItem(u8"�����", "", &SetWin[W_SET_MASK].show);
                ImGui::MenuItem(u8"����", "", &SetWin[W_SET_COLOR].show);
                ImGui::MenuItem(u8"������������", "", &SetWin[W_SET_COM].show);
                ImGui::MenuItem(u8"������", "", &SetWin[W_SET_HW].show);
                ImGui::MenuItem(u8"����������", "", &SetWin[W_SET_INFO].show);
            ImGui::EndMenu();
        }
        ImGui::Separator();
        if (ImGui::BeginMenu(u8"��������� ���������")){
                ImGui::MenuItem(u8"������� ���", "", &SetWin[W_SELF_COLORS].show);
                ImGui::MenuItem(u8"�������", "", &SetWin[W_SELF_LOG].show);
                if(ImGui::MenuItem(u8"������ �����", NULL, &V.UI.Fullscreen)){FullscreenChanged=1;}
                //ImGui::SliderFloat("Value", &f, 0.0f, 1.0f);
                //ImGui::InputFloat("Input", &f, 0.1f);
                //ImGui::Combo("Combo", &n, "Yes\0No\0Maybe\0\0");
            ImGui::EndMenu();
        }

        //if (ImGui::MenuItem(u8"�����", "Alt+F4")) {}
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

        MenuBarStateList();

        SameLine(GetWindowWidth()-140);
        ImGui::Text(WindowName.c_str());

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

                ImGuiWindowFlags MatWinFlags = ImGuiWindowFlags_AlwaysAutoResize;
                if(i == W_MAT_OUT) {MatWinFlags |= ImGuiWindowFlags_MenuBar;}

                ImGui::Begin(MatWin[i].title.c_str(), p_open, MatWinFlags);
                    if(i == W_MAT_OUT){
                        if (ImGui::BeginMenuBar()){
                            if (ImGui::BeginMenu(u8"��������")){
                                    ImGui::MenuItem(u8"�������", "", &V.Show.Contours);
                                    ImGui::MenuItem(u8"������", "", &V.Show.Centers);
                                    ImGui::MenuItem(u8"�������", "", &V.Show.BBoxes);
                                    ImGui::MenuItem(u8"������� ����", "", &V.Show.AvgColor);
                                    ImGui::MenuItem(u8"�������", "", &V.Show.Diameter);
                                    ImGui::MenuItem(u8"�������", "", &V.Show.Area);
                                    ImGui::MenuItem(u8"��������� ������� ������", "", &V.Show.FillAvg);
                                    ImGui::MenuItem(u8"��������� �����������", "", &V.Show.FilContour);
                                ImGui::EndMenu();
                            }
                        ImGui::EndMenuBar();
                        }
                    }
                    //if (V.Input.Source){
                            ImGui::Image(texture[i], ImVec2(MatWin[i].mat->cols, MatWin[i].mat->rows));//}
                    //else {ImGui::Image(texture[i], ImVec2(V.Cam.Width,V.Cam.Height));}

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

        if (SetWin[W_SET_FILE].show){
            ImGui::Begin("test", p_open, ImGuiWindowFlags_AlwaysAutoResize);
            static ImGuiFs::Dialog dlg;

            if (BrowseMode == BROWSE_SAVE){
                const char* chosenPath = dlg.saveFileDialog(browseButtonPressed);
                    if (strlen(chosenPath)>0) {
                        std::cout<<chosenPath<<std::endl;
                        SetWin[W_SET_FILE].show = 0;
                        File.SaveConfig(chosenPath);
                    }
            }

            if (BrowseMode == BROWSE_LOAD){
                const char* chosenPath = dlg.chooseFileDialog(browseButtonPressed);
                    if (strlen(chosenPath)>0) {
                        std::cout<<chosenPath<<std::endl;
                        SetWin[W_SET_FILE].show = 0;
                        File.ReadConfig(chosenPath);
                    }
            }

            browseButtonPressed = 0;
            ImGui::End();
         }
}

int thismarker = 0;
void GUI_class::drawSettingsWindow(void){

    ImGui::ShowDemoWindow();

    if (ImGui::Begin(u8"���������", p_open, ImGuiWindowFlags_None)){
        static int catSelected = 0;
        ImGui::BeginChild("##wfsettingsleft", ImVec2(ImGui::GetWindowWidth()/3.0, 0), true);
        for (int i = 0; i < NR_CAT; i++){
            if (ImGui::Selectable(settingsCatNames[i].c_str(), catSelected == i))
                catSelected = i;
        }
        ImGui::EndChild();
        ImGui::SameLine();

        ImGui::BeginGroup();
            ImGui::BeginChild("##wfitems", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));
            switch (catSelected) {
                case CAT_INPUT: {
                    ImGui::Combo(u8"�������� ��������", &V.Input.Source, u8"������\0�����\0\0");
                    GUI.ShowHelpMarker(u8"����� � ������ ��� �� ����� \"test_video.avi\"");

                    ImGui::SliderInt(u8"�������� ������ n-��� ����", &Img.show_mat_upd_target,  0, 127);
                    GUI.ShowHelpMarker(u8"����� �������� ��������������");

                    ImGui::Separator();

                    if (V.Input.Source == 0){
                        ImGui::Text(u8"��������� ������:");
                        ImGui::InputInt(u8"����� ������", &V.Cam.Number);
                        GUI.ShowHelpMarker(u8"�� ��������� 0");

                        const char* reso[] = { "320x240", "640x480", "800x600", "1024x768", "1280x720"};
                        const long  resol[] = {320,240,640,480,800,600,1024,768,1280,720};
                        static int reso_curr = 0;
                        if(ImGui::Combo(u8"����������", &reso_curr, reso, IM_ARRAYSIZE(reso))){
                            V.Cam.Width  = resol[reso_curr*2];
                            V.Cam.Height = resol[reso_curr*2+1];
                            Img.cam_open();
                        }
                        GUI.ShowHelpMarker(u8"��� ������, ��� �������");

                        if (ImGui::SliderFloat(u8"������� ������", &V.Cam.FPS,  5.0, 200.0,"%.0f")) Img.cam_update();
                        GUI.ShowHelpMarker(u8"�������������� �� ��� ��������\n��������� �������� ������� �� ����������");

                        ImGui::Text(u8"��������� �����:");
                        if (ImGui::SliderInt(u8"��������", &V.Cam.Gain,  0, 127)) Img.cam_update();
                        GUI.ShowHelpMarker(u8"������� ������������� ���������� �������� �� ��������� ����");

                        if (ImGui::SliderInt(u8"��������", &V.Cam.Contrast,  0, 127)) Img.cam_update();

                        if (ImGui::SliderInt(u8"����������", &V.Cam.Exposure,  -5, 0)) Img.cam_update();
                        GUI.ShowHelpMarker(u8"������� ��������������� � ��������");

                        if (ImGui::SliderInt(u8"������������", &V.Cam.Saturation,  0, 127)) Img.cam_update();

                        ImGui::Checkbox(u8"����-����", &V.Input.FreezeFrame);

                        if  (!V.Input.CaptureRun) {if (ImGui::Button(u8"������� ����� � ������")) {Img.cam_open();}}
                        else {if (ImGui::Button(u8"������� ����� � ������")) {V.Input.CaptureRun = 0;}}
                    }

                    if (V.Input.Source == 1){
                        ImGui::Text(u8"��������� ����� �����:");
                        //ImGui::InputText(u8"��� �����", test_text, 64);
                        //if (ImGui::Button(u8"������� ����")) {}
                        //if (ImGui::Button(u8"�����")){}
                        //ImGui::SameLine();
                        //if (ImGui::Button(u8"�������")){}
                        Img.capture_file = V.Input.Source;
                        if  (!V.Input.CaptureRun) {if (ImGui::Button(u8"������� ����� �� �����")) {Img.video_open();}}
                        else                {if (ImGui::Button(u8"������� ����� �� �����")) {Img.video_close();}}
                        ImGui::Text(u8"�������� ����� �����:");
                        ImGui::Text("%.0f x %.0f", videoPlayer.frameW, videoPlayer.frameH);
                        ImGui::Text(u8"� %u �� %u", videoPlayer.startFrame, videoPlayer.endFrame);
                        ImGui::Text(u8"%u, %.2f ms", videoPlayer.playbackMarker, videoPlayer.playbackMs);
                        ImGui::Text(u8"%u frames, %.1f FPS, %.1f FPS now", videoPlayer.fileLengthFrames, videoPlayer.videoFPS, videoPlayer.fps);
                        ImGui::Text(u8"part len = %u frames, portion = %.3f", videoPlayer.partLength, videoPlayer.playbackPortion);
                        ImGui::ProgressBar(videoPlayer.playbackPortion);
                        ImGui::SliderInt("start", &videoPlayer.startFrame, 0, videoPlayer.fileLengthFrames, "%u");
                        ImGui::SliderInt("end", &videoPlayer.endFrame, 0, videoPlayer.fileLengthFrames, "%u");
                        ImGui::Checkbox("pause", &videoPlayer.pause);

                        ImGui::SliderFloat("fpsLimiter", &videoPlayer.fpsLimiter, 1.0f, 200.0f, "%.1f");
                        if (ImGui::SliderInt("set position", &videoPlayer.playbackMarker, videoPlayer.startFrame, videoPlayer.endFrame, "&u"))
                            videoPlayer.setMarker(videoPlayer.playbackMarker);

                        /*
                        Rotation = Rotate;
                        cap.set(CAP_PROP_POS_FRAMES, StartFrame);
                        frameH = cap.get(CAP_PROP_FRAME_HEIGHT);
                        frameW = cap.get(CAP_PROP_FRAME_WIDTH);
                        playbackMarker = cap.get(CAP_PROP_POS_FRAMES);
                        playbackMs = cap.get(CAP_PROP_POS_MSEC);

                        videoFPS = cap.get(CAP_PROP_FPS);
                        fileLengthFrames =  cap.get(CAP_PROP_FRAME_COUNT);
                        fileLengthMs = fileLengthFrames / videoFPS;
                        */

                    }
                    break;}

                case CAT_PROCESSING: {
                    ImGui::Text(u8"��������� ������������� �����������:");
                    break;}

                case CAT_WF_CONTOURS: {
                    ImGui::Text(u8"��������� �������� ������, ���������� ���������:");
                    ImGui::DragFloat(u8"min ������� �����", &V.Contours.MinBBoxArea, 1.0, 1.0, V.Contours.MaxBBoxArea, "%.0f");
                    //ImGui::SliderFloat(, ,  0.0f, 100.0f+V.Contours.WideAreaRange*10000.0f, "%.0f");
                    GUI.ShowHelpMarker(u8"(pix^2)");
                    //ImGui::SliderFloat(u8"max ������� �����", &V.Contours.MaxBBoxArea,  0.0f, 10000.0f+V.Contours.WideAreaRange*20000.0f, "%.0f");
                    ImGui::DragFloat(u8"max ������� �����", &V.Contours.MaxBBoxArea, 1.0, V.Contours.MinBBoxArea, 500000000.0, "%.0f");
                    GUI.ShowHelpMarker(u8"(pix^2)");
                break;}

                case CAT_WF_EDGE: {
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
                    ImGui::Combo(u8"��� ����� ��������������", &V.Morph.Type, u8"RECT\0ELLIPSE\0CROSS\0\0");
                    GUI.ShowHelpMarker(u8"��� ��������� �������� ���������������� ��������������\nRECT ����� �������, ELLIPSE ������� ���������, �� ����� ������������");
                    ImGui::SliderInt(u8"������ �����", &V.Morph.Size,  1, 10);
                    GUI.ShowHelpMarker(u8"������ ��������� �������� ���������������� ��������������\n��������� �������� ����� �����, ������� /""���������/"" ����� ������");
                    break;}

                case CAT_WF_COLOR: {
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
                    ImGui::Text(u8"��������� HUD ��������");
                    ImGui::Checkbox(u8"�������", &V.Show.Contours);
                    ImGui::Checkbox(u8"������", &V.Show.Centers);
                    ImGui::Checkbox(u8"�������", &V.Show.BBoxes);
                    ImGui::Checkbox(u8"�������", &V.Show.Area);
                    ImGui::Checkbox(u8"�������", &V.Show.Diameter);
                    ImGui::Checkbox(u8"������ ������� ������� ������", &V.Show.FillAvg);
                    ImGui::Checkbox(u8"������ ������� �����������", &V.Show.FilContour);
                    break;}

                case CAT_B_COLOR: {break;}
                case CAT_B_SIZE: {break;}
                case CAT_B_MORPH: {break;}
                case CAT_B_BLUR: {break;}
                case CAT_B_ACCUM: {break;}
                case CAT_B_INFO: {break;}

                case CAT_COM: {
                    static int com_selected = 0;
                    //static int histo_selected = 0;
                    if (ImGui::Button(u8"����������� �����")) {COM.List();}
                    GUI.ShowHelpMarker(u8"������� ������ ������ 16 ��������� ������");
                    ImGui::Text(u8"��������� �����:");
                    for (int i = 0; i < 16; i++){
                        if (COM.IsPresent[i] == 1){
                            char label[128];
                            sprintf(label, "COM %d", i);
                            if (ImGui::Selectable(label, com_selected == i))
                            com_selected = i;
                        }
                    }
                    const char* comspeeds[] = { "1200", "2400", "4800", "9600", "19200", "38400", "57600", "115200"};
                    const long  com_speeds[] = { 1200,2400,4800,9600,19200,38400,57600,115200};

                    static int speed_current = 0;
                    ImGui::Combo(u8"��������", &speed_current, comspeeds, IM_ARRAYSIZE(com_speeds));
                    V.ComPort.Speed = com_speeds[speed_current];
                    if(!V.ComPort.Connected){if (ImGui::Button(u8"������������")){COM.Open(com_selected);}}
                    else                {if (ImGui::Button(u8"�����������")){COM.Close(com_selected);}}
                    break;}

                case CAT_UI: {break;}
                case CAT_STATS: {
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
                case CAT_DEBUG: {break;}
            }
            ImGui::EndChild();
        ImGui::EndGroup();
    }
    ImGui::End();
}

void GUI_class::drawSettingsWindowOld(void){
     if (SetWin[W_SET_IN].show){
        ImGui::Begin(SetWin[W_SET_IN].title.c_str(), p_open, /*ImGuiWindowFlags_MenuBar|*/ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::End();
    }

    if (SetWin[W_SET_BG].show){
        ImGui::Begin(SetWin[W_SET_BG].title.c_str(), p_open, /*ImGuiWindowFlags_MenuBar|*/ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::End();
    }

    if (SetWin[W_SET_CONTOUR].show){
        ImGui::Begin(SetWin[W_SET_CONTOUR].title.c_str(), p_open, /*ImGuiWindowFlags_MenuBar|*/ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::End();
    }

    if (SetWin[W_SET_MASK].show){
        ImGui::Begin(SetWin[W_SET_MASK].title.c_str(), p_open, /*ImGuiWindowFlags_MenuBar|*/ImGuiWindowFlags_AlwaysAutoResize);
           ImGui::End();
    }

    if (SetWin[W_SET_COLOR].show){
        ImGui::Begin(SetWin[W_SET_COLOR].title.c_str(), p_open, /*ImGuiWindowFlags_MenuBar|*/ImGuiWindowFlags_AlwaysAutoResize);

        ImGui::End();
    }

    if (SetWin[W_SET_OUT].show){
        ImGui::Begin(SetWin[W_SET_OUT].title.c_str(), p_open, /*ImGuiWindowFlags_MenuBar|*/ImGuiWindowFlags_AlwaysAutoResize);

        ImGui::End();
    }

    if (SetWin[W_SET_COM].show){
        ImGui::Begin(SetWin[W_SET_COM].title.c_str(), p_open, /*ImGuiWindowFlags_MenuBar|*/ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::End();
    }

    if (SetWin[W_SET_HW].show){
        ImGui::Begin(SetWin[W_SET_HW].title.c_str(), p_open, /*ImGuiWindowFlags_MenuBar|*/ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::Separator();
            ImGui::Separator();
            ImGui::BulletText(u8"��� ���� ������ �� ��������");
            ImGui::Separator();
            ImGui::Separator();
            //ImGui::Checkbox(u8"�������� ���������", &b);
            //ImGui::SliderInt(u8"�������������", &test_int,  0, 255);
            ImGui::Separator();
            //ImGui::Checkbox(u8"�������� �� ���������", &b);
            //ImGui::SliderInt(u8"�������������", &test_int,  0, 255);
            ImGui::Separator();
            static ImVec4 color = ImColor(114, 144, 154, 200);
            ImGui::ColorPicker4(u8"������ ���������", (float*)&color);
            ImGui::Separator();
            ImGui::SliderInt(u8"�������� ��������", &test_int,  0, 255);
        ImGui::End();
    }

    if (SetWin[W_SET_INFO].show){
        ImGui::Begin(SetWin[W_SET_INFO].title.c_str(), p_open, /*ImGuiWindowFlags_MenuBar|*/ImGuiWindowFlags_AlwaysAutoResize);

        ImGui::End();
    }

    if (SetWin[W_SELF_COLORS].show){
        ImGui::Begin(SetWin[W_SELF_COLORS].title.c_str(), p_open, /*ImGuiWindowFlags_MenuBar|*/ImGuiWindowFlags_AlwaysAutoResize);
            static int style_idx = 1;
            if (ImGui::Combo(u8"�������� �����", &style_idx, u8"������������\0Ҹ����\0�������\0")){
                switch (style_idx){
                    case 0: ImGui::StyleColorsClassic(); break;
                    case 1: ImGui::StyleColorsDark(); break;
                    case 2: ImGui::StyleColorsLight(); break;
                }
            }
            ImGui::ShowFontSelector(u8"�����");
            if (ImGui::TreeNode(u8"�������")){
                ImGuiStyle& style = ImGui::GetStyle();
                ImGui::Text("Main");
                ImGui::SliderFloat2("WindowPadding", (float*)&style.WindowPadding, 0.0f, 20.0f, "%.0f");
                ImGui::SliderFloat("PopupRounding", &style.PopupRounding, 0.0f, 16.0f, "%.0f");
                ImGui::SliderFloat2("FramePadding", (float*)&style.FramePadding, 0.0f, 20.0f, "%.0f");
                ImGui::SliderFloat2("ItemSpacing", (float*)&style.ItemSpacing, 0.0f, 20.0f, "%.0f");
                ImGui::SliderFloat2("ItemInnerSpacing", (float*)&style.ItemInnerSpacing, 0.0f, 20.0f, "%.0f");
                ImGui::SliderFloat2("TouchExtraPadding", (float*)&style.TouchExtraPadding, 0.0f, 10.0f, "%.0f");
                ImGui::SliderFloat("IndentSpacing", &style.IndentSpacing, 0.0f, 30.0f, "%.0f");
                ImGui::SliderFloat("ScrollbarSize", &style.ScrollbarSize, 1.0f, 20.0f, "%.0f");
                ImGui::SliderFloat("GrabMinSize", &style.GrabMinSize, 1.0f, 20.0f, "%.0f");
                ImGui::Spacing();
                ImGui::Text("Borders");
                ImGui::SliderFloat("WindowBorderSize", &style.WindowBorderSize, 0.0f, 1.0f, "%.0f");
                ImGui::SliderFloat("ChildBorderSize", &style.ChildBorderSize, 0.0f, 1.0f, "%.0f");
                ImGui::SliderFloat("PopupBorderSize", &style.PopupBorderSize, 0.0f, 1.0f, "%.0f");
                ImGui::SliderFloat("FrameBorderSize", &style.FrameBorderSize, 0.0f, 1.0f, "%.0f");
                ImGui::Spacing();
                ImGui::Text("Rounding");
                ImGui::SliderFloat("WindowRounding", &style.WindowRounding, 0.0f, 14.0f, "%.0f");
                ImGui::SliderFloat("ChildRounding", &style.ChildRounding, 0.0f, 16.0f, "%.0f");
                ImGui::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 12.0f, "%.0f");
                ImGui::SliderFloat("ScrollbarRounding", &style.ScrollbarRounding, 0.0f, 12.0f, "%.0f");
                ImGui::SliderFloat("GrabRounding", &style.GrabRounding, 0.0f, 12.0f, "%.0f");
                ImGui::Spacing();
                ImGui::Text("Alignment");
                ImGui::SliderFloat2("WindowTitleAlign", (float*)&style.WindowTitleAlign, 0.0f, 1.0f, "%.2f");
                ImGui::SliderFloat2("ButtonTextAlign", (float*)&style.ButtonTextAlign, 0.0f, 1.0f, "%.2f"); ImGui::SameLine(); GUI.ShowHelpMarker("Alignment applies when a button is larger than its text content.");
                ImGui::Text("Safe Area Padding"); ImGui::SameLine(); GUI.ShowHelpMarker("Adjust if you cannot see the edges of your screen (e.g. on a TV where scaling has not been configured).");
                ImGui::SliderFloat2("DisplaySafeAreaPadding", (float*)&style.DisplaySafeAreaPadding, 0.0f, 30.0f, "%.0f");
            ImGui::TreePop();
        }
        ImGui::End();


    }

    if (SetWin[W_SELF_LOG].show){
            GUI_Log.Draw(u8"�������", p_open);
    }

}
void GUI_class::Draw(void){
    drawMenuBar();
    drawMatWindows();
    drawSettingsWindow();
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

    MatWin[W_MAT_IN].title=u8"�����������: ����";
    MatWin[W_MAT_CONTOUR].title=u8"�����������: �������";
    MatWin[W_MAT_MASK].title=u8"�����������: �����";
    MatWin[W_MAT_BG].title=u8"��������� ����: ������ ����";
    MatWin[W_MAT_MOG].title=u8"��������� ����: ���������";
    MatWin[W_MAT_MORPH].title=u8"�����������: ����������";
    MatWin[W_MAT_OUT].title=u8"�����������: �����";

    MatWin[W_MAT_IN].mat = &Img.img_in;
    MatWin[W_MAT_CONTOUR].mat = &Img.img_canny_output;
    MatWin[W_MAT_MASK].mat = &Img.img_wholemask;
    MatWin[W_MAT_BG].mat = &Img.img_bs_back;
    MatWin[W_MAT_MOG].mat = &Img.img_mog_output;
    MatWin[W_MAT_MORPH].mat = &Img.img_morph_out;
    MatWin[W_MAT_OUT].mat = &Img.img_output;

    MatWin[W_MAT_DEBUG].mat = &Img.img_debug;
    MatWin[W_MAT_DEBUG].title=u8"W_MAT_DEBUG";
    MatWin[W_MAT_DEBUG].show = 1;

    settingsCatNames[CAT_INPUT] = u8"���� �����������";
    settingsCatNames[CAT_PROCESSING] = u8"���������";
    settingsCatNames[CAT_WF_EDGE] = u8"- �������: ����� ������";
    settingsCatNames[CAT_WF_CONTOURS] = u8"- �������: �������";
    settingsCatNames[CAT_WF_MORPHO] = u8"- �������: ����������";
    settingsCatNames[CAT_WF_COLOR] = u8"- �������: ����";
    settingsCatNames[CAT_WF_BS] = u8"- �������: ��������� ����";
    //settingsCatNames[CAT_WF_BS_MOG] = u8"-- �������: �� MOG";
    //settingsCatNames[CAT_WF_BS_MOG2] = u8"-- �������: �� MOG2";
    //settingsCatNames[CAT_WF_BS_CNT] =  u8"-- �������: �� CNT";
    settingsCatNames[CAT_WF_SHOW] = u8"- �������: HUD";
    settingsCatNames[CAT_COM] = u8"������������";
    settingsCatNames[CAT_UI] = u8"���������";
    settingsCatNames[CAT_DEBUG] = u8"Debug";
    settingsCatNames[CAT_STATS] = u8"����������";
    settingsCatNames[CAT_B_COLOR] = u8"- ������: ����";
    settingsCatNames[CAT_B_SIZE] = u8"- ������: �������";
    settingsCatNames[CAT_B_MORPH] = u8"- ������: ����������";
    settingsCatNames[CAT_B_BLUR] = u8"- ������: ��������";
    settingsCatNames[CAT_B_ACCUM] = u8"- ������: �����������";
    settingsCatNames[CAT_B_INFO] = u8"- ������: ����������";
}

void GUI_class::Fill_Textures(void){
    for(int i=0; i<W_MAT_NR; i++){
        if (MatWin[i].show){
            MatWin[i].write.lock();
            if(!MatWin[i].mat->empty()){
                if (MatWin[i].mat->type() == 16){cv::cvtColor(*MatWin[i].mat, frameRGBA[i], cv::COLOR_BGR2RGBA);}
                else if (MatWin[i].mat->type() == 0){cv::cvtColor(*MatWin[i].mat, frameRGBA[i], cv::COLOR_GRAY2RGBA);}

                image[i].create(frameRGBA[i].cols, frameRGBA[i].rows, frameRGBA[i].ptr());
                texture[i].loadFromImage(image[i]);
                sprite[i].setTexture(texture[i]);
            }
            else{

            }
             MatWin[i].write.unlock();
        }
    }
}






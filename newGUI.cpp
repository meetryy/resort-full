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
    ConsoleOut(u8"ИНТЕРФЕЙС: Закгрузка шрифтов завершена");
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
        if (ImGui::Button(u8"Очистить")) Clear();
        ImGui::SameLine();
        bool copy = ImGui::Button(u8"Копировать");
        ImGui::SameLine();
        Filter.Draw(u8"Фильтр", -100.0f);
        ImGui::Checkbox(u8"Вывод сюда (иначе в cout)", &V.Info.ConsoleDestination);
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
        StatedText(u8"Камера", V.Input.CaptureRun);
        ImGui::Separator();
        StatedText(u8"Комуникация", V.Input.CaptureRun);
        ImGui::Separator();
        StatedText(u8"Сепарация", V.Input.CaptureRun);
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
        if (ImGui::BeginMenu(u8"Файл")){
                if (ImGui::MenuItem(u8"Загрузить конфигурацию")){
                    BrowseMode = BROWSE_LOAD;
                    browseButtonPressed = 1;
                    SetWin[W_SET_FILE].show = 1;
                }

                if (ImGui::MenuItem(u8"Сохранить конфигурацию")){
                    BrowseMode = BROWSE_SAVE;
                    browseButtonPressed = 1;
                    SetWin[W_SET_FILE].show = 1;
                    };
                if (ImGui::MenuItem(u8"Выйти из программы")){ImGui::SFML::Shutdown();};
            ImGui::EndMenu();
        }
        ImGui::Separator();
        if (ImGui::BeginMenu(u8"Показать")){
                ImGui::MenuItem(u8"Вход", NULL, &MatWin[W_MAT_IN].show);
                ImGui::MenuItem(u8"Вычитание фона: результат", NULL, &MatWin[W_MAT_WF_MOG].show);
                ImGui::MenuItem(u8"Вычитание фона: модель фона", NULL, &MatWin[W_MAT_WF_BG].show);
                ImGui::MenuItem(u8"Контуры", NULL, &MatWin[W_MAT_WF_CONTOUR].show);
                ImGui::MenuItem(u8"Морфология", NULL, &MatWin[W_MAT_WF_MORPH].show);
                ImGui::MenuItem(u8"Маска", NULL, &MatWin[W_MAT_WF_MASK].show);
                ImGui::MenuItem(u8"Выход", NULL, &MatWin[W_MAT_WF_OUT].show);
            ImGui::EndMenu();
        }
        ImGui::Separator();
         if (ImGui::BeginMenu(u8"Настройки обработки")){
                ImGui::MenuItem(u8"Вход", "", &SetWin[W_SET_IN].show);
                ImGui::MenuItem(u8"Отделение фона", "", &SetWin[W_SET_BG].show);
                ImGui::MenuItem(u8"Поиск контуров", "", &SetWin[W_SET_CONTOUR].show);
                ImGui::MenuItem(u8"Маска", "", &SetWin[W_SET_MASK].show);
                ImGui::MenuItem(u8"Цвет", "", &SetWin[W_SET_COLOR].show);
                ImGui::MenuItem(u8"Коммуникации", "", &SetWin[W_SET_COM].show);
                ImGui::MenuItem(u8"Железо", "", &SetWin[W_SET_HW].show);
                ImGui::MenuItem(u8"Информация", "", &SetWin[W_SET_INFO].show);
            ImGui::EndMenu();
        }
        ImGui::Separator();
        if (ImGui::BeginMenu(u8"Настройки программы")){
                ImGui::MenuItem(u8"Внешний вид", "", &SetWin[W_SELF_COLORS].show);
                ImGui::MenuItem(u8"Консоль", "", &SetWin[W_SELF_LOG].show);
                if(ImGui::MenuItem(u8"Полный экран", NULL, &V.UI.Fullscreen)){FullscreenChanged=1;}
                //ImGui::SliderFloat("Value", &f, 0.0f, 1.0f);
                //ImGui::InputFloat("Input", &f, 0.1f);
                //ImGui::Combo("Combo", &n, "Yes\0No\0Maybe\0\0");
            ImGui::EndMenu();
        }

        //if (ImGui::MenuItem(u8"Выход", "Alt+F4")) {}
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
                if(i == W_MAT_WF_OUT) {MatWinFlags |= ImGuiWindowFlags_MenuBar;}

                ImGui::Begin(MatWin[i].title.c_str(), p_open, MatWinFlags);
                    if(i == W_MAT_WF_OUT){
                        if (ImGui::BeginMenuBar()){
                            if (ImGui::BeginMenu(u8"Показать")){
                                    ImGui::MenuItem(u8"Контуры", "", &V.Show.Contours);
                                    ImGui::MenuItem(u8"Центры", "", &V.Show.Centers);
                                    ImGui::MenuItem(u8"Коробки", "", &V.Show.BBoxes);
                                    ImGui::MenuItem(u8"Средний цвет", "", &V.Show.AvgColor);
                                    ImGui::MenuItem(u8"Диаметр", "", &V.Show.Diameter);
                                    ImGui::MenuItem(u8"Площадь", "", &V.Show.Area);
                                    ImGui::MenuItem(u8"Заполнить средним цветом", "", &V.Show.FillAvg);
                                    ImGui::MenuItem(u8"Заполнить результатом", "", &V.Show.FilContour);
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
                    ImGui::Text(u8"Изображение отсутстует!");
                    ImGui::Text(u8"(обоработка не начата)");
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

    if (ImGui::Begin(u8"Настройки", p_open, ImGuiWindowFlags_None)){
        static int catSelected = 0;
        ImGui::BeginChild("##wfsettingsleft", ImVec2(ImGui::GetWindowWidth()/3.5, 0), true);
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
                    ImGui::Text(u8"Настройки источника изображения");
                    ImGui::Separator();

                    ImGui::Combo(u8"Источник картинки", &V.Input.Source, u8"Камера\0Видео\0\0");
                    GUI.ShowHelpMarker(u8"Видео с камеры или из файла \"test_video.avi\"");

                    ImGui::SliderInt(u8"Выводить каждый n-ный кадр", &Img.show_mat_upd_target,  0, 127);
                    GUI.ShowHelpMarker(u8"Может ускорить быстродействие");

                    ImGui::Separator();

                    if (V.Input.Source == 0){
                        ImGui::Text(u8"Настройки камеры:");
                        ImGui::InputInt(u8"Номер камеры", &V.Cam.Number);
                        GUI.ShowHelpMarker(u8"По умолчанию 0");

                        const char* reso[] = { "320x240", "640x480", "800x600", "1024x768", "1280x720"};
                        const long  resol[] = {320,240,640,480,800,600,1024,768,1280,720};
                        static int reso_curr = 0;
                        if(ImGui::Combo(u8"Разрешение", &reso_curr, reso, IM_ARRAYSIZE(reso))){
                            V.Cam.Width  = resol[reso_curr*2];
                            V.Cam.Height = resol[reso_curr*2+1];
                            Img.cam_open();
                        }
                        GUI.ShowHelpMarker(u8"Чем меньше, тем быстрее");

                        if (ImGui::SliderFloat(u8"Частота кадров", &V.Cam.FPS,  5.0, 200.0,"%.0f")) Img.cam_update();
                        GUI.ShowHelpMarker(u8"Поддерживаются не все скорости\nВозможная скорость зависит от экспозиции");

                        ImGui::Text(u8"Коррекция цвета:");
                        if (ImGui::SliderInt(u8"Усиление", &V.Cam.Gain,  0, 127)) Img.cam_update();
                        GUI.ShowHelpMarker(u8"Следует устанавливать наименьшее значение во избежание шума");

                        if (ImGui::SliderInt(u8"Контраст", &V.Cam.Contrast,  0, 127)) Img.cam_update();

                        if (ImGui::SliderInt(u8"Экспозиция", &V.Cam.Exposure,  -5, 0)) Img.cam_update();
                        GUI.ShowHelpMarker(u8"Обратно пропорциональна к скорости");

                        if (ImGui::SliderInt(u8"Насыщенность", &V.Cam.Saturation,  0, 127)) Img.cam_update();

                        ImGui::Checkbox(u8"Стоп-кадр", &V.Input.FreezeFrame);


                    }

                    if (V.Input.Source == 1){
                        //ImGui::Text(u8"Настройки видео файла:");
                        //ImGui::InputText(u8"Имя файла", test_text, 64);
                        //if (ImGui::Button(u8"Открыть файл")) {}
                        //if (ImGui::Button(u8"Пауза")){}
                        //ImGui::SameLine();
                        //if (ImGui::Button(u8"Сначала")){}
                        Img.capture_file = V.Input.Source;

                        #warning TODO add video file list here
                        #warning TODO open video file by name

                        ImGui::Text(u8"Свойства видео файла:");
                        ImGui::Text("%.0f x %.0f", videoPlayer.frameW, videoPlayer.frameH);
                        //ImGui::Text(u8"с %u до %u", videoPlayer.startFrame, videoPlayer.endFrame);
                        //ImGui::Text(u8"%u, %.2f ms", videoPlayer.playbackMarker, videoPlayer.playbackMs);
                        ImGui::Text(u8"Всего %u кадров, %.1f FPS, %.1f FPS now", videoPlayer.fileLengthFrames, videoPlayer.videoFPS, videoPlayer.fps);

                        char pBarBuf[64];
                        sprintf(pBarBuf, "%i / %i", videoPlayer.playbackMarker - videoPlayer.startFrame, videoPlayer.partLength);

                        ImGui::ProgressBar(videoPlayer.playbackPortion, ImVec2(0.f,0.f), pBarBuf);
                        ImGui::SliderInt(u8"Начало, кадр", &videoPlayer.startFrame, 0, videoPlayer.fileLengthFrames, "%u");
                        ImGui::SliderInt(u8"Конец, кадр", &videoPlayer.endFrame, videoPlayer.startFrame, videoPlayer.fileLengthFrames, "%u");
                        ImGui::Checkbox(u8"Пауза", &videoPlayer.pause);

                        ImGui::SliderFloat(u8"Ограничитель скорости", &videoPlayer.fpsLimiter, 1.0f, 200.0f, "%.1f ms");
                        if (videoPlayer.pause){
                            if (ImGui::SliderInt(u8"Позиция", &videoPlayer.playbackMarker, videoPlayer.startFrame, videoPlayer.endFrame, "%u"))
                                videoPlayer.setMarker(videoPlayer.playbackMarker);
                        }
                    }

                    if  (!V.Input.CaptureRun) {
                            if (ImGui::Button(u8"Открыть поток")) {
                                if (V.Input.Source == 0){
                                    Img.cam_open();
                                }

                                if (V.Input.Source == 1){
                                    videoPlayer.Start("test_video.avi", videoPlayer.startFrame, -1, -1);
                                    //BeltProcessor.Init();
                                    BeltProcessor.initDone = 0;
                                    V.Input.CaptureRun = 1;
                                    //Img.video_open();
                                }
                            }
                        }

                        else {
                            if (ImGui::Button(u8"Закрыть поток")) {
                                if (V.Input.Source == 0){
                                    V.Input.CaptureRun = 0;
                                }

                                if (V.Input.Source == 1){
                                    videoPlayer.Stop();
                                    V.Input.CaptureRun = 0;
                                    BeltProcessor.initDone = 0;
                                }
                            }
                        }

                    break;}

                case CAT_PROCESSING: {
                    ImGui::Text(u8"Настройки обработки изображения");
                    ImGui::Separator();

                    ImGui::Combo(u8"Основной алгоритм работы", &V.procType, u8"Водопад\0Ремень\0\0");
                    ImGui::Separator();

                    ImGui::Text(u8"Настройки препроцессора изображения:");
                    ImGui::Checkbox("on", &preprocessor.isOn);

                    const char* items[] = { u8"0", u8"90 ЧС", u8"180", u8"90 ПЧС"};
                    static int item_current = 0;
                    ImGui::Combo(u8"Поворот", &item_current, items, IM_ARRAYSIZE(items));
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
                    ImGui::Text(u8"Настройки коммуникации с аппартным обеспечением");
                    ImGui::Separator();

                    static int com_selected = 0;
                    //static int histo_selected = 0;
                    if (ImGui::Button(u8"Сканировать порты")) {COM.List();}
                    GUI.ShowHelpMarker(u8"Вывести список первых 16 доступных портов");
                    ImGui::Text(u8"Доступные порты:");
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
                    ImGui::Combo(u8"Скорость", &speed_current, comspeeds, IM_ARRAYSIZE(com_speeds));
                    V.ComPort.Speed = com_speeds[speed_current];
                    if(!V.ComPort.Connected){if (ImGui::Button(u8"Подключиться")){COM.Open(com_selected);}}
                    else                {if (ImGui::Button(u8"Отключиться")){COM.Close(com_selected);}}
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
                case CAT_DEBUG: {
                    ImGui::Checkbox("show debug mat", &MatWin[W_MAT_DEBUG].show);

                    break;}
            }
            ImGui::EndChild();
        ImGui::EndGroup();
    }
    ImGui::End();
}
/*
    if (SetWin[W_SELF_COLORS].show){
        ImGui::Begin(SetWin[W_SELF_COLORS].title.c_str(), p_open, ImGuiWindowFlags_AlwaysAutoResize);
            static int style_idx = 1;
            if (ImGui::Combo(u8"Цветовая схема", &style_idx, u8"Классическая\0Тёмная\0Светлая\0")){
                switch (style_idx){
                    case 0: ImGui::StyleColorsClassic(); break;
                    case 1: ImGui::StyleColorsDark(); break;
                    case 2: ImGui::StyleColorsLight(); break;
                }
            }
            ImGui::ShowFontSelector(u8"Шрифт");
            if (ImGui::TreeNode(u8"Размеры")){
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
            GUI_Log.Draw(u8"Консоль", p_open);
    }
*/





void GUI_class::Draw(void){
    drawMenuBar();
    drawMatWindows();
    drawSettingsWindow();

    ImGui::Begin(u8"Изображения", p_open, ImGuiWindowFlags_None);
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
}


void GUI_class::VarInit(void){
    SetWin[W_SET_IN].title=u8"Настройки: вход";
    SetWin[W_SET_BG].title=u8"Настройки: вычитание фона";
    SetWin[W_SET_CONTOUR].title=u8"Настройки: контуры";
    SetWin[W_SET_MASK].title=u8"Настройки: маска";
    SetWin[W_SET_COLOR].title=u8"Настройки: цвета";
    SetWin[W_SET_OUT].title=u8"Настройки: выход";
    SetWin[W_SET_COM].title=u8"Настройки: коммуникации";
    SetWin[W_SET_INFO].title=u8"Настройки: информация";
    SetWin[W_SET_HW].title=u8"Управление оборудованием";
    SetWin[W_SELF_COLORS].title=u8"Настройки программы: интерфейс";

    MatWin[W_MAT_IN].title=u8"Вход";
    MatWin[W_MAT_IN].mat = &Img.img_in;
    MatWin[W_MAT_IN].cat = W_CAT_INPUT;

    MatWin[W_MAT_WF_CONTOUR].title=u8"Контуры";
    MatWin[W_MAT_WF_CONTOUR].mat = &Img.img_canny_output;
    MatWin[W_MAT_WF_CONTOUR].cat = W_CAT_WF;

    MatWin[W_MAT_WF_MASK].title=u8"Маска";
    MatWin[W_MAT_WF_MASK].mat = &Img.img_wholemask;
    MatWin[W_MAT_WF_MASK].cat = W_CAT_WF;

    MatWin[W_MAT_WF_BG].title=u8"Модель фона";
    MatWin[W_MAT_WF_BG].mat = &Img.img_bs_back;
    MatWin[W_MAT_WF_BG].cat = W_CAT_WF;

    MatWin[W_MAT_WF_MOG].title=u8"Результат вычитания фона";
    MatWin[W_MAT_WF_MOG].mat = &Img.img_mog_output;
    MatWin[W_MAT_WF_MOG].cat = W_CAT_WF;

    MatWin[W_MAT_WF_MORPH].title=u8"Морфология";
    MatWin[W_MAT_WF_MORPH].mat = &Img.img_morph_out;
    MatWin[W_MAT_WF_MORPH].cat = W_CAT_WF;

    MatWin[W_MAT_WF_OUT].title=u8"Выход";
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
    MatWin[W_MAT_B_HUD].mat = &BeltProcessor.mainMatHUD;
    MatWin[W_MAT_B_HUD].cat = W_CAT_BELT;

    MatWin[W_MAT_B_ACCUM].title=u8"W_MAT_B_ACCUM";
    MatWin[W_MAT_B_ACCUM].mat = &BeltProcessor.matSatRendered;
    MatWin[W_MAT_B_ACCUM].cat = W_CAT_BELT;

    MatWin[W_MAT_B_RANGED].title=u8"W_MAT_B_RANGED";
    MatWin[W_MAT_B_RANGED].mat = &BeltProcessor.matSatRanged;
    MatWin[W_MAT_B_RANGED].cat = W_CAT_BELT;

    settingsCatNames[CAT_INPUT] = u8"Вход изображения";
    settingsCatNames[CAT_PROCESSING] = u8"Обработка";
    settingsCatNames[CAT_WF_EDGE] = u8"- Водопад: Поиск границ";
    settingsCatNames[CAT_WF_CONTOURS] = u8"- Водопад: Контуры";
    settingsCatNames[CAT_WF_MORPHO] = u8"- Водопад: Морфология";
    settingsCatNames[CAT_WF_COLOR] = u8"- Водопад: Цвет";
    settingsCatNames[CAT_WF_BS] = u8"- Водопад: отделение фона";
    settingsCatNames[CAT_WF_SHOW] = u8"- Водопад: HUD";
    settingsCatNames[CAT_COM] = u8"Коммуникация";
    settingsCatNames[CAT_UI] = u8"Интерфейс";
    settingsCatNames[CAT_DEBUG] = u8"Debug";
    settingsCatNames[CAT_STATS] = u8"Статистика";
    settingsCatNames[CAT_B_COLOR] = u8"- Ремень: Цвет";
    settingsCatNames[CAT_B_SIZE] = u8"- Ремень: Размеры";
    settingsCatNames[CAT_B_MORPH] = u8"- Ремень: Морфология";
    settingsCatNames[CAT_B_BLUR] = u8"- Ремень: Размытие";
    settingsCatNames[CAT_B_ACCUM] = u8"- Ремень: Аккумулятор";
    settingsCatNames[CAT_B_INFO] = u8"- Ремень: Статистика";
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
            MatWin[i].write.unlock();
        }
    }
}






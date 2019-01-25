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
//#include "ImGuiFileDialog.h"
#include "imguifilesystem.h"

void GUI_FileDialogue(void);
std::string FilePath(void);

static int test_int = 0;

bool wide_area_range = 1;

float col[3]={0.0};
bool* p_open;
using namespace ImGui;
float test_color[4]={0.0};

cv::Mat frameRGB[W_MAT_NR], frameRGBA[W_MAT_NR];
sf::Image image[W_MAT_NR];
sf::Sprite sprite[W_MAT_NR];
sf::Texture texture[W_MAT_NR];

void GUI_FileDialogue(void);

enum BrowseAlias {BROWSE_SAVE, BROWSE_LOAD};
bool BrowseMode = 0;

struct mat_window_t MatWin[W_MAT_NR];
struct set_window_t SetWin[W_SET_NR];

static void ShowHelpMarker(const char* desc)
{
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



struct ExampleAppLog
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
        Filter.Draw("Filter", -100.0f);
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

static ExampleAppLog GUI_Log;

static bool browseButtonPressed;

void Draw_ImGui(void){
    //ImGui::StyleColorsLight();
// =============================== MENUBAR =============================
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
                if(ImGui::MenuItem(u8"������ �����", NULL, &fullscreen)){FullscreenChanged=1;}
                //ImGui::SliderFloat("Value", &f, 0.0f, 1.0f);
                //ImGui::InputFloat("Input", &f, 0.1f);
                //ImGui::Combo("Combo", &n, "Yes\0No\0Maybe\0\0");
            ImGui::EndMenu();
        }

        //if (ImGui::MenuItem(u8"�����", "Alt+F4")) {}
        ImGui::Separator();
        ImGui::Text("GUI %.1f FPS",ImGui::GetIO().Framerate);

        ImGui::Separator();
        ImGui::Text(u8"��������� %.1f FPS", info_fps);


        static time_t now;
        now = time(NULL);
        static char time_buff[64];
        strftime(time_buff, 64, "%H:%M:%S %d.%m.%Y ", gmtime(&now));

        ImGui::Separator();
        ImGui::Text("%s", time_buff);


        SameLine(GetWindowWidth()-140);
        ImGui::Text("ReSort v0.1 12.2018");

    ImGui::EndMainMenuBar();
    }
//==================== MAT WINDOWS ==========================================
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
                                    ImGui::MenuItem(u8"�������", "", &show_contours);
                                    ImGui::MenuItem(u8"������", "", &show_centers);
                                    ImGui::MenuItem(u8"�������", "", &show_bboxes);
                                    ImGui::MenuItem(u8"������� ����", "", &show_avgcolor);
                                    ImGui::MenuItem(u8"�������", "", &show_diameter);
                                    ImGui::MenuItem(u8"�������", "", &show_area);
                                    ImGui::MenuItem(u8"��������� ������� ������", "", &show_fill_avgcolor);
                                    ImGui::MenuItem(u8"��������� �����������", "", &show_fill_contcolor);
                                ImGui::EndMenu();
                            }
                        ImGui::EndMenuBar();
                        }
                    }
                    if (capture_file){ImGui::Image(texture[i], ImVec2(640,360));}
                    else {ImGui::Image(texture[i], ImVec2(cam_width,cam_height));}

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
                SaveConfig(chosenPath);
            }
    }

    if (BrowseMode == BROWSE_LOAD){
        const char* chosenPath = dlg.chooseFileDialog(browseButtonPressed);
            if (strlen(chosenPath)>0) {
                std::cout<<chosenPath<<std::endl;
                SetWin[W_SET_FILE].show = 0;
                ReadConfig(chosenPath);
            }
    }

    browseButtonPressed = 0;
    ImGui::End();
 }



//==================== SETTINGS WINDOWS ==========================================



    if (SetWin[W_SET_IN].show){
        ImGui::Begin(SetWin[W_SET_IN].title.c_str(), p_open, /*ImGuiWindowFlags_MenuBar|*/ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::Combo(u8"��������", &input_source, u8"������\0�����\0\0");
            ShowHelpMarker(u8"����� � ������ ��� �� ����� \"test_video.avi\"");
            ImGui::SliderInt(u8"�������� ������ n-��� ����", &show_mat_upd_target,  0, 127);
            ShowHelpMarker(u8"����� �������� ��������������");
            if (input_source == 0){
                ImGui::InputInt(u8"����� ������", &cam_number);
                const char* reso[] = { "320x240", "640x480", "800x600", "1024x768", "1280x720"};
                const long  resol[] = {320,240,640,480,800,600,1024,768,1280,720};
                static int reso_curr = 0;

                if(ImGui::Combo(u8"����������", &reso_curr, reso, IM_ARRAYSIZE(reso))){
                    cam_width  = resol[reso_curr*2];
                    cam_height = resol[reso_curr*2+1];
                    cam_open();
                }
                ShowHelpMarker(u8"��� ������, ��� �������");

                if  (!capture_run) {if (ImGui::Button(u8"������� �����")) {cam_open(); capture_run = 1;}}
                else                {if (ImGui::Button(u8"������� �����")) {capture_run = 0;}}
                if (ImGui::SliderFloat(u8"��������", &cam_fps,  5.0, 400.0,"%1.2f")) cam_update();
                ShowHelpMarker(u8"�������������� �� ��� ��������\n��������� �������� ������� �� ����������");
                if (ImGui::SliderInt(u8"��������", &cam_gain,  0, 127)) cam_update();
                ShowHelpMarker(u8"������������� ���������� �������� �� ��������� ����");
                if (ImGui::SliderInt(u8"��������", &cam_contrast,  0, 127)) cam_update();
                if (ImGui::SliderInt(u8"����������", &cam_exposure,  -5, 0)) cam_update();
                ShowHelpMarker(u8"������� ��������������� � ��������");
                if (ImGui::SliderInt(u8"������������", &cam_saturation,  0, 127)) cam_update();
                ImGui::Checkbox(u8"����-����", &freeze_frame);
            }
            if (input_source == 1){
                //ImGui::InputText(u8"��� �����", test_text, 64);
                //if (ImGui::Button(u8"������� ����")) {}
                //if (ImGui::Button(u8"�����")){}
                //ImGui::SameLine();
                //if (ImGui::Button(u8"�������")){}
                capture_file = input_source;
                if  (!capture_run) {if (ImGui::Button(u8"������� �����")) {capture_run = 1;}}
                else                {if (ImGui::Button(u8"������� �����")) {capture_run = 0;}}

            }
        ImGui::End();
    }

    if (SetWin[W_SET_BG].show){
        ImGui::Begin(SetWin[W_SET_BG].title.c_str(), p_open, /*ImGuiWindowFlags_MenuBar|*/ImGuiWindowFlags_AlwaysAutoResize);
            if(ImGui::Combo(u8"��������", &bs_current_algo, u8"MOG\0MOG2\0CNT\0KNN\0GSOC\0\0")){BS_Init(bs_current_algo);}
            ShowHelpMarker(u8"MOG � MOG2 �� ��������� ��������, �� �� ����� ����\nCNT �������� �������, �� ������� ��������������");
            ImGui::SliderInt(u8"��������������� ��������", &blur_before_mog,  1, 10);
            ShowHelpMarker(u8"�������� ���������� �� ������ � ����� ��������� ����");
            if (bs_current_algo==BS_MOG){
                ImGui::Checkbox(u8"��������", &bs_mog_learning);
                ImGui::SliderInt(u8"�������", &bs_mog_history,  1, 300);
                ImGui::SliderInt(u8"��������������", &bs_mog_mixtures,  1, 10);
                ImGui::SliderFloat(u8"Background ratio", &bs_mog_backratio, 0.01f, 0.99f, "%1.2f");
                ImGui::SliderFloat(u8"�������� ��������", &bs_mog_lrate, 0.01f, 0.99f, "%1.2f");
                ImGui::SliderFloat(u8"Noise sigma", &bs_mog_noisesigma, 0.0f, 1.0f, "%1.2f");

            }
            if (bs_current_algo==BS_MOG2){
                ImGui::Checkbox(u8"��������", &bs_mog2_learning);
                ImGui::SliderInt(u8"�������", &bs_mog2_history,  1, 300);
                ImGui::SliderFloat(u8"�����", &bs_mog2_thresh, 0.1f, 200.0f, "%1.1f");
                ImGui::SliderFloat(u8"�������� ��������", &bs_mog2_lrate, 0.01f, 0.99f, "%1.2f");
                ImGui::Checkbox(u8"������ ����", &bs_mog2_shadows);

            }
            if (bs_current_algo==BS_CNT){
                ImGui::Checkbox(u8"��������", &bs_cnt_learning);
                ShowHelpMarker(u8"������ ������ ���� ��������");
                ImGui::SliderInt(u8"���. ���������� ������", &bs_cnt_min_pix_stability,  1, 200);
                ShowHelpMarker(u8"����� ������� ������ ������� ��������� �������");
                ImGui::SliderInt(u8"����. ���������� ������", &bs_cnt_max_pix_stability,  1, 200);
                ShowHelpMarker(u8"������������ ������� ������� � �������");
                ImGui::SliderInt(u8"������/���", &bs_cnt_fps,  1, 100);
                ShowHelpMarker(u8"������� �������� ������");
                ImGui::Checkbox(u8"������������ �������", &bs_cnt_use_history);
                ImGui::Checkbox(u8"�������� �����������", &bs_cnt_isparallel);
                ShowHelpMarker(u8"������ ��������, ������ ����������");
                ImGui::SliderFloat(u8"�������� ��������", &bs_cnt_lrate, 0.01f, 0.99f, "%1.2f");
                ShowHelpMarker(u8"�������� ������ ���������");

            }
            if (bs_current_algo==BS_KNN){
                ImGui::Checkbox(u8"��������", &bs_knn_learning);
                ImGui::SliderInt(u8"�������", &bs_knn_history,  1, 300);
                ImGui::SliderFloat(u8"�����", &bs_knn_thresh, 0.0f, 1.0f, "%1.2f");
                ImGui::SliderFloat(u8"�������� ��������", &bs_knn_lrate, 0.01f, 0.99f, "%1.2f");
                ImGui::Checkbox(u8"������ ����", &bs_knn_shadows);

            }

            if (bs_current_algo==BS_GSOC){
                ImGui::Checkbox(u8"��������", &bs_gsoc_learning);
                ImGui::SliderFloat(u8"�������� ��������", &bs_gsoc_lrate, 0.01f, 0.99f, "%1.2f");
                ImGui::SliderInt(u8"����������� �������� ������", &bs_gsoc_mc,  0, 1);
                ImGui::SliderInt(u8"��������", &bs_gsoc_samples,  1, 200);
                ImGui::SliderFloat(u8"Propagation rate", &bs_gsoc_proprate, 0.0f, 1.0f, "%1.3f");
                ImGui::SliderFloat(u8"Replace rate", &bs_gsoc_reprate, 0.0f, 1.0f, "%1.3f");
                ImGui::SliderInt(u8"Hits threshold", &bs_gsoc_hits_thresh,  1, 200);
                ImGui::SliderFloat(u8"alpha", &bs_gsoc_alpha, 0.0f, 1.0f, "%1.3f");
                ImGui::SliderFloat(u8"beta", &bs_gsoc_beta, 0.0f, 1.0f, "%1.3f");
                ImGui::SliderFloat(u8"blinkingSupressionDecay ", &bs_gsoc_bs_decay, 0.0f, 1.0f, "%1.3f");
                ImGui::SliderFloat(u8"blinkingSupressionMultiplier ", &bs_gsoc_bs_mul, 0.0f, 1.0f, "%1.3f");
                ImGui::SliderFloat(u8"noiseRemovalThresholdFacBG ", &bs_gsoc_noise_bg, 0.0f, 1.0f, "%1.3f");
                ImGui::SliderFloat(u8"noiseRemovalThresholdFacFG ", &bs_gsoc_noise_fg, 0.0f, 1.0f, "%1.3f");



            }

             if (ImGui::Button(u8"��������� ���������")) {BS_Init(bs_current_algo);}
            ShowHelpMarker(u8"��������� ��������� �� ����������� �������������");

        ImGui::End();
    }

    if (SetWin[W_SET_CONTOUR].show){
        ImGui::Begin(SetWin[W_SET_CONTOUR].title.c_str(), p_open, /*ImGuiWindowFlags_MenuBar|*/ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::Combo(u8"��������", &contour_current_algo, u8"Canny\0Scharr\0\0");
            if (contour_current_algo==0){
                use_scharr=0;
                ImGui::SliderInt(u8"��������", &canny_blur_value,  1, 5);
                ShowHelpMarker(u8"��������������� �������� �������� ���������� �� ������\n�������� �������� �������� �����");
                ImGui::SliderInt(u8"����� 1", &canny_thresh_value1, 1, 500);
                ImGui::SliderInt(u8"����� 2", &canny_thresh_value2, 1, 500);
                ShowHelpMarker(u8"�� ��������� ����� 2 �������� � ��� ���� ������ ������ 1");
            }

            if (contour_current_algo==1){
                ImGui::Text(u8"�������� ���� ��������� �����!");
                use_scharr=1;
                //ImGui::SliderInt(u8"��������", &test_int,  1, 2);
                //ImGui::InputFloat(u8"�����", &f, 1,2, .1f);
            }
        ImGui::End();
    }

    if (SetWin[W_SET_MASK].show){
        ImGui::Begin(SetWin[W_SET_MASK].title.c_str(), p_open, /*ImGuiWindowFlags_MenuBar|*/ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::SliderFloat(u8"����������� ������� �����", &min_bbox_area,  0.0f, 100.0f+wide_area_range*10000.0f, "%.0f");
            ImGui::SliderFloat(u8"������������ ������� �����", &max_bbox_area,  0.0f, 10000.0f+wide_area_range*20000.0f, "%.0f");
            ShowHelpMarker(u8"������� ������, ���������� ���������");
            ImGui::Checkbox(u8"����������� ������� �������", &wide_area_range);
            ImGui::Combo(u8"��� ����� ��������������", &morpho_type, u8"RECT\0ELLIPSE\0CROSS\0\0");
            ShowHelpMarker(u8"��� ��������� �������� ���������������� ��������������\nRECT ����� �������, ELLIPSE ������� ���������, �� ����� ������������");
            ImGui::SliderInt(u8"������ �����", &morph_size,  1, 10);
            ShowHelpMarker(u8"������ ��������� �������� ���������������� ��������������\n��������� �������� ����� �����, ������� /""���������/"" ����� ������");
        ImGui::End();
    }

    if (SetWin[W_SET_COLOR].show){
        ImGui::Begin(SetWin[W_SET_COLOR].title.c_str(), p_open, /*ImGuiWindowFlags_MenuBar|*/ImGuiWindowFlags_AlwaysAutoResize);
            static float tolh = (float)color_tolerance_hsv.val[0];
            static float tols = (float)color_tolerance_hsv.val[1];
            static float tolv = (float)color_tolerance_hsv.val[2];

            static float tolr = (float)color_tolerance_rgb.val[2];
            static float tolg = (float)color_tolerance_rgb.val[1];
            static float tolb = (float)color_tolerance_rgb.val[0];

            if(ImGui::ColorPicker3(u8"���������� ����", (float*)&test_color)){
                float hue, saturation, value;
                ImGui::ColorConvertRGBtoHSV(test_color[0], test_color[1], test_color[2], hue, saturation, value);
                good_color_hsv.val[0] = hue*180.0;
                good_color_hsv.val[1] = saturation*255.0;
                good_color_hsv.val[2] = value*255.0;
                good_color_rgb.val[2] = test_color[0]*255.0;
                good_color_rgb.val[1] = test_color[1]*255.0;
                good_color_rgb.val[0] = test_color[2]*255.0;
            }

            ImGui::Checkbox(u8"�������� ������������ �������", &good_colorspace);
            ShowHelpMarker(u8"������������ HSV ��� RGB ������");
            if(good_colorspace){
                if (ImGui::SliderFloat(u8"������ H", &tolh, 0.0f, 180.0f, "%3.1f"))   color_tolerance_hsv.val[0] = (double)tolh;
                if (ImGui::SliderFloat(u8"������ S", &tols, 0.0f, 255.0f, "%3.1f"))   color_tolerance_hsv.val[1] = (double)tols;
                if (ImGui::SliderFloat(u8"������ V", &tolv, 0.0f, 255.0f, "%3.1f"))   color_tolerance_hsv.val[2] = (double)tolv;
            }
            else {
                if (ImGui::SliderFloat(u8"������ R", &tolr, 0.0f, 180.0f, "%3.1f"))   color_tolerance_rgb.val[2] = (double)tolr;
                if (ImGui::SliderFloat(u8"������ G", &tolg, 0.0f, 255.0f, "%3.1f"))   color_tolerance_rgb.val[1] = (double)tolg;
                if (ImGui::SliderFloat(u8"������ B", &tolb, 0.0f, 255.0f, "%3.1f"))   color_tolerance_rgb.val[0] = (double)tolb;
            }
        ImGui::End();
    }

    if (SetWin[W_SET_OUT].show){
        ImGui::Begin(SetWin[W_SET_OUT].title.c_str(), p_open, /*ImGuiWindowFlags_MenuBar|*/ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::Checkbox(u8"�������", &show_contours);
            ImGui::Checkbox(u8"������", &show_centers);
            ImGui::Checkbox(u8"�������", &show_bboxes);
            ImGui::Checkbox(u8"�������", &show_area);
            ImGui::Checkbox(u8"�������", &show_diameter);
            ImGui::Checkbox(u8"������ ������� ������", &show_fill_avgcolor);
            ImGui::Checkbox(u8"������ �����������", &show_fill_contcolor);
        ImGui::End();
    }

    if (SetWin[W_SET_COM].show){
        ImGui::Begin(SetWin[W_SET_COM].title.c_str(), p_open, /*ImGuiWindowFlags_MenuBar|*/ImGuiWindowFlags_AlwaysAutoResize);
            static int com_selected = 0;
            //static int histo_selected = 0;
            if (ImGui::Button(u8"����������� �����")) {list_COM();}
            ShowHelpMarker(u8"������� ������ ������ 16 ��������� ������");
            ImGui::Text(u8"��������� �����:");
            for (int i = 0; i < 16; i++){
                if (COM_present[i] == 1){
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
            com_speed = com_speeds[speed_current];
            if(!com_connected){if (ImGui::Button(u8"������������")){open_COM(com_selected);}}
            else                {if (ImGui::Button(u8"�����������")){close_COM(com_selected);}}

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
            //static float arr[] = { 0.6f, 0.1f, 1.0f, 0.5f, 0.92f, 0.1f, 0.2f };
             //   ImGui::PlotLines("Frame Times", arr, IM_ARRAYSIZE(arr));

            static float fps_values[100] = {0};
            static float contours_values[100] = {0};
            static float good_contours_values[100] = {0};

                static int values_offset = 0;
                static double refresh_time = 0.0;
                if (refresh_time == 0.0f) refresh_time = ImGui::GetTime();
                while (refresh_time < ImGui::GetTime()) // Create dummy data at fixed 60 hz rate for the demo
                {
                    fps_values[values_offset] = info_fps;
                    contours_values[values_offset] = (float)info_total_contours;
                    good_contours_values[values_offset] = (float)good_contours;

                    values_offset = (values_offset+1) % IM_ARRAYSIZE(fps_values);
                    refresh_time += 1.0f/5.0f;
                }
            ImGui::Spacing();
                char fps_buf[8];
                sprintf(fps_buf,"%2.1f",  info_fps);
                ImGui::PlotLines("FPS", fps_values, IM_ARRAYSIZE(fps_values), values_offset, fps_buf, 0.0f, *std::max_element(std::begin(fps_values), std::end(fps_values)), ImVec2(0,80));
            ImGui::Spacing();
                char cont_buf[8];
                sprintf(cont_buf,"%lu",  info_total_contours);
                ImGui::PlotLines(u8"��������", contours_values, IM_ARRAYSIZE(contours_values), values_offset, cont_buf, 0.0f, *std::max_element(std::begin(contours_values), std::end(contours_values)), ImVec2(0,80));
            ImGui::Spacing();
                char useful_cont_buf[8];
                sprintf(useful_cont_buf,"%lu",good_contours);
                ImGui::PlotLines(u8"�������� ��������", good_contours_values, IM_ARRAYSIZE(good_contours_values), values_offset, useful_cont_buf, 0.0f, 70.0f, ImVec2(0,80));
            ImGui::Spacing();
            ImGui::SliderInt(u8"��������� FPS ���������", &fps_average,  1, 127);
            //ImGui::Checkbox(u8"����-����", &freeze_frame);

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
                ImGui::SliderFloat2("ButtonTextAlign", (float*)&style.ButtonTextAlign, 0.0f, 1.0f, "%.2f"); ImGui::SameLine(); ShowHelpMarker("Alignment applies when a button is larger than its text content.");
                ImGui::Text("Safe Area Padding"); ImGui::SameLine(); ShowHelpMarker("Adjust if you cannot see the edges of your screen (e.g. on a TV where scaling has not been configured).");
                ImGui::SliderFloat2("DisplaySafeAreaPadding", (float*)&style.DisplaySafeAreaPadding, 0.0f, 30.0f, "%.0f");
            ImGui::TreePop();
        }
        ImGui::End();


    }

    if (SetWin[W_SELF_LOG].show){
            GUI_Log.Draw(u8"�������", p_open);
    }

}



void GUI_VarInit(void){

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

    MatWin[W_MAT_IN].mat = &img_in;
    MatWin[W_MAT_CONTOUR].mat = &img_canny_output;
    MatWin[W_MAT_MASK].mat = &img_wholemask;
    MatWin[W_MAT_BG].mat = &img_bs_back;
    MatWin[W_MAT_MOG].mat = &img_mog_output;
    MatWin[W_MAT_MORPH].mat = &img_morph_out;
    MatWin[W_MAT_OUT].mat = &img_output;
}


void GUI_Fill_Textures(void){
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






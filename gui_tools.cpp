#include <iostream>
#include <fstream>
#include <string>

#include "imgui.h"
#include "imgui-SFML.h"

#include "main.h"
#include "file.h"
#include "img.h"
#include "newGUI.h"
#include "gui_tools.h"

#include "gui/imguifilesystem.h"

bool* fileDlgOpen;
static bool browseButtonPressed;
static bool openFileDialog;

int BrowseMode = 0;

void GUI_class::openFileBrowser(int type){
    BrowseMode = type;
    browseButtonPressed = 1;
    openFileDialog = 1;
    std::cout << "openFileBrowser " << type << std::endl;
};

ImVec2 fileDlgSize = ImVec2(500, 400);

void GUI_class::drawFileBrowser(void){
    if (openFileDialog){
        static ImGuiFs::Dialog dlg;
        ImVec2 fileDlgPos = ImVec2((GUI.ScreenW - fileDlgSize.x)/2,(GUI.ScreenH - fileDlgSize.y)/2);
          switch (BrowseMode){
                case BROWSE_SAVE_CFG: {
                    const char* chosenPath = dlg.saveFileDialog(browseButtonPressed, NULL, NULL, ".ini", u8"��������� ������������", fileDlgSize, fileDlgPos, 0.875f);
                    if (strlen(chosenPath) > 0){
                        File.SaveConfig(chosenPath);
                        openFileDialog = 0;
                    }
                    break;}

                case BROWSE_LOAD_CFG: {

                    const char* chosenPath = dlg.chooseFileDialog(browseButtonPressed, NULL, ".ini", u8"��������� ������������", fileDlgSize, fileDlgPos, 0.875f);
                    if (strlen(chosenPath) > 0){
                        File.ReadConfig(chosenPath);
                        openFileDialog = 0;

                    }
                    break;}

                case BROWSE_LOAD_VIDEO: {
                    const char* chosenPath = dlg.chooseFileDialog(browseButtonPressed, NULL, ".avi", u8"��������� �����", fileDlgSize, fileDlgPos, 0.875f);
                    if (strlen(chosenPath) > 0){
                        Img.videoFileName = chosenPath;
                        openFileDialog = 0;

                    }
                    break;}

                default: {openFileDialog = 0; break;}
            }
            browseButtonPressed = 0;
     }
};


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
        if (ImGui::Button(u8"��������")) Clear();
        ImGui::SameLine();
        bool copy = ImGui::Button(u8"����������");
        ImGui::SameLine();
        Filter.Draw(u8"������", -100.0f);
        //ImGui::Checkbox(u8"����� ���� (����� � cout)", &V.Info.ConsoleDestination);

        ImGui::SameLine();
        if (ImGui::Button(u8"-Rx/Tx")) sprintf(Filter.InputBuf, "-Rx, -Tx");
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
        //ImGui::End();
    }
};



static LogWidget_t GUI_Log;

bool* consoleOpen;
void drawConsole(void){
     GUI_Log.Draw(u8"�������", consoleOpen);
}

//void GUI_class::ConsoleOut (std::string InString){
void GUI_class::ConsoleOut (const char *fmt, ...){
    time_t now;
    char the_date[32];
    the_date[0] = '\0';
    now = time(NULL);
    strftime(the_date, 32, "%H:%M:%S", gmtime(&now));

    char buffer[512];
    va_list args;
    va_start(args, fmt);
    int rc = vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    std::string tmp(buffer);
    GUI_Log.AddLog("%s: %s\n", the_date, tmp.c_str());
   // }
    //else{
   //     cout << the_date << " " << InString <<endl;
  //  }
}

void GUI_class::ConsoleOut (std::string InString){
    time_t now;
    char the_date[32];
    the_date[0] = '\0';
    now = time(NULL);
    strftime(the_date, 32, "%H:%M:%S", gmtime(&now));


    GUI_Log.AddLog("%s: %s\n", the_date, InString.c_str());
}




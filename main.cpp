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
#include "main.h"
#include "file.h"
#include "img.h"
#include "rs232.h"
#include "COM_port.h"
#include "mog.h"
#include "real_dimensions.h"
#include "newGUI.h"
#include "gui/ImGuiFileDialog.h"
#include "belt_processor.h"
#include "omp.h"
#include "calibration.h"

#define FPS_AVERAGE 50

using namespace cv;
using namespace std;

GUI_t GUI;

bool        FullscreenChanged = 1;
sf::Window  window;
V_t         V;

std::string WindowName = "ReSort v0.25";

int main(int argc, const char *argv[])
{
    File.ReadConfig("settings.ini");
    File.SaveConfig("settings.ini");
    Calib.readData();

    GUI.Init();
    GUI.VarInit();

    COM.List();
    COM.startThread();


    Img.stopCapture();

    Img.BS_Init(BS_MOG);

    Img.RunProcessor();
    GUI.Worker();
}

void exitApplication(void){
    Img.procRun = 0;
    Img.stopCapture();
    COM.tryGoodbye();
    ImGui::SFML::Shutdown();
}














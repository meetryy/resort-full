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

#define FPS_AVERAGE 50

using namespace cv;
using namespace std;

GUI_class GUI;

void    comport_thread(void);
void    image_thread(void);
void    ProcessImg(void);

bool    FullscreenChanged =0;
sf::Window window;

V_t V;

std::string WindowName = "ReSort v0.1";

int main(int argc, const char *argv[])
{
    GUI.Init();
    GUI.VarInit();

    File.ReadConfig("settings.ini");
    File.SaveConfig("settings.ini");

    COM.List();
    std::thread t1(comport_thread);
    t1.detach();

    V.Input.CaptureRun = 0;

    Img.BS_Init(BS_MOG);

    omp_set_num_threads(32);

    //thread ImgProcessor_t(ImgProcessor);
    //ImgProcessor_t.detach();


    Img.RunProcessor();

    GUI.Worker();



}

void comport_thread(void){
    std::cout << "com thread started" << std::endl;
    while(1){
        //std::cout << "task1 says:";
        //cv::waitKey(100);
        if (V.ComPort.Connected) {COM.listen();}
    }
}














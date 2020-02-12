#include <iostream>
#include <fstream>
#include <stdexcept>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <SFML/Graphics/Texture.hpp>
extern cv::Mat img;
//extern bool proce;

extern bool exit_program;
extern std::vector<double> points;
void GUI_FullscreenSwitch(void);

void exitApplication(void);
extern bool FullscreenChanged;

enum results{RESULT_OK, RESULT_ERROR};

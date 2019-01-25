#define WINDOW1_NAME	"Control Panel"
#define ERROR_WINDOW_NAME 	"Error!"


extern bool small_GUI;
extern int cfg_file_number;

void UI_init();
void UI_routine(void);
void UI_routine_new(void);
void CallBackFunc(int event, int x, int y, int flags, void* userdata);
int isWindowOpen(const cv::String &name);
void openWindow(const cv::String &name);
void closeWindow(const cv::String &name);
void ShowError(std::string message);



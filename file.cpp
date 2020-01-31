#include <iostream>
#include <fstream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "variables1.h"
#include "file.h"
#include "img.h"
#include "newGUI.h"

using namespace cv;
using namespace std;

File_class File;

int nthOccurrence(const std::string& str, const std::string& findMe, int nth)
{
    size_t  pos = 0;
    int     cnt = 0;

    while( cnt != nth )
    {
        pos+=1;
        pos = str.find(findMe, pos);
        if ( pos == std::string::npos )
            return -1;
        cnt++;
    }
    return pos;
}

/*

template <typename T>
void ProcessLine(string input, T*value, string paramname){
    if (std::is_same<T, float>::value) {ProcessLineFloat(input, &value, paramname);}
    else if (std::is_same<T, int>::value) {ProcessLineInt(input, &value, paramname);}
    else if (std::is_same<T, bool>::value) {ProcessLineBool(input, &value, paramname);}
    else if (std::is_same<T, double>::value)  {ProcessLineDouble(input, &value, paramname);}
    else if (std::is_same<T, long>::value)  {ProcessLineLong(input, &value, paramname);}
    else if (std::is_same<T, cv::Scalar>::value) {ProcessLineScalar(input, &value, paramname);}
}
*/

void File_class::ReadConfig(string filename){
    ifstream in;            // Create an input file stream.
    in.open(filename);
    if (!in) GUI.ConsoleOut(u8"ОШИБКА: Указанный файл конфигурации не найден!");

    else
    {
        GUI.ConsoleOut(u8"ФАЙЛ: Файл конфигурации найден");
        string str;
        getline(in, str);  // Get the frist line from the file, if any.
        while (in) {  // Continue if the line was sucessfully read.
            if (str.find("#") == string::npos){
                //input
                //ProcessLineBool(str, &V.Input.CaptureRun, "V.Input.CaptureRun");
                //ProcessLineBool(str, &input_correction_on, "input_correction_on");
                //ProcessLineScalar(str, &hsv_input_correction, "hsv_input_correction");

                //ProcessLineDouble(str, &cut_up, "cut_up");
                //ProcessLineDouble(str, &cut_down, "cut_down");
                //ProcessLineDouble(str, &cut_left, "cut_left");
                //ProcessLineDouble(str, &cut_right, "cut_right");
                ProcessLine(str, &V.Cam.FPS, "V.Cam.FPS");

                //edge detection
                ProcessLine(str, &V.Edge.BlurValue, "V.Edge.BlurValue");
                ProcessLine(str, &V.Edge.CannyThresh1, "V.Edge.CannyThresh1");
                ProcessLine(str, &V.Edge.CannyThresh2, "V.Edge.CannyThresh2");
                ProcessLine(str, &V.Edge.UseScharr, "V.Edge.UseScharr");
                ProcessLine(str, &V.Edge.ScharrThresh, "V.Edge.ScharrThresh");
                ProcessLine(str, &V.Contours.CurrentAlgo, "V.Contours.CurrentAlgo");

                //contours
                ProcessLine(str, &V.Contours.MinBBoxArea, "V.Contours.MinBBoxArea");
                ProcessLine(str, &V.Contours.MaxBBoxArea, "V.Contours.MaxBBoxArea");
                ProcessLine(str, &V.Contours.morph_mask, "V.Contours.morph_mask");
                ProcessLine(str, &V.Morph.Size, "V.Morph.Size");


                //color
                ProcessLine(str, &V.Color.GoodRGB, "V.Color.GoodRGB");
                ProcessLine(str, &V.Color.ToleranceRGB, "V.Color.ToleranceRGB");
                ProcessLine(str, &V.Color.GoodHSV, "V.Color.GoodHSV");
                ProcessLine(str, &V.Color.ToleranceHSV, "V.Color.ToleranceHSV");
                ProcessLine(str, &V.Color.GoodSpace, "V.Color.GoodSpace");

                //output
                ProcessLine(str, &V.Show.Contours, "V.Show.Contours");
                ProcessLine(str, &V.Show.Centers, "V.Show.Centers");
                ProcessLine(str, &V.Show.BBoxes, "V.Show.BBoxes");
                ProcessLine(str, &V.Show.AvgColor, "V.Show.AvgColor");
                ProcessLine(str, &V.Show.Area, "V.Show.Area");
                ProcessLine(str, &V.Show.FillAvg, "V.Show.FillAvg");
                ProcessLine(str, &V.Show.FilContour, "V.Show.FilContour");

                //com port
                ProcessLine(str, &V.ComPort.Connected, "V.ComPort.Connected");
                ProcessLine(str, &V.ComPort.Number, "V.ComPort.Number");
                ProcessLine(str, &V.ComPort.Speed, "V.ComPort.Speed");

                //mog
                ProcessLine(str, &V.BS.MOG.History, "V.BS.MOG.History");
                ProcessLine(str, &V.BS.MOG.Mixtures, "V.BS.MOG.Mixtures");
                ProcessLine(str, &V.BS.MOG.BackRatio, "V.BS.MOG.BackRatio");
                ProcessLine(str, &V.BS.MOG.NoiseSigma, "V.BS.MOG.NoiseSigma");
                ProcessLine(str, &V.BS.MOG.LRate, "V.BS.MOG.LRate");
                ProcessLine(str, &V.BS.MOG.Learning, "V.BS.MOG.Learning");

                //mog2
                ProcessLine(str, &V.BS.MOG2.History, "V.BS.MOG2.History");
                ProcessLine(str, &V.BS.MOG2.Thresh, "V.BS.MOG2.Thresh");
                ProcessLine(str, &V.BS.MOG2.DetectShadows, "V.BS.MOG2.DetectShadows");
                ProcessLine(str, &V.BS.MOG2.Learning, "V.BS.MOG2.Learning");

                //knn
                //ProcessLineInt(str, &Img.bs_knn_history, "Img.bs_knn_history");
                //ProcessLineFloat(str, &Img.bs_knn_thresh, "Img.bs_knn_thresh");
                //ProcessLineBool(str, &Img.bs_knn_shadows, "Img.bs_knn_shadows");
                //ProcessLineBool(str, &Img.bs_knn_learning, "Img.bs_knn_learning");

                //cnt
                ProcessLine(str, &V.BS.CNT.MinPixStability, "V.BS.CNT.MinPixStability");
                ProcessLine(str, &V.BS.CNT.MaxPixStability, "V.BS.CNT.MaxPixStability");
                ProcessLine(str, &V.BS.CNT.UseHistory, "V.BS.CNT.UseHistory");
                ProcessLine(str, &V.BS.CNT.IsParallel, "V.BS.CNT.IsParallel");
                ProcessLine(str, &V.BS.CNT.FPS, "V.BS.CNT.FPS");

                //gsoc
                //ProcessLineInt(str, &Img.bs_gsoc_mc, "Img.bs_gsoc_mc");
                //ProcessLineInt(str, &Img.bs_gsoc_samples, "Img.bs_gsoc_samples");
                //ProcessLineFloat(str, &Img.bs_gsoc_reprate, "Img.bs_gsoc_reprate");
                //ProcessLineFloat(str, &Img.bs_gsoc_proprate, "Img.bs_gsoc_proprate");
                //ProcessLineInt(str, &Img.bs_gsoc_hits_thresh, "Img.bs_gsoc_hits_thresh");
                //ProcessLineFloat(str, &Img.bs_gsoc_alpha, "Img.bs_gsoc_alpha");
                //ProcessLineFloat(str, &Img.bs_gsoc_beta, "Img.bs_gsoc_beta");
                //ProcessLineFloat(str, &Img.bs_gsoc_Img.bs_decay, "Img.bs_gsoc_Img.bs_decay");
                //ProcessLineFloat(str, &Img.bs_gsoc_Img.bs_mul, "Img.bs_gsoc_Img.bs_mul");
                //ProcessLineFloat(str, &Img.bs_gsoc_noise_bg, "Img.bs_gsoc_noise_bg");
                //ProcessLineFloat(str, &Img.bs_gsoc_noise_fg, "Img.bs_gsoc_noise_fg");
                //ProcessLineBool(str, &Img.bs_gsoc_learning, "Img.bs_gsoc_learning");

                //com camera
                ProcessLine(str, &V.Cam.Number, "V.Cam.Number");
                ProcessLine(str, &V.Cam.Width, "V.Cam.Width");
                ProcessLine(str, &V.Cam.Height, "V.Cam.Height");
                ProcessLine(str, &V.Cam.Gain, "V.Cam.Gain");
                ProcessLine(str, &V.Cam.Contrast, "V.Cam.Contrast");
                ProcessLine(str, &V.Cam.Brightness, "V.Cam.Brightness");
                ProcessLine(str, &V.Cam.Exposure, "V.Cam.Exposure");
                ProcessLine(str, &V.Cam.Saturation, "V.Cam.Saturation");
                ProcessLine(str, &V.Cam.Hue, "V.Cam.Hue");

                //misc
                ProcessLine(str, &pix_per_100mm, "pix_per_100mm");
                ProcessLine(str, &V.Morph.Type, "V.Morph.Type");
                ProcessLine(str, &V.Contours.CurrentAlgo, "V.Contours.CurrentAlgo");
                ProcessLine(str, &V.BS.CurrentAlgo, "V.BS.CurrentAlgo");
                ProcessLine(str, &V.BS.BlurBeforeMog, "V.BS.BlurBeforeMog");
                ProcessLine(str, &V.UI.Fullscreen, "V.UI.Fullscreen");

            }
            getline(in, str);   // Try to get another line.
        }
        GUI.ConsoleOut(u8"ФАЙЛ: Загрузка конфигурации завершена");
    }
}

int File_class::ProcessLine(string str, long *parameter, string paramname){
    int to_return = RESULT_FAIL;
    int position = str.find(paramname);         //find a variable name
    if (position != string::npos) {             //if exist
        string value = str.substr(str.find("=")+1, str.find(";"));  //save value as string
        std::string::size_type sz;     // alias of size_t
        *parameter = std::stoi (value,&sz);  //string to int
        cout << "reading long " << paramname << " = " << *parameter << " OK!"<< endl;
        to_return = RESULT_OK;
        //cout << paramname << " found in line" << endl;
        //GUI.ConsoleOut("%s = %lu", paramname, parameter);
    }
    return to_return;
}

int File_class::ProcessLine(string str, float *parameter, string paramname){
    int to_return = RESULT_FAIL;
    int position = str.find(paramname);         //find a variable name
    if (position != string::npos) {             //if exist
        string value = str.substr(str.find("=")+1, str.find(";"));  //save value as string
        std::string::size_type sz;     // alias of size_t
        *parameter = std::stof (value,&sz);  //string to int
        cout << "reading float " << paramname << " = " << *parameter << " OK!"<< endl;
        to_return = RESULT_OK;
        //cout << paramname << " found in line" << endl;
    }
    //else {cout << paramname << " NOT found in line" << endl;}
    return to_return;
}

int File_class::ProcessLine(string str, double *parameter, string paramname){
    int to_return = RESULT_FAIL;
    int position = str.find(paramname);         //find a variable name
    if (position != string::npos) {             //if exist
        string value = str.substr(str.find("=")+1, str.find(";"));  //save value as string
        std::string::size_type sz;     // alias of size_t
        *parameter = std::stof (value,&sz);  //string to int
        cout << "reading double " << paramname << " = " << *parameter << " OK!"<< endl;
        to_return = RESULT_OK;
        //cout << paramname << " found in line" << endl;
    }
    //else {cout << paramname << " NOT found in line" << endl;}
    return to_return;
}

int File_class::ProcessLine(string str, int *parameter, string paramname){
    int to_return = RESULT_FAIL;
    int position = str.find(paramname);         //find a variable name
    if (position != string::npos) {             //if exist
        string value = str.substr(str.find("=")+1, str.find(";"));  //save value as string
        std::string::size_type sz;     // alias of size_t
        *parameter = std::stoi (value,&sz);  //string to int
        cout << "reading int " << paramname << " = " << *parameter << " OK!"<< endl;
        to_return = RESULT_OK;
    }
    return to_return;
}

int File_class::ProcessLine(string str, bool *parameter, string paramname){
    int to_return = RESULT_FAIL;
    int position = str.find(paramname);         //find a variable name
    if (position != string::npos) {             //if exist
        string value = str.substr(str.find("=")+1, str.find(";"));  //save value as string
        //cout << "bool value = " << value << endl;
        std::string::size_type sz;     // alias of size_t
        *parameter = std::stoi (value,nullptr,2);;  //string to int
        cout << "reading bool " << paramname << " = " << *parameter << " OK!"<< endl;
        //cout << paramname << " FOUND in line" << endl;
        to_return = RESULT_OK;
    }
    //else{cout << paramname << " NOT found in line" << endl;}
    return to_return;
}

int File_class::ProcessLine(string str, Scalar *parameter, string paramname){
    int to_return = RESULT_FAIL;
    //hsv_input_correction=(50.0,100.0,150.0);
    int position = str.find(paramname);         //find a variable name
    if (position != string::npos) {             //if exist
        string value = str.substr(str.find("=")+1, str.find(";"));  //save value as string

        int comma1 = nthOccurrence(value, ",", 1);
        int comma2 = nthOccurrence(value, ",", 2);
        int comma3 = nthOccurrence(value, ",", 3);

        int from = value.find("[")+1;
        int len =  comma1 - from;
        string subvalue1 = value.substr(from, len);  //save value as string
        std::string::size_type sz;     // alias of size_t
        double val0 = std::stof (subvalue1,&sz);  //string to int

        from = comma1 +1;
        len = comma2 - from;
        string subvalue2 = value.substr(from, len);  //save value as string
        double val1 = std::stof (subvalue2,&sz);  //string to int

        from = comma2 +1;
        len = comma3 - from;
        string subvalue3 = value.substr(from, len);  //save value as string
        double val2 = std::stof (subvalue3,&sz);  //string to in

        //string subvalue4 = value.substr(value.find_last_of(",")+1, value.find(",0]")-2);  //save value as string
        //cout << subvalue3 << endl;
        double val3 = 0;//std::stof (subvalue3,&sz);  //string to int

        *parameter = Scalar(val0,val1,val2,val3);
        cout << "reading scalar " << paramname << " = " << *parameter << " OK!"<< endl;
        to_return = RESULT_OK;


    }
    //else {cout << paramname << " NOT found in line" << endl;}
    return to_return;
}


void File_class::SaveConfig(string filename){
    ofstream file;
    file.open(filename,  fstream::out);
    GUI.ConsoleOut(u8"ФАЙЛ: Начинаем запись конфигурации");

    file << "# input\n";
    file << "V.Input.CaptureRun=" << V.Input.CaptureRun <<";\n";

    file << "Img.input_correction_on=" << Img.input_correction_on <<";\n";
    file << "Img.hsv_input_correction=" << Img.hsv_input_correction <<";\n";
    //file << "cut_up=" << cut_up <<";\n";
    //file << "cut_down=" << cut_down <<";\n";
    //file << "cut_left=" << cut_left <<";\n";
    //file << "cut_right=" << cut_right <<";\n";

    file << "\n";
    file << "# edge detection\n";
    file << "V.Edge.BlurValue=" << V.Edge.BlurValue <<";\n";
    file << "V.Edge.CannyThresh1=" << V.Edge.CannyThresh1 <<";\n";
    file << "V.Edge.CannyThresh2=" << V.Edge.CannyThresh2 <<";\n";
    file << "V.Contours.CurrentAlgo=" << V.Contours.CurrentAlgo <<";\n";
    file << "V.Edge.UseScharr=" << V.Edge.UseScharr <<";\n";
    file << "V.Edge.ScharrThresh=" << V.Edge.ScharrThresh <<";\n";

    file << "\n";
    file << "# contours\n";
    file << "V.Contours.MinBBoxArea=" << V.Contours.MinBBoxArea <<";\n";
    file << "V.Contours.MaxBBoxArea=" << V.Contours.MaxBBoxArea <<";\n";
    file << "V.Contours.morph_mask=" << V.Contours.morph_mask <<";\n";
    file << "Img.input_correction_on=" << Img.input_correction_on <<";\n";
    file << "V.Morph.Size=" << V.Morph.Size <<";\n";

    file << "\n";
    file << "# color\n";
    file << "V.Color.GoodRGB=" << V.Color.GoodRGB <<";\n";
    file << "V.Color.ToleranceRGB=" << V.Color.ToleranceRGB <<";\n";
    file << "V.Color.GoodHSV=" << V.Color.GoodHSV <<";\n";
    file << "V.Color.ToleranceHSV=" << V.Color.ToleranceHSV <<";\n";
    file << "V.Color.GoodSpace=" << V.Color.GoodSpace <<";\n";

    file << "\n";
    file << "# output\n";
    file << "V.Show.Contours=" << V.Show.Contours <<";\n";
    file << "V.Show.Centers=" << V.Show.Centers <<";\n";
    file << "V.Show.BBoxes=" << V.Show.BBoxes <<";\n";
    file << "V.Show.AvgColor=" << V.Show.AvgColor <<";\n";
    file << "V.Show.Area=" << V.Show.Area <<";\n";
    file << "V.Show.BBoxes=" << V.Show.BBoxes <<";\n";
    file << "V.Show.FillAvg=" << V.Show.FillAvg <<";\n";
    file << "V.Show.FilContour=" << V.Show.FilContour <<";\n";

    file << "\n";
    file << "# com port\n";
    file << "V.ComPort.Connected=" << V.ComPort.Connected <<";\n";
    file << "V.ComPort.Number=" << V.ComPort.Number <<";\n";
    file << "V.ComPort.Speed=" << V.ComPort.Speed <<";\n";

    file << "\n";
    file << "# mog\n";
    file << "V.BS.CurrentAlgo=" << V.BS.CurrentAlgo <<";\n";
    file << "V.BS.BlurBeforeMog=" << V.BS.BlurBeforeMog <<";\n";

    file << "V.BS.MOG.BackRatio=" << V.BS.MOG.BackRatio <<";\n";
    file << "V.BS.MOG.History=" << V.BS.MOG.History <<";\n";
    file << "V.BS.MOG.Learning=" << V.BS.MOG.Learning <<";\n";
    file << "V.BS.MOG.LRate=" << V.BS.MOG.LRate <<";\n";
    file << "V.BS.MOG.Mixtures=" << V.BS.MOG.Mixtures <<";\n";
    file << "V.BS.MOG.NoiseSigma=" << V.BS.MOG.NoiseSigma <<";\n";


    file << "\n";
    file << "# mog2\n";
    file << "V.BS.MOG2.History=" << V.BS.MOG2.History <<";\n";
    file << "V.BS.MOG2.Thresh=" << V.BS.MOG2.Thresh <<";\n";
    file << "V.BS.MOG2.DetectShadows=" << V.BS.MOG2.DetectShadows <<";\n";
    file << "V.BS.MOG2.Learning=" << V.BS.MOG2.Learning <<";\n";
    file << "V.BS.MOG2.LRate=" << V.BS.MOG2.LRate <<";\n";

    file << "\n";
    file << "# knn\n";
    file << "Img.bs_knn_history=" << Img.bs_knn_history <<";\n";
    file << "Img.bs_knn_thresh=" << Img.bs_knn_thresh <<";\n";
    file << "Img.bs_knn_shadows=" << Img.bs_knn_shadows <<";\n";
    file << "Img.bs_knn_learning=" << Img.bs_knn_learning <<";\n";
    file << "Img.bs_knn_lrate=" << Img.bs_knn_lrate <<";\n";

    file << "\n";
    file << "# cnt\n";
    file << "V.BS.CNT.MinPixStability=" << V.BS.CNT.MinPixStability <<";\n";
    file << "V.BS.CNT.MaxPixStability=" << V.BS.CNT.MaxPixStability <<";\n";
    file << "V.BS.CNT.UseHistory=" << V.BS.CNT.UseHistory <<";\n";
    file << "V.BS.CNT.IsParallel=" << V.BS.CNT.IsParallel <<";\n";
    file << "V.BS.CNT.FPS=" << V.BS.CNT.FPS <<";\n";
    file << "V.BS.CNT.Learning=" << V.BS.CNT.Learning <<";\n";
    file << "V.BS.CNT.LRate=" << V.BS.CNT.LRate <<";\n";


    file << "\n";
    file << "# gsoc\n";
    file << "Img.bs_gsoc_mc=" << Img.bs_gsoc_mc <<";\n";
    file << "Img.bs_gsoc_samples=" << Img.bs_gsoc_samples <<";\n";
    file << "Img.bs_gsoc_reprate=" << Img.bs_gsoc_reprate <<";\n";
    file << "Img.bs_gsoc_proprate=" << Img.bs_gsoc_proprate <<";\n";
    file << "Img.bs_gsoc_hits_thresh=" << Img.bs_gsoc_hits_thresh <<";\n";
    file << "Img.bs_gsoc_alpha=" << Img.bs_gsoc_alpha <<";\n";
    file << "Img.bs_gsoc_beta=" << Img.bs_gsoc_beta <<";\n";
    file << "Img.bs_gsoc_bs_decay=" << Img.bs_gsoc_bs_decay <<";\n";
    file << "Img.bs_gsoc_bs_mul=" << Img.bs_gsoc_bs_mul <<";\n";
    file << "Img.bs_gsoc_noise_bg=" << Img.bs_gsoc_noise_bg <<";\n";
    file << "Img.bs_gsoc_noise_fg=" << Img.bs_gsoc_noise_fg <<";\n";
    file << "Img.bs_gsoc_learning=" << Img.bs_gsoc_learning <<";\n";

    file << "\n";
    file << "# camera\n";
    file << "V.Cam.Number=" << V.Cam.Number <<";\n";
    file << "V.Cam.Width=" << V.Cam.Width <<";\n";
    file << "V.Cam.Height=" << V.Cam.Height <<";\n";
    file << "V.Cam.Gain=" << V.Cam.Gain <<";\n";
    file << "V.Cam.Contrast=" << V.Cam.Contrast <<";\n";
    file << "V.Cam.Brightness=" << V.Cam.Brightness <<";\n";
    file << "V.Cam.Saturation=" << V.Cam.Saturation <<";\n";
    file << "V.Cam.Exposure=" << V.Cam.Exposure <<";\n";
    file << "V.Cam.Hue=" << V.Cam.Hue <<";\n";
    file << "V.Cam.FPS=" << V.Cam.FPS <<";\n";


    file << "\n";
    file << "# misc\n";
    file << "pix_per_100mm=" << pix_per_100mm <<";\n";
    file << "V.Morph.Type=" << V.Morph.Type <<";\n";
    file << "V.Contours.CurrentAlgo=" << V.Contours.CurrentAlgo <<";\n";

    file << "V.UI.Fullscreen=" << V.UI.Fullscreen <<";\n";
    file.close();
    GUI.ConsoleOut(u8"ФАЙЛ: Сохранение конфигурации завершено");
}

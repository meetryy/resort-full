#include <iostream>
#include <fstream>
#include <Windows.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "variables1.h"
#include "file.h"
#include "img.h"
#include "newGUI.h"
#include "preprocessor.h"
#include "belt_processor.h"

using namespace cv;
using namespace std;

File_class File;

int nthOccurrence(const std::string& str, const std::string& findMe, int nth){
    size_t  pos = 0;
    int     cnt = 0;

    while( cnt != nth ){
        pos+=1;
        pos = str.find(findMe, pos);
        if ( pos == std::string::npos )
            return -1;
        cnt++;
    }
    return pos;
}


void File_class::ReadConfig(string filename){
    ifstream in;            // Create an input file stream.
    in.open(filename);
    if (!in) GUI.ConsoleOut(u8"ОШИБКА: Файл конфигурации %s не найден!", filename.c_str());

    else
    {
        std::size_t found = filename.find_last_of("/\\");
        V.settingsFileName = filename.substr(found+1);

        GUI.ConsoleOut(u8"ФАЙЛ: Файл конфигурации %s найден", filename.c_str());
        string str;
        getline(in, str);  // Get the frist line from the file, if any.
        while (in) {  // Continue if the line was sucessfully read.
            if (str.find("#") == string::npos){

                ProcessLine(str, &V.Input.Source, "V.Input.Source");
                ProcessLine(str, &preprocessor.brightness, "preprocessor.brightness");
                ProcessLine(str, &preprocessor.contrast, "preprocessor.contrast");
                ProcessLine(str, &preprocessor.isOn, "preprocessor.isOn");
                ProcessLine(str, &preprocessor.marginDown, "preprocessor.marginDown");
                ProcessLine(str, &preprocessor.marginLeft, "preprocessor.marginLeft");
                ProcessLine(str, &preprocessor.marginRight, "preprocessor.marginRight");
                ProcessLine(str, &preprocessor.marginUp, "preprocessor.marginUp");
                ProcessLine(str, &preprocessor.rotation, "preprocessor.rotation");
                ProcessLine(str, &preprocessor.saturation, "preprocessor.saturation");
                ProcessLine(str, &preprocessor.sharpTimes, "preprocessor.sharpTimes");

                ProcessLine(str, &V.procType, "V.procType");

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
                //ProcessLine(str, &V.ComPort.isOpen, "V.ComPort.isOpen");
                ProcessLine(str, &V.ComPort.Number, "V.ComPort.Number");
                ProcessLine(str, &V.ComPort.Speed, "V.ComPort.Speed");
                ProcessLine(str, &V.ComPort.shakeAnswer, "V.ComPort.shakeAnswer");
                ProcessLine(str, &V.ComPort.shakeQuery, "V.ComPort.shakeQuery");
                ProcessLine(str, &V.ComPort.shakeTimeout, "V.ComPort.shakeTimeout");

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

                //cnt
                ProcessLine(str, &V.BS.CNT.MinPixStability, "V.BS.CNT.MinPixStability");
                ProcessLine(str, &V.BS.CNT.MaxPixStability, "V.BS.CNT.MaxPixStability");
                ProcessLine(str, &V.BS.CNT.UseHistory, "V.BS.CNT.UseHistory");
                ProcessLine(str, &V.BS.CNT.IsParallel, "V.BS.CNT.IsParallel");
                ProcessLine(str, &V.BS.CNT.FPS, "V.BS.CNT.FPS");


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
                ProcessLine(str, &V.Cam.FPS, "V.Cam.FPS");

                //belt
                ProcessLine(str, &BeltProcessor.ejectorDecZoneX, "BeltProcessor.ejectorDecZoneX");
                ProcessLine(str, &BeltProcessor.high_H, "BeltProcessor.high_H");
                ProcessLine(str, &BeltProcessor.high_S, "BeltProcessor.high_S");
                ProcessLine(str, &BeltProcessor.high_V, "BeltProcessor.high_V");
                ProcessLine(str, &BeltProcessor.low_H, "BeltProcessor.low_H");
                ProcessLine(str, &BeltProcessor.low_S, "BeltProcessor.low_S");
                ProcessLine(str, &BeltProcessor.low_V, "BeltProcessor.low_V");
                ProcessLine(str, &BeltProcessor.minArea, "BeltProcessor.minArea");
                ProcessLine(str, &BeltProcessor.maxArea, "BeltProcessor.maxArea");
                ProcessLine(str, &BeltProcessor.mmPerPix, "BeltProcessor.mmPerPix");
                ProcessLine(str, &BeltProcessor.mmPerFrame, "BeltProcessor.mmPerFrame");
                ProcessLine(str, &BeltProcessor.satRangeMax, "BeltProcessor.satRangeMax");
                ProcessLine(str, &BeltProcessor.satRangeMin, "BeltProcessor.satRangeMin");
                ProcessLine(str, &BeltProcessor.saturationWeight, "BeltProcessor.saturationWeight");
                ProcessLine(str, &BeltProcessor.blurSize, "BeltProcessor.blurSize");
                ProcessLine(str, &BeltProcessor.morphShape, "BeltProcessor.morphShape");
                ProcessLine(str, &BeltProcessor.morphType, "BeltProcessor.morphType");
                ProcessLine(str, &BeltProcessor.morphArea, "BeltProcessor.morphArea");


                //misc
                ProcessLine(str, &pix_per_100mm, "pix_per_100mm");
                ProcessLine(str, &V.Morph.Type, "V.Morph.Type");
                ProcessLine(str, &V.Contours.CurrentAlgo, "V.Contours.CurrentAlgo");
                ProcessLine(str, &V.BS.CurrentAlgo, "V.BS.CurrentAlgo");
                ProcessLine(str, &V.BS.BlurBeforeMog, "V.BS.BlurBeforeMog");
                ProcessLine(str, &V.UI.Fullscreen, "V.UI.Fullscreen");
                ProcessLine(str, &V.UI.guiFPS, "V.UI.guiFPS");
                ProcessLine(str, &GUI.ScreenH, "GUI.ScreenH");
                ProcessLine(str, &GUI.ScreenW, "GUI.ScreenW");
                ProcessLine(str, &Img.matLimiterTarget, "Img.matLimiterTarget");
                ProcessLine(str, &Img.videoFileName, "Img.videoFileName");
                ProcessLine(str, &V.settingsFileName, "V.lastSettingsFileName");


            }
            getline(in, str);   // Try to get another line.
        }
        GUI.ConsoleOut(u8"ФАЙЛ: Загрузка конфигурации завершена");
    }
}


void File_class::SaveConfig(string filename){
    ofstream file;
    file.open(filename,  fstream::out);
    GUI.ConsoleOut(u8"ФАЙЛ: Начинаем запись конфигурации %s", filename.c_str());

    file << "# input\n";
    file << "V.Input.Source=" << V.Input.Source <<";\n";
    file << "Img.input_correction_on=" << Img.input_correction_on <<";\n";
    file << "Img.hsv_input_correction=" << Img.hsv_input_correction <<";\n";


    file << "\n# camera\n";
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


    file << "\n# preprocessor\n";
    file << "preprocessor.brightness=" << preprocessor.brightness <<";\n";
    file << "preprocessor.contrast=" << preprocessor.contrast <<";\n";
    file << "preprocessor.isOn=" << preprocessor.isOn <<";\n";
    file << "preprocessor.marginDown=" << preprocessor.marginDown <<";\n";
    file << "preprocessor.marginLeft=" << preprocessor.marginLeft <<";\n";
    file << "preprocessor.marginRight=" << preprocessor.marginRight <<";\n";
    file << "preprocessor.marginUp=" << preprocessor.marginUp <<";\n";
    file << "preprocessor.rotation=" << preprocessor.rotation <<";\n";
    file << "preprocessor.saturation=" << preprocessor.saturation <<";\n";
    file << "preprocessor.sharpTimes=" << preprocessor.sharpTimes <<";\n";

    file << "\n# processing\n";
    file << "V.procType=" << V.procType <<";\n";

    file << "\n# edge detection\n";
    file << "V.Edge.BlurValue=" << V.Edge.BlurValue <<";\n";
    file << "V.Edge.CannyThresh1=" << V.Edge.CannyThresh1 <<";\n";
    file << "V.Edge.CannyThresh2=" << V.Edge.CannyThresh2 <<";\n";
    file << "V.Contours.CurrentAlgo=" << V.Contours.CurrentAlgo <<";\n";
    file << "V.Edge.UseScharr=" << V.Edge.UseScharr <<";\n";
    file << "V.Edge.ScharrThresh=" << V.Edge.ScharrThresh <<";\n";

    file << "\n# contours\n";
    file << "V.Contours.MinBBoxArea=" << V.Contours.MinBBoxArea <<";\n";
    file << "V.Contours.MaxBBoxArea=" << V.Contours.MaxBBoxArea <<";\n";
    file << "V.Contours.morph_mask=" << V.Contours.morph_mask <<";\n";
    file << "Img.input_correction_on=" << Img.input_correction_on <<";\n";
    file << "V.Morph.Size=" << V.Morph.Size <<";\n";

    file << "\n# color\n";
    file << "V.Color.GoodRGB=" << V.Color.GoodRGB <<";\n";
    file << "V.Color.ToleranceRGB=" << V.Color.ToleranceRGB <<";\n";
    file << "V.Color.GoodHSV=" << V.Color.GoodHSV <<";\n";
    file << "V.Color.ToleranceHSV=" << V.Color.ToleranceHSV <<";\n";
    file << "V.Color.GoodSpace=" << V.Color.GoodSpace <<";\n";


    file << "\n# output\n";
    file << "V.Show.Contours=" << V.Show.Contours <<";\n";
    file << "V.Show.Centers=" << V.Show.Centers <<";\n";
    file << "V.Show.BBoxes=" << V.Show.BBoxes <<";\n";
    file << "V.Show.AvgColor=" << V.Show.AvgColor <<";\n";
    file << "V.Show.Area=" << V.Show.Area <<";\n";
    file << "V.Show.BBoxes=" << V.Show.BBoxes <<";\n";
    file << "V.Show.FillAvg=" << V.Show.FillAvg <<";\n";
    file << "V.Show.FilContour=" << V.Show.FilContour <<";\n";

    file << "\n# com port\n";
//    file << "V.ComPort.isOpen=" << V.ComPort.isOpen <<";\n";
    file << "V.ComPort.Number=" << V.ComPort.Number <<";\n";
    file << "V.ComPort.Speed=" << V.ComPort.Speed <<";\n";
    file << "V.ComPort.shakeQuery=" << V.ComPort.shakeQuery <<";\n";
    file << "V.ComPort.shakeAnswer=" << V.ComPort.shakeAnswer <<";\n";
    file << "V.ComPort.shakeTimeout=" << V.ComPort.shakeTimeout <<";\n";

    file << "\n# mog\n";
    file << "V.BS.CurrentAlgo=" << V.BS.CurrentAlgo <<";\n";
    file << "V.BS.BlurBeforeMog=" << V.BS.BlurBeforeMog <<";\n";

    file << "V.BS.MOG.BackRatio=" << V.BS.MOG.BackRatio <<";\n";
    file << "V.BS.MOG.History=" << V.BS.MOG.History <<";\n";
    file << "V.BS.MOG.Learning=" << V.BS.MOG.Learning <<";\n";
    file << "V.BS.MOG.LRate=" << V.BS.MOG.LRate <<";\n";
    file << "V.BS.MOG.Mixtures=" << V.BS.MOG.Mixtures <<";\n";
    file << "V.BS.MOG.NoiseSigma=" << V.BS.MOG.NoiseSigma <<";\n";


    file << "\n# mog2\n";
    file << "V.BS.MOG2.History=" << V.BS.MOG2.History <<";\n";
    file << "V.BS.MOG2.Thresh=" << V.BS.MOG2.Thresh <<";\n";
    file << "V.BS.MOG2.DetectShadows=" << V.BS.MOG2.DetectShadows <<";\n";
    file << "V.BS.MOG2.Learning=" << V.BS.MOG2.Learning <<";\n";
    file << "V.BS.MOG2.LRate=" << V.BS.MOG2.LRate <<";\n";

    /*
    file << "\n";
    file << "# knn\n";
    file << "Img.bs_knn_history=" << Img.bs_knn_history <<";\n";
    file << "Img.bs_knn_thresh=" << Img.bs_knn_thresh <<";\n";
    file << "Img.bs_knn_shadows=" << Img.bs_knn_shadows <<";\n";
    file << "Img.bs_knn_learning=" << Img.bs_knn_learning <<";\n";
    file << "Img.bs_knn_lrate=" << Img.bs_knn_lrate <<";\n";

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
    */

    file << "\n# cnt\n";
    file << "V.BS.CNT.MinPixStability=" << V.BS.CNT.MinPixStability <<";\n";
    file << "V.BS.CNT.MaxPixStability=" << V.BS.CNT.MaxPixStability <<";\n";
    file << "V.BS.CNT.UseHistory=" << V.BS.CNT.UseHistory <<";\n";
    file << "V.BS.CNT.IsParallel=" << V.BS.CNT.IsParallel <<";\n";
    file << "V.BS.CNT.FPS=" << V.BS.CNT.FPS <<";\n";
    file << "V.BS.CNT.Learning=" << V.BS.CNT.Learning <<";\n";
    file << "V.BS.CNT.LRate=" << V.BS.CNT.LRate <<";\n";

    file << "\n# belt\n";
    file << "BeltProcessor.ejectorDecZoneX=" << BeltProcessor.ejectorDecZoneX <<";\n";
    file << "BeltProcessor.high_H=" << BeltProcessor.high_H <<";\n";
    file << "BeltProcessor.high_S=" << BeltProcessor.high_S <<";\n";
    file << "BeltProcessor.high_V=" << BeltProcessor.high_V <<";\n";
    file << "BeltProcessor.low_H=" << BeltProcessor.low_H <<";\n";
    file << "BeltProcessor.low_S=" << BeltProcessor.low_S<<";\n";
    file << "BeltProcessor.low_V=" << BeltProcessor.low_V <<";\n";
    file << "BeltProcessor.minArea=" << BeltProcessor.minArea <<";\n";
    file << "BeltProcessor.maxArea=" << BeltProcessor.maxArea <<";\n";
    file << "BeltProcessor.mmPerPix=" << BeltProcessor.mmPerPix <<";\n";
    file << "BeltProcessor.mmPerFrame=" << BeltProcessor.mmPerFrame <<";\n";
    file << "BeltProcessor.satRangeMax=" << BeltProcessor.satRangeMax <<";\n";
    file << "BeltProcessor.satRangeMin=" << BeltProcessor.satRangeMin <<";\n";
    file << "BeltProcessor.saturationWeight=" << BeltProcessor.saturationWeight <<";\n";
    file << "BeltProcessor.blurSize=" << BeltProcessor.blurSize <<";\n";
    file << "BeltProcessor.morphShape=" << BeltProcessor.morphShape <<";\n";
    file << "BeltProcessor.morphType=" << BeltProcessor.morphType <<";\n";
    file << "BeltProcessor.morphArea=" << BeltProcessor.morphArea <<";\n";

    file << "\n# misc\n";
    file << "pix_per_100mm=" << pix_per_100mm <<";\n";
    file << "V.Morph.Type=" << V.Morph.Type <<";\n";
    file << "V.Contours.CurrentAlgo=" << V.Contours.CurrentAlgo <<";\n";
    file << "Img.videoFileName=" << Img.videoFileName <<";\n";
    file << "V.UI.Fullscreen=" << V.UI.Fullscreen <<";\n";
    file << "V.UI.guiFPS=" << V.UI.guiFPS<<";\n";
    file << "GUI.screenW=" << GUI.ScreenW<<";\n";
    file << "GUI.screenH=" << GUI.ScreenH<<";\n";
    file << "Img.matLimiterTarget=" << Img.matLimiterTarget<<";\n";
    file << "V.settingsFileName =" << V.settingsFileName <<";\n";



    file.close();
    GUI.ConsoleOut(u8"ФАЙЛ: Сохранение конфигурации завершено");
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

int File_class::ProcessLine(string str, string *parameter, string paramname){
    int to_return = RESULT_FAIL;
    int position = str.find(paramname);         //find a variable name
    if (position != string::npos) {             //if exist
        string value = str.substr(str.find("=")+1, str.find(";"));  //save value as string
        if (!value.empty()) value.pop_back();
        *parameter = value;
        cout << "reading string " << paramname << " = " << *parameter << " OK!"<< endl;
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


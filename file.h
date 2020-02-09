#include <iostream>
#include <fstream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>


class File_class{
private:
    bool ParseSuccess = 0;
    enum ResultAlias {RESULT_OK, RESULT_FAIL};
/*
    template <typename T>
    void ProcessLine(std::string input, T&value, std::string paramname){
        if (std::is_same<T, float>::value) {ProcessLineFloat(input, value, paramname);}
        else if (std::is_same<T, int>::value) {ProcessLineInt(input, value, paramname);}
        else if (std::is_same<T, bool>::value) {ProcessLineBool(input, value, paramname);}
        else if (std::is_same<T, double>::value)  {ProcessLineDouble(input, value, paramname);}
        else if (std::is_same<T, long>::value)  {ProcessLineLong(input, value, paramname);}
        else if (std::is_same<T, cv::Scalar>::value) {ProcessLineScalar(input, value, paramname);}
    }
*/

public:
    void ReadConfig(std::string filename);
    void SaveConfig(std::string filename);

    int ProcessLine(std::string str, long *parameter, std::string paramname);
    int ProcessLine(std::string str, bool *parameter, std::string  paramname);
    int ProcessLine(std::string  str, cv::Scalar *parameter, std::string paramname);
    int ProcessLine(std::string  str, int *parameter, std::string  paramname);
    int ProcessLine(std::string  str, double *parameter, std::string  paramname);
    int ProcessLine(std::string  str, float *parameter, std::string  paramname);
    int ProcessLine(std::string str, std::string *parameter, std::string paramname);
    void LineList(void);
};

extern File_class File;

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <thread>

#include "variables1.h"

long    pix_per_100mm;

double pix2mm(long pixels){
    return (((double)pixels/(double)pix_per_100mm)*100.0);
}




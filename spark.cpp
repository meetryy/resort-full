#include <iostream>
#include <vector>
#include <algorithm>

#include "main.h"
#include "img.h"
#include "variables1.h"
#include "spark.h"

using namespace cv;
using namespace std;

//enum SparkAlias {SPARK_FPS, SPARK_GOOD, SPARK_USEFUL};

double Spark_FPS[SparkLen]={0,1,2,3,4,5,6,7,8,9};


void AddToSpark(long AddValue, double *Array);


void r_right(double *a, long n) {
    int tmp=a[n-1];
    memmove(a+1,a,sizeof(int)*(n-1));
    a[0]=tmp;
 }

 void r_left(double *a, long n) {
  int tmp = a[0];
  memmove(a,a+1,sizeof(int)*(n-1));
  a[n-1]=tmp;
}

void AddToSpark(long AddValue, double *Array){
    //double n=sizeof(Array)/sizeof(Array[0]);
    //for (int i = 0; i < 10; i++){
    // shiftRight(Array);
    //}

    //Array[0] = AddValue;
    //show(AddValue,n);
    //r_right(AddValue,n);
    //show(AddValue,n);

    //double x[3] = {1, 2, 3};
    //std::vector<double> v(x, x + sizeof x / sizeof x[0]);
}

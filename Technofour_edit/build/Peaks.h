#ifndef PEAKS_H
#define PEAKS_H

#include <iostream>
#include <algorithm>
#include "technofour_config.h"

class Peaks{
  private:
    int max(double* data, int size);
    int min(double* data, int size);
    double min(double a, double b);
    int max(int a,int b);
    int min(int a, int b);
    void swap(double *a, double *b);
    void sort(double* data, int size);
    void swap(int *a, int *b);
    void sort(int * data, int size);
    void sort2(double* data1, int* data2,int size);
    int check_pit(int p1, int p2);
    int  peak1,peak2,peak3=-1;
    //FILE* plog;
    int* find_peaks_contacttype2(double* hil,int N);
    int WATER_PEAK_WINDOW_SIZE = 200;

  public:
    int peaks[100];
    int inv_peaks[100];
    int p[3];
    int wp[3];
    double peakvalues[100];
    int peaklength;
    double wrel=0.0,mrel=0.0;
    int waterlength;
    int peakscount=0,pitscount=0;
    int* find_peaks_contacttype(double* hil,int N);
    int* find_peaks_watertype(double* hil,int N);
};

#endif

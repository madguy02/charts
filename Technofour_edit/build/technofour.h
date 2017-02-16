#ifndef TECHNOFOUR_H
#define TECHNOFOUR_H

#include "../lib/ftd2xx.h"
#include "SysConfig.h"
#include "technofour_config.h"
#include "Filter.h"
#include "FFT.h"
#include "GNUPlot.h"
#include "Peaks.h"
#include <csignal>
#include <iostream>
#include <thread>
#include <iomanip>
#include <time.h>

class Technofour{
  private:
    FT_HANDLE ftHandle;
    int N;
    GNUPlot gnuplot;
    FT_STATUS ftStatus = FT_OK;
    int serialNumber;
    int ok=1;
    std::thread *processThread,*peaksPlotThread;
    int measurementType,isCaliberation=0;


    typedef struct Data{      //struct to save statistical data
      double mode;
      double median;
      double rel;
      int freq;
      double relf;
      double waterColumn;
    } Data;

    typedef struct Thickness{     //struct to save thickness data
      double thickness_mm;
      double rel;
      double waterColumn;
    }Thickness;

    typedef struct Mode{     //struct to save Mode
      double mode_value;
      int count;
    }Mode ;

    int check_header(unsigned char c,int i);
    int get_header_index(unsigned char c);
    Mode find_mode(double *array, int size);
    double find_mean(double *array,int size);
    int* getData();
    double* filterDataAndGetHilbert(int* data);
    Peaks getPeaks(double* hil);
    Thickness getThickness(Peaks p);
    Data getResult(double *lendat,double *rel,int count);
    Data getResult(double *lendat,double *rel,double* wc,int count);
    double process_thread(int seconds);
    int close();
  public:
    double thickness,reliability,numberOfPulses,waterColumn,last_thickness,LC;
    int value_number=0;
    Technofour(int serialNumber);
    int setPeakWindowSize(int size);
    int setNoiseLevel(int level);
    int setup(int measurementType);
    int setup(int measurementType, int probeFrequency,double velocity);
    int setup(int measurementType, int probeFrequency,double velocity,int gain,int voltage,int prr,double velocity_in_water,int data_length,int samplerate,int cycles);
    int process();
    double caliberate(double thickness);
    int stop();
};
#endif

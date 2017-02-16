#ifndef GNUPLOT_H
#define GNUPLOT_H

#include <fftw3.h>
#include <math.h>
#include <thread>


class GNUPlot{
private:
  FILE* pipeGNU;
  int window_no = 0;
  int plotting=0;
public:
  GNUPlot();
  void plotintData (int* dat,int N,int id);
  void plotcompData(fftw_complex* dat,int N,int type,int id);
  void plotdoubleData(double* dat,int N,int id);
  void plotdoublePeaks(double* hil,int* peaks,int N,int id);
  void plotintPeaks(int* hil,int* peaks,int N,int id);
  void plotHist(double* dat,int N,int n,int id);
  void plotintintData(int* dat1,int* dat2,int N, int id);
};
#endif

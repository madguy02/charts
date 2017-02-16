#ifndef NOISE_FILTER_H
#define NOISE_FILTER_H
#define MAXFILTERSIZE 100000
#define BFILTERSETVALUE 20      //TODO make this value dynamic
#define LOWPASSFILTERVALUE 0.1

#include <fftw3.h>
#include "technofour_config.h"


class Filter{
private:
  int MedianOfFive(int n1,int n2,int n3, int n4, int n5);
public:
  int *rawData;
  int datasize;
  int b_fil_data[MAXFILTERSIZE];
  int med_fil_data[MAXFILTERSIZE];
  double mov_fil_data[MAXFILTERSIZE];
  double gauss_fil_data[MAXFILTERSIZE];
  double sgolay_fil_data[MAXFILTERSIZE];



  Filter(int *data,int datasize){
    this->rawData=data;
    this->datasize=datasize;
  }
  void baseFilter(int* data);
  void median5filter(int* data);
  void movavg_filter(int* data,int times,int windowsize);
  void gaussian_filter(int* data, int times);
  void sgolay_filter(int* data, int times);
  fftw_complex* lowpass_filter(fftw_complex* ft);

};
#endif

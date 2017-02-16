#ifndef FFT_H
#define FFT_H

#define PI 3.141592654

#include <fftw3.h>
#include <math.h>

class FFT{
private:
  int N;
public:
  FFT(int N){
    this->N=N;
  }
  fftw_complex* fft(int* input);
  fftw_complex* han_fft(int* input);
  fftw_complex* ham_fft(int* input);
  fftw_complex* fft(double* input);
  fftw_complex* ifft(fftw_complex* ft);
  fftw_complex* ham_ifft(fftw_complex* ft);
  double* hilbert(fftw_complex* lp);
};


#endif

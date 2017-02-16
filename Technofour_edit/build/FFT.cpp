#include "FFT.h"

/*Function to find the FFT of integer array input */
fftw_complex* FFT::fft(int* input){
  fftw_complex* in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
  fftw_complex* ft = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
  for(int i=0;i<N;i++){
    in[i][0]=(double)input[i];
    in[i][1]=0.0;
  }
  fftw_plan p;
  p = fftw_plan_dft_1d(N, in, ft, FFTW_FORWARD, FFTW_ESTIMATE);
  fftw_execute(p);
  fftw_destroy_plan(p);
  fftw_free(in);
  return ft;
}

/*Function to find the FFT of double array input*/
fftw_complex* FFT::fft(double* input){
  fftw_complex* in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
  fftw_complex* ft = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
  for(int i=0;i<N;i++){
    in[i][0]=input[i];
    in[i][1]=0.0;
  }
  fftw_plan p;
  p = fftw_plan_dft_1d(N, in, ft, FFTW_FORWARD, FFTW_ESTIMATE);
  fftw_execute(p);
  fftw_destroy_plan(p);
  fftw_free(in);
  return ft;
}

/*Function to find the FFT of integer array input after applying Hanning window*/
fftw_complex* FFT::han_fft(int* input){
  double hann[100000];
  for (int i = 0; i < N; i++) {
    double multiplier = 0.5 * (1 - cos(2*PI*i/(N-1)));
    hann[i] = multiplier * input[i];
  }
  fftw_complex* in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
  fftw_complex* ft = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
  for(int i=0;i<N;i++){
    in[i][0]=hann[i];
    in[i][1]=0.0;
  }
  fftw_plan p;
  p = fftw_plan_dft_1d(N, in, ft, FFTW_FORWARD, FFTW_ESTIMATE);
  fftw_execute(p);
  fftw_destroy_plan(p);
  fftw_free(in);
  return ft;
}

/*Function to find the FFT of integer array input after applying Hamming window*/
fftw_complex* FFT::ham_fft(int* input){
  double hann[100000];
  for (int i = 0; i < N; i++) {
    double multiplier = 0.54 - 0.46*cos(2*PI*i/(N-1));
    hann[i] = multiplier * input[i];
  }
  fftw_complex* in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
  fftw_complex* ft = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
  for(int i=0;i<N;i++){
    in[i][0]=hann[i];
    in[i][1]=0.0;
  }
  fftw_plan p;
  p = fftw_plan_dft_1d(N, in, ft, FFTW_FORWARD, FFTW_ESTIMATE);
  fftw_execute(p);
  fftw_destroy_plan(p);
  fftw_free(in);
  return ft;
}

/*Function to find the IFFT of fftw_complex array ft*/
fftw_complex* FFT::ifft(fftw_complex* ft){
  fftw_complex* ifft = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
  fftw_plan q = fftw_plan_dft_1d(N, ft, ifft, FFTW_BACKWARD, FFTW_ESTIMATE);
  fftw_execute(q);
  fftw_destroy_plan(q);
  for (int i = 0; i < N; i++) {
    ifft[i][0] *= 1./N;
    ifft[i][1] *= 1./N;
  }
  return ifft;
}

/*Function to find the IFFT of fftw_complex array ft that was scaled with Hamming window*/
fftw_complex* FFT::ham_ifft(fftw_complex* ft){
  fftw_complex* ifft = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
  fftw_plan q = fftw_plan_dft_1d(N, ft, ifft, FFTW_BACKWARD, FFTW_ESTIMATE);
  fftw_execute(q);
  fftw_destroy_plan(q);
  for (int i = 0; i < N; i++) {
    double multiplier = 0.54 - 0.46*cos(2*PI*i/(N-1));
    ifft[i][0] *= 1./(N*multiplier);
    ifft[i][1] *= 1./(N*multiplier);
  }
  return ifft;
}

/*Function to return signal envelope(Modulus of analytic signal) of a data if the FFT of the data is passed as fftw_complex array lp*/
double* FFT::hilbert(fftw_complex* lp){
  fftw_plan q;
  static double hilbert[100000];
  fftw_complex* trans = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
  fftw_complex* hil = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
  trans[0][0]=lp[0][0];
  trans[0][1]=lp[0][1];
  for(int i=1;i<(N+1)/2;i++)
  {
    trans[i][0]=2*lp[i][0];
    trans[i][1]=2*lp[i][1];
  }
  if(N%2==0)
  {
    trans[N/2][0]=lp[N/2][0];
    trans[N/2][1]=lp[N/2][1];
  }
  for(int i=(N/2)+1;i<N;i++)
  {
    trans[i][0]=0;
    trans[i][1]=0;
  }
  q = fftw_plan_dft_1d(N, trans, hil, FFTW_BACKWARD, FFTW_ESTIMATE);
  fftw_execute(q);
  fftw_destroy_plan(q);
  for (int i = 0; i < N; i++) {
    hil[i][0] *= 1./N;
    hil[i][1] *= 1./N;
  }
  fftw_free(trans);
  for(int i=0;i<N;i++){
    hilbert[i]=pow(pow(hil[i][0],2)+pow(hil[i][1],2),0.5);
  }
  fftw_free(hil);
  return hilbert;
}

#include "Filter.h"


/*Base Filter to remove abrupt noise in data*/
void Filter::baseFilter(int* data){
  int set=0;
  int setno=0;
  NOISE_THRESHOLD = 8.0;        //Noise floor below which all data is ignored.
  for(int i=0;i<datasize;i++){
    if(set==0){
      if(data[i] > 122 || data[i] < -123){
        setno=0;
      }else{
        setno++;
      }
      if(setno > BFILTERSETVALUE){        //if a number of samples are low, the initial orignal pulse is assumed to have settled
        set=1;
        setno=0;
      }
      b_fil_data[i]=data[i];
    }else if (set==1){
      if(data[i]<-125){       //removing all data less than -125
        if(!(data[i+1]<-125))
        b_fil_data[i]=(data[i-1]+data[i+1])/2;        //replacing such data with the mean of the neighbours
        else
        b_fil_data[i]=data[i-1];
      }else if(data[i]<NOISE_THRESHOLD && data[i]>-NOISE_THRESHOLD){        //removing all data less than noise floor
        b_fil_data[i]=0;
      }else{
        b_fil_data[i]=data[i];
      }
    }
  }
}

/*Function to find median of 5 integers*/
int Filter::MedianOfFive(int n1,int n2,int n3, int n4, int n5){
  int *a=&n1,*b=&n2,*c=&n3,*d=&n4,*e=&n5;
  int *tmp;
  if(*b<*a){      // makes a < b and b < d
    tmp = a;
    a = b;
    b = tmp;
  }
  if(*d<*c){
    tmp = c;
    c = d;
    d = tmp;
  }
  if(*c<*a){  // eleminate the lowest
    tmp = b;
    b = d;
    d = tmp;
    c = a;
  }
  a = e;      // gets e in
  if(*b<*a){      // makes a < b and b < d
    tmp = a;
    a = b;
    b = tmp;
  }
  if(*a<*c){  // eliminate another lowest  remaing: a,b,d
    tmp = b;
    b = d;
    d = tmp;
    a = c;
  }
  if(*d < *a)
  return *d;
  else
  return *a;

}

/*Funtion to median filter the data with a data window size of 5*/
void Filter::median5filter(int* data){
  med_fil_data[0]=data[0];
  med_fil_data[1]=data[1];
  med_fil_data[datasize-1]=data[datasize-1];
  med_fil_data[datasize-2]=data[datasize-2];

  for(int i=2;i<datasize-2;i++){
    med_fil_data[i]=this->MedianOfFive(data[i-2],data[i-1],data[i],data[i+1],data[i+2]);
  }
}

/*Feunction to get the moving average filtered output of the data with a windowsize after times iterations
movavg_filter(data,3,3) -   to implement moving average filter of window size 3 on data 3 times*/
void Filter::movavg_filter(int* data,int times,int windowsize){
  int s=0;
  mov_fil_data[0]=data[0];
  mov_fil_data[datasize-1]=data[datasize-1];
  for(int i=1;i<(windowsize-1)/2-1;i++){
    mov_fil_data[i]=data[i];
    mov_fil_data[datasize-i]=data[datasize-i];
  }
  for(int i=(windowsize-1)/2;i<datasize-(windowsize-1)/2;i++){
    s=data[i];
    for(int j=1;j<(windowsize+1)/2;j++){
      s+=(data[i-j]+data[i+j]);
    }
    mov_fil_data[i]=s/windowsize;
    s=0;
  }
  for(int k=0;k<times-1;k++){
    for(int i=0;i<datasize;i++){
      s=mov_fil_data[i];
      for(int j=1;j<(windowsize+1)/2;j++){
        s+=(mov_fil_data[i-j]+mov_fil_data[i+j]);
      }
      mov_fil_data[i]=s/windowsize;
      s=0;
    }
  }
}

/*Feunction to get the Gaussian filtered output of the data and after times iterations
gaussian_filter(data,3) -   to implement Gaussian filter 3 times*/
void Filter::gaussian_filter(int* data, int times){
  double coeff[] = {0.0545, 0.2442, 0.4026, 0.2442, 0.0545};      //Discrete Gaussian coefficients for a 5 sample window convolution
  gauss_fil_data[0]=data[0];
  gauss_fil_data[1]=data[1];
  gauss_fil_data[datasize-1]=data[datasize-1];
  gauss_fil_data[datasize-2]=data[datasize-2];
  for(int i=2;i<datasize-2;i++){        //Data is convoluted with the coefficients
    gauss_fil_data[i]=(double)(coeff[0]*data[i-2]+coeff[1]*data[i-1]+coeff[2]*data[i]+coeff[3]*data[i-1]+coeff[4]*data[i-2]);
  }
  for(int k=0;k<times-1;k++)      //repeat for k times
  for(int i=2;i<datasize-2;i++){
    gauss_fil_data[i]=(double)(coeff[0]*gauss_fil_data[i-2]+coeff[1]*gauss_fil_data[i-1]+coeff[2]*gauss_fil_data[i]+coeff[3]*gauss_fil_data[i-1]+coeff[4]*gauss_fil_data[i-2]);
  }
}

/*Feunction to get the Sgolay filtered output of the data and after times iterations
sgolay_filter(data,3) -   to implement Sgolay filter 3 times*/
void Filter::sgolay_filter(int* data, int times){
  double coeff[] = {-0.0857,0.3428, 0.4857, 0.3428, -0.0857};
  sgolay_fil_data[0]=data[0];
  sgolay_fil_data[1]=data[1];
  sgolay_fil_data[datasize-1]=data[datasize-1];
  sgolay_fil_data[datasize-2]=data[datasize-2];
  for(int i=2;i<datasize-2;i++){       //Data is convoluted with the coefficients
    sgolay_fil_data[i]=(double)(coeff[0]*data[i-2]+coeff[1]*data[i-1]+coeff[2]*data[i]+coeff[3]*data[i-1]+coeff[4]*data[i-2]);
  }
  for(int k=0;k<times-1;k++)       //repeat for k times
  for(int i=2;i<datasize-2;i++){
    sgolay_fil_data[i]=(double)(coeff[0]*sgolay_fil_data[i-2]+coeff[1]*sgolay_fil_data[i-1]+coeff[2]*sgolay_fil_data[i]+coeff[3]*sgolay_fil_data[i-1]+coeff[4]*sgolay_fil_data[i-2]);
  }
}

/*To implement low pass filter on a data ft in the frequency domian
Note: The FFT of the data is to be passed as input and the FFT of the filtered data will stored in the array lp. A call to this function is to be followed by IFFT operation to get the final output*/
fftw_complex* Filter::lowpass_filter(fftw_complex* ft){
  fftw_complex* lp = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * datasize);
  for(int i=0;i<datasize*LOWPASSFILTERVALUE;i++){     //removing all frequency components less LOWPASSFILTERVALUE
    lp[i][0]=ft[i][0]*(0.5/LOWPASSFILTERVALUE);       //compensating for energy lost due to low pass filtering by amplifying the filtered signal
    lp[i][1]=ft[i][1]*(0.5/LOWPASSFILTERVALUE);
  }
  for(int i=datasize*LOWPASSFILTERVALUE;i<datasize-datasize*LOWPASSFILTERVALUE;i++){
    lp[i][0]=0.0;
    lp[i][1]=0.0;
  }
  for(int i=datasize-datasize*LOWPASSFILTERVALUE;i<datasize;i++){     //repeating for frequency components above the Nyquist frequency
    lp[i][0]=ft[i][0]*(0.5/LOWPASSFILTERVALUE);
    lp[i][1]=ft[i][1]*(0.5/LOWPASSFILTERVALUE);
  }
  return lp;
}

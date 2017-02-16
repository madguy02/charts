#include "Peaks.h"

int Peaks::max(int a, int b){
  return a>b?a:b;
}

int Peaks::min(int a, int b){
  return a<b?a:b;
}

double Peaks::min(double a, double b){
  return a<b?a:b;
}

void Peaks::swap(double *a, double *b){
  double temp = *a;
  *a = *b;
  *b = temp;
}

void Peaks::swap(int *a, int *b){
  double temp = *a;
  *a = *b;
  *b = temp;
}
int Peaks::max(double* data, int size){
  int max=data[0];
  for(int j=0;j<size;j++){
    if(data[j]>data[max])
    max=j;
  }
  return max;
}

int Peaks::min(double* data, int size){
  int min=data[0];
  for(int j=0;j<size;j++){
    if(data[j]<data[min])
    min=j;
  }
  return min;
}

void Peaks::sort(int *data, int size){
  for(int j=0;j<size;j++){
    for(int k=j;k<size;k++){
      if(data[j]<data[k]){
        swap(&data[j],&data[k]);
      }
    }
  }
}

void Peaks::sort2(double* data1, int* data2, int size){       //sort two array based on the elements of the first array
  for(int j=0;j<size;j++){
    for(int k=j;k<size;k++){
      if(data1[j]<data1[k]){
        swap(&data1[j],&data1[k]);
        swap(&data2[j],&data2[k]);
      }
    }
  }
}

int Peaks::check_pit(int p1, int p2){       //check if there is a pit between the peaks p1 and p2
  for(int k=0;k<pitscount;k++){
    if(inv_peaks[k]>p1 && inv_peaks[k]<p2){
      return 1;
    }
  }
  return 0;
}

/*Function to find the peaks in water for signal envelope data of size N*/
int* Peaks::find_peaks_watertype(double* hil,int N){
  peaklength= 0;
  int count=0;
  int initial_peak = 231/PROBE_FREQUENCY;//100000000/(SAMPLE_RATE*PROBE_FREQUENCY);       //calculation initial_peak as Samplingrate/probeFrequency
  //plog = fopen("plog.dat","a");
  int max_index = min((int)round(0.98*N),(int)(N - 300));     //taking 98% of the available data as the last 2% of data has distortions as a result of FFT(Gibbs phenomenon)
  for(int j=1000;j<max_index;j++){          //Takin data after 1000 samples as the initial original pulse has to be ignored
    if(hil[j]>hil[j-1]&&hil[j]>hil[j+1]){         // A similar more speed optimum O(n) but a less space optimised approach is described at http://www.originlab.com/doc/Origin-Help/PA-Algorithm
      if(max(hil+j- WATER_PEAK_WINDOW_SIZE, 2*WATER_PEAK_WINDOW_SIZE)==WATER_PEAK_WINDOW_SIZE)      //checking if hil[j] is a peak
      {
        peaks[count]=j;     //adding j to peak indices array
        peakvalues[count++]=hil[j];   //adding hil[j] to peak values array
      }
    }
  }
  sort2(peakvalues,peaks,count);      //sort based on the height of the peaks
  int a,b,i;
  for(i=peaks[0];i<peaks[0]+500;i++){   //find the pulse width by taking points half the peak height left and right of the peak and taking the midpoint of the pulse width to avoid multiple hidden peaks
    if(hil[i]<0.5*hil[peaks[0]]){
      break;
    }
  }
  b=i;
  for(i=peaks[0];i>peaks[0]-500;i--){
    if(hil[i]<0.5*hil[peaks[0]]){
      break;
    }
  }
  a=i;
  peaks[0] = (a+b)/2;
  int waterpeak =peaks[0];      // min(peaks[0],peaks[1]);
  if(peakvalues[0]-peakvalues[1]<50){
    waterpeak = min(peaks[0],peaks[1]);
  }
  this->waterlength = waterpeak - initial_peak;
  double wplimit = 75*exp(-0.00000024*pow((PROBE_FREQUENCY/1000000),2)*(waterlength-2500));      //Calculating estimated peak height based on attenuation of sound in water
                                                //Values Taken from  http://www.kayelaby.npl.co.uk/general_physics/2_4/2_4_1.html
  if(hil[waterpeak]<wplimit){
    wrel=0;
    mrel=0;
    wp[0] =0;
    wp[1] = 0;
    wp[2] = 0;
    peaklength =0;
    //fflush(plog);
    //fclose(plog);
    return wp;
  }
  find_peaks_contacttype2(hil + waterpeak,min(0.5*waterlength,0.98*N-waterpeak));     //finding the metal reflections
  wp[0] = waterpeak;
  wp[1] = p[1] + waterpeak;
  wp[2] = p[2] + waterpeak;   //adding waterpeak index to get the absolute index
  //fflush(plog);
  //fclose(plog);
  wrel = min(max((hil[waterpeak]-wplimit),0.0)/wplimit,1.0);      //calculating water reliability based on estimated pulse height calcalated earlier
  mrel = min(4.0*hil[wp[1]]/hil[waterpeak],1.0);        //calculation metal reading reliability to be atleast one fourth of waterpeak
  if(peaklength<100){
    wrel=0;
    mrel=0;
    //std::cout<<"low";
    //fflush(stdout);
    //wp[1] = 0;
    //wp[2] = 0;
  }
  if(hil[wp[1]]<wplimit/4){
    mrel=0;
  }
  if(peaklength>4*waterlength){
    wrel=0;
    mrel=0;
    wp[0] =0;
    wp[1] = 0;
    wp[2] = 0;
  }
  return wp;
}

int* Peaks::find_peaks_contacttype2(double* hil,int N){       //function to find metal reflections after water reflections pulse
  //int rise=-1;
  int count=0,icount=0;
  int a,b,i;
  int prevpeak;
  peakscount=0;
  int max_index = N - PEAK_WINDOW_SIZE;
  for(int j=1;j<max_index;j++){         // A similar more speed optimum O(n) but a less space optimised approach is described at http://www.originlab.com/doc/Origin-Help/PA-Algorithm
    if(hil[j]>hil[j-1] && hil[j]>hil[j+1]){
      if(hil[max(hil+j- PEAK_WINDOW_SIZE, 2*PEAK_WINDOW_SIZE)]==hil[PEAK_WINDOW_SIZE])
      {
        peaks[count]=j;
        peakvalues[count++]=hil[j];
      }
    }else if(hil[j]<hil[j-1] && hil[j]<hil[j+1]){
      if(count>0){
        prevpeak = peakvalues[count-1];     //saving the height of the previus peak
      }else{
        prevpeak = hil[0];
      }
      if(hil[min(hil+j- PEAK_WINDOW_SIZE, 2*PEAK_WINDOW_SIZE)]==hil[PEAK_WINDOW_SIZE])// && hil[j] < 0.3*prevpeak)
      {
        inv_peaks[icount++]=j;    //finding pits and saving it in the pits indices array
      }
    }
  }
  peakscount = count;
  pitscount = icount;
  sort2(peakvalues,peaks,peakscount);
  //fprintf(plog, "Peaks(%d): ",peakscount);
  for (int i = 0; i < peakscount; i++) {
    //fprintf(plog, "%d ", peaks[i]);
  }
  //fprintf(plog, "Pits(%d): ",pitscount);
  for (int i = 0; i < pitscount; i++) {
    //fprintf(plog, "%d ", inv_peaks[i]);
  }
  p[0]=0;
  for(i=peaks[0];i<peaks[0]+300;i++){       // //find the pulse width by taking points half the peak height left and right of the peak and taking the midpoint of the pulse width to avoid multiple hidden peaks
    if(hil[i]<0.7*hil[peaks[0]]){
      break;
    }
  }
  b=i;
  for(i=peaks[0];i>peaks[0]-300;i--){
    if(hil[i]<0.7*hil[peaks[0]]){
      break;
    }
  }
  a=i;
  if(a<100){
    p[1]=0;
    p[2]=0;
    fflush(stdout);
    return p;
  }
  peaks[0] = (a+b)/2;
  for(i=peaks[1];i<peaks[1]+300;i++){       // //find the pulse width by taking points half the peak height left and right of the peak and taking the midpoint of the pulse width to avoid multiple hidden peaks
    if(hil[i]<0.7*hil[peaks[1]]){
      break;
    }
  }
  b=i;
  for(i=peaks[1];i>peaks[1]-300;i--){
    if(hil[i]<0.7*hil[peaks[1]]){
      break;
    }
  }
  a=i;
  peaks[1] = (a+b)/2;
  for(i=peaks[2];i<peaks[2]+300;i++){
    if(hil[i]<0.7*hil[peaks[2]]){
      break;
    }
  }
  b=i;
  for(i=peaks[2];i>peaks[2]-300;i--){
    if(hil[i]<0.7*hil[peaks[2]]){
      break;
    }
  }
  a=i;
  peaks[2] = (a+b)/2;
  if(peakscount > 1){
    if((peaks[1]-peaks[0])>0.95*(peaks[0]-p[0]) && (peaks[1]-peaks[0])< 1.05*(peaks[0]-p[0])  && (check_pit(p[0],peaks[0])==1) && (check_pit(peaks[0],peaks[1])==1)){
      p[1] = peaks[0];                                          //checking if the second reflection is at a resonable distance from the first peak to prevent detection of sheer transverse waves and also checking for pits inbetween peaks
      p[2] = peaks[1];
      peaklength = round(0.5*(double)p[2]-0.5*(double)p[0]);
      //fprintf(plog, " Case 1");
    }else if(peakscount>2){
      if((peaks[2]-peaks[0])>0.95*(peaks[0]-p[0]) && (peaks[2]-peaks[0])< 1.05*(peaks[0]-p[0]) && (check_pit(p[0],peaks[0])==1) && (check_pit(peaks[0],peaks[2])==1)){
        p[1] = peaks[0];                                         //checking if the third heighest peak is the actual second reflection pulse
        p[2] = peaks[2];
        peaklength = round(0.5*(double)p[2]-0.5*(double)p[0]);
        //fprintf(plog, " Case 2");
      } else if((check_pit(p[0],peaks[0])==1)){         //if there are no second reflection, check atleast the first reflection pulse is a valid pulse
        peaklength=peaks[0]-p[0];
        p[1] = peaks[0];
        p[2]= 0;
        //fprintf(plog, " Case 3");
      }else{      //No peaks
        //fprintf(plog, "Nothing1");
        p[1]=0;
        p[2]=0;
        fflush(stdout);
      }
    }  else if((check_pit(p[0],peaks[0])==1)){
      peaklength=peaks[0]-p[0];
      p[1] = peaks[0];
      p[2]= 0;
      //fprintf(plog, " Case 4");
    }else{      //No peaks
      //fprintf(plog, "Nothing2");
      p[1]=0;
      p[2]=0;
      fflush(stdout);
    }
  }else if(peakscount==1 && (check_pit(p[0],peaks[0])==1)){
    peaklength = peaks[0]-p[0];
    p[1] = peaks[0];
    p[2]=0;
    //fprintf(plog, " Case 5");
  }else{      //No peaks
    //fprintf(plog, "Nothing3");
    p[1]=0;
    p[2]=0;
    fflush(stdout);
  }
  //fprintf(plog, "  peaklength: %d\n",peaklength);
  return p;
}

/*Function to find the peaks in contact for signal envelope data of size N*/
int* Peaks::find_peaks_contacttype(double* hil,int N){        //function to find peaks in contact
  int count=0;
  int initial_peak = 231.0/PROBE_FREQUENCY;//100000000/(SAMPLE_RATE*PROBE_FREQUENCY);       //initial_peak = sampling rate/probeFrequency
  int max_index = min((int)round(0.98*N),(int)(N - PEAK_WINDOW_SIZE));   //ignore the last part of data due to distortions
  for(int j=1;j<max_index;j++){         // A similar more speed optimum O(n) but a less space optimised approach is described at http://www.originlab.com/doc/Origin-Help/PA-Algorithm
    if(hil[j]>hil[j-1]&&hil[j]>hil[j+1]){
      if(max(hil+j- PEAK_WINDOW_SIZE, 2*PEAK_WINDOW_SIZE)==PEAK_WINDOW_SIZE)      //find peaks
      {
        peaks[count]=j;
        peakvalues[count++]=hil[j];
      }
    }
  }
  peakscount = count;
  p[0]=initial_peak;
  int a,b,i;
  sort2(peakvalues,peaks,peakscount);
  for(i=peaks[1];i<peaks[1]+300;i++){       // //find the pulse width by taking points half the peak height left and right of the peak and taking the midpoint of the pulse width to avoid multiple hidden peaks
    if(hil[i]<0.5*hil[peaks[1]]){
      break;
    }
  }
  b=i;
  for(i=peaks[1];i>peaks[1]-300;i--){
    if(hil[i]<0.5*hil[peaks[1]]){
      break;
    }
  }
  a=i;
  if(a<150){
    p[1]=0;
    p[2]=0;
    return p;
  }
  peaks[1] = (a+b)/2;
  for(i=peaks[2];i<peaks[2]+300;i++){       // //find the pulse width by taking points half the peak height left and right of the peak and taking the midpoint of the pulse width to avoid multiple hidden peaks
    if(hil[i]<0.5*hil[peaks[2]]){
      break;
    }
  }
  b=i;
  for(i=peaks[2];i>peaks[2]-300;i--){
    if(hil[i]<0.5*hil[peaks[2]]){
      break;
    }
  }
  a=i;
  peaks[2] = (a+b)/2;
  for(i=peaks[3];i<peaks[3]+300;i++){       // //find the pulse width by taking points half the peak height left and right of the peak and taking the midpoint of the pulse width to avoid multiple hidden peaks
    if(hil[i]<0.5*hil[peaks[3]]){
      break;
    }
  }
  b=i;
  for(i=peaks[3];i>peaks[3]-300;i--){
    if(hil[i]<0.5*hil[peaks[3]]){
      break;
    }
  }
  a=i;
  peaks[3] = (a+b)/2;
  p[1] = peaks[1];
  p[2] = peaks[2];
  if(peakscount > 2){
    if((peaks[2]-peaks[1])>0.95*(peaks[1]-p[0]) && (peaks[2]-peaks[1])< 1.05*(peaks[1]-p[0])){
                                  // //checking if the second reflection is at a resonable distance from the first peak to prevent detection of sheer transverse waves and also checking for pits inbetween peaks
      p[1] = peaks[1];
      p[2] = peaks[2];
      peaklength = round(p[2]-p[1]);
    }else if(peakscount>3){
      if((peaks[3]-peaks[1])>0.95*(peaks[1]-p[0]) && (peaks[3]-peaks[1])< 1.05*(peaks[1]-p[0])){     //checking if the third heighest peak is the actual second reflection pulse
        p[1] = peaks[1];
        p[2] = peaks[3];
        peaklength = round(p[2]-p[1]);
      }else{       //if there are no second reflection, check atleast the first reflection pulse is a valid pulse
        peaklength=peaks[1]-p[0];
        p[1] = peaks[1];
        p[2]= 0;
      }
    }else{       //if there are no second reflection, check atleast the first reflection pulse is a valid pulse
      peaklength=peaks[1]-p[0];
      p[1] = peaks[1];
      p[2]= 0;
    }
  }else if(peakscount==2){       //if there are no second reflection, check atleast the first reflection pulse is a valid pulse
    peaklength = peaks[1]-p[0];
    p[1] = peaks[1];
    p[2]=0;
  }

  wrel = 1.0;
  double contact_threshold = 50*GAIN_VALUE/30;      //A minimum estimated peak height for water reflection pulses to calculte reliability
  if(hil[p[1]]>contact_threshold){
    mrel=min((hil[p[1]]-contact_threshold)/contact_threshold,1.0);
  }else{
    mrel=0.0;
  }

  return p;
}

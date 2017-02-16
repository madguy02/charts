#include "GNUPlot.h"

GNUPlot::GNUPlot(){       //Constructor: opens gnupipe
  //pipeGNU =  popen("gnuplot -persist", "w");
     pipeGNU =  popen("gnuplot -persist > gnuplot_log.log 2>&1", "w");
}

/*Function to plot complex data.
plotcompData(data,N,0,1)  -   to plot the real part of fftw_complex array data of size N on gnuplot figure 1
*/
void GNUPlot::plotcompData(fftw_complex* dat,int N,int type,int id){
  if(plotting!=0){
    return;
  }
  plotting++;
  if(type==0){
    fprintf(pipeGNU,"set terminal qt %d\n",id);
    fprintf(pipeGNU," plot \'-\' using 1:2 with lines title \'Low Pass Filter\'\n");
    for(int j=0;j<N;j++){
      fprintf(pipeGNU,"%d %f\n",j,dat[j][0]);
    }
    fprintf(pipeGNU,"e\n");
    fflush(pipeGNU);
  }else if(type==1){
    fprintf(pipeGNU,"set terminal qt %d\n",window_no++);
    fprintf(pipeGNU," plot \'-\' using 1:2 with lines title \'Fourier Transform\'\n");
    for(int j=0;j<N;j++){
      fprintf(pipeGNU,"%d %f\n",j,pow(pow(dat[j][0],2)+pow(dat[j][1],2),0.5));
    }
    fprintf(pipeGNU,"e\n");
    fflush(pipeGNU);
  }
  plotting--;
}

/*Function to plot double data.
plotcompData(data,N,2)  -   to plot the double array data of size N on gnuplot figure 2
*/
void GNUPlot::plotdoubleData(double* dat,int N,int id){
  if(plotting!=0){
    return;
  }
  plotting++;
  fprintf(pipeGNU,"set terminal qt %d\n",id);
  fprintf(pipeGNU, "set xrange [0:3000]\n");
  fprintf(pipeGNU," plot [:][0:1000] \'-\' using 1:2 with lines\n");
  for(int j=0;j<N;j++){
    fprintf(pipeGNU,"%d %f\n",j,dat[j]);
  }
  fprintf(pipeGNU,"e\n");
  fflush(pipeGNU);
  plotting--;
}

/*Function to plot integer data.
plotcompData(data,N,3)  -   to plot the integer array data of size N on gnuplot figure 3
*/
void GNUPlot::plotintData(int* dat,int N,int id){
  if(plotting!=0){
    return;
  }
  plotting++;
  fprintf(pipeGNU,"set terminal qt %d\n",id);
  fprintf(pipeGNU, "set xrange [0:3000]\n");
  fprintf(pipeGNU," plot [:][-125:125]\'-\' using 1:2 with lines\n");
  for(int j=0;j<N;j++){
    fprintf(pipeGNU,"%d %d\n",j,dat[j]);
  }
  fprintf(pipeGNU,"e\n");
  fflush(pipeGNU);
  plotting--;
}

/*Function to plot double data along with peaks
plotcompData(data,peaks,N,4)  -   to plot the double array data of size N along with the peaks in array peaks on gnuplot figure 4
*/
void GNUPlot::plotdoublePeaks(double* hil,int* peaks,int N,int id){
  if(plotting!=0){
    return;
  }
  plotting++;
  int count=0;
  fprintf(pipeGNU,"set terminal qt %d\n",id);
  fprintf(pipeGNU, "set xrange [0:%d]\n",N-200);
  fprintf(pipeGNU," plot [0:%d][0:400] \'-\' using 1:2 with lines title \'Hilbert Transform\', \'-\' using 1:2 with lines title \'Peaks\'\n",N-200);
  //printf(" plot [:][-150:150] \'-\' using 1:2 with lines\n");
  for(int j=0;j<N;j++){
    fprintf(pipeGNU,"%d %f\n",j,hil[j]);
  }
  fprintf(pipeGNU,"e\n");
  for(int j=0;j<N;j++){
    if(j==peaks[count]){
      fprintf(pipeGNU,"%d 1000\n",j);
      fprintf(pipeGNU,"set label %d \"%d\" at %d,0\n",count,j,j);
      count++;
    }else{
      fprintf(pipeGNU,"%d 0\n",j);
    }
  }
  fprintf(pipeGNU,"e\n");
  for(int j=0;j<3;j++){
    fprintf(pipeGNU,"set label %d \"%d\" at %d,500\n",j+1,peaks[j],peaks[j]);
    count++;
  }
  fflush(pipeGNU);
  plotting--;
}

/*Function to plot integer data along with peaks
plotcompData(data,peaks,N,5)  -   to plot the integer array data of size N along with the peaks in array peaks on gnuplot figure 5
*/
void GNUPlot::plotintPeaks(int* hil,int* peaks,int N,int id){
  if(plotting!=0){
    return;
  }
  plotting++;
  int count=0;
  fprintf(pipeGNU,"set terminal qt %d\n",id);
  fprintf(pipeGNU," plot [:][-150:150] \'-\' using 1:2 with lines title \'Data\', \'-\' using 1:2 with lines title \'Peaks\'\n");
  //printf(" plot [:][-150:150] \'-\' using 1:2 with lines\n");
  for(int j=0;j<N;j++){
    fprintf(pipeGNU,"%d %d\n",j,hil[j]);
  }
  fprintf(pipeGNU,"e\n");
  for(int j=0;j<N;j++){
    if(j==peaks[count]){
      fprintf(pipeGNU,"%d 249\n",j);
      fprintf(pipeGNU,"set label %d \"%d\" at %d,0\n",count,j,j);
      count++;
    }else{
      fprintf(pipeGNU,"%d 0\n",j);
    }
  }
  fprintf(pipeGNU,"e\n");
  for(int j=0;j<10;j++){
    fprintf(pipeGNU,"set label %d \"%d\" at %d,90\n",j+1,peaks[j],peaks[j]);
    count++;
  }
  fflush(pipeGNU);
  plotting--;
}

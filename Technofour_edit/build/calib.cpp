#include <iostream>
#include <unistd.h>
#include "technofour.h"

#define CONTACT_TYPE 0
#define WATER_TYPE 1

Technofour *tf;

void signalHandler(int signum){
  tf->stop();
  signum = signum;
  exit(0);
}

int main(int argc,char** argv){
  if (getuid()){
    std::cerr<< "You must be root!"<<std::endl;
    return -1;
  }
  tf = new Technofour(11111111);
  int probeFrequency = 3500000, measurementType=WATER_TYPE;
  std::signal(SIGINT, signalHandler);
  double thickness;
  if(argc>1){
      thickness = atof(argv[1]);
  }else{
    std::cerr<<"No thickness specified!"<<std::endl;
    return 1;
  }
  if(argc>2){
      probeFrequency = (int)(atof(argv[2])*1000000);
  }
  int setup_out = tf->setup(measurementType, probeFrequency, 6300.0);
  if(setup_out!=0){
    std::cerr<<"Error in setting up Err_code:"<<setup_out<<std::endl;
  }
  double velocity = tf->caliberate(thickness);
  std::cout<<velocity<<std::endl;
  return 0 ;
}

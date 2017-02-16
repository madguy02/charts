#include <iostream>
#include "technofour.h"
#include "../Simple-WebSocket-Server/build/ws_examples"



#define CONTACT_TYPE 0
#define WATER_TYPE 1

Technofour tf(11111111);

void signalHandler(int signum){
  tf.stop();
  signum = signum;
  exit(0);
}

int main(int argc,char** argv){
  int probeFrequency = 1000000, measurementType=WATER_TYPE,velocity=6300;
  std::signal(SIGINT, signalHandler);
  if(argc>1){
      probeFrequency = (int)(atof(argv[1])*1000000);
  }
  if(argc>2){
      measurementType = atoi(argv[2]);
  }
  if(argc>3){
      velocity = atoi(argv[3]);
  }
  int setup_out = tf.setup(measurementType, probeFrequency, velocity);
  if(setup_out!=0){
    std::cerr<<"Error in setting up Err_code:"<<setup_out<<std::endl;
  }
  int MEASUREMENTTYPE=measurementType;
  int VELOCITY=velocity;
  int PROBEFREQUENCY=probeFrequency;

  std::cout<<MEASUREMENTTYPE<<std::endl;

  //probeFrequency=pF;
  //velocity=vel;
  //std::cout<<mT<<std::endl;
  //std::cout<<velocity<<std::endl;
  /*velocity = tf.caliberate(89.9);
  std::cout<<"Caliberated at Speed: "<< velocity<<"m/s"<<std::endl;
  setup_out = tf.setup(measurementType, probeFrequency, velocity);
  if(setup_out!=0){
    std::cerr<<"Error in setting up Err_code:"<<setup_out<<std::endl;
  }*/
  //tf.server();
  tf.process();
  int oldno=0;
  while(1){
    if(tf.value_number!=oldno){
      oldno = tf.value_number;
      std::cout<<oldno<<" "<<tf.thickness<<" "<<tf.reliability<<" "<<tf.waterColumn<<std::endl;
    }
  }

}

#include "technofour.h"

int integer=0;
Filter* f;

Technofour::Technofour(int serialNumber){
  this->serialNumber = serialNumber;
  system("sudo rmmod ftdi_sio && sudo rmmod usbserial");
}

/*This function checks if the given character is a part of the header and returns 1 if true*/

int Technofour::check_header(unsigned char c,int i){
  unsigned char header[9] = {0xff, 0x00, 0xaa, 0x55, 0xdd, 0x22,0xbb, 0x44, (unsigned char)DATA_LENGTH};      //9 byte data header
  if(c==header[i]){
    return 1;
  }
  else
  return 0;
}

/*This function returns the index of a character in the header*/
int Technofour::get_header_index(unsigned char c){
  unsigned char header[9] = {0xff, 0x00, 0xaa, 0x55, 0xdd, 0x22,0xbb, 0x44, (unsigned char)DATA_LENGTH};      //9 byte data header
  for(int i=0;i<9;i++){
    if(header[i]==c){
      return i+1;
    }
  }
  return -1;
}

/*Finds the mode of the given array. If the mode is zero, returns the element with the next highest frequency. Returns 0 only if all elements of the array are 0*/
Technofour::Mode Technofour::find_mode(double *array, int size){      //function to find mode
  double number = array[0];
  double mode = number;
  int count = 1;
  int countMode = 1;
  double summode = 0;
  int nmode = 0;

  for (int i=1; i<size; i++){
    if (array[i] == number)  { // count occurrences of the current number
      count++;
    }
    else  { // now this is a different number
      if (count > countMode && number!=0.0){        //ignore zero as zero is not to be considered as a mode
        countMode = count; // mode is the biggest ocurrences
        mode = number;
        summode = mode;
        nmode = 1;
      }else if(count == countMode && number!=0.0){        //ignore zero as zero is not to be considered as a mode
        nmode++;      //if there are multiple modes take the average of the all the modes
        summode+=number;
        mode=summode/(double)nmode;
      }
      count = 1; // reset count for the new number
      number = array[i];
    }
  }
  if (count > countMode && number!=0.0){      //ignore zero as zero is not to be considered as a mode
    countMode = count; // mode is the biggest ocurrences
    mode = number;
    summode = mode;
    nmode = 1;
  }
  Mode mod;
  mod.mode_value = mode;
  mod.count = countMode;
  return mod;
}

/*Returns the mean of the given array*/
double Technofour::find_mean(double *array,int size){     //function to find mean
  double sum=0;
  for(int i=0;i<size;i++){
    sum+=array[i];
  }
  return (sum/(double)size);
}

/*This function to get the data for a single pulse from the Technofour. */
int* Technofour::getData(){
  static int data[100000];      //static array to store and return data
  DWORD dwBytesRead,numBytes;
  LPVOID pcBufRead[BUF_SIZE];
  int pulse_state = 0;
  int head_char_count = 0,head_match_count=0;
  int X_axis = 0;


  while (ok) {
    ftStatus = FT_GetQueueStatus(ftHandle, &numBytes);          // checking the buffer
    if (ftStatus != FT_OK){
      std::cerr<<"Get Queue Status Failed"<<std::endl;
      return NULL;
    }
    if(numBytes==65536){      //if the buffer queue size reaches 2^16, the buffer is full and data has overflown
      std::cerr << "*********BUFFER OVERFLOW!!**********" << std::endl;
    }
    if (numBytes > 0) {
      ftStatus = FT_Read(ftHandle, pcBufRead, numBytes, &dwBytesRead);
      if(ftStatus != FT_OK){
        std::cerr<<"FT_READ Failed"<<std::endl;
        return NULL;
        continue;
      }else{
      unsigned char* buffer = (unsigned char*)pcBufRead;      //casting to 1byte char
      for (unsigned int i = 0;(i < dwBytesRead) && ok; i++) {
        if (pulse_state == 1) {     //pulse has started and the data is read
          data[X_axis]=buffer[i] - 128;
          X_axis++;   //increment the index
          if (X_axis > N){
            return data;
          }
        } else if(pulse_state==0){      //if the pulse has ended
          if ((buffer[i]) > 0){
            pulse_state=2;
            head_match_count+=check_header(buffer[i],head_char_count);    //check for header`
            head_char_count++;
            if(head_match_count==0){
              if(get_header_index(buffer[i])!=-1){      //synchronise the data header checking algorithm
                head_char_count = get_header_index(buffer[i]);
                head_match_count = 1;
              }
            }
          }
        }else if (pulse_state==2){      //pulse has started and is in the header state
          head_match_count+=check_header(buffer[i],head_char_count);
          head_char_count++;
          if(head_match_count==0){
            if(get_header_index(buffer[i])!=-1){      //synchronise the data header checking algorithm
              head_char_count = get_header_index(buffer[i]);
              head_match_count = 1;
            }
          }
          if(head_char_count>=9){
            if(head_match_count>5){     //check if 5 characters match in the header
              pulse_state=1;
            }
            else{
              pulse_state=0;
            }
            head_char_count=0;
            head_match_count=0;
          }
        }
      }
    }
  }
  }
  return NULL;
}

int Technofour::setPeakWindowSize(int size){
  PEAK_WINDOW_SIZE = size;
  return 0;
}

int Technofour::setNoiseLevel(int level){
  NOISE_THRESHOLD = level;
  return 0;
}

int Technofour::setup(int measurementType){      //function to setup Technofour UTUSB
  this->measurementType = measurementType;
  ftStatus = FT_OpenEx((PVOID)std::to_string(serialNumber).c_str(),FT_OPEN_BY_SERIAL_NUMBER,&ftHandle);         // opening the port serial 11111111
  if (ftStatus != FT_OK) {
    std::cerr<<"FT_OpenEx device of Serial Number 11111111 failed"<<std::endl;
    return 1;
  }
  std::cout<<"FT_Open succeeded.  Handle is "<<ftHandle<<std::endl;
  N = 256*pow(2,DATA_LENGTH);     //Calculate data length`
  SysConfig sys_config;
  if (sys_config.initialize(ftHandle) != 0) {       // initializing the device settings
    return 2;
  }
  if(measurementType==1){
    LC=NON_CONTACT_LC;
    GAIN_VALUE = 10;
  }else{
    LC=CONTACT_LC;
    GAIN_VALUE = 30;
  }
  return 0;
}


int Technofour::setup(int measurementType, int probeFrequency,double velocity){      //function to setup Technofour UTUSB
  ftStatus = FT_OpenEx((PVOID)std::to_string(serialNumber).c_str(),FT_OPEN_BY_SERIAL_NUMBER,&ftHandle);         // opening the port serial 11111111
  if (ftStatus != FT_OK) {
    std::cerr<<"FT_OpenEx device of Serial Number 11111111 failed"<<std::endl;
    return 1;
  }
  std::cerr<<"FT_Open succeeded.  Handle is "<<ftHandle<<std::endl;
  this->measurementType = measurementType;
  PROBE_FREQUENCY = probeFrequency;
  VELOCITY = velocity;
  N = 256*pow(2,DATA_LENGTH);
  if(PROBE_FREQUENCY>4000000){
    LOW_PASS_FREQUENCY = 5;
  }
  if(measurementType==1){
    LC=NON_CONTACT_LC;
    GAIN_VALUE = 10;
  }else{
    LC=CONTACT_LC;
    GAIN_VALUE = 30;
  }
  return 0;
}

int Technofour::setup(int measurementType, int probeFrequency,double velocity,int gain,int voltage,int prr,double velocity_in_water,int data_length,int samplerate,int cycles){      //function to setup Technofour UTUSB
  ftStatus = FT_OpenEx((PVOID)std::to_string(serialNumber).c_str(),FT_OPEN_BY_SERIAL_NUMBER,&ftHandle);         // opening the port serial 11111111
  if (ftStatus != FT_OK) {
    std::cerr<<"FT_OpenEx device of Serial Number 11111111 failed"<<std::endl;
    return 1;
  }
  std::cerr<<"FT_Open succeeded.  Handle is "<<ftHandle<<std::endl;
  this->measurementType = measurementType;
  PROBE_FREQUENCY = probeFrequency;
  VELOCITY = velocity;
  if(PROBE_FREQUENCY>4000000){
    LOW_PASS_FREQUENCY = 5;
  }
  GAIN_VALUE = gain;
  VOLTAGE = voltage;
  PRR_RATE = prr;
  VELOCITY_OF_SOUND_IN_WATER = velocity_in_water;
  DATA_LENGTH = data_length;
  SAMPLE_RATE = samplerate;
  NO_OF_CYCLES = cycles;
  N = 256*pow(2,DATA_LENGTH);
  if(measurementType==1){
    LC=NON_CONTACT_LC;
    GAIN_VALUE = 10;
  }else{
    LC=CONTACT_LC;
    GAIN_VALUE = 30;
  }
  return 0;
}


double* Technofour::filterDataAndGetHilbert(int* data){     //funtion to get the magnitude of analytic signal of the data
  static Filter filter(data,N);      //initializing the filter object
  f = &filter;
  filter.baseFilter(data);     //apply base filter to remove random noise
  filter.median5filter(filter.b_fil_data);    //apply medianfilter to remove abrupt noise
  FFT f(N);
  fftw_complex* ft = f.fft(filter.med_fil_data);
  fftw_complex* lp = filter.lowpass_filter(ft);   //apply low pass filter to smooth the data
  double* hil = f.hilbert(lp);      //find the signal envelope of the data
  fftw_free(ft);
  fftw_free(lp);
  return hil;     //return the signal envelope as double array
}

Peaks Technofour::getPeaks(double* hil){      //function to get peaks
  Peaks p;
  switch (measurementType) {
    case 0:
      p.find_peaks_contacttype(hil,N);
      break;
    case 1:
      p.find_peaks_watertype(hil,N);
      break;
  }
  return p;
}

Technofour::Thickness Technofour::getThickness(Peaks p){    //function to calculate thickness
  Thickness thickness;
  thickness.thickness_mm =  ((((double)p.peaklength*((double)SAMPLE_RATE))/100.0)*VELOCITY/1000.0)/2.0;     //thickness calculation
  if(isCaliberation){
    thickness.thickness_mm = round(p.peaklength/2)*2;
  }
  thickness.rel = p.wrel*p.mrel;
  if(measurementType==1){
    thickness.thickness_mm = round(thickness.thickness_mm/NON_CONTACT_LC)*NON_CONTACT_LC;
    thickness.waterColumn = ((((double)p.waterlength*((double)SAMPLE_RATE))/100.0)*VELOCITY_OF_SOUND_IN_WATER/1000.0)/2.0;    //waterColumn calculation
  }else{
    thickness.thickness_mm = round(thickness.thickness_mm/CONTACT_LC)*CONTACT_LC;
  }
  return thickness;
}

Technofour::Data Technofour::getResult(double *lendat,double *rel,int count){     //function to calculate to final modal thickness and results
  std::sort(lendat,lendat+count);   //sort the thickness array
  Data data;
  Mode mode = find_mode(lendat,count);    //find the modal thickness
  data.mode = mode.mode_value;
  data.median = lendat[count/2];
  double relsum=0.0;
  int mode_count=mode.count;
  for(int a=0;a<count;a++){
    if(lendat[a] == mode.mode_value){   //find the average reliability
      relsum+=rel[a];
      integer++;
    }
  }
  data.rel = relsum/(double)count;
  data.freq = mode_count;
  data.relf = relsum/(double)integer;
  if(integer==0 || mode_count==0){
    data.relf = 0;
  }
  return data;
}

Technofour::Data Technofour::getResult(double *lendat,double *rel,double* wc,int count){     //function to calculate to final modal thickness and results
  std::sort(lendat,lendat+count);
  Data data;
  Mode mode = find_mode(lendat,count);
  data.mode = mode.mode_value;
  data.median = lendat[count/2];
  double relsum=0.0;
  //int mode_count=mode.count;
  for(int a=0;a<count;a++){
    if(lendat[a] == mode.mode_value){
      relsum+=rel[a];
      integer++;
    }
  }
  data.rel = relsum/(double)count;
  data.freq =integer;
  data.relf = relsum/(double)integer;
  if(integer==0 || mode.count==0){
    data.relf = 0;
  }
  std::sort(wc,wc+count);
  double mean = find_mean(wc,count);      //find mean of waterColumn
  data.waterColumn = mean;
  return data;
}

int Technofour::process(){
  processThread = new std::thread(&Technofour::process_thread,this,1);      //start the process in a new thread
  return 0;
}

double Technofour::caliberate(double thickness){
  isCaliberation = 1;
  double samples = process_thread(2);
  isCaliberation = 0;
  double velocity =  thickness*200000.0/((samples-0.5)*((double)SAMPLE_RATE));    //calculte velocity
  if(velocity>0)
    return velocity;
  else
    return -1.0;
}

double Technofour::process_thread(int seconds) {
  int count=0;
  time_t start = time(NULL);
  SysConfig sys_config;
  if (sys_config.initialize(ftHandle) != 0) {       // initializing the device settings
    return -1.0;
  }
  while(ok){
    double lendat[1000],wc[1000],rel[1000];
    int tm = difftime(time(NULL), start);
    while(tm < seconds){
      int* data = getData();    //get a single pulse of data
      if(data==NULL){
        close();
        return -1.0;
      }
      double* hil = filterDataAndGetHilbert(data);
      Peaks p = getPeaks(hil);
      Thickness thickness = getThickness(p);
      rel[count] = thickness.rel;
      if(measurementType==1){
        wc[count] = thickness.waterColumn;
      }
      lendat[count++] = thickness.thickness_mm;
      if(count%10==0){
        switch (measurementType) {      //start the plot in a new thread and detach it
          case 0:
            peaksPlotThread = new std::thread(&GNUPlot::plotdoublePeaks, &gnuplot,hil,p.p,N,1);
            break;
          case 1:
            peaksPlotThread = new std::thread(&GNUPlot::plotdoublePeaks, &gnuplot,hil,p.wp,N,1);
            break;
        }
        peaksPlotThread->detach();
      }
      tm = difftime(time(NULL), start);   //check time
    }
    Data data;
    if(measurementType==1){
       data = getResult(lendat,rel,wc,count);
       this->waterColumn = data.waterColumn;
    }else{
      data = getResult(lendat,rel,count);
    }
    std::cout << std::setprecision(1) << std::fixed<< std::setw(2) << std::setfill( '0' );
    integer=0;
    thickness=data.mode;
    last_thickness =data.mode;
    reliability = data.relf;
    numberOfPulses=count;
    if(reliability<0.5 || thickness ==0.0){      //replacing unreliable values with -1
      thickness = -1.0;
    }else{
      if(!isCaliberation)
      value_number++;       //update status that a new thickness reading is ready
    }
    count=0;
    start = time(NULL);
    tm = difftime(time(NULL), start);
    if(isCaliberation){
      if(reliability>0.5){
        close();
        return thickness;
      }
    }
  }
  close();
  return thickness;
}

int Technofour::stop(){
  ok=0;
  if(!isCaliberation)
  processThread->join();
  return 0;
}

int Technofour::close() {
  if (ftHandle != NULL)
  ftStatus = FT_Close(ftHandle);
  if(ftStatus!=FT_OK){
    std::cerr <<"FTCLOSE ERROR!!!" << std::endl;
    std::terminate();
  }
  return 0;
}

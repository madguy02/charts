#include "SysConfig.h"

/*Function to initialize the Technofour with the configuration values in the technofour_config.cpp file*/
int SysConfig::initialize(FT_HANDLE ftHandle) {
  FT_STATUS ftStatus = FT_OK;
  ftStatus = FT_ResetDevice(ftHandle);
  if (ftStatus != FT_OK) {
    std::cerr<<"Failure.  FT_ResetDevice returned" <<(int)ftStatus<<std::endl;
    return 1;
  }
  ftStatus = FT_SetBaudRate(ftHandle, (ULONG)baudRate);
  if (ftStatus != FT_OK) {
    std::cerr<<"Failure.  FT_SetBaudRate("<<baudRate<<") returned "<<(int)ftStatus<<std::endl;
    return 1;
  }
  ftStatus = FT_SetDataCharacteristics(ftHandle, FT_BITS_8, FT_STOP_BITS_1, FT_PARITY_NONE);
  if (ftStatus != FT_OK) {
    std::cerr<<"Failure.  FT_SetDataCharacteristics returned "<<(int)ftStatus<<std::endl;
    return 1;
  }
  // Indicate our presence to remote computer
  ftStatus = FT_SetDtr(ftHandle);
  if (ftStatus != FT_OK) {
    std::cerr<<"Failure.  FT_SetDtr returned "<<(int)ftStatus<<std::endl;
    return 1;
  }
  // Flow control is needed for higher baud rates
  ftStatus = FT_SetFlowControl(ftHandle, FT_FLOW_RTS_CTS, 0, 0);
  if (ftStatus != FT_OK) {
    std::cerr<<"Failure.  FT_SetFlowControl returned " <<(int)ftStatus<<std::endl;
    return 1;
  }
  // Assert Request-To-Send to prepare remote computer
  ftStatus = FT_SetRts(ftHandle);
  if (ftStatus != FT_OK) {
    std::cerr<<"Failure.  FT_SetRts returned "<<(int)ftStatus <<std::endl;
    return 1;
  }
  ftStatus = FT_SetTimeouts(ftHandle, 0, 0);
  if (ftStatus != FT_OK) {
    std::cerr<<"Failure.  FT_SetTimeouts returned "<< (int)ftStatus<<std::endl;
    return 1;
  }
  this->setGain(ftHandle, GAIN_VALUE);
  this->setTrigger(ftHandle, TRIGGER_OFF, AUTO_PRR_OFF, PRR_RATE);
  this->setPulse(ftHandle, PULSE_TYPE, PROBE_FREQUENCY, VOLTAGE, NO_OF_CYCLES,0);
  this->setSettings(ftHandle, PROBE_TYPE, SAMPLE_RATE, LOW_PASS_FREQUENCY,HIGH_PASS_FREQUENCY, AVG_A_SCANS);
  this->setDampDataLen(ftHandle, 0, DATA_LENGTH);
  this->setTrigger(ftHandle, TRIGGER_STATUS, AUTO_PRR_OFF, PRR_RATE);
  ftStatus = FT_SetBitMode(ftHandle, 0xFF, 0x1);
  if (ftStatus != FT_OK) {
    std::cerr<<"Failure.  FT_SetBitMode returned " <<(int)ftStatus<<std::endl;
    return 1;
  }
  return 0;
}

int SysConfig::my_write(FT_HANDLE ftHandle, char testPattern[],DWORD bytesToWrite) {
  DWORD bytesWritten = 0;
  FT_STATUS ftStatus = FT_OK;
  ftStatus = FT_Write(ftHandle, testPattern, bytesToWrite, &bytesWritten);
  if (ftStatus != FT_OK) {
    std::cerr<<"Failure.  FT_Write returned "<<(int)ftStatus<<std::endl;
    goto exit;
  }
  if (bytesWritten != bytesToWrite) {
    std::cerr<<"Failure.  FT_Write wrote "<<(int)bytesWritten<<" bytes instead of "<<(int)bytesToWrite <<std::endl;
    goto exit;
  }
  return 1;

  exit:
  if (ftHandle != NULL)
  FT_Close(ftHandle);
  exit(-1);
  return -1;
}

double SysConfig::setGain(FT_HANDLE ftHandle, double gainVal) {
  char instruct[5];
  instruct[0] = 'G';
  double g1, m1;
  double nMultiplier;
  unsigned short g2;
  int boost = (gainVal > (PA_GAIN + MAX_DAC_GAIN)) ? 1 : 0;
  if (gainVal <= MIN_DAC_GAIN) {
    instruct[1] = instruct[2] = 0;
    m1 = pow(10.0, (double)((PA_GAIN - gainVal) / 20.0));
    nMultiplier = 1.0 / m1;
  } else if (gainVal > MIN_DAC_GAIN && gainVal <= MAX_GAIN) {
    g1 = (gainVal - (boost ? (PA_GAIN + BOOST_GAIN) : PA_GAIN));
    g2 = (unsigned short)(g1 * UT_CONSTANT);
    instruct[1] = (g2 & 0x0300) >> 8;
    instruct[2] = g2 & 0x00ff;
    nMultiplier = 1.0;
  } else if (gainVal > MAX_GAIN) {
    instruct[1] = 0x03;
    instruct[2] = (unsigned char)0xff;
    nMultiplier = pow(10.0, (double)((MAX_GAIN - gainVal) / 20.0));
  }
  instruct[3] = boost;
  instruct[4] = 0;
  int check = my_write(ftHandle, instruct, 5);
  if (check != 1) {
    std::cerr<<"Failed Setting Gain!"<<std::endl;
    goto exit;
  }
  return nMultiplier;

  exit:
  if (ftHandle != NULL)
  FT_Close(ftHandle);
  exit(-1);
  return -1;
}

// PRR pulse repetition rate - pulse per second - pulse frequency
int SysConfig::setTrigger(FT_HANDLE ftHandle, int tMode, int autoPrr,int prrRate) {
  char instruct[5];
  instruct[0] = 'T';
  instruct[1] = tMode;
  double tPeriod = autoPrr ? (1.0 / AUTO_PRR) : (1.0 / prrRate);
  tPeriod *= 1000000.0 / 100.0;
  instruct[2] = (char)tPeriod;
  instruct[3] = instruct[4] = 0;
  int check = my_write(ftHandle, instruct, 5);
  if (check != 1) {
    std::cerr<<"Failed Setting Trigger!"<<std::endl;
    goto exit;
  }
  return 1;

  exit:
  if (ftHandle != NULL)
  FT_Close(ftHandle);
  exit(-1);
  return -1;
}

int SysConfig::setPulse(FT_HANDLE ftHandle, int pulseType, double probeFreq,int vPulse, int nCycles, int dampEn) {
  char instruct[5];
  instruct[0] = 'P';
  double tPeriod = 1000000000.0 / probeFreq;
  tPeriod = tPeriod / 10.0;
  tPeriod /= 2;
  if (pulseType == PULSE_BIPOLAR) {
    instruct[1] = instruct[2] = (char)(tPeriod);
  } else if (pulseType == PULSE_UNIPOLAR) {
    instruct[1] = (char)(tPeriod);
    instruct[2] = 0;
  } else if (pulseType == PULSE_SPIKE) {
    instruct[1] = 2;
    instruct[2] = 0;
  }
  if (dampEn == 0)
  instruct[3] = 0;
  else
  instruct[3] = (char)(dampEn);
  instruct[4] = vPulse | (nCycles << 4);
  int check = my_write(ftHandle, instruct, 5);
  if (check != 1) {
    std::cerr<<"Failed Setting Pulse!"<<std::endl;
    goto exit;
  }
  return 1;

  exit:
  if (ftHandle != NULL)
  FT_Close(ftHandle);
  exit(-1);
  return -1;
}

int SysConfig::setSettings(FT_HANDLE ftHandle, int prType, int sampleRate,int lpf, int hpf, int nAvg) {
  char instruct[5];
  instruct[0] = 'S';
  instruct[1] = (char)sampleRate;
  instruct[2] = (char)prType;
  instruct[3] = lpf | (hpf << 3);
  instruct[4] = 0;
  nAvg = nAvg;
  int check = my_write(ftHandle, instruct, 5);
  if (check != 1) {
    std::cerr<<"Failed Setting Settings!"<<std::endl;
    goto exit;
  }
  return 1;

  exit:
  if (ftHandle != NULL)
  FT_Close(ftHandle);
  exit(-1);
  return -1;
}

int SysConfig::setDampDataLen(FT_HANDLE ftHandle, int delayVal, int dataLen) {
  char instruct[5];
  instruct[0] = 'D';
  instruct[1] = (char)delayVal;
  instruct[2] = (char)dataLen;
  instruct[3] = 0;
  instruct[4] = 0;
  int check = my_write(ftHandle, instruct, 5);
  if (check != 1) {
    std::cerr<<"Failed Setting Damp Data Length!"<<std::endl;
    goto exit;
  }
  return 1;

  exit:
  if (ftHandle != NULL)
  FT_Close(ftHandle);
  exit(-1);
  return -1;
}

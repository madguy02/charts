#ifndef SYS_CONFIG_H
#define SYS_CONFIG_H

#include "../lib/ftd2xx.h"
#include "technofour_config.h"
#include <iostream>
#include <stdio.h>

class SysConfig {
public:
  int initialize(FT_HANDLE ftHandle);

private:
  int my_write(FT_HANDLE ftHandle, char testPattern[], DWORD bytesToWrite);
  int DelayRange(FT_HANDLE ftHandle,double nDelayMS, double nRangeMS, double nZeroMS, int nVelocity);
  double setGain(FT_HANDLE ftHandle, double gainVal);
  int setTrigger(FT_HANDLE ftHandle, int tMode, int autoPrr, int prrRate);
  int setPulse(FT_HANDLE ftHandle, int pulseType, double probeFreq, int vPulse,int nCycles, int dampEn);
  int setSettings(FT_HANDLE ftHandle, int prType, int sampleRate, int lpf,int hpf, int nAvg);
  int setDampDataLen(FT_HANDLE ftHandle, int delayVal, int dataLen);
};

#endif

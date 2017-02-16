#!/bin/bash
g++ -o calib calib.cpp technofour.cpp technofour_config.cpp FFT.cpp Filter.cpp GNUPlot.cpp Peaks.cpp SysConfig.cpp -Wall -Wextra -L. -lftd2xx -lpthread -lfftw3 -ldl -lrt -Wl,-rpath /usr/local/lib -std=c++11 -g

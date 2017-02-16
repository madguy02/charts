#!/bin/bash
g++ -o tech test.cpp SysConfig.cpp technofour.cpp technofour_config.cpp FFT.cpp Filter.cpp GNUPlot.cpp Peaks.cpp -Wall -Wextra -L. -lftd2xx -lpthread -lfftw3 -ldl -lrt -Wl,-rpath /usr/local/lib -std=c++11 -g

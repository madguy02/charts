#ifndef TECHNOFOUR_CONFIG_H
#define TECHNOFOUR_CONFIG_H

#include <math.h>

#define MIN_DAC_GAIN 20.0 // GAIN Parameters
#define MAX_DAC_GAIN 100.0
#define PA_GAIN 20.0
#define BOOST_GAIN 16.0
#define MAX_GAIN (PA_GAIN + MAX_DAC_GAIN + BOOST_GAIN)
#define UT_CONSTANT 12.276

extern double time_constant;

// Trigger
#define TRIGGER_OFF 1
#define TRIGGER_INT 2 // Trigger On Off and External
#define TRIGGER_EXT 4

extern int TRIGGER_STATUS;

#define AUTO_PRR_OFF 0
#define AUTO_PRR_ON 1

#define MIN_PRR 40
#define MAX_PRR 2000

#define MANUAL_PRR_VALUE 60

extern int PRR_RATE;

#define AUTO_PRR 100 // Pulse Repetetion Rate

// Pulse
#define PULSE_BIPOLAR 0
#define PULSE_UNIPOLAR 1 // Pulse Type
#define PULSE_SPIKE 2

extern int PULSE_TYPE;

#define CYCLE_1 1
#define CYCLE_2 2
#define CYCLE_4 4
#define CYCLE_8 8 // Number of Cycles

// Choose Number of cycles
extern int NO_OF_CYCLES;

#define VOL_40 0
#define VOL_70 4
#define VOL_100 5 // Voltage
#define VOL_150 6
#define VOL_200 7

// Choose Voltage of the pulseType
extern int VOLTAGE;

// Settings
#define PROBE_PULSE_ECHO 0
#define PROBE_TRANSMISSION 1 // Probe Type

extern int PROBE_TYPE;

#define SAMPLE_RATE_100MHz 1
#define SAMPLE_RATE_50MHz 2
#define SAMPLE_RATE_25MHz 4 // Sample Rate
#define SAMPLE_RATE_12_5MHz 8

// Choose Sample Rate
extern int SAMPLE_RATE;

#define LOW_PF_27MHz 0
#define LOW_PF_16MHz 7
#define LOW_PF_10MHz 6
#define LOW_PF_06MHz 5 // Low Pass Filter frequency
#define LOW_PF_04MHz 4

// Choose Low pass filter frequency
extern int LOW_PASS_FREQUENCY;

#define HIGH_PF_500kHz 0
#define HIGH_PF_1MHz 1
#define HIGH_PF_2MHz 2
#define HIGH_PF_4MHz 3 // High Pass Filter frequency

// Choose High pass filter frequency
extern int HIGH_PASS_FREQUENCY;

// Data Length
#define DATALEN_256B 0
#define DATALEN_512B 1
#define DATALEN_1024B 2
#define DATALEN_2048B 3
#define DATALEN_4096B 4
#define DATALEN_8192B 5 // Data Length
#define DATALEN_16kB 6
#define DATALEN_32kB 7
#define DATALEN_64kB 8
#define DATALEN_128kB 9

extern int DATA_LENGTH;

// Average number of A scans
#define T4UTUSB_AVG_1 0
#define T4UTUSB_AVG_2 1
#define T4UTUSB_AVG_4 2 // Average setting
#define T4UTUSB_AVG_8 3
#define T4UTUSB_AVG_16 4
#define T4UTUSB_AVG_32 5

#define SOUND_VELOCITY 5720.0

#define ABSOLUTE 1
#define REAL 2

#define BUF_SIZE 128 * 1024

extern int AVG_A_SCANS;

extern int baudRate;

extern double PROBE_FREQUENCY; // Probe Frequency

extern int GAIN_VALUE; // Gain setting

extern double VELOCITY;

extern double VELOCITY_OF_SOUND_IN_WATER;

extern int PEAK_WINDOW_SIZE;

extern int NOISE_THRESHOLD;

extern double CONTACT_LC;

extern double NON_CONTACT_LC;

#endif

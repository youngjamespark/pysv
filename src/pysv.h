#ifndef __PYSV_MODULE_H__
#define __PYSV_MODULE_H__
#include "sv-p56.h"

/* State for speech voltmeter function */
typedef struct {
    float f;                      /* sampling frequency, in Hz */
    unsigned long a[15];          /* activity count */
    double c[15];                 /* threshold level; 15 is the no.of thres. */
    unsigned long hang[15];       /* hangover count */
    unsigned long n;              /* number of samples read since last reset */
    double s;                     /* sum of all samples since last reset */
    double sq;                    /* squared sum of samples since last reset */
    double p;                     /* intermediate quantities */
    double q;                     /* envelope */
    double max;                   /* max absolute value found since last reset */
    double refdB;                 /* 0 dB reference point, in [dB] */
    double rmsdB;                 /* rms value found since last reset */
    double maxP, maxN;            /* maximum pos/neg.values since last reset */
    double DClevel;               /* average level since last reset */
    double ActivityFactor;        /* Activity factor since last reset */
    double ActiveSpeechLevel;     /* Active speech level */
    double rmsPkF;                /* rms peak factor */
    double ActPkF;                /* Active peak factor */
    double Gain;                  /* equalization factor to be used in the output file */
} pysv_state;

pysv_state calculate(char *FileIn);
pysv_state normalize(char *FileIn, char *FileOut, double targetdB);
int samplerate_change(char *FileIn, char *FileOut, int out_samplerate);

#endif // __PYSV_MODULE_H__
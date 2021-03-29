#include <stdio.h>
#include <math.h>

#include "pysv.h"
#include "sv-p56.h"
#include "sv56.h"


pysv_state calculate(char *FileIn)
{
    pysv_state state;
    SVP56_state sv_state;

    actlevel(FileIn, &sv_state);

    // print_act_short_summary(stderr, FileIn, &sv_state, sv_state.ActiveSpeechLevel, ActiveLeveldB, Overflow, gain);
    // fprintf(stderr, "FIle: \t%s\n", FileIn);
    // fprintf(stderr, "Samples: %5ld\n", sv_state.n);
    // fprintf(stderr, "Min: %5.0f\n", sv_state.maxN);
    // fprintf(stderr, "Max: %5.0f\n", sv_state.maxP);
    // fprintf(stderr, "DC: %7.2f\n", sv_state.DClevel);
    // fprintf(stderr, "RMSLev[dB]: %7.3f\n", sv_state.rmsdB);
    // fprintf(stderr, "ActLev[dB]: %7.3f\n", sv_state.ActiveSpeechLevel);
    // fprintf(stderr, "%%Active: %7.3f\n", sv_state.ActivityFactor);
    // fprintf(stderr, "RMSPkF[dB]: %7.3f\n", sv_state.rmsPkF);
    // fprintf(stderr, "ActPkF[dB]: %7.3f\n", sv_state.ActPkF);
    // fprintf(stderr, "Gain[]: %7.3f\n", sv_state.Gain);

    state.f = sv_state.f;
    state.n = sv_state.n;
    state.s = sv_state.s;
    state.sq = sv_state.sq;
    state.p = sv_state.p;
    state.q = sv_state.q;
    state.max = sv_state.max;
    state.refdB = sv_state.refdB;
    state.rmsdB = sv_state.rmsdB;
    state.maxN = sv_state.maxN;
    state.maxP = sv_state.maxP;
    state.DClevel = sv_state.DClevel;
    state.ActivityFactor = sv_state.ActivityFactor;
    state.ActiveSpeechLevel = sv_state.ActiveSpeechLevel;
    state.rmsPkF = sv_state.rmsPkF;
    state.ActPkF = sv_state.ActPkF;

    return state;
}

pysv_state normalize(char *FileIn, char *FileOut, double targetdB)
{
    pysv_state state;
    SVP56_state sv_state;

    sv56demo(FileIn, FileOut, targetdB);

    actlevel(FileOut, &sv_state);
    state.f = sv_state.f;
    state.n = sv_state.n;
    state.s = sv_state.s;
    state.sq = sv_state.sq;
    state.p = sv_state.p;
    state.q = sv_state.q;
    state.max = sv_state.max;
    state.refdB = sv_state.refdB;
    state.rmsdB = sv_state.rmsdB;
    state.maxN = sv_state.maxN;
    state.maxP = sv_state.maxP;
    state.DClevel = sv_state.DClevel;
    state.ActivityFactor = sv_state.ActivityFactor;
    state.ActiveSpeechLevel = sv_state.ActiveSpeechLevel;
    state.rmsPkF = sv_state.rmsPkF;
    state.ActPkF = sv_state.ActPkF;

    return state;
}

int samplerate_change(char *FileIn, char *FileOut, int out_samplerate)
{
    ssrc(FileIn, FileOut, out_samplerate);
}
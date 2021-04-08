/*                                                              V2.4 10.Jun.98
  ============================================================================

  ACTLEVEL.C
  ~~~~~~~~~~

  Description:
  ~~~~~~~~~~~~

  Example program that calculates the active speech level of a file
  relative to a 0 dB reference. Here, the default is 0 dB below
  system overload.

  The calculation of speech file's active power uses the algorithm
  in ITU-T Recommendation P.56, and as a by-product of
  this mudule, also  calculates the activity [%], RMS power,
  positive and negative peaks for the file, and peak factors.

  In general, input files are in integer representation,
  16-bit words, 2's complement. In UGST convention, this data must be
  left-adjusted, RATHER than right-adjusted. Since the speech voltmeter
  uses `float' input data, it is necessary to convert from short (in the
  mentioned format) to float; this is carried out by the function
  `sh2fl()'. This function has the option of `normalize' the input data
  to the range -1..+1, what is done.

  The default values for the AD,DA systems resolution is 16 bits, for
  the sampling rate is 16000 Hz. To change this on-line, just specify
  the proper options (see below) in the command line. For example, 14
  bits and 8000 Hz:

  $ actlevel -blk 256 -star 1 -n 100 -fs 8000 -bits 14 filein

  Usage:
  ~~~~~~
  $ actlevel [-?] (displays the help message message!)
  or
  $ actlevel [-options] file [file ...]
  where:
  file ....... is the input file to be measured; if not specified, no actions
               are taken.

  Options:
  ~~~~~~~~
  -blk len  .. is the block size in number of samples;
               this parameter is optional, and the default is block size
           of 256 samples;
  -start sb .. define `sb' as the first block to be measured
               [default: first block of the file]
  -n nb ...... define `nb' as the number of blocks to be measured
               [default: whole file]
  -end eb .... define `eb' as the last block to be measured
               [default: till end of file]
  -sf f ...... sets the sampling rate to `f' Hz [default: 16000Hz]
  -bits n .... sets the digital system resolution (AD,DA systems) to `n',
               in number of bits [default: 16 bits]
  -lev ndB ... causes the program to CALCULATE the gain necessary to
               equalize the *active* level of the input file(s) to
               ndB. However, the program will NOT level-equalize the file.
  -rms ....... when -lev is selected, reports the gain necessary to be
               applied to the input file(s) to normalizes to the long
               term level, instead of the active speech level.
  -log file .. print the statistics log into file rather than stdout
  -q ......... quiet operation; don't print progress flag, results are
               printed all in one line.


  Modules used:
  ~~~~~~~~~~~~~
  > sv-P56.c:   contains the functions related to active speech
            level measurement according to P.56,
            init_speech_voltmeter(), speech_voltmeter() and
            bin_interp(). Their prototypesare in `sv-p56.h'.
  > ugst-utl.c: utility functions; here are used the gain/loss
            (scaling) algorithm of scale() and the data type
        conversion functions sh2fl() and fl2sh(). Prototypes
        are in `ugst-utl.h'.

  Exit values:
  ~~~~~~~~~~~~
  0      success (all but VMS);
  1      success (only in VMS);
  2      error opening input file;
  3      error creating output file;
  4      error moving pointer to desired start of conversion;
  5      error reading input file;
  6      error writing to file;

  Compilation:
  ~~~~~~~~~~~~
  VaxC:   cc actlevel.c
          link actlevel
  TurboC: tcc actlevel.c sv-p56.c ugst-utl.c
  Sun-OS: cc -o actlevel actlevel.c sv-p56.c ugst-utl.c -lm
  gcc:    cc -o actlevel actlevel.c sv-p56.c ugst-utl.c -lm

  Author:
  ~~~~~~~
  Simao Ferraz de Campos Neto
  DDS/Pr11                      Tel: +55-192-39-1396
  CPqD/Telebras                 Fax: +55-192-53-4754
  13085 Campinas SP Brazil      E-mail: <tdsimao@venus.cpqd.ansp.br>

  Log of changes:
  ~~~~~~~~~~~~~~~
  01.Jun.92     1.0        1st version, based on the sv56demo.c version 2.1.
  01.Feb.95     2.0        Added options and multi-file measurement option.
  21.Aug.95     2.1        Fixed bug with zero-sample files.
  06.Dec.96     2.2        Fixed small bug in the calculation of peak
                           factors: the variable holding the absolute
                           maximum in dB was never initialized,
                           causing a wrong peak factor to be reported
                           when the -q option was *not* used (peak
                           factor wasn't reported with -q) . Took the
                           advantage to add the reporting of the peak
                           factor when option -q is used. Updated
                           display_usage().
  06.Apr.98     2.3        Solved small bug that occurred when the file size
                           was not a multiple of the frame size. The
                           program was truncating the output file size
                           to a multiple of the current block
                           size. Solved also a bug with long file
                           names which were causing memory invasion by
                           increasing max file name length to 150
                           characters and changing strcpy() to
                           strncpy() in the filename copy process.
                           <simao>
  ============================================================================
*/
#define _CRT_SECURE_NO_WARNINGS

/* ... Include for all UGST demos -- should be the 1st include ... */
#include "ugstdemo.h"

/* ... Includes in general ... */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>             /* for strstr() */
#include <math.h>

/* ... Includes for O.S. specific headers ... */
#if defined(MSDOS)
#include <fcntl.h>
#include <io.h>
#include <sys\stat.h>
#elif defined(VMS)
#include <perror.h>
#include <file.h>
#include <stat.h>
#else
#include <sys/stat.h>
#endif

/* ... Include of speech-voltmeter-related routines ... */
#include "sv-p56.h"

/* ... Include of utilities ... */
#include "ugst-utl.h"

/* ... Include of wav header informations ... */
#include "sv56.h"

/* ... Local definitions ... */
#define DEF_BLK_LEN 256         /* samples per block */
#define MIN_LOG_OFFSET 1.0e-20  /* To avoid sigularity with log(0.0) */

/*
  ============================================================================

        void display_usage (void)
        ~~~~~~~~~~~~~~~~~~

        Display usage information and quit program.

        Parameter:
        ~~~~~~~~~~
        None.

        Returns
        ~~~~~~~
        To calling program, nothing; to calling environment, returns OK
        (1 in VAX/VMS or 0, otherwise).

        Original author
        ~~~~~~~~~~~~~~~
        simao@ctd.comsat.com

        Log of changes
        ~~~~~~~~~~~~~~
        01.Feb.94	v1.0	Creation.

  ============================================================================
*/
#ifdef ACTLEVEL
void display_usage() {
    printf("ACTLEVEL.C - Version 2.0 of 01/Feb/1995 \n");
    printf(" Calculate the active speech level of a file, relative to the\n");
    printf(" system overload point [dBov], using the P.56 algorithm.\n");
    printf(" Reports positive and negative peaks, RMS and active level, \n");
    printf(" activity factor [%%], and long-term and active peak factors \n");
    printf(" for the file\n");
    printf("\n");
    printf(" Usage:\n");
    printf("  $ actlevel [-options] file [file ...]\n");
    printf(" where:\n");
    printf("  file ....... is the input file(s) to be measured\n");
    printf(" Options: \n");
    printf("  -blk len  .. is the block size in number of samples;\n");
    printf("               this parameter is optional, and the default is  \n");
    printf("               block size of 256 samples;\n");
    printf("  -start sb .. define `sb' as the first block to be measured \n");
    printf("               [default: first block of the file] \n");
    printf("  -end eb .... define `eb' as the last block to be measured\n");
    printf("  -n nb ...... define `nb' as the number of blocks to be measured \n");
    printf("               [default: whole file]\n");
    printf("  -sf f ...... sets the sampling rate to `f' Hz [default: 16000Hz]\n");
    printf("  -bits n .... sets the digital system resolution (AD,DA systems) \n");
    printf("               to `n' bits [default: 16 bits]\n");
    printf("  -lev ndB ... CALCULATES the gain necessary to equalize the\n");
    printf("               *active* level of the input file(s) to ndB.\n");
    printf("               The program will NOT level-equalize the file.\n");
    printf("  -rms ....... when -lev is selected, reports the gain necessary\n");
    printf("               to normalizes to the long term level, instead of the\n");
    printf("               active speech level. Does NOT change the file(s).\n");
    printf("  -log file ... log statistics into file rather than stdout\n");
    printf("  -q ......... quiet operation; don't print progress flag, results\n");
    printf("               are printed all in one line.\n");

    /* Quit program */
    exit(-128);
}
#endif // ACTLEVEL
/* ....................... End of display_usage() .......................... */


/*
  ============================================================================

       void print_act_long_summary(char *file, SVP56_state state, double al_dB,
       ~~~~~~~~~~~~~~~~~~~~~~~~~~~ double NdB, double gain, double ratio,
                                   long N, long N1, long N2, long bitno);

       Print full summary of P.56 statistics

       Parameter:
       ~~~~~~~~~~
       file ..... File name
       state .... P.56 state variable
       al_dB .... active level in dB
       NdB ...... desired dB level for output
       gain ..... gain to equalize file to desired level
       ratio .... ratio of maximum number representable in the system (based on
                  the number of bits, or resolution, of the input speech file)
          to the range of the input signal. E.g., for a 16 bit system
          with a +-1 signal range, ratio = 32760 / 1.0.
       N ........ number of samples per block
       N1 ....... number of first block processed in the file
       N2 ....... total number of blocks processed from the file
       bitno .... number of bits per sample (input signal resolution)

       Returns
       ~~~~~~~
       None

       Original author
       ~~~~~~~~~~~~~~~
       simao@ctd.comsat.com

       Log of changes
       ~~~~~~~~~~~~~~
       01.Nov.96	v1.0	Creation.

  ============================================================================
*/
void print_act_long_summary(FILE* out, char* file, SVP56_state state, double al_dB, double NdB, double ratio, double gain, long N, long N1, long N2, long bitno) {
    static char unity[5] = "dBov";
    double abs_max_dB, max_gain, new_max;

    /* Calculate peak in dB */
    abs_max_dB = 20 * log10(SVP56_get_abs_max(state) + MIN_LOG_OFFSET)
        - state.refdB;

    /* Computes the maximum gain that will not cause saturation */
    max_gain = al_dB - 20 * log10(SVP56_get_abs_max(state));

    /* Print the relevant messages */
    // fprintf (out, "%s%s", " ---------------------------", "----------------------------");
    fprintf(out, "\n  Input file: ................... %s, ", file);
    fprintf(out, "%2ld bits, fs=%5.0f Hz", bitno, state.f);
    fprintf(out, "\n  Block Length: ................. %7ld [samples]", N);
    fprintf(out, "\n  Starting Block: ............... %7ld []", N1 + 1);
    fprintf(out, "\n  Number of Blocks: ............. %7ld []", N2);
    if (gain > 0)
        fprintf(out, "\n  %s desired for output: ...... %7.3f [%s]", unity, NdB, unity);

    /* Skip if filesize is zero */
    if (state.n == 0) {
        fprintf(out, "%s%s", "\n -***-----------------------", "----------------------------\n");
        return;
    }

    /* If the activity factor is 0, don't report many things */
    if (SVP56_get_activity(state) == 0) {
        fprintf(out, "\n  Activity factor is ZERO -- the file is silence");
        fprintf(out, " or idle noise");
        fprintf(out, "%s%s", "\n ---------------------------", "----------------------------");
        fprintf(out, "\n  DC level: ..................... %7.0f [PCM]", ratio * SVP56_get_DC_level(state));
        fprintf(out, "\n  Maximum positive value: ....... %7.0f [PCM]", ratio * SVP56_get_pos_max(state));
        fprintf(out, "\n  Maximum negative value: ....... %7.0f [PCM]", ratio * SVP56_get_neg_max(state));
        fprintf(out, "%s%s", "\n ---------------------------", "----------------------------");
        fprintf(out, "\n  Noise/silence energy (rms): ... %7.3f [dB]", SVP56_get_rms_dB(state));
    }
    else {
        if (gain > 0) {
            fprintf(out, "\n  Norm factor desired is: ....... %7.3f [times]", gain);
            fprintf(out, "\n  Max norm WITHOUT saturation: .. %7.3f [%s]", max_gain, unity);
        }

        fprintf(out, "%s%s", "\n ---------------------------", "----------------------------");
        fprintf(out, "\n  DC level: ..................... %7.0f [PCM]", ratio * SVP56_get_DC_level(state));
        fprintf(out, "\n  Maximum positive value: ....... %7.0f [PCM]", ratio * SVP56_get_pos_max(state));
        fprintf(out, "\n  Maximum negative value: ....... %7.0f [PCM]", ratio * SVP56_get_neg_max(state));
        fprintf(out, "%s%s", "\n ---------------------------", "----------------------------");
        fprintf(out, "\n  Long term energy (rms): ....... %7.3f [%s]", SVP56_get_rms_dB(state), unity);
        fprintf(out, "\n  Active speech level: .......... %7.3f [%s]", al_dB, unity);
        fprintf(out, "\n  RMS peak-factor found: ........ %7.3f [dB]", abs_max_dB - SVP56_get_rms_dB(state));
        fprintf(out, "\n  Active peak factor found: ..... %7.3f [dB]", abs_max_dB - al_dB);
        fprintf(out, "\n  Activity factor: .............. %7.3f [%%]", SVP56_get_activity(state));
    }
    fprintf(out, "%s%s", "\n ---------------------------", "----------------------------");

    /* Tests if the equalization will cause saturation */
    new_max = SVP56_get_abs_max(state) * gain;
    if (gain > 0 && new_max > 1.0) {
        /* Print message */
        fprintf(out, "\n%%SV-W-SAT, the dB level chosen causes SATURATION: ");
        fprintf(out, "old max=%5.0f; new max=%6.0f", ratio * SVP56_get_abs_max(state), ratio * new_max);
        fprintf(out, "\n%%SV-I-MAXLEVDB, the maximum norm factor ");
        fprintf(out, "to PREVENT clipping is %7.3fdB; ", max_gain);
        fprintf(out, "\n -------------------------------------------------------");
    }
}

/* .................... End of print_act_long_summary() ................... */


/*
  ============================================================================

       void print_act_short_summary (char *file, SVP56_state state,
       ~~~~~~~~~~~~~~~~~~~~~~~~  double al_dB, double ratio);

       Print one-line summary of P.56 statistics

       Parameter:
       ~~~~~~~~~~
       file ..... File name
       state .... P.56 state variable
       al_dB .... active level in dB
       ratio .... ratio of maximum number representable in the system (based on
                  the number of bits, or resolution, of the input speech file)
          to the range of the input signal. E.g., for a 16 bit system
          with a +-1 signal range, ratio = 32760 / 1.0.

       Returns
       ~~~~~~~
       None

       Original author
       ~~~~~~~~~~~~~~~
       simao@ctd.comsat.com

       Log of changes
       ~~~~~~~~~~~~~~
       01.Nov.96	v1.0	Creation.

  ============================================================================
*/
void print_act_short_summary(FILE* out, char* file, SVP56_state state, double al_dB, double ratio, double gain) {
    double abs_max_dB;

    /* Calculate peak in dB */
    abs_max_dB = 20 * log10(SVP56_get_abs_max(state) + MIN_LOG_OFFSET)
        - state.refdB;

    /* Report number of samples */
    fprintf(out, "Samples: %5ld, ", state.n);

    /* Skip if filesize is zero */
    if (state.n == 0) {
        fprintf(out, "%s%s%s%s", "Min: ------ Max: ----- DC: ------- ", "RMSLev[dB]: ------- ActLev[dB]: ------- %Active:  ------ ", "RMSPkF[dB]: ------- ActPkF[dB]: -------", gain > 0 ? "Gain:  ------ " : "");
    }
    else {
        fprintf(out, "Min: %5.0f, ", ratio * state.maxN);
        fprintf(out, "Max: %5.0f, ", ratio * state.maxP);
        fprintf(out, "DC: %7.2f, ", ratio * state.DClevel);
        fprintf(out, "RMSLev[dB]: %7.3f, ", state.rmsdB);
        fprintf(out, "ActLev[dB]: %7.3f, ", al_dB);
        fprintf(out, "%%Active: %7.3f, ", state.ActivityFactor * 100);
        fprintf(out, "RMSPkF[dB]: %7.3f, ", abs_max_dB - SVP56_get_rms_dB(state));
        fprintf(out, "ActPkF[dB]: %7.3f, ", abs_max_dB - al_dB);
        if (gain > 0)
            fprintf(out, " Gain[]: %7.3f, ", gain);
    }
    fprintf(out, "\t%s\n", file);
}

/* ................... End of print_act_short_summary() .................... */


/*
   **************************************************************************
   ***                                                                    ***
   ***        Demo-Program for testing the correct implementation         ***
   ***               and to show how to use the programs                  ***
   ***                                                                    ***
   **************************************************************************
*/

#if ACTLEVEL

int main(int argc, char* argv[])
{
    /* Parameters for operation */
    long N = DEF_BLK_LEN, N1 = 1, N2 = 0;

    /* Intermediate storage variables for speech voltmeter */
    SVP56_state sv_state;
#ifdef LOCAL_PRINT
    double abs_max_dB;
#endif

    /* File-related variables */
    char FileIn[150];
    FILE* out = stdout;           /* where to print the statistical results */
#ifdef VMS
    char mrs[15];
#endif

    /* Other variables */
    long bitno = 16;
    double sf = 16000;            /* Hz */
    double level = 0, gain = 0;
    static char funny[] = "|/-\\|/-\\", funny_size = sizeof(funny), quiet = 0;
#ifdef LOCAL_PRINT
    static char unity[5] = "dBov";
#endif
    char use_active_level = 1;
    //pysv_state state;

    if (argc < 2)
        display_usage();
    else {
        while (argc > 1 && argv[1][0] == '-')
            if (strcmp(argv[1], "-sf") == 0) {
                /* Change default sampling frequency */
                sf = atof(argv[2]);

                /* Update argc/argv to next valid option/argument */
                argv += 2;
                argc -= 2;
            }
            else if (strcmp(argv[1], "-lev") == 0) {
                /* Set a desired level */
                level = atof(argv[2]);

                /* Update argc/argv to next valid option/argument */
                argv += 2;
                argc -= 2;
            }
            else if (strcmp(argv[1], "-rms") == 0) {
                /* Use the RMS level to normalize instead of the active level */
                use_active_level = 0;

                /* Update argc/argv to next valid option/argument */
                argv++;
                argc--;
            }
            else if (strcmp(argv[1], "-bits") == 0) {
                /* Change default sampling frequency */
                bitno = atoi(argv[2]);

                /* Update argc/argv to next valid option/argument */
                argv += 2;
                argc -= 2;
            }
            else if (strcmp(argv[1], "-blk") == 0) {
                /* Change default sampling frequency */
                N = atoi(argv[2]);

                /* Update argc/argv to next valid option/argument */
                argv += 2;
                argc -= 2;
            }
            else if (strcmp(argv[1], "-start") == 0) {
                /* Change default sampling frequency */
                N1 = atoi(argv[2]);

                /* Update argc/argv to next valid option/argument */
                argv += 2;
                argc -= 2;
            }
            else if (strcmp(argv[1], "-n") == 0) {
                /* Change default sampling frequency */
                N2 = atoi(argv[2]);

                /* Update argc/argv to next valid option/argument */
                argv += 2;
                argc -= 2;
            }
            else if (strcmp(argv[1], "-end") == 0) {
                /* Change number of blocks based on the last block */
                N2 = atol(argv[2]) - N1 + 1;

                /* Update argc/argv to next valid option/argument */
                argv += 2;
                argc -= 2;
            }
            else if (strcmp(argv[1], "-log") == 0) {
                /* Log statistics into a file */
                if ((out = fopen(argv[2], WT)) == NULL)
                    KILL(argv[2], 2);
                else
                    fprintf(stderr, "Statistics will be logged in %s\n", argv[2]);

                /* Update argc/argv to next valid option/argument */
                argv += 2;
                argc -= 2;
            }
            else if (strcmp(argv[1], "-q") == 0) {
                /* Don't print progress indicator */
                quiet = 1;

                /* Move argv over the option to the next argument */
                argv++;
                argc--;
            }
            else if (strcmp(argv[1], "--") == 0) {
                /* No more options: */
                /* Move argv over the option to the next argument and quit loop */
                argv++;
                argc--;
                break;
            }
            else if (strcmp(argv[1], "-?") == 0 || strstr(argv[1], "-help")) {
                /* Print help */
                display_usage();
            }
            else {
                fprintf(stderr, "ERROR! Invalid option \"%s\" in command line\n\n", argv[1]);
                display_usage();
            }
    }

    /* Get new file name and update argument line pointer/counter */
    while (argc > 1) {
        /* Get new file name and update argument line pointer/counter */
        printf("%s\n", argv[1]);
        strncpy(FileIn, argv[1], sizeof(FileIn));
        argv++;
        argc--;
    }

    actlevel(FileIn, &sv_state);

    fprintf(stderr, "FIle: \t%s\n", FileIn);
    fprintf(stderr, "Samples: %5ld\n", sv_state.n);
    fprintf(stderr, "Min: %5.0f\n", sv_state.maxN);
    fprintf(stderr, "Max: %5.0f\n", sv_state.maxP);
    fprintf(stderr, "DC: %7.2f\n", sv_state.DClevel);
    fprintf(stderr, "RMSLev[dB]: %7.3f\n", sv_state.rmsdB);
    fprintf(stderr, "ActLev[dB]: %7.3f\n", sv_state.ActiveSpeechLevel);
    fprintf(stderr, "%%Active: %7.3f\n", sv_state.ActivityFactor);
    fprintf(stderr, "RMSPkF[dB]: %7.3f\n", sv_state.rmsPkF);
    fprintf(stderr, "ActPkF[dB]: %7.3f\n", sv_state.ActPkF);
    fprintf(stderr, "Gain[]: %7.3f\n", sv_state.Gain);

    //state.f = sv_state.f;
    //state.n = sv_state.n;
    //state.s = sv_state.s;
    //state.sq = sv_state.sq;
    //state.p = sv_state.p;
    //state.q = sv_state.q;
    //state.max = sv_state.max;
    //state.refdB = sv_state.refdB;
    //state.rmsdB = sv_state.rmsdB;
    //state.maxN = sv_state.maxN;
    //state.maxP = sv_state.maxP;
    //state.DClevel = sv_state.DClevel;
    //state.ActivityFactor = sv_state.ActivityFactor;
    //state.ActiveSpeechLevel = sv_state.ActiveSpeechLevel;
    //state.rmsPkF = sv_state.rmsPkF;
    //state.ActPkF = sv_state.ActPkF;

    //return state;
    return 0;
}
#endif

int actlevel(char* FileIn, SVP56_state* sv_state)
{
    /* Parameters for operation */
    double Overflow;              /* Max.positive value for AD_resolution bits */
    long N = DEF_BLK_LEN, N1 = 1, N2 = 0, i, l;

    wav_header header;
    int header_offset = 0;

    /* Intermediate storage variables for speech voltmeter */
    SVP56_state state;

    /* File-related variables */
    //char FileIn[256];
    FILE* Fi;                     /* input file pointer */
    FILE* out = stdout;           /* where to print the statistical results */
#ifdef VMS
    char mrs[15];
#endif

    /* Other variables */
    short buffer[4096];
    float Buf[4096];
    long bitno = 16;
    double sf = 16000;            /* Hz */
    double ActiveLeveldB, level = 0, gain = 0;
    char use_active_level = 1;
    int name_len;

    char FileLog[256] = "log.txt";

    if ((out = fopen(FileLog, "at")) == NULL) {
        fprintf(stderr, "log file open error.\n");
        return -1;
    }

    /* ......... SOME INITIALIZATIONS ......... */
    //start_byte = --N1;
    //start_byte *= N * sizeof(short);
    //N2_ori = N2;

    /* Overflow (saturation) point */
    Overflow = pow((double)2.0, (double)(bitno - 1));

    /* Reset variables for speech level measurements */
    init_speech_voltmeter(&state, sf);

    /* check file extension */
    /* Read wave header and offset */
    name_len = strlen(FileIn);
    if (name_len > 4)
    {
        if (strcmp(FileIn + name_len - 4, ".wav") == 0)
            header_offset = wav_header_read(FileIn, &header);
        if (strcmp(FileIn + name_len - 4, ".WAV") == 0)
            header_offset = wav_header_read(FileIn, &header);
        if (strcmp(FileIn + name_len - 4, ".PCM") == 0) {
            header_offset = 0;
            struct stat st;
            stat(FileIn, &st);
            header.data_bytes = st.st_size;
        }
        if (strcmp(FileIn + name_len - 4, ".pcm") == 0) {
            header_offset = 0;
            struct stat st;
            stat(FileIn, &st);
            header.data_bytes = st.st_size;
        }
    }

    /* ......... FILE PREPARATION ......... */

    /* Opening input file; abort if there's any problem */
#ifdef VMS
    sprintf(mrs, "mrs=%d", 2 * N);
#endif
    if ((Fi = fopen(FileIn, "rb")) == NULL)
        KILL(FileIn, 2);

    /* Initialize number of blocks to all samples */
    N2 = (long) ceil(header.data_bytes / (double)(N * sizeof(short)));

    /* Move pointer after wave header */
    if (fseek(Fi, header_offset, 0) < 0l)
        KILL(FileIn, 4);

    /* ... MEASUREMENT OF ACTIVE SPEECH LEVEL ACCORDING P.56 ... */
    int total_samples = header.data_bytes / 2;
    int no_samples = 0;

    /* Read samples ... */
    for (i = 0; i < (N2 - 1); i++) {
        if ((l = fread(buffer, sizeof(short), N, Fi)) > 0) {
            /* ... Convert samples to float */
            sh2fl((long)l, buffer, Buf, bitno, 1);

            /* ... Get the active level */
            ActiveLeveldB = speech_voltmeter(Buf, (long)l, &state);
            total_samples -= l;
            no_samples += l;
        }
        else {
            KILL(FileIn, 5);
        }
    }
    // for the last block samples, no < N
    if ((l = fread(buffer, sizeof(short), total_samples, Fi)) > 0) {
        /* ... Convert samples to float */
        sh2fl((long)l, buffer, Buf, bitno, 1);

        /* ... Get the active level */
        ActiveLeveldB = speech_voltmeter(Buf, (long)l, &state);
        total_samples -= l;
        no_samples += l;
    }
    else {
        KILL(FileIn, 5);
    }

    if (level != 0) {
        /* Computes the equalization factor to be used in the output file */
        if (use_active_level)
            gain = pow(10.0, (level - ActiveLeveldB) / 20.0);
        else
            gain = pow(10.0, (level - SVP56_get_rms_dB(state)) / 20.0);
    }

    print_act_short_summary(out, FileIn, state, ActiveLeveldB, Overflow, gain);

    double abs_max_dB;
    abs_max_dB = 20 * log10(SVP56_get_abs_max(state) + MIN_LOG_OFFSET);

    sv_state->maxN = Overflow * state.maxN;
    sv_state->maxP = Overflow * state.maxP;
    sv_state->DClevel = Overflow * state.DClevel;
    sv_state->rmsdB = state.rmsdB;
    sv_state->ActiveSpeechLevel = ActiveLeveldB;
    sv_state->ActivityFactor = state.ActivityFactor * 100;
    sv_state->rmsPkF = abs_max_dB - SVP56_get_rms_dB(state);
    sv_state->ActPkF = abs_max_dB - ActiveLeveldB;
    sv_state->Gain = gain;
    sv_state->n = state.n;
    /* Close current file */
    fclose(Fi);

    /* FINALIZATIONS */
    /* ... Close log file, if it is the case */
    if (out != stdout)
        fclose(out);

    /* ... Exit cleanly */
#if !defined(VMS)

    return (0);
#endif
        }

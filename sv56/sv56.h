#ifndef __SV56_H__
#define __SV56_H__
#include "sv-p56.h"
#include <stdio.h>

typedef struct wav_header {
    // RIFF Header
    unsigned char riff_header[4];   // Contains "RIFF"
    int wav_size;                   // Size of the wav portion of the file, which follows the first 8 bytes. File size - 8
    unsigned char wave_header[4];   // Contains "WAVE"

    // Format Header
    unsigned char fmt_header[4];    // Contains "fmt " (includes trailing space)
    int fmt_chunk_size;             // Should be 16 for PCM
    short audio_format;             // Should be 1 for PCM. 3 for IEEE Float
    short num_channels;
    int sample_rate;
    int byte_rate;                  // Number of bytes per second. sample_rate * num_channels * Bytes Per Sample
    short sample_alignment;         // num_channels * Bytes Per Sample
    short bit_depth;                // Number of bits per sample
    // Data
    unsigned char data_header[4];   // Contains "data"
    int data_bytes;                 // Number of bytes in data. Number of samples * num_channels * sample byte size
    // uint8_t bytes[];             // Remainder of wave file is bytes
} wav_header;

int wav_header_read(FILE* fp, wav_header* header);
int actlevel(char* FileIn, SVP56_state* sv_state);
int sv56demo(char* FileIn, char* FileOut, double targetdB);
int ssrc(char* sfn, char* dfn, int dfrq);

#endif
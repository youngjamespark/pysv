#define _CRT_SECURE_NO_WARNINGS

#include "sv56.h"
#include <string.h>

#define WAV_HEADER_NOK          -1
#define WAV_HEADER_NOT_PCM      -2
#define WAV_HEADER_NOT_MONO     -3
#define WAV_HEADER_NOT_16BIT    -4

int wav_header_read(char* FileIn, wav_header* header)
{
    FILE* Fi;
    int header_offset = 0;

    /* Parsing wave header information */
    Fi = fopen(FileIn, "rb");
    fseek(Fi, 0, SEEK_SET);
    // "RIFF"
    fread(header->riff_header, sizeof(char), 4, Fi);
    if (strncmp((unsigned char*)header->riff_header, "RIFF", 4) != 0) {
        fprintf(stderr, "not RIFF,%s \n", header->riff_header);
        return WAV_HEADER_NOK;
    }

    // wave size = filesize - 8
    fread(&header->wav_size, sizeof(int), 1, Fi);

    // "WAVE"
    fread(header->wave_header, sizeof(char), 4, Fi);
    if (strncmp((char*)header->wave_header, "WAVE", 4) != 0) {
        fprintf(stderr, "not WAVE\n");
        return WAV_HEADER_NOK;
    }

    // "fmt "
    fread(header->fmt_header, sizeof(char), 4, Fi);
    if (strncmp(header->fmt_header, "fmt ", 4) != 0) {
        fprintf(stderr, "not fmt \n");
        return WAV_HEADER_NOK;
    }

    // chunk size = 16 or 18
    fread(&header->fmt_chunk_size, sizeof(int), 1, Fi);

    // audio format = 1
    fread(&header->audio_format, sizeof(short), 1, Fi);
    if (header->audio_format != 1) {
        fprintf(stderr, "not PCM\n");
        return WAV_HEADER_NOT_PCM;
    }

    // channel number = 1
    fread(&header->num_channels, sizeof(short), 1, Fi);
    if (header->num_channels != 1) {
        fprintf(stderr, "not MONO channel\n");
        return WAV_HEADER_NOT_MONO;
    }

    // sample rate = 16000
    fread(&header->sample_rate, sizeof(int), 1, Fi);
    if (header->sample_rate != 16000) {
        fprintf(stderr, "sample rate = %d, not 16000Hz\n", header->sample_rate);
        return -1;
    }

    // byte rate = 16000 * 2
    fread(&header->byte_rate, sizeof(int), 1, Fi);
    if (header->byte_rate != 32000) {
        fprintf(stderr, "byte rate = %d, not wideband\n", header->byte_rate);
        return -1;
    }

    // sample alignment = 2
    fread(&header->sample_alignment, sizeof(short), 1, Fi);

    // bit depth = 16
    fread(&header->bit_depth, sizeof(short), 1, Fi);
    {
        fprintf(stderr, "not 16 bit data\n");
        return WAV_HEADER_NOT_16BIT;
    }

    // two bytes chunk
    if (header->fmt_chunk_size == 0x12)
        fseek(Fi, 2, SEEK_CUR);

    // "data"
    fread(header->data_header, sizeof(char), 4, Fi);
    if (strncmp(header->data_header, "data", 4) != 0) {
        fprintf(stderr, "not data\n");
        return WAV_HEADER_NOK;
    }

    // data bytes
    fread(&header->data_bytes, sizeof(int), 1, Fi);

    if (header->fmt_chunk_size == 0x12)
        header_offset = sizeof(struct wav_header) + sizeof(short);
    else
        header_offset = sizeof(struct wav_header);

    fclose(Fi);

    return header_offset;
}
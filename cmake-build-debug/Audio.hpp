//
// Created by Birdh on 2025-02-22.
//

#ifndef Audio_HPP
#define Audio_HPP

#include <stdio.h>
#include <fstream>
#include <iostream>
#include <math.h>
#include <cmath>
#include <vector>
#include <cstdint>

class Audio {

public:

    struct WAVHEADER {
        char riff_header[4];
        int32_t wav_size;
        char wave_header[4];

        char fmt_header[4];
        int32_t fmt_chunk_size;
        int16_t audio_format;
        int16_t num_channels;
        int32_t sample_rate;
        int32_t byte_rate;
        int16_t block_align;
        int16_t bits_per_sample;
    };

    typedef struct {
        uint32_t samples;
        int16_t *data;
    } sound_t;


    void write_as_btyes(std::ofstream &file, int value, int byte_size);
    int audioimport();


    static bool read_wav_header(FILE* file, WAVHEADER& header);
    static bool read_in_wav_data(const char *filename, sound_t *sound);
    static bool double_frequency(sound_t* input, sound_t* output);
    static bool output_to_wav(const char *filename, sound_t* sound, WAVHEADER *header);

    void set_sample_rate(int samples) { SampleRate = samples; }
    void set_n_channels(int n) { NumChannels = n; }


    std::pair<short, short> &operator[](unsigned i) { return data[i]; };


private:
    char type[5];
    char format[5];
    char Subchunk1ID[5];
    char Subchunk2ID[5];

    int ChunkSize;
    int Subchunk1Size;
    int SampleRate;
    int ByteRate;
    int Subchunk2Size;

    short AudioFormat;
    short NumChannels;
    short BlockAlign;
    short BitsPerSample;

    // utility
    unsigned NumSamples;

    std::vector<std::pair<short, short> > data;







};













#endif //Audio_HPP

//
// Created by Birdh on 2025-02-24.
//

#ifndef MAIN_HPP
#define MAIN_HPP

#include <fstream>
#include <cstdint>
#include <vector>

class main {

private:

    struct wav_header {
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

    struct sound_t{
        std::vector<int16_t> samples;
        uint32_t sample_rate;
        uint16_t num_channels;
        uint16_t bits_per_sample;
    };

    static bool read_in_wav_data(const char *filename, sound_t *sound);

};

#endif //MAIN_HPP

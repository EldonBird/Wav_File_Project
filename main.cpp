#include <stdio.h>
#include <fstream>
#include <iostream>
#include <math.h>
#include <cmath>
#include <vector>
#include <cstdint>
#include <cstring>

#include "main.hpp"



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

using namespace std;


bool read_in_wav_data(const char *filename, sound_t *sound);
bool output_to_wav(const char *filename, sound_t *sound);

// TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or
// click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.
int main(){

    sound_t sound;

    read_in_wav_data("test.wav", &sound);
    output_to_wav("output.wav", &sound);

    return 1;
}


bool read_in_wav_data(const char *filename, sound_t *sound) {

    ifstream file(filename, ios::binary);

    if (!file.is_open()) {
        std::cerr << "Could not open file " << filename << std::endl;
        return false;
    }

    wav_header header;

    if (!file.read(reinterpret_cast<char*>(&header), sizeof(wav_header))) {
        return false;
    }

    if (strcmp(header.riff_header, "RIFF") ||
        strcmp(header.wave_header, "WAVE") ||
        strcmp(header.fmt_header, "fmt ") ||
        header.audio_format != 1) {

        return false;
    }

    sound->sample_rate = header.sample_rate;
    sound->num_channels = header.num_channels;
    sound->bits_per_sample = header.bits_per_sample;


    char chunk_id[4];
    uint32_t chunk_size;

    while (file.read(chunk_id, 4)) {
        file.read(reinterpret_cast<char*>(&chunk_size), 4);

        if (std::string(chunk_id, 4) == "data") {
            break;
        }

        file.seekg(chunk_size, std::ios::cur);
    }

    size_t num_samples = chunk_size / (header.bits_per_sample / 8);
    sound->samples.resize(num_samples);

    file.read(reinterpret_cast<char*>(sound->samples.data()), chunk_size);

    return true;
}


bool output_to_wav(const char *filename, sound_t *sound) {


    std::ofstream file(filename, ios::binary);
    if (!file.is_open()) {
        std::cerr << "Could not open file " << filename << std::endl;
        return false;
    }


    uint32_t data_size = sound->samples.size() * sizeof(int16_t);
    uint32_t wav_size = 16 + data_size;

    file.write("RIFF", 4);
    file.write(reinterpret_cast<char*>(&wav_size), 4);
    file.write("WAVE", 4);

    file.write("fmt ", 4);

    uint32_t fmt_chunk_size = 16;
    uint16_t format = 1;
    uint32_t byte_per_sample = sound->sample_rate * sound->num_channels * (sound->bits_per_sample / 8);
    uint16_t block_align = sound->num_channels * (sound->bits_per_sample / 8);

    file.write(reinterpret_cast<char*>(&fmt_chunk_size), 4);
    file.write(reinterpret_cast<char*>(&format), 2);
    file.write(reinterpret_cast<char*>(&sound->num_channels), 2);
    file.write(reinterpret_cast<char*>(&sound->sample_rate), 4);
    file.write(reinterpret_cast<char*>(&byte_per_sample), 4);
    file.write(reinterpret_cast<char*>(&block_align), 2);
    file.write(reinterpret_cast<char*>(&sound->bits_per_sample), 2);

    return true;
}

// TIP See CLion help at <a
// href="https://www.jetbrains.com/help/clion/">jetbrains.com/help/clion/</a>.
//  Also, you can try interactive lessons for CLion by selecting
//  'Help | Learn IDE Features' from the main menu.
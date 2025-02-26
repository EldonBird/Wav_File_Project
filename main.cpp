#include <stdio.h>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <math.h>
#include <cmath>
#include <vector>
#include <cstdint>
#include <cstring>

#include "main.hpp"



struct wav_header {

    char riff_header[5];
    int wav_size;
    char wave_header[5];

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


    sound_t *sound = new sound_t();

    if (read_in_wav_data("test.wav", sound)) {
        output_to_wav("output.wav", sound);
    }

    delete sound;
    return 1;
}


bool read_in_wav_data(const char *filename, sound_t *sound) {

    FILE *fp = fopen(filename, "rb");

    wav_header *header = new wav_header;

    if (!fp) {
        std::cerr << "Error opening file " << filename << std::endl;
        return false;
    }


    fread(header->riff_header, sizeof(char), 4, fp);
    if (strncmp(header->riff_header, "RIFF", 4) != 0) {
        throw std::runtime_error("Not a RIFF file!");
        return false;
    }

    fread(&header->wav_size, sizeof(int), 1, fp);
    fread(header->wave_header, sizeof(char), 4, fp);
    if (strncmp(header->wave_header, "WAVE", 4) != 0) {
        std::cerr << "Not a WAVE file!" << std::endl;
        return false;
    }

    fread(header->Subchunk1ID, sizeof(char), 4, fp);
    if (strncmp(header->Subchunk1ID, "fmt ", 4) != 0) {
        std::cerr << "Not the right fmt header" << std::endl;
        return false;
    }

    fread(&header->Subchunk1Size, sizeof(int), 1, fp);
    fread(&header->AudioFormat, sizeof(short), 1, fp);
    fread(&header->NumChannels, sizeof(short), 1, fp);
    fread(&header->SampleRate, sizeof(int), 1, fp);
    fread(&header->ByteRate, sizeof(int), 1, fp);
    fread(&header->BlockAlign, sizeof(short), 1, fp);
    fread(&header->BitsPerSample, sizeof(short), 1, fp);

    fread(header->Subchunk2ID, sizeof(char), 4, fp);
    if (strncmp(header->Subchunk2ID, "data", 4) ==0 ) {
        std::cerr << "Not a data file" << std::endl;
        return false;
    }

    fread(&header->Subchunk2Size, sizeof(int), 1, fp);

    int NumSamples = header->Subchunk2Size / (header->NumChannels*(header->BitsPerSample / 8));

    header->data = std::vector<std::pair<short, short> >(NumSamples);
    for (int i = 0; i < NumSamples; i++) {
        fread(&header->data[i].first, sizeof(short), 1, fp);
        fread(&header->data[i].second, sizeof(short), 1, fp);
    }

    fclose(fp);
    return true;
}

/* bool read_in_wav_data(const char *filename, sound_t *sound) {

    ifstream file(filename, ios::binary);

    if (!file.is_open()) {
        std::cerr << "Could not open file " << filename << std::endl;
        return false;
    }

    wav_header header;

    if (!file.read(reinterpret_cast<char*>(&header), sizeof(wav_header))) {
        std::cerr << "Could not read header properly";
        return false;
    }

    std::cout << "\n riff: " << header.riff_header << "\n wav: " << header.wave_header << "\n fmt: " << header.fmt_header << "\n audio format: " << header.audio_format << "\n channels: " << header.num_channels << "\n sample rate: " << header.sample_rate << std::endl;

    if (strncmp(header.riff_header, "RIFF") ||
        strncmp(header.wave_header, "WAVE") ||
        strncmp(header.fmt_header, "fmt ") ||
        header.audio_format != 1) {

        std::cerr << "Invalid wav file format" << std::endl;
        return false;
    }

    sound->sample_rate = header.sample_rate;
    sound->num_channels = header.num_channels;
    sound->bits_per_sample = header.bits_per_sample;

    std::cout << "Got here";


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
} */


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

    file.write("data", 4);
    file.write(reinterpret_cast<char*>(&data_size), 4);

    file.write(reinterpret_cast<char*>(sound->samples.data()), data_size);

    return true;
}

// TIP See CLion help at <a
// href="https://www.jetbrains.com/help/clion/">jetbrains.com/help/clion/</a>.
//  Also, you can try interactive lessons for CLion by selecting
//  'Help | Learn IDE Features' from the main menu.
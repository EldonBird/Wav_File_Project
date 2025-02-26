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



struct wav_t {

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


    unsigned NumSamples;

    std::vector<std::pair<short, short> > data;
};

using namespace std;


bool read_in_wav_data(const char *filename, wav_t *sound);
bool output_to_wav(const char *filename, wav_t *sound);
bool double_sound(wav_t *sound);


int main(){


    wav_t *sound = new wav_t();

    if (read_in_wav_data("coin.wav", sound)) {

        double_sound(sound);
        output_to_wav("D:/Projects/My software/Wav_File_Project/output.wav", sound);
    }

    delete sound;
    return 0;
}


bool read_in_wav_data(const char *filename, wav_t *sound) {

    FILE *fp = fopen(filename, "rb");


    if (!fp) {
        std::cerr << "Error opening file " << filename << std::endl;
        return false;
    }


    fread(sound->riff_header, sizeof(char), 4, fp);
    std::cout << sound->riff_header << std::endl;
    if (strncmp(sound->riff_header, "RIFF", 4) != 0) {
        std::cerr << "Invalid RIFF header" << std::endl;
        return false;
    }

    fread(&sound->wav_size, sizeof(int), 1, fp);
    std::cout << sound->wav_size << std::endl;
    fread(sound->wave_header, sizeof(char), 4, fp);
    std::cout << sound->wave_header << std::endl;
    if (strncmp(sound->wave_header, "WAVE", 4) != 0) {
        std::cerr << "Not a WAVE file!" << std::endl;
        return false;
    }

    fread(sound->Subchunk1ID, sizeof(char), 4, fp);
    if (strncmp(sound->Subchunk1ID, "fmt ", 4) != 0) {
        std::cerr << "Not the right fmt header" << std::endl;
        return false;
    }

    fread(&sound->Subchunk1Size, sizeof(int), 1, fp);
    fread(&sound->AudioFormat, sizeof(short), 1, fp);
    fread(&sound->NumChannels, sizeof(short), 1, fp);
    fread(&sound->SampleRate, sizeof(int), 1, fp);
    fread(&sound->ByteRate, sizeof(int), 1, fp);
    fread(&sound->BlockAlign, sizeof(short), 1, fp);
    fread(&sound->BitsPerSample, sizeof(short), 1, fp);

    std::cout << sound->Subchunk1Size<< std::endl;
    std::cout << sound->AudioFormat << std::endl;
    std::cout << sound->NumChannels << std::endl;
    std::cout << sound->SampleRate << std::endl;
    std::cout << sound->ByteRate << std::endl;
    std::cout << sound->BlockAlign << std::endl;
    std::cout << sound->BitsPerSample << std::endl;

    fread(sound->Subchunk2ID, sizeof(char), 4, fp);
    if (strncmp(sound->Subchunk2ID, "data", 4) != 0 ) {
        std::cerr << "Something wrong with the 'data' " << std::endl;
        return false;
    }

    fread(&sound->Subchunk2Size, sizeof(int), 1, fp);

    int NumSamples = sound->Subchunk2Size / (sound->NumChannels*(sound->BitsPerSample / 8));

    sound->data = std::vector<std::pair<short, short> >(NumSamples);
    for (int i = 0; i < NumSamples; i++) {
        fread(&sound->data[i].first, sizeof(short), 1, fp);
        fread(&sound->data[i].second, sizeof(short), 1, fp);
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

    wav_t header;

    if (!file.read(reinterpret_cast<char*>(&header), sizeof(wav_t))) {
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


bool output_to_wav(const char *filename, wav_t *sound) {

    FILE *file = fopen(filename, "wb");

    if (!file) {
        std::cout << "failed to open file" << std::endl;
        return false;
    }


    fwrite(sound->riff_header, sizeof(char), 4, file);
    fwrite(&sound->wav_size, sizeof(int), 1, file);
    fwrite(sound->wave_header, sizeof(char), 4, file);

    fwrite(sound->Subchunk1ID, sizeof(char), 4, file);
    fwrite(&sound->Subchunk1Size, sizeof(int), 1, file);
    fwrite(&sound->AudioFormat, sizeof(short), 1, file);
    fwrite(&sound->NumChannels, sizeof(short), 1, file);
    fwrite(&sound->SampleRate, sizeof(int), 1, file);
    fwrite(&sound->ByteRate, sizeof(int), 1, file);
    fwrite(&sound->BlockAlign, sizeof(short), 1, file);
    fwrite(&sound->BitsPerSample, sizeof(short), 1, file);

    fwrite(sound->Subchunk2ID, sizeof(char), 4, file);
    fwrite(&sound->Subchunk2Size, sizeof(int), 1, file);

    for (int i = 0; i < sound->data.size(); i++) {
        fwrite(&sound->data[i].first, sizeof(short), 1, file);
        fwrite(&sound->data[i].second, sizeof(short), 1, file);
    }

    fclose(file);

    std::cout << "Wrote file " << filename << std::endl;

    return true;
}

bool double_sound(wav_t *sound) {

    std::vector<std::pair<short, short>> originals = sound->data;
    int origional_size = originals.size();

    std::vector<std::pair<short, short>> new_data(origional_size / 2);

    for (int i = 0; i < origional_size / 2; i++) {
        new_data[i] = originals[i * 2];
    }

    sound->data = new_data;
    sound->Subchunk2Size = sound->data.size() * sound->NumChannels * (sound->BitsPerSample / 8);
    sound->wav_size = 36 + sound->Subchunk2Size;

    return true;



}




// TIP See CLion help at <a
// href="https://www.jetbrains.com/help/clion/">jetbrains.com/help/clion/</a>.
//  Also, you can try interactive lessons for CLion by selecting
//  'Help | Learn IDE Features' from the main menu.
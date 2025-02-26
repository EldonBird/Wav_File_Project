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

    char chunk_id[5];
    int chunk_size;
    char format[5];

    char Subchunk1ID[5];
    char Subchunk2ID[5];

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
bool double_length(wav_t *sound);


int main(){


    wav_t *sound = new wav_t();

    if (read_in_wav_data("boom.wav", sound)) {

        //double_length(sound);
        output_to_wav("throughboom.wav", sound);
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


    fread(sound->chunk_id, sizeof(char), 4, fp);
    std::cout << sound->chunk_id << std::endl;
    if (strncmp(sound->chunk_id, "RIFF", 4) != 0) {
        std::cerr << "Invalid RIFF header" << std::endl;
        return false;
    }

    fread(&sound->chunk_size, sizeof(int), 1, fp);
    std::cout << sound->chunk_size << std::endl;
    fread(sound->format, sizeof(char), 4, fp);
    std::cout << sound->format << std::endl;
    if (strncmp(sound->format, "WAVE", 4) != 0) {
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

    std::cout << "subchunk1size : " << sound->Subchunk1Size<< std::endl;
    std::cout << "format : " << sound->AudioFormat << std::endl;
    std::cout << "channels : " << sound->NumChannels << std::endl;
    std::cout << "Sample rate : " << sound->SampleRate << std::endl;
    std::cout << "Byte rate : " << sound->ByteRate << std::endl;
    std::cout << "Block align : " << sound->BlockAlign << std::endl;
    std::cout << "bits per sample : " << sound->BitsPerSample << std::endl;

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

bool output_to_wav(const char *filename, wav_t *sound) {

    FILE *file = fopen(filename, "wb");

    if (!file) {
        std::cout << "failed to open file" << std::endl;
        return false;
    }


    fwrite(sound->chunk_id, sizeof(char), 4, file);
    fwrite(&sound->chunk_size, sizeof(int), 1, file);
    fwrite(sound->format, sizeof(char), 4, file);

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
    sound->chunk_size = 36 + sound->Subchunk2Size;

    return true;

}

bool double_length(wav_t *sound) {

    std::vector<std::pair<short, short>> originals = sound->data;
    int original_size = sound->data.size();

    std::vector<std::pair<short, short>> new_data(original_size * 2);

    int current = 0;

    for (int i = 0; i < original_size; i++) {

        new_data[i] = originals[i * 2];


    }
    sound->data = new_data;
    sound->Subchunk2Size = sound->data.size() * sound->NumChannels * (sound->BitsPerSample / 8);
    sound->chunk_size = 36 + sound->Subchunk2Size;

    return true;

}



// TIP See CLion help at <a
// href="https://www.jetbrains.com/help/clion/">jetbrains.com/help/clion/</a>.
//  Also, you can try interactive lessons for CLion by selecting
//  'Help | Learn IDE Features' from the main menu.
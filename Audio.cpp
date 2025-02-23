//
// Created by Birdh on 2025-02-20.
//

#include <stdio.h>
#include <fstream>
#include <iostream>
#include <math.h>
#include <cmath>
#include <vector>
#include <cstdint>
#include <cstring>


#include "cmake-build-debug/Audio.hpp"

using namespace std;

const string chunk_id = "RIFF";
const string chunk_size = "----";
const string formating = "WAVE";


const string sub_chunk_1_id = "fmt ";
const int sub_chunk_1_size = 16;
const int audio_format = 1;
const int num_channels = 1; // variable
const int sample_rate = 44100;
const int byte_rate = sample_rate * num_channels * (sub_chunk_1_size/ 8 );
const int block_align = num_channels * (sub_chunk_1_size / 8);
const int bits_per_sample = 16;

const string sub_chunk_2_id = "data";
const string sub_chunk_2_size = "----";

const int duration = 2;
const int max_amplitude = 32760;
const double frequency = 250;


int main() {
    FILE* file = fopen("test.wav", "rb");

    if (!file) {
        std::cerr << "Failed to open file\n";
        return 1;
    }

    Audio::WAVHEADER wav_header;
    Audio::sound_t sound;

    if (!Audio::read_wav_header(file, wav_header)) {
        std::cerr << "Failed to read WAV header\n";
        fclose(file);
        return 1;
    }

    if (!Audio::read_in_wav_data("test.wav", &sound)) {
        std::cerr << "Failed to read WAV data\n";
        fclose(file);
        return 1;
    }

    std::cout << "Sample rate: " << wav_header.sample_rate << " Hz\n";
    std::cout << "Channels: " << wav_header.num_channels << "\n";
    std::cout << "Bits per sample: " << wav_header.bits_per_sample << "\n";

    fclose(file);


    return 0;
}

void Audio::write_as_btyes(ofstream &file, int value, int byte_size) {

    file.write(reinterpret_cast<const char*>(&value), byte_size);


}


int Audio::audioimport() {

    ofstream wav;

    wav.open("test.wav", ios::binary);

    if (wav.is_open()) {

        wav << chunk_id;
        wav << chunk_size;
        wav << formating;

        wav << sub_chunk_1_id;
        write_as_btyes(wav, sub_chunk_1_size, 4);
        write_as_btyes(wav, audio_format, 2);
        write_as_btyes(wav, num_channels, 2);
        write_as_btyes(wav, sample_rate, 4);
        write_as_btyes(wav, byte_rate, 4);
        write_as_btyes(wav, block_align, 2);
        write_as_btyes(wav, bits_per_sample, 2);

        wav << sub_chunk_2_id;
        wav << sub_chunk_1_size;

        int start_audio = wav.tellp();

        for (int i = 0; i < sample_rate * duration; i++) {

            double amp = ((double) i / (double) sample_rate) * max_amplitude;
            double value = sin((2 * 3.14 * i * frequency) / sample_rate);

            double channel1 = amp * value / 2;
            //double channel2 = (max_amplitude - amp) * value;

            write_as_btyes(wav, channel1, 2);
            //write_as_btyes(wav, channel2, 2);

        }
        int end_audio = wav.tellp();

        wav.seekp(start_audio - 4);
        write_as_btyes(wav, end_audio - start_audio, 4);

        wav.seekp(4, ios::beg);
        write_as_btyes(wav, end_audio - 8, 4);

    }

    wav.close();


    return 0;
}

bool Audio::read_in_wav_data(const char *filename, Audio::sound_t *sound) {

    bool return_value = true;

    char magic[4];
    int32_t file_size;
    int32_t format_length;
    int16_t format_type;
    int16_t num_channels;
    int32_t sample_rate;
    int32_t byte_per_second;
    int16_t block_align;
    int16_t bits_per_sample;
    int32_t data_size;


    FILE *file = fopen(filename, "rb");

    if (file == NULL) {
        std::cerr << "Failed to open file\n";
        return false;
    }

    fread(magic, 1, 4, file);

    if (magic[0] != 'R' || magic[1] != 'I' || magic[2] != 'F' || magic[3] != 'F') {
        std::cerr << "Not RIFF\n";
        return false;
    }

    fread(&file_size, 4,1, file);

    fread(magic, 1, 4, file);

    if (magic[0] != 'W' || magic[1] != 'A' || magic[2] != 'V' || magic[3] != 'E') {
        std::cerr << "Not WAVE\n";
        return false;
    }

    fread(magic, 1, 4, file);
    if (magic[0] != 'f' || magic[1] != 'm' || magic[2] != 't' || magic[3] != ' ') {
        std::cerr << "Not fmt  \n";
        return false;
    }

    fread(&format_length, 4, 1, file);
    fread(&format_type, 2, 1, file);

    if (format_type != 1) {
        std::cerr << "wrong format type \n";
        return false;
    }

    fread(&num_channels, 2, 1, file);
    if (num_channels != 1) {
        std::cerr << "wrong number of channels, Expected : 1 \n";
        return false;
    }

    fread(&sample_rate, 4, 1, file);
    if (sample_rate != 44100) {
        std::cerr << "wrong sample rate \n";
        return false;
    }

    fread(&byte_per_second, 4, 1, file);
    fread(&block_align, 2, 1, file);
    fread(&bits_per_sample, 2, 1, file);

    if (bits_per_sample != 16) {
        std::cerr << "wrong bits per sample \n";
        return false;
    }

    fread(magic, 1, 4, file);

    while (true) {
        if (magic[0] == 'd' || magic[1] == 'a' || magic[2] == 't' || magic[3] == 'a') {
            std::cerr << "found the data \n";
            break;
        }
        fseek(file, data_size, SEEK_CUR);
    }



    fread(&data_size, 4, 1, file);

    //might need to allocate memory, will have to figure that out later

    sound->data = (int16_t*) malloc(data_size);

    if (fread(sound->data, 1, data_size, file) != data_size) {
        std::cerr << "Something wrong with data" << sound->data << data_size;
        return false;
    }


    sound->samples = data_size / 2;



    std::cout << sound->data << std::endl;


    fclose(file);
    return true;




}

bool Audio::read_wav_header(FILE* file, WAVHEADER& header) {

    size_t bytes_read = fread(&header, 1, sizeof(WAVHEADER), file);

    if (bytes_read != sizeof(WAVHEADER)) { return false; }

    return true;



}

bool Audio::output_to_wav(const char *filename, sound_t *sound, Audio::WAVHEADER *header) {

    ofstream wav;

    wav.open(filename, ios::binary);

    if (wav.is_open()) {

        wav << header->riff_header;
        wav << header->wav_size;
        wav << header->wave_header;

        wav << header->fmt_header;
        wav << header->fmt_chunk_size;
        wav << header->audio_format;

        wav << header->num_channels;
        wav << header->sample_rate;

        wav << header->byte_rate;
        wav << header->block_align;
        wav << header->bits_per_sample;

        wav << "data";




    }



    return true;
}

bool Audio::double_frequency(sound_t* input, sound_t* output) {

    if (!input || !input->data || input->samples == 0) {
        std::cerr << "Missing input data, cant double";
        return false;
    }

    output->samples = input->samples / 2;

    output->data = new int16_t[input->samples];

    if (!output->data) {
        std::cerr << "Memory allocation failed\n";
        return false;
    }

    for (uint32_t i = 0; i < output->samples; i++) {

        output->data[i] = input->data[i * 2];

    }

    return true;
}



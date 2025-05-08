#ifndef ALSA_H
#define ALSA_H

#include <iostream>
#include <fstream>
#include <vector>
#include <alsa/asoundlib.h>  // ALSA library for audio hardware access
#include "dr_wav.h"          // A lightweight WAV file handling library

using namespace std;

// Constants for recording
#define CHANNELS 1         // Mono audio
#define FRAME_SIZE 2       // 16-bit PCM (2 bytes per sample)
#define SECONDS 5          // Duration for recording (5 seconds)

// Defines the format of a standard WAV file header
struct WAVHeader {
    // RIFF Header
    char riff[4] = { 'R', 'I', 'F', 'F' };    // Identiy the file as RIFF format (Resource Interchange File Format)
                                              // "RIFF is always stored as ASCII characters at the very beginning of a valid WAV file"
    uint32_t chunkSize = 0;                   // Total file size (to be filled in later)
    char wave[4] = { 'W', 'A', 'V', 'E' };    // Specify WAVE as RIFF type

    // Format Header
    char fmt[4] = { 'f', 'm', 't', ' ' };     // Marks beginning of format chunk
    uint32_t subchunk1Size = 16;              // Specify size of rest of format chunk (bytes)
    uint16_t audioFormat = 1;                 // Indicate uncompressed PCM
    uint16_t numChannels = CHANNELS;          // 1 (mono)
    uint32_t sampleRate = 16000;              // may be overwritten in ALSA call
    // data rate in bytes per second
    uint32_t byteRate = sampleRate * numChannels * FRAME_SIZE;
    // number of bytes per sample slice including all channels
    uint16_t blockAlign = numChannels * FRAME_SIZE;
    uint16_t bitsPerSample = 16;               // number of bits to represent each sample

    // Data Header
    char data[4] = { 'd', 'a', 't', 'a' };     // Marks beginning of data chunk
    uint32_t subchunk2Size = 0;                // Specify size of audio data (bytes)
};

// Function declarations
bool load_wav_file(const char* filename, vector<float>& pcm, int& sample_rate);
void updateWAVHeader(const char* filename, uint32_t dataSize);
void alsa(int SAMPLE_RATE);

#endif


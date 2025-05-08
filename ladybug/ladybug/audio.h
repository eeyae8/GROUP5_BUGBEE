#ifndef AUDIO_H
#define AUDIO_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <cassert>
#include <cstdlib>
#include <mutex>
#include "dr_wav.h"
#include "codec2.h"  // Adjust include path if needed
#include "alsa.h"
#include "radio.h"

using namespace std;

// ---------- Constants ----------
#define CODEC_SAMPLE_RATE 8000 // 8 kHz sample rate for codec

// ---------- External Globals ----------
extern mutex radioMutex;  // Mutex for safely switching between sending and receiving

// Assuming 'radio' is defined somewhere else using RF24 or similar
extern RF24 radio;

// ---------- Function Declarations ----------
void encode(const char* inputFilename);
void decodeFile(string inputFilename, string outputFilename);
void convertWav(string rawFilename, string wavFilename);
void playFile(string audioFilename);
void transmit_audio(string transmitFilename);

#endif

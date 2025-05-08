#ifndef TEXT_H
#define TEXT_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>

#include "alsa.h"
#include "radio.h"

#include "whisper.h"  // Required for whisper_context and Whisper functions

using namespace std;

// ------------ Constants ------------
#define WHISPER_SAMPLE_RATE 16000

// ------------ External Variables ------------
extern mutex radioMutex;

// ------------ Function Declarations ------------
void textToSpeech(string filename);

bool process_audio_to_whisper(const char* wavFile, struct whisper_context* ctx);
void whisper(string textFile);

void transmit_text_STT(string transmitFilename);
void transmit_text_TTT(string receivedText, string transmitFilename);

#endif

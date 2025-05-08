//
//#include <iostream>
//#include <vector>
//#include <wiringPi.h>
//#include <RF24/RF24.h>
//#include <thread>
//#include <mutex>
//#include <termios.h>
//#include <unistd.h>
//#include <fstream>
//#include <cstring>
//#include <cstdlib>
//#include <assert.h>
//#include <errno.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include "c2file.h"
//#include "codec2.h"
//#include "whisper.h"  // header file for Whisper AI model
//#include <string>
//#include <alsa/asoundlib.h>
//#include "dr_wav.h"  // library used to read and decode WAV files
//#include <sstream>
//
//#define WHISPER_SAMPLE_RATE 16000  // 16 kHz sample rate for Whisper
//#define CODEC_SAMPLE_RATE 8000 // 8 kHz sample rate for codec
//#define CHANNELS 1         // Mono audio
//#define FRAME_SIZE 2       // 16-bit PCM (2 bytes per sample)
//#define SECONDS 5          // Duration for recording (5 seconds)
//#define BUFFER_SIZE 1024   // Buffer size for audio input (in frames)
//
//
//#define PIN_CE 17  // Chip Enable
//#define PIN_CSN 0  // Chip Select Not
//
//using namespace std;
//
//extern RF24 radio;
//extern bool running;
//extern mutex radioMutex;  // Mutex for safely switching between sending and receiving
//extern int serial_port;
//const int PAYLOAD_SIZE = 32;
//
//
//// WAV header structure
//struct WAVHeader {
//    char riff[4] = {'R', 'I', 'F', 'F'};
//    uint32_t fileSize = 0;
//    char wave[4] = {'W', 'A', 'V', 'E'};
//    char fmt[4] = {'f', 'm', 't', ' '};
//    uint32_t fmtSize = 16;
//    uint16_t audioFormat = 1;  // PCM
//    uint16_t numChannels = CHANNELS;
//    uint32_t sampleRate = WHISPER_SAMPLE_RATE;
//    uint32_t byteRate = WHISPER_SAMPLE_RATE * CHANNELS * FRAME_SIZE;
//    uint16_t blockAlign = CHANNELS * FRAME_SIZE;
//    uint16_t bitsPerSample = 16;
//    char data[4] = {'d', 'a', 't', 'a'};
//    uint32_t dataSize = 0;
//};
//
//bool load_wav_file(const char* filename, vector<float>& pcm, int& sample_rate);
//void updateWAVHeader(const char* filename, uint32_t dataSize);
//void alsa(int SAMPLE_RATE);
//void encode();
//void decodeFile(string inputFilename, string outputFilename);
//void convertWav(string rawFilename, string wavFilename);
//void playFile(string audioFilename);
//void transmit_audio(string transmitFilename);
//void receiver();
//void receiveFile(string receivedFilename);
//void sendFile(string filename);
//void sendString(string message);
//void textToSpeech(string filename);
//bool process_audio_to_whisper(const char* wavFile, struct whisper_context* ctx);
//void whisper(string textFile);
//void transmit_text(string transmitFilename);
//void transmit_text_STT(string transmitFilename);
//void transmit_text_TTT(string receivedText,string transmitFilename);
//void updateNextion(string receivedFilename, string text);
//void sendCommand(string command);
//void updateChatLog(string text);





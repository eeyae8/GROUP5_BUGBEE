#ifndef RADIO_H
#define RADIO_H

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <RF24/RF24.h>
#include "audio.h"
#include "text.h"
#include "screen.h"

using namespace std;

// ------------ Constants ------------
#define PIN_CE 17
#define PIN_CSN 0
#define PAYLOAD_SIZE 32

// ------------ Global Variables ------------
extern RF24 radio;
extern bool running;
extern mutex radioMutex;
extern bool emergencyTriggered;

// ------------ Function Declarations ------------
void receiver();
void receiveFile(string receivedFilename);
void sendFile(string filename);
void sendString(string message);
void playEmergencyAudio();

#endif

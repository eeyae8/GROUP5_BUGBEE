#ifndef MAIN_H
#define MAIN_H

#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <fcntl.h>
#include <mutex>
#include <unistd.h>
#include <termios.h>
#include <wiringPi.h>
#include <RF24/RF24.h>

using namespace std;

// ---------- Pin Definitions ----------
#define PIN_CE 17
#define PIN_CSN 0

// ---------- External Globals ----------
extern RF24 radio;
extern bool running;
extern mutex radioMutex;
extern int serial_port;
extern bool emergencyTriggered;

// ---------- Function Declarations ----------
void receiver();  // Thread function to receive data
void transmit_audio(string transmitFilename);
void transmit_text_STT(string filename);
void transmit_text_TTT(string text, string filename);

#endif

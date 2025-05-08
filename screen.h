#ifndef SCREEN_H
#define SCREEN_H

#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <fcntl.h>

using namespace std;

// ------------ Global Variables ------------
extern int serial_port;

// ------------ Function Declarations ------------
void sendCommand(string command);
void updateChatLog(string text);
void updateNextion(string receivedFilename, string text);
void switchNextionPage(string pageName);

#endif

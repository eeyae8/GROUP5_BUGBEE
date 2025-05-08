#include "screen.h"

// Function to send a command to the Nextion screen
void sendCommand(string command) {
    write(serial_port, command.c_str(), command.length()); // Send the formatted string
    write(serial_port, "\xFF\xFF\xFF", 3); // Send the Nextion end command
}

void updateChatLog(string text) {
    // Shift messages up
    sendCommand("t1.txt=t2.txt");
    sendCommand("t2.txt=t3.txt");
    sendCommand("t3.txt=t4.txt");
    sendCommand("t4.txt=t5.txt");

    // Add new message at the bottom
    sendCommand("t5.txt=\"" + text + "\"");
}


void updateNextion(string receivedFilename, string text) {

    cout<< text << endl;

    ostringstream command;

    string formattedCommand = command.str(); // Convert to string

    sendCommand(formattedCommand);
    updateChatLog(text);
}

 // Function to switch Nextion page
void switchNextionPage(string pageName) {
    sendCommand("page " + pageName);
}

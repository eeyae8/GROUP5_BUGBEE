#include "radio.h"

// Receiver function (Runs in a separate thread)
void receiver() {
    if (!radio.begin()) {
        cerr << "Error: NRF24 module not detected!" << endl;
        return;
    }

    // Configure radio settings
    radio.setChannel(76);
    radio.setPALevel(RF24_PA_LOW);
    radio.setDataRate(RF24_2MBPS);
    radio.openReadingPipe(0, 0x7878787878LL);
    radio.setAutoAck(true);
    radio.enableAckPayload();
    radio.enableDynamicPayloads();
    radio.startListening();

    cout << "Receiver started..." << endl;

    while (running) {
        radioMutex.lock();  // Lock while accessing the radio
        char buffer[32] = {0};

        if (radio.available()) {
            radio.read(&buffer, sizeof(buffer));
            string mode(buffer);
            cout << "Waiting for mode selection..." << endl;
            if (mode.find("audio") != string::npos) {
                cout << "Audio mode selected." << endl;
                receiveFile("output_compressed.bin");
                decodeFile("output_compressed.bin", "output_decoded.raw");
                convertWav("output_decoded.raw", "output_decoded.wav");
                playFile("output_decoded.wav");
            } else if (mode.find("STT") != string::npos) {
                cout << "Text_STT mode selected." << endl;
                receiveFile("received.txt");
                textToSpeech("received.txt");
            }else if (mode.find("text") != string::npos) {
                cout << "Text_TTT mode selected." << endl;
                receiveFile("received.txt");

                ifstream file("received.txt");
                string receivedText;
                getline(file, receivedText); // Read a single line
                file.close();

                // preset commands
                if (receivedText.find("*") != string::npos) {
                    // Remove the first character *
                    receivedText = receivedText.substr(1);

                    // Save the modified text back to OUTPUT_TEXT
                    ofstream outFile("received_txt");
                    outFile << receivedText;
                    outFile.close();
                    textToSpeech("received_txt");
                }
                else if (receivedText == "EMERGENCY") {
                    emergencyTriggered = true;
                    switchNextionPage("ack");
                    cout << "Emergency triggered\n";
                    thread emergencyAudioThread(playEmergencyAudio);
                    while (emergencyTriggered){
                        //playFile("emergency.wav");

                        char buffer[64];
                        string screenText = "";
                        int bytes_read = read(serial_port, buffer, sizeof(buffer) - 1);
                        if (bytes_read > 0) {
                            buffer[bytes_read] = '\0';  // Null-terminate received data
                            screenText += buffer;
                            // Display received data
                            cout << "screen send: " << screenText << endl;
                            if (screenText == "EMERGENCY_CANCEL") {
                                emergencyTriggered = false;
                                emergencyAudioThread.join();
                            }
                        }
                        usleep(10000); // Small delay to avoid hammering the CPU
                    }
                }

                else updateNextion("received_txt", receivedText);

            }else {
                cout << "Invalid mode received." << endl;
            }
        }
        radioMutex.unlock();  // Unlock for other operations
        usleep(1000);  // Small delay to prevent CPU overload
    }

    radio.stopListening();
}


void playEmergencyAudio(){
    while (emergencyTriggered){
        playFile("emergency.wav");
    }

}

void receiveFile(string receivedFilename) {
    ofstream receivedFile(receivedFilename, ios::binary | ios::trunc);
    if (!receivedFile.is_open()) {
        cout << "Failed to open received file: " << receivedFilename << endl;
        return;
    }

    char buffer[32] = {0};
    cout << "Waiting for file data..." << endl;

    while (true) {
        if (radio.available()) {
            int bytesRead = radio.getDynamicPayloadSize();
            if (bytesRead > 0) {
                radio.read(&buffer, bytesRead);
                if (strncmp(buffer, "END_OF_FILE", min(bytesRead, (int)strlen("END_OF_FILE"))) == 0) {
                    cout << "End of file received. Closing file." << endl;
                    break;
                }
                receivedFile.write(buffer, bytesRead);
                receivedFile.flush();
                cout << "Received " << bytesRead << " bytes." << endl;
            }
        }
    }
    receivedFile.close();
    cout << "File successfully saved to " << receivedFilename << endl;
}

void sendFile(string filename) {
    ifstream file(filename, ios::binary);  // Open file for reading
    if (!file.is_open()) {
        cout << "Failed to open file: " << filename << endl;
        return;
    }

    char buffer[PAYLOAD_SIZE] = {0};

    while (file.read(buffer, PAYLOAD_SIZE) || file.gcount() > 0) {
        int actualSize = file.gcount();  // Get actual number of bytes read

        if (radio.write(&buffer, actualSize)) {  // Send only the actual bytes read
            cout << "Sent " << actualSize << " bytes" << endl;
        } else {
            cout << "Failed to send data" << endl;
        }

        memset(buffer, 0, PAYLOAD_SIZE);  // Clear buffer before next read
        delay(50);
    }

    const string END_SIGNAL = "END_OF_FILE";
    // **Send the termination signal**
    char endSignal[PAYLOAD_SIZE] = {0};
    strncpy(endSignal, END_SIGNAL.c_str(), END_SIGNAL.size());  // Copy "END_OF_FILE" into buffer
    radio.write(&endSignal, END_SIGNAL.size() + 1);  // Send termination message

    cout << "File transmission complete. Sent termination signal." << endl;

    file.close();
}

void sendString(string message) {
    char buffer[PAYLOAD_SIZE] = {0};
    strncpy(buffer, message.c_str(), PAYLOAD_SIZE);
    radio.write(&buffer, PAYLOAD_SIZE);
    delay(50);
}



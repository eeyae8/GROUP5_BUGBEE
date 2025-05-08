#include "main.h"

RF24 radio(PIN_CE, PIN_CSN);
bool running = true;
mutex radioMutex;
int serial_port;
bool emergencyTriggered;


int main() {
    if (wiringPiSetupGpio() == -1) {
        cerr << "Error: WiringPi initialization failed!" << endl;
        return EXIT_FAILURE;
    }

    radio.begin();

    // Configure radio settings
    radio.setChannel(76);
    radio.setPALevel(RF24_PA_LOW);
    radio.setDataRate(RF24_2MBPS);
    radio.openReadingPipe(0, 0x7878787878LL);
    radio.setAutoAck(true);
    radio.enableAckPayload();
    radio.enableDynamicPayloads();
    radio.startListening();

    thread recvThread(receiver);  // Start receiver in a separate thread

    cout << "Press 't' to transmit text file." << endl;
    cout << "Press 'a' to transmit audio file." << endl;
    cout << "Press 'q' to exit." << endl;


    // Open serial port
    serial_port = open("/dev/ttyAMA0", O_RDWR | O_NOCTTY);
    if (serial_port < 0) {
        cerr << "Error: Unable to open serial port!" << endl;
        return 1;
    }

    // Configure serial settings
    struct termios tty;
    if (tcgetattr(serial_port, &tty) != 0) {
        cerr << "Error: Failed to get serial attributes!" << endl;
        close(serial_port);
        return 1;
    }

    tty.c_cflag = B9600 | CS8 | CREAD | CLOCAL;
    tty.c_iflag = IGNPAR;
    tty.c_oflag = 0;
    tty.c_lflag = 0;
    tcflush(serial_port, TCIFLUSH);
    tcsetattr(serial_port, TCSANOW, &tty);

    while (true) {
        cout << "Listening for Nextion input..." << endl;
        char buffer[64];
        string receivedText = "";

        int bytes_read = read(serial_port, buffer, sizeof(buffer) - 1);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';  // Null-terminate received data
            receivedText += buffer;

            // Display received data
            cout << "Received: " << receivedText << endl;

            // Detect and act on the "STS" or "STT" command
            if (receivedText.find("STS") != string::npos) {
                transmit_audio("output_compressed.bin");  // Run STS function
            } else if (receivedText.find("STT") != string::npos) {
                transmit_text_STT("transcription.txt");  // Run STT function
            }
            else  transmit_text_TTT(receivedText, "transcription.txt");

            // Clear the buffer after processing to avoid buildup
            receivedText.clear();
            tcflush(serial_port, TCIFLUSH); // Flush the serial buffer to prepare for new input
        }
        usleep(100000);  // Small delay to prevent high CPU usage
    }

    close(serial_port);

    running = false;  // Stop the receiver thread
    recvThread.join();  // Wait for the receiver thread to finish

    radio.powerDown();  // Properly shut down the NRF24 module
    cout << "Program terminated." << endl;

    return EXIT_SUCCESS;
}

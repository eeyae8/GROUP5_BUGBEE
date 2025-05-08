#define DR_WAV_IMPLEMENTATION // enable implementation of functions in dr_wav.h
#include "audio.h"


void encode(const char* outputFilename){
    // Call alsa function to record audio from microphone using 8000Hz sample rate
    alsa(CODEC_SAMPLE_RATE);

    // Set up Codec2 in 700C mode, which is very low bitrate mode (700 bits/second)
    int mode = CODEC2_MODE_2400;
    // Create Codec2 instance with this mode
    CODEC2 *codec2 = codec2_create(mode);
    FILE *fout;

    // Open output file in binary write mode
    fout = fopen(outputFilename, "wb");
    if (!fout) {
        cerr << "Error opening output compressed file." << endl;
        exit(1);   // Exit with error if file can't be opened
    }

    // Get codec parameters
    int nsam = codec2_samples_per_frame(codec2);   // Gets the number of audio samples per codec frame
                                                   // (typically 320 samples for Codec2 700C)
    int nbit = codec2_bits_per_frame(codec2);      // Gets the number of bits per encoded frame
                                                   // (typically around 28 bits for 700C)

    int nbyte = (nbit + 7) / 8;                    // calculates the number of bytes needed to store these bits

    // Allocate buffers
    // input buffer (buf): holds one frame of PCM audio samples
    short *buf = (short *)malloc(nsam * sizeof(short));
    // output buffer (bits): holds the compressed data for one frame
    unsigned char *bits = (unsigned char *)malloc(nbyte * sizeof(char));

    // Configure codec
    codec2_set_natural_or_gray(codec2, 1);  // Use Gray coding (adjacent values differ by one bit)
    codec2_700c_eq(codec2, 0);              // No equalization (audio frequency compensation)

    // Open and read WAV file that was recorded earlier
    drwav wav;
    drwav_init_file(&wav, "audio.wav", NULL);

    // Create a vector sized to hold all audio samples
    vector<short> audioData(wav.totalPCMFrameCount);
    // Reads all frames from the WAV file as 16-bit PCM data
    drwav_read_pcm_frames_s16(&wav, wav.totalPCMFrameCount, audioData.data());
    // Close WAV file
    drwav_uninit(&wav);

    // Encoding loop
    for (size_t frameIndex = 0; frameIndex < audioData.size(); frameIndex += nsam) {
        // Copies one frame of audio samples (typically 320 samples) to the buffer
        memcpy(buf, &audioData[frameIndex], nsam * sizeof(short));
        // Encode this frame using Codec2
        codec2_encode(codec2, bits, buf);
        // Write the compressed data to output file
        fwrite(bits, sizeof(char), nbyte, fout);
    }

    // Clean up by destroying all Codec2 instance, free memory buffer and close output file
    codec2_destroy(codec2);
    free(buf);
    free(bits);
    fclose(fout);
}


void decodeFile(string inputFilename, string outputFilename) {
    // Open input file
    FILE *fin = fopen(inputFilename.c_str(), "rb");
    if (!fin) {
        cerr << "Error opening input file: " << strerror(errno) << endl;
        return;
    }

    // Open output file
    FILE *fout = fopen(outputFilename.c_str(), "wb");
    if (!fout) {
        cerr << "Error opening output file: " << strerror(errno) << endl;
        fclose(fin);
        return;
    }

    // Set up Codec2 in 700C mode
    CODEC2 *codec2 = codec2_create(CODEC2_MODE_2400);
    assert(codec2);

    // Get codec parameters
    int nsam = codec2_samples_per_frame(codec2);
    int nbit = codec2_bits_per_frame(codec2);
    int nbyte = (nbit + 7) / 8;

    // Allocate buffers
    short *buf = (short *)malloc(nsam * sizeof(short));
    unsigned char *bits = (unsigned char *)malloc(nbyte * sizeof(char));
    assert(buf && bits);

    // Process audio data frame by frame
    while (fread(bits, sizeof(char), nbyte, fin) == (size_t)nbyte) {
        codec2_decode(codec2, buf, bits);
        fwrite(buf, sizeof(short), nsam, fout);
    }

    codec2_destroy(codec2);
    free(buf);
    free(bits);
    fclose(fin);
    fclose(fout);
    cout << "Decoding complete. Output saved to " << outputFilename << endl;
}
// Convert raw file to WAV file
void convertWav(string rawFilename, string wavFilename) {
    string command = "sox -r 8000 -e signed -b 16 -c 1 '" + rawFilename + "' '" + wavFilename + "'";
    system(command.c_str());
    cout << "Converted to WAV format." << endl;
}

// Play a WAV file
void playFile(string audioFilename) {
    string playCommand = "aplay -D plughw:2 '" + audioFilename + "'";
    system(playCommand.c_str());
}



// Transmitter function for audio mode
void transmit_audio(string transmitFilename) {
    radioMutex.lock();  // Lock to prevent receiving during transmission
    radio.stopListening();  // Stop listening while sending
    encode("output_compressed.bin");

    radio.setRetries(15, 15);
    radio.openWritingPipe(0x7878787878LL);
    sendString("audio");
    delay(100);

    sendFile(transmitFilename);

    radio.startListening();  // Resume listening after transmission
    radioMutex.unlock();  // Unlock to allow receiving
}

#include "text.h"

void textToSpeech(string filename) {
    // -f tells eSpeak to read text from filename
    // audio output is piped to aplay, and played using ALSA
    string command = "espeak -f '" + filename + "' --stdout | aplay -D plughw:2";
    system(command.c_str());
}

// Takes a WAV file and a Whisper context, performs transcription using Whisper model
// wavFile: path to audio file
// ctx: pointer to Whisper context (loaded model instance)
bool process_audio_to_whisper(const char* wavFile, struct whisper_context* ctx) {
    // buffer to hold the audio samples as floating point
    vector<float> pcm;
    // sample rate of audio file (16000 Hz)
    int sample_rate;

    // use dr_wav library to load a WAV file
    // populates pcm with audio samples and sample_rate with audio's sampling rate
    if (!load_wav_file(wavFile, pcm, sample_rate)) {
        cerr << "Error loading WAV file: " << wavFile << endl;
        return false;
    }

    // Get default transcription parameters
    // WHISPER_SAMPLING_GREEDY; simple transcription mode using greedy decoding
    struct whisper_full_params params = whisper_full_default_params(WHISPER_SAMPLING_GREEDY);

    // disable console output
    params.print_realtime = false;
    params.print_progress = false;
    params.print_timestamps = false;
    params.print_special = false;
    params.translate = false;        // don't translate, just transcribe
    params.language = "en";          // language of speech (English)
    params.n_threads = 4;            // use 4 CPU threads for transcription

    // run transcription using loaded model
    int ret = whisper_full(ctx, params, pcm.data(), pcm.size());
    // return true if successful, otherwise false
    return ret == 0;
}

// Captures audio, transcribe using Whisper and saves result to txt file
void whisper(string textFile){
    // Load Whisper model from specified .bin file, use tiny model (fast but less accurate than base/medium/large)
    struct whisper_context* ctx = whisper_init_from_file("/home/group5/whisper.cpp/models/ggml-tiny.bin");

    // captures audio from microphone using ALSA
    alsa(WHISPER_SAMPLE_RATE);

    // Transcribe audio file using Whisper
    process_audio_to_whisper("audio.wav", ctx);

    // Gets the number of transcription segments returned by Whisper
    const int n_segments = whisper_full_n_segments(ctx);
    // Opens output file for writing
    ofstream output_file(textFile);


    // Loops over each transcription segment
    for (int i = 0; i < n_segments; ++i) {
        // Returns a chunk of recognized text
        const char* text = whisper_full_get_segment_text(ctx, i);
        // Write text to the output file, trimming the first character which is a space
        output_file << (i == 0 ? string(text).substr(text[0] ==  ' ' ? 1 : 0) : text) << endl;
        cout << "Whisper transcription:" << (i == 0 ? string(text).substr(text[0] ==  ' ' ? 1 : 0) : text) << endl;
    }

    //cout << "Transcription written to 'transcription.txt'." << endl;

    // Free model from memory
    whisper_free(ctx);
    // Close text file
    output_file.close();
}

// Transmitter function for text mode
void transmit_text_STT(string transmitFilename) {
    radioMutex.lock();  // Lock to prevent receiving during transmission
    radio.stopListening();  // Stop listening while sending
    whisper("transcription.txt");

    radio.setRetries(15, 15);
    radio.openWritingPipe(0x7878787878LL);
    sendString("STT");
    delay(100);

    sendFile(transmitFilename);

    radio.startListening();  // Resume listening after transmission
    radioMutex.unlock();  // Unlock to allow receiving
}


void transmit_text_TTT(string receivedText, string transmitFilename){
    radioMutex.lock();  // Lock to prevent receiving during transmission
    radio.stopListening();  // Stop listening while sending

    ofstream output_file(transmitFilename);
    output_file << receivedText << endl;

    radio.setRetries(15, 15);
    radio.openWritingPipe(0x7878787878LL);
    sendString("text");
    delay(100);

    sendFile(transmitFilename);

    radio.startListening();  // Resume listening after transmission
    radioMutex.unlock();  // Unlock to allow receiving
}

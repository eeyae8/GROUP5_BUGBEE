#include "alsa.h"


// Function to load and read a WAV audio file into a format the program can work with
// filename: path to WAV file
// pcm: reference to a vector that will store the audio samples as floating point values
// sample_rate: a reference to an integer that will store the audio's sample rate
bool load_wav_file(const char* filename, vector<float>& pcm, int& sample_rate) {
    drwav wav;
    // Open and initialise WAV file
    if (!drwav_init_file(&wav, filename, NULL)) {
        return false;
    }

    // Read the audio data as 32 bit floating point samples
    // Extract the sample rate
    pcm.resize(wav.totalPCMFrameCount);
    drwav_read_pcm_frames_f32(&wav, wav.totalPCMFrameCount, pcm.data());
    sample_rate = wav.sampleRate;

    // Close the file when done
    drwav_uninit(&wav);
    return true;
}


// Function to update the WAV header with correct file size and data size
void updateWAVHeader(const char* filename, uint32_t dataSize) {
    // Open WAV file in binary mode for both reading and writing
    fstream file(filename, ios::in | ios::out | ios::binary);
    if (file) {
        // Navigate to position 4 to update the overall file size
        file.seekp(4, ios::beg);
        // Calculate file size based on data size plus header size minus 8 bytes
        uint32_t fileSize = dataSize + sizeof(WAVHeader) - 8;
        // Write updated values back to file
        file.write(reinterpret_cast<const char*>(&fileSize), 4);
        // Navigate to position 40 to update data chunk size
        file.seekp(40, ios::beg);
        file.write(reinterpret_cast<const char*>(&dataSize), 4);
        file.close();
    }
}

// Function to record audio from a microphone
void alsa(int SAMPLE_RATE){
    // Create handle for capture device and hardware parameters
    snd_pcm_t* capture_handle;
    snd_pcm_hw_params_t* hw_params;
    // Open default audio capture device (system microphone)
    const char* device = "default";

    // Open ALSA capture device
    snd_pcm_open(&capture_handle, device, SND_PCM_STREAM_CAPTURE, 0);

    // Configure ALSA hardware parameters for 8kHz/16kHz sample rate, mono, 16-bit PCM
    snd_pcm_hw_params_malloc(&hw_params);
    snd_pcm_hw_params_any(capture_handle, hw_params);
    // Set interleaved access mode (standard way to organise audio data)
    snd_pcm_hw_params_set_access(capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
    // Use 16 bit signed little-endian format (common audio format)
    snd_pcm_hw_params_set_format(capture_handle, hw_params, SND_PCM_FORMAT_S16_LE);
    // Set sample rate according to the parameter passed
    snd_pcm_hw_params_set_rate(capture_handle, hw_params, SAMPLE_RATE, 0);
    // Configure for mono audio (CHANNELS = 1)
    snd_pcm_hw_params_set_channels(capture_handle, hw_params, CHANNELS);

    // Apply hardware parameters
    snd_pcm_hw_params(capture_handle, hw_params);

    snd_pcm_hw_params_free(hw_params);
    snd_pcm_prepare(capture_handle);

    // Allocate memory for audio buffer based on sample rate and recording duration
    short* buffer = new short[SAMPLE_RATE * SECONDS];

    // Open WAV file for writing in binary mode
    ofstream audioFile("audio.wav", ios::binary);

    // Write initial empty WAV header that will be updated later
    WAVHeader header;
    audioFile.write(reinterpret_cast<const char*>(&header), sizeof(WAVHeader));

    // Record audio for the duration of "SECONDS"
    cout << "Recording for " << SECONDS << " seconds..." << endl;
    // Stores recorded data in the buffer and return number of frames captured
    int frames = snd_pcm_readi(capture_handle, buffer, SAMPLE_RATE * SECONDS);


    // Write the recorded audio to the WAV file
    audioFile.write(reinterpret_cast<const char*>(buffer), frames * FRAME_SIZE);
    updateWAVHeader("audio.wav", frames * FRAME_SIZE); // Update WAV header with correct sizes

    cout << "Recording complete! Audio saved as 'audio.wav'." << endl;

    // Cleanup by freeing allocated buffer, closing WAV file and closing ALSA capture dvice
    delete[] buffer;
    audioFile.close();
    snd_pcm_close(capture_handle);
}

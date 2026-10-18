#pragma once

#include <miniaudio/miniaudio.h>

class AudioPlayer {
public:
    AudioPlayer();
    
    bool isInitialized() const;
    bool playWav(const char* filePath);

    AudioPlayer(const AudioPlayer&) = delete;
    AudioPlayer& operator=(const AudioPlayer&) = delete;

    ~AudioPlayer();
private:
    ma_engine engine{};
    bool initialized = false;
};

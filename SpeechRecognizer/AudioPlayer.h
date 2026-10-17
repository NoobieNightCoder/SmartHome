#pragma once

#include <miniaudio/miniaudio.h>

class AudioPlayer {
public:
    AudioPlayer();
    ~AudioPlayer();

    AudioPlayer(const AudioPlayer&) = delete;
    AudioPlayer& operator=(const AudioPlayer&) = delete;

    bool isInitialized() const;

    bool playWav(const char* filePath);

private:
    ma_engine engine_{};
    bool initialized_ = false;
};

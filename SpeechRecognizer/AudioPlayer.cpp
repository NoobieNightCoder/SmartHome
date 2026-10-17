#include "AudioPlayer.h"

#include <iostream>

using namespace std;

AudioPlayer::AudioPlayer() {
    ma_result result = ma_engine_init(nullptr, &engine_);

    if (result != MA_SUCCESS) {
        cerr << "Ошибка инициализации аудиовыхода\n";
        return;
    }

    initialized_ = true;
}

AudioPlayer::~AudioPlayer() {
    if (initialized_) {
        ma_engine_uninit(&engine_);
    }
}

bool AudioPlayer::isInitialized() const {
    return initialized_;
}

bool AudioPlayer::playWav(const char* filePath) {
    if (!initialized_ || filePath == nullptr) {
        return false;
    }

    ma_sound sound{};

    ma_result result = ma_sound_init_from_file(
        &engine_,
        filePath,
        0,
        nullptr,
        nullptr,
        &sound
    );

    if (result != MA_SUCCESS) {
        cerr << "Не удалось открыть аудиофайл: "
                  << filePath << '\n';
        return false;
    }

    result = ma_sound_start(&sound);

    if (result != MA_SUCCESS) {
        cerr << "Не удалось воспроизвести аудио\n";
        ma_sound_uninit(&sound);
        return false;
    }

    while (ma_sound_is_playing(&sound)) {
        ma_sleep(20);
    }

    ma_sound_uninit(&sound);
    return true;
}
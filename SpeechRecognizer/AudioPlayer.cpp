#include "AudioPlayer.h"

#include <iostream>
#include <thread>
#include <chrono>

using namespace std;

AudioPlayer::AudioPlayer() {
    ma_result result = ma_engine_init(nullptr, &engine);

    if (result != MA_SUCCESS) {
        cerr << "Ошибка инициализации аудиовыхода\n";
    } else {
        initialized = true;
    }
}

AudioPlayer::~AudioPlayer() {
    if (initialized) {
        ma_engine_uninit(&engine);
    }
}

bool AudioPlayer::isInitialized() const {
    return initialized;
}

bool AudioPlayer::playWav(const char* filePath) {
    bool result = true;

    if (initialized && filePath != nullptr) {
        ma_sound sound{};

        ma_result audioResult = ma_sound_init_from_file(&engine, filePath, 0, nullptr, nullptr, &sound);

        if (audioResult != MA_SUCCESS) {
            cerr << "Не удалось открыть аудиофайл: " << filePath << '\n';
            result = false;
        }

        if(result)
            audioResult = ma_sound_start(&sound);

        if (result && audioResult != MA_SUCCESS) {
            cerr << "Не удалось воспроизвести аудио\n";
            ma_sound_uninit(&sound);
            result = false;
        }

        while (ma_sound_is_playing(&sound) && result) {
            this_thread::sleep_for(chrono::milliseconds(20));
        }

        if(result)
            ma_sound_uninit(&sound);
    } else result = false;

    return result;
}
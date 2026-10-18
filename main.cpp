#include <iostream>
#include <windows.h>
#include <filesystem>

#include "SpeechRecognizer/SpeechRecognizer.h"
#include "SpeechRecognizer/AudioPlayer.h"

using namespace std;

namespace {
string resolveModelPath() {
    char modulePath[MAX_PATH]{};
    DWORD length = GetModuleFileNameA(nullptr, modulePath, MAX_PATH);

    if (length > 0 && length < MAX_PATH) {
        filesystem::path exePath(modulePath);
        filesystem::path candidate = exePath.parent_path() / "models" / "vosk-model-small-ru-0.22";
        if (filesystem::exists(candidate))
            return candidate.string();
    }

    return "bin/models/vosk-model-small-ru-0.22";
}

string resolveResponsePath() {
    char modulePath[MAX_PATH]{};
    DWORD length = GetModuleFileNameA(nullptr, modulePath, MAX_PATH);

    if (length > 0 && length < MAX_PATH) {
        filesystem::path projectRoot = filesystem::path(modulePath).parent_path().parent_path();
        filesystem::path candidate = projectRoot / "response.wav";
        if (filesystem::exists(candidate))
            return candidate.string();
    }

    filesystem::path candidate = filesystem::current_path() / "response.wav";
    if (filesystem::exists(candidate))
        return candidate.string();

    throw runtime_error("Не найден response.wav в корне проекта или текущей рабочей папке.");
}

void waitForExit() {
    cout << "Speak... Press Ctrl+C to exit.\n";

    while (true) {
        Sleep(1000);
    }
}
} // namespace

void setupConsole() {
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    cout.setf(std::ios::unitbuf);
    cerr.setf(std::ios::unitbuf);
}

int main() {
    setupConsole();

    try {
        const string modelPath = resolveModelPath();
        cout << "[Startup] Model path: " << modelPath << endl;
        cout << "[Startup] Loading voice assistant..." << endl;

        SpeechRecognizer recognizer(modelPath);
        cout << "[Startup] Model loaded." << endl;

        AudioPlayer audioPlayer;
        if (!audioPlayer.isInitialized()) {
            throw runtime_error("Не удалось инициализировать аудиоплеер.");
        }

        const string responsePath = resolveResponsePath();
        cout << "[Startup] Playing audio: " << responsePath << endl;
        if (!audioPlayer.playWav(responsePath.c_str())) {
            throw runtime_error("Не удалось воспроизвести response.wav.");
        }

        recognizer.start();
        cout << "[Startup] Microphone started." << endl;

        waitForExit();

        recognizer.stop();
    } catch (const exception& ex) {
        cerr << "[Startup error] " << ex.what() << endl;
        cout << "Press any key to close..." << endl;
        cin.get();
        return 1;
    }

    return 0;
}

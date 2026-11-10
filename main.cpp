#include <iostream>
#include <windows.h>
#include <filesystem>

#include "SpeechRecognizer/SpeechRecognizer.h"
#include "SpeechRecognizer/AudioPlayer.h"
#include "Exceptions/IExceptions.h"

using namespace std;

/*
    TODO LIST:
    1. Закончить перепись функций
    2. Добавить к каждым функциям описание (чтобы в будущем понимать, что за что отвечает)
    3. Выделить часто используемые функции в namespace и вынести в отдельную библиотеку-обработчик
    4. Подкорректировать exceptions
    5. Сделать программу более гибкой для модификаций
    6. Добавить интерфейс для подключения LLM модели
    7. Сделать автоматическое воспроизведение звука
*/

namespace {
const string modelName = "vosk-model-small-ru-0.22";

string resolveModelPath() {
    char modulePath[MAX_PATH]{};
    DWORD length = GetModuleFileNameA(nullptr, modulePath, MAX_PATH);
    if (length <= 0 || length >= MAX_PATH)
        throw FilePathException(FilePathException::messages::rootFileNotFound);

    filesystem::path exePath(modulePath);
    filesystem::path candidate = exePath.parent_path() / "models" / modelName;
    if (!filesystem::exists(candidate))
        throw FilePathException(FilePathException::messages::modelNotFound);

    return candidate.string();
}

string resolveResponsePath() {
    char modulePath[MAX_PATH]{};
    DWORD length = GetModuleFileNameA(nullptr, modulePath, MAX_PATH);

    if (length <= 0 || length >= MAX_PATH)
        throw FilePathException("RootFolder not found");

    filesystem::path projectRoot = filesystem::path(modulePath).parent_path().parent_path();
    filesystem::path candidate = projectRoot / "response.wav";
    
    if (!filesystem::exists(candidate))
        throw FilePathException("Answer not found");

    return candidate.string();
}

void waitForExit() {
    cout << "Speak... Press Ctrl+C to exit." << endl;

    while (true) {
        Sleep(1000);
    }
}

void setupConsole() {
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    cout.setf(std::ios::unitbuf);
    cerr.setf(std::ios::unitbuf);
}

} // namespace

int main() {
    int returnResult = 0;

    setupConsole();

    try {
        const string modelPath = resolveModelPath();
        cout << "[Startup] Model path: " << modelPath << endl;
        cout << "[Startup] Loading voice assistant..." << endl;

        SpeechRecognizer recognizer(modelPath);
        cout << "[Startup] Model loaded." << endl;

        AudioPlayer audioPlayer;
        if (!audioPlayer.isInitialized())
            throw AudioException(AudioException::messages::audioInitializeError);

        const string responsePath = resolveResponsePath();
        cout << "[Startup] Playing audio: " << responsePath << endl;
        if (!audioPlayer.playWav(responsePath.c_str()))
            throw AudioException(AudioException::messages::audioPlayError);

        recognizer.start();
        cout << "[Startup] Microphone started." << endl;

        waitForExit();

        recognizer.stop();
    } catch (const exception& ex) {
        returnResult = 1;
        cerr << "[Startup error] " << ex.what() << endl;
        cout << "Press any key to close..." << endl;
        cin.get();
    }

    return returnResult;
}

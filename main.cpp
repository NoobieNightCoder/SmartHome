#include <iostream>
#include <windows.h>
#include <filesystem>

#include "SpeechRecognizer/SpeechRecognizer.h"

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

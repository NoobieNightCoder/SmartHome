#pragma once

#include <miniaudio/miniaudio.h>
#include "vosk_api.h"
#include "../commands/ICommand.h"
#include <memory>
#include <unordered_map>
#include <vector>

#define SAMPLE_RATE 16000.0f

class SpeechRecognizer;

struct AudioUserData {
    SpeechRecognizer* owner{};
    VoskRecognizer* recognizer{};
};

struct ModelDeleter {
    void operator()(VoskModel* model) const {
        if (model)
            vosk_model_free(model);
    }
};

struct RecognizerDeleter {
    void operator()(VoskRecognizer* recognizer) const {
        if (recognizer)
            vosk_recognizer_free(recognizer);
    }
};

using ModelPtr = std::unique_ptr<VoskModel, ModelDeleter>;
using RecognizerPtr = std::unique_ptr<VoskRecognizer, RecognizerDeleter>;

class SpeechRecognizer {
private:
    ModelPtr model;
    RecognizerPtr recognizer;
    AudioUserData userData;
    ma_device device;
    bool deviceStarted{false};
    bool shutdownComplete{false};
    bool wakeWordArmed{false};
    std::unordered_map<std::string, std::unique_ptr<ICommand>> commandRegistry;

    bool initializeMicrophone();
    bool startRecording();
    std::string commandSplit(std::string text) const;
    std::string extractVoskText(const std::string& jsonText) const;
    bool stripWakeWord(const std::string& text, std::string& remainder) const;
    std::string findBestCommandId(const std::string& text, std::string& matchedPhrase, double& matchedScore) const;
    std::string stripMatchedPhrase(const std::string& text, const std::string& matchedPhrase) const;
    void buildCommandRegistry();
    void executeCommand(const std::string& commandId, const std::string& argument);
    void shutdown();
public:
    SpeechRecognizer(const std::string& path);
    void start();
    void stop();
    void handleFinalText(const std::string& jsonText);
    ~SpeechRecognizer();
};

#pragma once

#include <miniaudio/miniaudio.h>
#include "vosk_api.h"
#include "CommandProcessor.h"
#include "../commands/CommandDispatcher.h"

#include <condition_variable>
#include <deque>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

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
public:
    enum class State {
        Ready,
        Listening,
        Stopped
    };

private:
    static constexpr float SAMPLE_RATE = 16000.0f;
    static constexpr size_t MAX_PENDING_RESULTS = 16;

    ModelPtr model;
    RecognizerPtr recognizer;
    AudioUserData userData;
    ma_device device{};
    CommandProcessor commandProcessor;
    CommandDispatcher commandDispatcher;

    mutable std::mutex stateMutex;
    State state{State::Ready};
    bool deviceInitialized{false};

    std::mutex queueMutex;
    std::condition_variable queueCondition;
    std::deque<std::string> pendingResults;
    bool workerStopRequested{false};
    std::thread workerThread;

    bool initializeMicrophone();
    bool startRecording();
    void startWorker();
    void processResults();
    void processFinalText(const std::string& jsonText);
    std::string extractVoskText(const std::string& jsonText) const;
    void shutdown();
public:
    explicit SpeechRecognizer(const std::string& path);
    SpeechRecognizer(const SpeechRecognizer&) = delete;
    SpeechRecognizer& operator=(const SpeechRecognizer&) = delete;

    void start();
    void stop();
    void handleFinalText(const std::string& jsonText);
    State getState() const;
    ~SpeechRecognizer();
};

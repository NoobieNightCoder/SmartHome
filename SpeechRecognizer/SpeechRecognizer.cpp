#include "SpeechRecognizer.h"
#include "../Exceptions/IExceptions.h"
#include "../TextUtils.h"

#include <iostream>

using namespace std;

namespace {
string jsonFieldValue(const string& jsonText, const string& fieldName) {
    string result;
    const string marker = "\"" + fieldName + "\"";
    size_t fieldPos = jsonText.find(marker);
    size_t valueStart = string::npos;

    if (fieldPos != string::npos) {
        size_t colonPos = jsonText.find(':', fieldPos + marker.size());
        if (colonPos != string::npos)
            valueStart = jsonText.find('"', colonPos + 1);
    }

    if (valueStart != string::npos) {
        ++valueStart;
        bool escaped = false;

        for (size_t i = valueStart; i < jsonText.size(); ++i) {
            char ch = jsonText[i];
            if (escaped) {
                result.push_back(ch);
                escaped = false;
            } else if (ch == '\\') {
                escaped = true;
            } else if (ch == '"') {
                break;
            } else {
                result.push_back(ch);
            }
        }
    }

    return result;
}
} // namespace

static void audio_callback(ma_device* device, void* output, const void* input, ma_uint32 frameCount) {
    auto* userData = static_cast<AudioUserData*>(device->pUserData);

    if (userData && userData->recognizer && input) {
        int byteCount = static_cast<int>(frameCount * sizeof(int16_t));

        if (vosk_recognizer_accept_waveform(userData->recognizer, static_cast<const char*>(input), byteCount)) {
            const char* rawResult = vosk_recognizer_result(userData->recognizer);
            if (userData->owner && rawResult)
                userData->owner->handleFinalText(rawResult);
            vosk_recognizer_reset(userData->recognizer);
        }
    }

    (void)output;
}

SpeechRecognizer::SpeechRecognizer(const string& path) {
    ModelPtr newModel(vosk_model_new(path.c_str()));
    if (!newModel)
        throw RecognitionException(RecognitionException::messages::modelLoadError);
    model = move(newModel);

    RecognizerPtr newRecognizer(vosk_recognizer_new(model.get(), SAMPLE_RATE));
    if (!newRecognizer)
        throw RecognitionException(RecognitionException::messages::recognizerFail);
    recognizer = move(newRecognizer);
    vosk_recognizer_set_max_alternatives(recognizer.get(), 0);

    userData.owner = this;
    userData.recognizer = recognizer.get();
}

bool SpeechRecognizer::initializeMicrophone() {
    ma_device_config config = ma_device_config_init(ma_device_type_capture);
    config.capture.format = ma_format_s16;
    config.capture.channels = 1;
    config.sampleRate = SAMPLE_RATE;
    config.dataCallback = audio_callback;
    config.pUserData = &userData;

    deviceInitialized = ma_device_init(nullptr, &config, &device) == MA_SUCCESS;
    return deviceInitialized;
}

bool SpeechRecognizer::startRecording() {
    return ma_device_start(&device) == MA_SUCCESS;
}

void SpeechRecognizer::startWorker() {
    workerStopRequested = false;
    workerThread = thread(&SpeechRecognizer::processResults, this);
}

void SpeechRecognizer::handleFinalText(const string& jsonText) {
    lock_guard<mutex> lock(queueMutex);

    if (!workerStopRequested) {
        if (pendingResults.size() >= MAX_PENDING_RESULTS)
            pendingResults.pop_front();
        pendingResults.push_back(jsonText);
        queueCondition.notify_one();
    }
}

void SpeechRecognizer::processResults() {
    bool shouldProcess = true;

    while (shouldProcess) {
        string jsonText;

        {
            unique_lock<mutex> lock(queueMutex);
            queueCondition.wait(lock, [this] {
                return workerStopRequested || !pendingResults.empty();
            });

            if (workerStopRequested) {
                pendingResults.clear();
                shouldProcess = false;
            } else if (!pendingResults.empty()) {
                jsonText = move(pendingResults.front());
                pendingResults.pop_front();
            }
        }

        if (!jsonText.empty())
            processFinalText(jsonText);
    }
}

string SpeechRecognizer::extractVoskText(const string& jsonText) const {
    return TextUtils::trimCopy(jsonFieldValue(jsonText, "text"));
}

void SpeechRecognizer::processFinalText(const string& jsonText) {
    string recognizedText = extractVoskText(jsonText);

    if (!recognizedText.empty()) {
        string normalizedText = TextUtils::normalizeUtf8Text(recognizedText);
        cout << "[Recognized] " << normalizedText << endl;

        CommandRequest request = commandProcessor.process(recognizedText);
        if (request.wakeWordDetected && !request.commandFound)
            cout << "[VoiceAssistant] Wake word detected, listening for command." << endl;
        else if (request.commandFound) {
            cout << "[VoiceAssistant] Matched command: " << request.commandId
                 << " (" << request.matchedScore << "%)" << endl;

            CommandResult result = commandDispatcher.execute(request.commandId, request.argument);
            if (!result.message.empty())
                cout << result.message << endl;
        } else {
            cout << "[VoiceAssistant] Command not recognized." << endl;
        }
    }
}

void SpeechRecognizer::start() {
    {
        lock_guard<mutex> lock(stateMutex);
        if (state != State::Ready)
            throw RecognitionException("Speech recognizer is not ready to start.");
    }

    if (!initializeMicrophone())
        throw RecognitionException(RecognitionException::messages::microphoneInitializeError);

    startWorker();
    if (!startRecording()) {
        ma_device_uninit(&device);
        deviceInitialized = false;
        {
            lock_guard<mutex> lock(queueMutex);
            workerStopRequested = true;
        }
        queueCondition.notify_all();
        workerThread.join();
        throw RecognitionException(RecognitionException::messages::recordFail);
    }

    {
        lock_guard<mutex> lock(stateMutex);
        state = State::Listening;
    }
    cout << "Ready to listen!" << endl;
}

void SpeechRecognizer::shutdown() {
    bool shouldShutdown = false;

    {
        lock_guard<mutex> lock(stateMutex);
        if (state != State::Stopped) {
            shouldShutdown = true;
            state = State::Stopped;
        }
    }

    if (shouldShutdown) {
        if (deviceInitialized) {
            ma_device_stop(&device);
            ma_device_uninit(&device);
            deviceInitialized = false;
        }

        {
            lock_guard<mutex> lock(queueMutex);
            workerStopRequested = true;
        }
        queueCondition.notify_all();

        if (workerThread.joinable())
            workerThread.join();
    }
}

void SpeechRecognizer::stop() {
    shutdown();
}

SpeechRecognizer::State SpeechRecognizer::getState() const {
    lock_guard<mutex> lock(stateMutex);
    return state;
}

SpeechRecognizer::~SpeechRecognizer() {
    shutdown();
}

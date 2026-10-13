#include "SpeechRecognizer.h"
#include "../commands/SimpleOpenCommands.h"
#include "../SpeechRecognizer/config.hpp"
#include "../TextUtils.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <sstream>
#include <vector>

#include <rapidfuzz/fuzz.hpp>

using namespace std;

namespace {
constexpr double WAKE_WORD_THRESHOLD = 88.0;
constexpr double COMMAND_MATCH_THRESHOLD = 78.0;

string trimCopy(string text) {
    auto notSpace = [](unsigned char ch) {
        return !std::isspace(ch);
    };

    text.erase(text.begin(), find_if(text.begin(), text.end(), notSpace));
    text.erase(find_if(text.rbegin(), text.rend(), notSpace).base(), text.end());
    return text;
}

string collapseSpaces(string text) {
    string result;
    bool inSpace = false;

    for (unsigned char ch : text) {
        if (std::isspace(ch)) {
            if (!inSpace) {
                result.push_back(' ');
                inSpace = true;
            }
        } else {
            result.push_back(static_cast<char>(ch));
            inSpace = false;
        }
    }

    return trimCopy(result);
}

vector<string> tokenize(const string& text) {
    istringstream stream(text);
    vector<string> tokens;
    string token;

    while (stream >> token) {
        tokens.push_back(token);
    }

    return tokens;
}

string joinTokens(const vector<string>& tokens, size_t startIndex) {
    if (startIndex >= tokens.size())
        return {};

    ostringstream stream;
    for (size_t i = startIndex; i < tokens.size(); ++i) {
        if (i > startIndex)
            stream << ' ';
        stream << tokens[i];
    }
    return stream.str();
}

string jsonFieldValue(const string& jsonText, const string& fieldName) {
    const string marker = "\"" + fieldName + "\"";
    size_t fieldPos = jsonText.find(marker);
    if (fieldPos == string::npos)
        return {};

    size_t colonPos = jsonText.find(':', fieldPos + marker.size());
    if (colonPos == string::npos)
        return {};

    size_t valueStart = jsonText.find('"', colonPos + 1);
    if (valueStart == string::npos)
        return {};

    ++valueStart;
    string value;
    bool escaped = false;

    for (size_t i = valueStart; i < jsonText.size(); ++i) {
        char ch = jsonText[i];
        if (escaped) {
            value.push_back(ch);
            escaped = false;
            continue;
        }
        if (ch == '\\') {
            escaped = true;
            continue;
        }
        if (ch == '"')
            break;
        value.push_back(ch);
    }

    return value;
}
} // namespace

static void audio_callback(ma_device* device, void* output, const void* input, ma_uint32 frameCount) {
    auto* userData = static_cast<AudioUserData*>(device->pUserData);

    if (!userData || !input)
        return;

    int byteCount = static_cast<int>(frameCount * sizeof(int16_t));

    if (vosk_recognizer_accept_waveform(userData->recognizer, static_cast<const char*>(input), byteCount)) {
        const char* rawResult = vosk_recognizer_result(userData->recognizer);
        if (userData->owner && rawResult) {
            userData->owner->handleFinalText(rawResult);
        }

        vosk_recognizer_reset(userData->recognizer);
    }

    (void)output;
}

bool SpeechRecognizer::initializeMicrophone() {
    ma_device_config config = ma_device_config_init(ma_device_type_capture);

    userData.owner = this;
    userData.recognizer = recognizer.get();

    config.capture.format = ma_format_s16;
    config.capture.channels = 1;
    config.sampleRate = SAMPLE_RATE;
    config.dataCallback = audio_callback;
    config.pUserData = &userData;

    return ma_device_init(nullptr, &config, &device) == MA_SUCCESS;
}

bool SpeechRecognizer::startRecording() {
    if (ma_device_start(&device) != MA_SUCCESS)
        return false;

    deviceStarted = true;
    return true;
}

string SpeechRecognizer::commandSplit(string text) const {
    return normalizeUtf8Text(text);
}

string SpeechRecognizer::extractVoskText(const string& jsonText) const {
    string text = jsonFieldValue(jsonText, "text");
    return trimCopy(text);
}

bool SpeechRecognizer::stripWakeWord(const string& text, string& remainder) const {
    vector<string> tokens = tokenize(text);
    for (size_t i = 0; i < tokens.size(); ++i) {
        double score = std::max(rapidfuzz::fuzz::WRatio(tokens[i], VA_PREFIX),
                                rapidfuzz::fuzz::partial_ratio(tokens[i], VA_PREFIX));
        if (score >= WAKE_WORD_THRESHOLD) {
            remainder = joinTokens(tokens, i + 1);
            return true;
        }
    }

    double fullScore = std::max(rapidfuzz::fuzz::WRatio(text, VA_PREFIX),
                                rapidfuzz::fuzz::partial_ratio(text, VA_PREFIX));
    if (fullScore >= WAKE_WORD_THRESHOLD) {
        remainder.clear();
        return true;
    }

    remainder.clear();
    return false;
}

string SpeechRecognizer::findBestCommandId(const string& text, string& matchedPhrase, double& matchedScore) const {
    string bestCommandId;
    matchedPhrase.clear();
    matchedScore = 0.0;

    for (const auto& command : VA_CMD_LIST()) {
        for (const auto& phrase : command.phrases) {
            string normalizedPhrase = commandSplit(phrase);
            double score = std::max({rapidfuzz::fuzz::WRatio(text, normalizedPhrase),
                                     rapidfuzz::fuzz::token_set_ratio(text, normalizedPhrase),
                                     rapidfuzz::fuzz::partial_ratio(text, normalizedPhrase)});

            if (score > matchedScore) {
                matchedScore = score;
                matchedPhrase = normalizedPhrase;
                bestCommandId = command.id;
            }
        }
    }

    if (matchedScore < COMMAND_MATCH_THRESHOLD) {
        matchedPhrase.clear();
        return {};
    }

    return bestCommandId;
}

string SpeechRecognizer::stripMatchedPhrase(const string& text, const string& matchedPhrase) const {
    if (matchedPhrase.empty())
        return text;

    if (text == matchedPhrase)
        return {};

    if (text.rfind(matchedPhrase + " ", 0) == 0)
        return trimCopy(text.substr(matchedPhrase.size() + 1));

    size_t phrasePos = text.find(matchedPhrase);
    if (phrasePos == string::npos)
        return {};

    string result = text;
    result.erase(phrasePos, matchedPhrase.size());
    return trimCopy(result);
}

void SpeechRecognizer::buildCommandRegistry() {
    commandRegistry.clear();

    commandRegistry.emplace("ctime", make_unique<showTime>());
    commandRegistry.emplace("open_browser", make_unique<openGoogle>());
    commandRegistry.emplace("open_calculator", make_unique<openCalculator>());
    commandRegistry.emplace("open_explorer", make_unique<openExplorer>());
    commandRegistry.emplace("open_notepad", make_unique<openNotepad>());
    commandRegistry.emplace("open_discord", make_unique<openDiscord>());
    commandRegistry.emplace("open_telegram", make_unique<openTelegram>());
    commandRegistry.emplace("open_settings", make_unique<openSettings>());
    commandRegistry.emplace("open_calendar", make_unique<openCalendar>());
    commandRegistry.emplace("open_vsc", make_unique<openVSC>());
    commandRegistry.emplace("open_steam", make_unique<openSteam>());
    commandRegistry.emplace("open_obsidian", make_unique<openObsidian>());
    commandRegistry.emplace("open_phonelink", make_unique<openPhoneLink>());
    commandRegistry.emplace("open_osu", make_unique<openOsu>());
    commandRegistry.emplace("open_osu_laser", make_unique<openOsuLaser>());
    commandRegistry.emplace("open_minecraft", make_unique<openMinecraft>());
    commandRegistry.emplace("open_kovaaks", make_unique<openKovaaks>());
    commandRegistry.emplace("open_cs2", make_unique<openCS2>());
    commandRegistry.emplace("google_search", make_unique<googleSearch>());
    commandRegistry.emplace("append_planList", make_unique<appendPlanList>());
    commandRegistry.emplace("read_planList", make_unique<readPlanList>());
}

void SpeechRecognizer::executeCommand(const string& commandId, const string& argument) {
    auto it = commandRegistry.find(commandId);
    if (it == commandRegistry.end()) {
        cout << "[VoiceAssistant] Command object not found for id: " << commandId << endl;
        return;
    }

    it->second->execute(argument);
}

void SpeechRecognizer::handleFinalText(const string& jsonText) {
    string recognizedText = extractVoskText(jsonText);
    string normalizedText = commandSplit(recognizedText);

    if (normalizedText.empty())
        return;

    cout << "[Recognized] " << normalizedText << endl;

    string commandText = normalizedText;
    string wakeRemainder;

    if (!wakeWordArmed) {
        if (!stripWakeWord(normalizedText, wakeRemainder))
            return;

        wakeWordArmed = true;
        commandText = wakeRemainder;
        if (commandText.empty()) {
            cout << "[VoiceAssistant] Wake word detected, listening for command." << endl;
            return;
        }
    }

    string matchedPhrase;
    double matchedScore = 0.0;
    string commandId = findBestCommandId(commandText, matchedPhrase, matchedScore);

    if (commandId.empty()) {
        cout << "[VoiceAssistant] Command not recognized." << endl;
        wakeWordArmed = false;
        return;
    }

    string argument = stripMatchedPhrase(commandText, matchedPhrase);
    cout << "[VoiceAssistant] Matched command: " << commandId << " (" << matchedScore << "%)" << endl;

    executeCommand(commandId, argument);
    wakeWordArmed = false;
}

void SpeechRecognizer::shutdown() {
    if (shutdownComplete)
        return;

    if (deviceStarted) {
        ma_device_stop(&device);
        ma_device_uninit(&device);
        deviceStarted = false;
    }

    cout << "\n[Final result]:\n" << vosk_recognizer_final_result(recognizer.get()) << endl;
    shutdownComplete = true;
}

SpeechRecognizer::SpeechRecognizer(const std::string& path) {
    ModelPtr newModel(vosk_model_new(path.c_str()));
    if (!newModel)
        throw runtime_error("Failed to load Vosk model.");
    model = move(newModel);

    RecognizerPtr newRecognizer(vosk_recognizer_new(model.get(), SAMPLE_RATE));
    if (!newRecognizer)
        throw runtime_error("Failed to create recognizer.");
    recognizer = move(newRecognizer);
    vosk_recognizer_set_max_alternatives(recognizer.get(), 0);

    userData.owner = this;
    userData.recognizer = recognizer.get();

    buildCommandRegistry();
}

void SpeechRecognizer::start() {
    try {
        bool microStatus = initializeMicrophone();
        if (!microStatus)
            throw runtime_error("Failed to initialize microphone!");

        bool recordingStatus = startRecording();
        if (!recordingStatus)
            throw runtime_error("Failed to start recording!");

        cout << "[VoiceAssistant] Listening for wake word: " << VA_PREFIX << endl;
    } catch (const exception& ex) {
        cout << ex.what() << endl;
    }
}

void SpeechRecognizer::stop() {
    shutdown();
}

SpeechRecognizer::~SpeechRecognizer() {
    shutdown();
}

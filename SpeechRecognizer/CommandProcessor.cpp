#include "CommandProcessor.h"
#include "../SpeechRecognizer/config.hpp"
#include "../TextUtils.h"

#include <algorithm>

#include <rapidfuzz/fuzz.hpp>

using namespace std;

namespace {
const double WAKE_WORD_THRESHOLD = 88.0;
const double COMMAND_MATCH_THRESHOLD = 78.0;
const chrono::seconds WAKE_WORD_TIMEOUT(5);
}

bool CommandProcessor::stripWakeWord(const string& text, string& remainder) const {
    bool result = false;
    vector<string> tokens = TextUtils::tokenize(text);

    for (size_t i = 0; i < tokens.size(); ++i) {
        double score = max(rapidfuzz::fuzz::WRatio(tokens[i], VA_PREFIX),
                           rapidfuzz::fuzz::partial_ratio(tokens[i], VA_PREFIX));
        if (score >= WAKE_WORD_THRESHOLD) {
            remainder = TextUtils::joinTokens(tokens, i + 1);
            result = true;
        }
    }

    if (!result) {
        double fullScore = max(rapidfuzz::fuzz::WRatio(text, VA_PREFIX),
                               rapidfuzz::fuzz::partial_ratio(text, VA_PREFIX));
        if (fullScore >= WAKE_WORD_THRESHOLD)
            result = true;
        remainder.clear();
    }

    return result;
}

string CommandProcessor::findBestCommandId(const string& text, string& matchedPhrase, double& matchedScore) const {
    string bestCommandId;
    matchedPhrase.clear();
    matchedScore = 0.0;

    for (const auto& command : VA_CMD_LIST()) {
        for (const auto& phrase : command.phrases) {
            string normalizedPhrase = TextUtils::normalizeUtf8Text(phrase);
            double score = max({rapidfuzz::fuzz::WRatio(text, normalizedPhrase),
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
        bestCommandId.clear();
        matchedPhrase.clear();
    }

    return bestCommandId;
}

string CommandProcessor::stripMatchedPhrase(const string& text, const string& matchedPhrase) const {
    string result;

    if (matchedPhrase.empty())
        result = text;
    else if (text != matchedPhrase) {
        if (text.rfind(matchedPhrase + " ", 0) == 0)
            result = TextUtils::trimCopy(text.substr(matchedPhrase.size() + 1));
        else {
            size_t phrasePos = text.find(matchedPhrase);
            if (phrasePos != string::npos) {
                result = text;
                result.erase(phrasePos, matchedPhrase.size());
                result = TextUtils::trimCopy(result);
            }
        }
    }

    return result;
}

bool CommandProcessor::wakeWordTimedOut() const {
    return wakeWordArmed && chrono::steady_clock::now() - wakeWordDetectedAt >= WAKE_WORD_TIMEOUT;
}

CommandRequest CommandProcessor::process(const string& recognizedText) {
    CommandRequest request;
    string commandText = TextUtils::normalizeUtf8Text(recognizedText);

    if (wakeWordTimedOut())
        wakeWordArmed = false;

    if (!commandText.empty()) {
        string wakeRemainder;

        if (!wakeWordArmed) {
            if (stripWakeWord(commandText, wakeRemainder)) {
                wakeWordArmed = true;
                wakeWordDetectedAt = chrono::steady_clock::now();
                request.wakeWordDetected = true;
                commandText = wakeRemainder;
            } else {
                commandText.clear();
            }
        }

        if (!commandText.empty() && wakeWordArmed) {
            string matchedPhrase;
            request.commandId = findBestCommandId(commandText, matchedPhrase, request.matchedScore);
            request.commandFound = !request.commandId.empty();
            request.argument = stripMatchedPhrase(commandText, matchedPhrase);
            wakeWordArmed = false;
        }
    }

    return request;
}

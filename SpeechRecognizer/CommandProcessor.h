#pragma once

#include <chrono>
#include <string>

struct CommandRequest {
    bool wakeWordDetected{false};
    bool commandFound{false};
    std::string commandId;
    std::string argument;
    double matchedScore{0.0};
};

class CommandProcessor {
private:
    bool wakeWordArmed{false};
    std::chrono::steady_clock::time_point wakeWordDetectedAt{};

    bool stripWakeWord(const std::string& text, std::string& remainder) const;
    std::string findBestCommandId(const std::string& text, std::string& matchedPhrase, double& matchedScore) const;
    std::string stripMatchedPhrase(const std::string& text, const std::string& matchedPhrase) const;
    bool wakeWordTimedOut() const;
public:
    CommandRequest process(const std::string& text);
};

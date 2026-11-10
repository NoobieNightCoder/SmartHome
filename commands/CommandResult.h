#pragma once

#include <string>
#include <utility>

struct CommandResult {
    bool success{false};
    std::string message;

    static CommandResult completed(std::string message = "") {
        return {true, std::move(message)};
    }

    static CommandResult failed(std::string message) {
        return {false, std::move(message)};
    }
};

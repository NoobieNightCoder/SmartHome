#pragma once

#include "ICommand.h"

#include <memory>
#include <string>
#include <unordered_map>

class CommandDispatcher {
private:
    std::unordered_map<std::string, std::unique_ptr<ICommand>> commandRegistry;

    void buildCommandRegistry();
public:
    CommandDispatcher();
    CommandResult execute(const std::string& commandId, const std::string& argument);
};

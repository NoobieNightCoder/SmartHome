#pragma once

#include <string>

#include "CommandResult.h"

class ICommand {
public:
    virtual CommandResult execute(const std::string& argument = "") = 0;
    virtual ~ICommand() = default;
};

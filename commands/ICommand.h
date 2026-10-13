#pragma once

#include <string>

class ICommand {
public:
    virtual void execute(const std::string& argument = "") = 0;
    virtual ~ICommand() = default;
};

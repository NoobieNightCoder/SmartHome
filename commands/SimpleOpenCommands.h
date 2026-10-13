#pragma once

#include "ICommand.h"

class openCalculator : public ICommand {
public:
    void execute(const std::string& argument = "") override;
};

class openExplorer : public ICommand {
public:
    void execute(const std::string& argument = "") override;
};

class openSettings : public ICommand {
public:
    void execute(const std::string& argument = "") override;
};

class openNotepad : public ICommand {
public:
    void execute(const std::string& argument = "") override;
};

class openCalendar : public ICommand {
public:
    void execute(const std::string& argument = "") override;
};

class openVSC : public ICommand {
public:
    void execute(const std::string& argument = "") override;
};

class openSteam : public ICommand {
public:
    void execute(const std::string& argument = "") override;
};

class openDiscord : public ICommand {
public:
    void execute(const std::string& argument = "") override;
};

class openObsidian : public ICommand {
public:
    void execute(const std::string& argument = "") override;
};

class openTelegram : public ICommand {
public:
    void execute(const std::string& argument = "") override;
};

class openGoogle : public ICommand {
public:
    void execute(const std::string& argument = "") override;
};

class openPhoneLink : public ICommand {
public:
    void execute(const std::string& argument = "") override;
};

class openOsu : public ICommand {
public:
    void execute(const std::string& argument = "") override;
};

class openOsuLaser : public ICommand {
public:
    void execute(const std::string& argument = "") override;
};

class openMinecraft : public ICommand {
public:
    void execute(const std::string& argument = "") override;
};

class openKovaaks : public ICommand {
public:
    void execute(const std::string& argument = "") override;
};

class openCS2 : public ICommand {
public:
    void execute(const std::string& argument = "") override;
};

class showTime : public ICommand {
public:
    void execute(const std::string& argument = "") override;
};

class googleSearch : public ICommand {
public:
    void execute(const std::string& argument = "") override;
};

class appendPlanList : public ICommand {
public:
    void execute(const std::string& argument = "") override;
};

class readPlanList : public ICommand {
public:
    void execute(const std::string& argument = "") override;
};

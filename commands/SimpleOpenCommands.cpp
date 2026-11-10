#include "SimpleOpenCommands.h"
#include "../SpeechRecognizer/config.hpp"
#include "../TextUtils.h"

#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

#include <windows.h>

using namespace std;

namespace {
string normalizeText(string text) {
    return TextUtils::normalizeUtf8Text(text);
}

CommandResult launch(const char* target) {
    HINSTANCE launchResult = ShellExecuteA(nullptr, "open", target, nullptr, nullptr, SW_SHOWNORMAL);
    intptr_t resultCode = reinterpret_cast<intptr_t>(launchResult);
    CommandResult result;

    if (resultCode > 32)
        result = CommandResult::completed();
    else
        result = CommandResult::failed("Failed to launch: " + string(target));

    return result;
}

string urlEncode(const string& text) {
    ostringstream encoded;
    encoded << hex << uppercase << setfill('0');

    for (unsigned char ch : text) {
        if (isalnum(ch) || ch == '-' || ch == '_' || ch == '.' || ch == '~') {
            encoded << static_cast<char>(ch);
        } else if (ch == ' ') {
            encoded << '+';
        } else {
            encoded << '%' << setw(2) << static_cast<int>(ch);
        }
    }

    return encoded.str();
}

string currentTimeString() {
    time_t now = time(nullptr);
    tm localTime{};
    localtime_s(&localTime, &now);

    ostringstream stream;
    stream << put_time(&localTime, "%H:%M:%S");
    return stream.str();
}
} // namespace

CommandResult openCalculator::execute(const string& argument) {
    (void)argument;
    return launch("calc.exe");
}

CommandResult openExplorer::execute(const string& argument) {
    (void)argument;
    return launch("explorer.exe");
}

CommandResult openSettings::execute(const string& argument) {
    (void)argument;
    return launch(VA_PROGRAMM_PATHS().at("Settings").c_str());
}

CommandResult openNotepad::execute(const string& argument) {
    (void)argument;
    return launch("notepad.exe");
}

CommandResult openCalendar::execute(const string& argument) {
    (void)argument;
    return launch(VA_PROGRAMM_PATHS().at("Outlook").c_str());
}

CommandResult openVSC::execute(const string& argument) {
    (void)argument;
    return launch(VA_PROGRAMM_PATHS().at("VSC").c_str());
}

CommandResult openSteam::execute(const string& argument) {
    (void)argument;
    return launch(VA_PROGRAMM_PATHS().at("Steam").c_str());
}

CommandResult openDiscord::execute(const string& argument) {
    (void)argument;
    return launch(VA_PROGRAMM_PATHS().at("Discord").c_str());
}

CommandResult openObsidian::execute(const string& argument) {
    (void)argument;
    return launch(VA_PROGRAMM_PATHS().at("Obsidian").c_str());
}

CommandResult openTelegram::execute(const string& argument) {
    (void)argument;
    return launch(VA_PROGRAMM_PATHS().at("Telegram").c_str());
}

CommandResult openGoogle::execute(const string& argument) {
    (void)argument;
    return launch(VA_PROGRAMM_PATHS().at("Google Chrome").c_str());
}

CommandResult openPhoneLink::execute(const string& argument) {
    (void)argument;
    return launch(VA_PROGRAMM_PATHS().at("Phone Link").c_str());
}

CommandResult openOsu::execute(const string& argument) {
    (void)argument;
    return launch(VA_PROGRAMM_PATHS().at("Osu").c_str());
}

CommandResult openOsuLaser::execute(const string& argument) {
    (void)argument;
    return launch(VA_PROGRAMM_PATHS().at("Osu Laser").c_str());
}

CommandResult openMinecraft::execute(const string& argument) {
    (void)argument;
    return launch(VA_PROGRAMM_PATHS().at("Minecraft").c_str());
}

CommandResult openKovaaks::execute(const string& argument) {
    (void)argument;
    return launch(VA_PROGRAMM_PATHS().at("Kovaaks").c_str());
}

CommandResult openCS2::execute(const string& argument) {
    (void)argument;
    return launch(VA_PROGRAMM_PATHS().at("CS2").c_str());
}

CommandResult showTime::execute(const string& argument) {
    (void)argument;
    return CommandResult::completed("[Time] " + currentTimeString());
}

CommandResult googleSearch::execute(const string& argument) {
    string query = normalizeText(argument);
    string url = "https://www.google.com/search?q=";

    if (!query.empty()) {
        url += urlEncode(query);
    }

    return launch(url.c_str());
}

CommandResult appendPlanList::execute(const string& argument) {
    string entry = normalizeText(argument);
    CommandResult result;

    if (!entry.empty()) {
        ofstream file(PLANLIST_PATH, ios::app);
        
        if (file.is_open()) {
            file << entry << '\n';
            result = CommandResult::completed("[PlanList] Added: " + entry);
        } else {
            result = CommandResult::failed("[PlanList] Failed to open file for writing: " + string(PLANLIST_PATH));
        }
    } else {
        result = CommandResult::failed("[PlanList] Empty entry, nothing to append.");
    }

    return result;
}

CommandResult readPlanList::execute(const string& argument) {
    (void)argument;
    CommandResult result;

    ifstream file(PLANLIST_PATH);
    
    if (file.is_open()) {
        string line;
        ostringstream content;
        content << "[PlanList]";

        while (getline(file, line)) {
            content << "\n - " << line;
        }
        result = CommandResult::completed(content.str());
    } else {
        result = CommandResult::failed("[PlanList] Failed to open file for reading: " + string(PLANLIST_PATH));
    }

    return result;
}

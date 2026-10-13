#include "SimpleOpenCommands.h"
#include "../SpeechRecognizer/config.hpp"
#include "../TextUtils.h"

#include <algorithm>
#include <cctype>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

#include <windows.h>
#include <shellapi.h>

using namespace std;

namespace {
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
        if (isspace(ch)) {
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

string normalizeText(string text) {
    return normalizeUtf8Text(text);
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

void openCalculator::execute(const string& argument) {
    (void)argument;
    ShellExecute(NULL, "open", "calc.exe", NULL, NULL, SW_SHOWNORMAL);
}

void openExplorer::execute(const string& argument) {
    (void)argument;
    ShellExecute(NULL, "open", "explorer.exe", NULL, NULL, SW_SHOWNORMAL);
}

void openSettings::execute(const string& argument) {
    (void)argument;
    ShellExecuteA(NULL, "open", VA_PROGRAMM_PATHS().at("Settings").c_str(), NULL, NULL, SW_SHOWNORMAL);
}

void openNotepad::execute(const string& argument) {
    (void)argument;
    ShellExecute(NULL, "open", "notepad.exe", NULL, NULL, SW_SHOWNORMAL);
}

void openCalendar::execute(const string& argument) {
    (void)argument;
    ShellExecuteA(NULL, "open", VA_PROGRAMM_PATHS().at("Outlook").c_str(), NULL, NULL, SW_SHOWNORMAL);
}

void openVSC::execute(const string& argument) {
    (void)argument;
    ShellExecuteA(NULL, "open", VA_PROGRAMM_PATHS().at("VSC").c_str(), NULL, NULL, SW_SHOWNORMAL);
}

void openSteam::execute(const string& argument) {
    (void)argument;
    ShellExecuteA(NULL, "open", VA_PROGRAMM_PATHS().at("Steam").c_str(), NULL, NULL, SW_SHOWNORMAL);
}

void openDiscord::execute(const string& argument) {
    (void)argument;
    ShellExecuteA(NULL, "open", VA_PROGRAMM_PATHS().at("Discord").c_str(), NULL, NULL, SW_SHOWNORMAL);
}

void openObsidian::execute(const string& argument) {
    (void)argument;
    ShellExecuteA(NULL, "open", VA_PROGRAMM_PATHS().at("Obsidian").c_str(), NULL, NULL, SW_SHOWNORMAL);
}

void openTelegram::execute(const string& argument) {
    (void)argument;
    ShellExecuteA(NULL, "open", VA_PROGRAMM_PATHS().at("Telegram").c_str(), NULL, NULL, SW_SHOWNORMAL);
}

void openGoogle::execute(const string& argument) {
    (void)argument;
    ShellExecuteA(NULL, "open", VA_PROGRAMM_PATHS().at("Google Chrome").c_str(), NULL, NULL, SW_SHOWNORMAL);
}

void openPhoneLink::execute(const string& argument) {
    (void)argument;
    ShellExecuteA(NULL, "open", VA_PROGRAMM_PATHS().at("Phone Link").c_str(), NULL, NULL, SW_SHOWNORMAL);
}

void openOsu::execute(const string& argument) {
    (void)argument;
    ShellExecuteA(NULL, "open", VA_PROGRAMM_PATHS().at("Osu").c_str(), NULL, NULL, SW_SHOWNORMAL);
}

void openOsuLaser::execute(const string& argument) {
    (void)argument;
    ShellExecuteA(NULL, "open", VA_PROGRAMM_PATHS().at("Osu Laser").c_str(), NULL, NULL, SW_SHOWNORMAL);
}

void openMinecraft::execute(const string& argument) {
    (void)argument;
    ShellExecuteA(NULL, "open", VA_PROGRAMM_PATHS().at("Minecraft").c_str(), NULL, NULL, SW_SHOWNORMAL);
}

void openKovaaks::execute(const string& argument) {
    (void)argument;
    ShellExecuteA(NULL, "open", VA_PROGRAMM_PATHS().at("Kovaaks").c_str(), NULL, NULL, SW_SHOWNORMAL);
}

void openCS2::execute(const string& argument) {
    (void)argument;
    ShellExecuteA(NULL, "open", VA_PROGRAMM_PATHS().at("CS2").c_str(), NULL, NULL, SW_SHOWNORMAL);
}

void showTime::execute(const string& argument) {
    (void)argument;
    cout << "[Time] " << currentTimeString() << endl;
}

void googleSearch::execute(const string& argument) {
    string query = normalizeText(argument);
    string url = "https://www.google.com/search?q=";

    if (!query.empty()) {
        url += urlEncode(query);
    }

    ShellExecuteA(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
}

void appendPlanList::execute(const string& argument) {
    string entry = normalizeText(argument);
    if (entry.empty()) {
        cout << "[PlanList] Empty entry, nothing to append." << endl;
        return;
    }

    ofstream file(PLANLIST_PATH, ios::app);
    if (!file.is_open()) {
        cout << "[PlanList] Failed to open file for writing: " << PLANLIST_PATH << endl;
        return;
    }

    file << entry << '\n';
    cout << "[PlanList] Added: " << entry << endl;
}

void readPlanList::execute(const string& argument) {
    (void)argument;

    ifstream file(PLANLIST_PATH);
    if (!file.is_open()) {
        cout << "[PlanList] Failed to open file for reading: " << PLANLIST_PATH << endl;
        return;
    }

    string line;
    cout << "[PlanList]" << endl;

    while (getline(file, line)) {
        cout << " - " << line << endl;
    }
}

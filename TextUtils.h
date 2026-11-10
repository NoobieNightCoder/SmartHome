#pragma once

#include <algorithm>
#include <cctype>
#include <cwctype>
#include <sstream>
#include <string>
#include <vector>
#include <windows.h>

namespace TextUtils {

inline std::string trimCopy(std::string text) {
    auto notSpace = [](unsigned char ch) {
        return !std::isspace(ch);
    };

    text.erase(text.begin(), std::find_if(text.begin(), text.end(), notSpace));
    text.erase(std::find_if(text.rbegin(), text.rend(), notSpace).base(), text.end());
    return text;
}

inline std::string collapseSpaces(std::string text) {
    std::string result;
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

inline std::vector<std::string> tokenize(const std::string& text) {
    std::istringstream stream(text);
    std::vector<std::string> tokens;
    std::string token;

    while (stream >> token) {
        tokens.push_back(token);
    }

    return tokens;
}

inline std::string joinTokens(const std::vector<std::string>& tokens, size_t startIndex) {
    std::string result;

    for (size_t i = startIndex; i < tokens.size(); ++i) {
        if (!result.empty())
            result += ' ';
        result += tokens[i];
    }

    return result;
}

inline std::wstring utf8ToWide(const std::string& text) {
    if (text.empty())
        return {};

    int required = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text.c_str(),
                                       static_cast<int>(text.size()), nullptr, 0);
    if (required <= 0)
        return {};

    std::wstring wide(static_cast<size_t>(required), L'\0');
    MultiByteToWideChar(CP_UTF8, 0, text.c_str(), static_cast<int>(text.size()), wide.data(), required);
    return wide;
}

inline std::string wideToUtf8(const std::wstring& text) {
    if (text.empty())
        return {};

    int required = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), static_cast<int>(text.size()), nullptr, 0, nullptr, nullptr);
    if (required <= 0)
        return {};

    std::string utf8(static_cast<size_t>(required), '\0');
    WideCharToMultiByte(CP_UTF8, 0, text.c_str(), static_cast<int>(text.size()), utf8.data(), required, nullptr, nullptr);
    return utf8;
}

inline std::string normalizeUtf8Text(const std::string& text) {
    std::wstring wide = utf8ToWide(text);
    if (!wide.empty()) {
        CharLowerBuffW(wide.data(), static_cast<DWORD>(wide.size()));

        std::wstring normalized;
        normalized.reserve(wide.size());
        bool inSpace = false;

        for (wchar_t ch : wide) {
            if (ch == L'.' || ch == L',' || ch == L'!' || ch == L'?' || ch == L';' || ch == L':' || ch == L'"')
                ch = L' ';

            if (std::iswspace(ch)) {
                if (!inSpace) {
                    normalized.push_back(L' ');
                    inSpace = true;
                }
            } else {
                normalized.push_back(ch);
                inSpace = false;
            }
        }

        std::wstring::size_type first = normalized.find_first_not_of(L' ');
        std::wstring::size_type last = normalized.find_last_not_of(L' ');
        if (first == std::wstring::npos)
            normalized.clear();
        else
            normalized = normalized.substr(first, last - first + 1);

        return wideToUtf8(normalized);
    }

    std::string ascii = text;
    std::transform(ascii.begin(), ascii.end(), ascii.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return ascii;
}

} // namespace TextUtils

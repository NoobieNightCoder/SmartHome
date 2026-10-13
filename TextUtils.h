#pragma once

#include <algorithm>
#include <cwctype>
#include <string>
#include <windows.h>

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

        while (!normalized.empty() && normalized.front() == L' ')
            normalized.erase(normalized.begin());
        while (!normalized.empty() && normalized.back() == L' ')
            normalized.pop_back();

        return wideToUtf8(normalized);
    }

    std::string ascii = text;
    std::transform(ascii.begin(), ascii.end(), ascii.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return ascii;
}

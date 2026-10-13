#pragma once

#include <map>
#include <string>
#include <vector>

struct VoiceCommandSpec {
    std::string id;
    std::vector<std::string> phrases;
};

inline constexpr const char* VA_NAME = "Ксения";
inline constexpr const char* VA_PREFIX = "ксения";
inline constexpr float VA_VER = 0.3f;
inline constexpr const char* PLANLIST_PATH = "C:/Users/Noobie/Documents/PlanList.txt";

inline const std::vector<std::string>& VA_TBR() {
    static const std::vector<std::string> value = {
        "скажи",
        "покажи",
        "ответь",
        "произнеси",
        "расскажи",
        "сколько",
    };
    return value;
}

inline const std::vector<VoiceCommandSpec>& VA_CMD_LIST() {
    static const std::vector<VoiceCommandSpec> value = {
        {"ctime", {"время", "текущее время", "сейчас времени", "который час"}},
        {"open_browser", {"открой браузер", "запусти браузер", "открой гугл хром", "гугл хром"}},
        {"open_calculator", {"открой калькулятор", "запусти калькулятор"}},
        {"open_explorer", {"открой проводник", "запусти проводник"}},
        {"open_notepad", {"открой блокнот", "запусти блокнот"}},
        {"open_discord", {"открой дискорд", "запусти дискорд"}},
        {"open_telegram", {"открой телеграмм", "запусти телеграмм"}},
        {"open_settings", {"открой настройки", "запусти настройки"}},
        {"open_calendar", {"открой календарь", "запусти календарь"}},
        {"open_vsc", {"открой студио код", "запусти студио код"}},
        {"open_steam", {"открой стим", "запусти стим"}},
        {"open_obsidian", {"открой обсидиан", "запусти обсидиан"}},
        {"open_phonelink", {"открой связь с телефоном", "запусти связь с телефоном"}},
        {"open_osu", {"открой осу", "запусти осу"}},
        {"open_osu_laser", {"открой осу лизер", "запусти осу лизер"}},
        {"open_minecraft", {"открой майнкрафт", "запусти майнкрафт"}},
        {"open_kovaaks", {"открой ковак", "запусти ковак"}},
        {"open_cs2", {"открой каэс два", "запусти каэс два"}},
        {"google_search", {"загугли", "найди в интернете", "поищи в интернете", "забей в интернет"}},
        {"append_planList", {"запиши в планы", "добавь в планы"}},
        {"read_planList", {"прочитай планы", "какие планы"}},
    };
    return value;
}

inline const std::vector<std::string>& VA_ANSWER_VARIATIONS() {
    static const std::vector<std::string> value = {
        "Принято...",
        "Слушаюсь...",
        "Секунду...",
    };
    return value;
}

inline const std::map<std::string, std::string>& VA_PROGRAMM_PATHS() {
    static const std::map<std::string, std::string> value = {
        {"Google Chrome", "C:/Program Files/Google/Chrome/Application/chrome.exe %s"},
        {"Discord", "C:/Users/Noobie/AppData/Local/Discord/app-1.0.9220/Discord.exe"},
        {"Telegram", "C:/Users/Noobie/AppData/Roaming/Telegram Desktop/Telegram.exe"},
        {"VSC", "C:/Users/Noobie/AppData/Local/Programs/Microsoft VS Code/Code.exe"},
        {"Steam", "C:/Program Files (x86)/Steam/steam.exe"},
        {"Obsidian", "C:/Program Files/Obsidian/Obsidian.exe"},
        {"Phone Link", "start shell:AppsFolder\\Microsoft.YourPhone_8wekyb3d8bbwe!App"},
        {"Outlook", "start outlookcal:"},
        {"Settings", "start ms-settings:"},
        {"Osu", "steam://rungameid/14229185454355251200"},
        {"Osu Laser", "steam://rungameid/13173001637049925632"},
        {"Minecraft", "steam://rungameid/17658396628924497920"},
        {"Kovaaks", "steam://rungameid/824270"},
        {"CS2", "steam://rungameid/730"},
    };
    return value;
}

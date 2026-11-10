#include "CommandDispatcher.h"
#include "SimpleOpenCommands.h"

using namespace std;

CommandDispatcher::CommandDispatcher() {
    buildCommandRegistry();
}

void CommandDispatcher::buildCommandRegistry() {
    commandRegistry.emplace("ctime", make_unique<showTime>());
    commandRegistry.emplace("open_browser", make_unique<openGoogle>());
    commandRegistry.emplace("open_calculator", make_unique<openCalculator>());
    commandRegistry.emplace("open_explorer", make_unique<openExplorer>());
    commandRegistry.emplace("open_notepad", make_unique<openNotepad>());
    commandRegistry.emplace("open_discord", make_unique<openDiscord>());
    commandRegistry.emplace("open_telegram", make_unique<openTelegram>());
    commandRegistry.emplace("open_settings", make_unique<openSettings>());
    commandRegistry.emplace("open_calendar", make_unique<openCalendar>());
    commandRegistry.emplace("open_vsc", make_unique<openVSC>());
    commandRegistry.emplace("open_steam", make_unique<openSteam>());
    commandRegistry.emplace("open_obsidian", make_unique<openObsidian>());
    commandRegistry.emplace("open_phonelink", make_unique<openPhoneLink>());
    commandRegistry.emplace("open_osu", make_unique<openOsu>());
    commandRegistry.emplace("open_osu_laser", make_unique<openOsuLaser>());
    commandRegistry.emplace("open_minecraft", make_unique<openMinecraft>());
    commandRegistry.emplace("open_kovaaks", make_unique<openKovaaks>());
    commandRegistry.emplace("open_cs2", make_unique<openCS2>());
    commandRegistry.emplace("google_search", make_unique<googleSearch>());
    commandRegistry.emplace("append_planList", make_unique<appendPlanList>());
    commandRegistry.emplace("read_planList", make_unique<readPlanList>());
}

CommandResult CommandDispatcher::execute(const string& commandId, const string& argument) {
    auto command = commandRegistry.find(commandId);
    CommandResult result;

    if (command == commandRegistry.end())
        result = CommandResult::failed("Command object not found for id: " + commandId);
    else
        result = command->second->execute(argument);

    return result;
}

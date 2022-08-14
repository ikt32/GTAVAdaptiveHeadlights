#pragma once
#include <string>

class CScriptSettings
{
public:
    CScriptSettings(std::string settingsFile);

    void Load();
    void Save();

    struct {
        bool EnableNPC = true;
    } Main;

private:
    std::string mSettingsFile;
};

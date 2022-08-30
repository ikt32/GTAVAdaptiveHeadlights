#pragma once
#include "HeadlightsScript.hpp"
#include "ScriptMenu.hpp"

namespace AdaptiveHeadlights {
    void ScriptMain();

    std::vector<CScriptMenu<CHeadlightsScript>::CSubmenu> BuildMenu();

    CScriptSettings& GetSettings();
    const std::vector<std::shared_ptr<CHeadlightsScript>>& GetScripts();
    const std::vector<CConfig>& GetConfigs();

    uint32_t LoadConfigs();
    void SaveConfigs();
    void SwitchMode(bool playerOnly);
}

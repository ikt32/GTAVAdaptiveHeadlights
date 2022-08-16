#pragma once
#include "HeadlightsScript.hpp"
#include "ScriptMenu.hpp"

namespace AdaptiveHeadlights {
    void ScriptMain();
    void ScriptInit();
    void ScriptTick();
    void UpdateNPC();
    void UpdateActiveConfigs();
    std::vector<CScriptMenu<CHeadlightsScript>::CSubmenu> BuildMenu();

    CScriptSettings& GetSettings();
    CHeadlightsScript* GetScript();
    uint64_t GetNPCScriptCount();
    void ClearNPCScripts();
    const std::vector<CConfig>& GetConfigs();

    uint32_t LoadConfigs();
    void SaveConfigs();
}

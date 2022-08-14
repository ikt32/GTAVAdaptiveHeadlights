#pragma once
#include "ScriptSettings.hpp"
#include "Config.hpp"

#include <vector>
#include <string>

class CHeadlightsScript {
public:
    CHeadlightsScript(
        CScriptSettings& settings,
        std::vector<CConfig>& configs);
    virtual ~CHeadlightsScript();
    virtual void Tick();

    CConfig* ActiveConfig() {
        return mActiveConfig;
    }

    void UpdateActiveConfig(bool playerCheck);

    // Applies the passed config onto the current active config.
    void ApplyConfig(const CConfig& config);

    Vehicle GetVehicle() {
        return mVehicle;
    }

protected:
    void update();

    const CScriptSettings& mSettings;
    std::vector<CConfig>& mConfigs;
    CConfig mDefaultConfig;

    Vehicle mVehicle;
    CConfig* mActiveConfig;

    bool mIsNPC;
};

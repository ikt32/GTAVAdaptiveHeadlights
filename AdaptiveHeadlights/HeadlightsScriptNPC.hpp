#pragma once
#include "HeadlightsScript.hpp"

class CHeadlightsScriptNPC : public CHeadlightsScript {
public:
    CHeadlightsScriptNPC(
        Vehicle vehicle,
        CScriptSettings& settings,
        std::vector<CConfig>& configs
    );

    ~CHeadlightsScriptNPC();

    void Tick() override;

private:
    bool mPlayerModeActive;
};


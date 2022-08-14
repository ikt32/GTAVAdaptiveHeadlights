#include "HeadlightsScriptNPC.hpp"

CHeadlightsScriptNPC::CHeadlightsScriptNPC(
    Vehicle vehicle,
    CScriptSettings& settings,
    std::vector<CConfig>& configs)
    : CHeadlightsScript(settings, configs) {
    mIsNPC = true;
    mVehicle = vehicle;
}

void CHeadlightsScriptNPC::Tick() {
    // Nope
}

#include "HeadlightsScriptNPC.hpp"

#include <inc/natives.h>

CHeadlightsScriptNPC::CHeadlightsScriptNPC(
    Vehicle vehicle,
    CScriptSettings& settings,
    std::vector<CConfig>& configs)
    : CHeadlightsScript(settings, configs) {
    mIsNPC = true;
    mVehicle = vehicle;
    mPlayerModeActive = false;
}

CHeadlightsScriptNPC::~CHeadlightsScriptNPC() {
    VEHICLE::SET_VEHICLE_USE_PLAYER_LIGHT_SETTINGS(mVehicle, false);
}

void CHeadlightsScriptNPC::Tick() {
    if (mSettings.Main.EnableNPC && !mPlayerModeActive) {
        VEHICLE::SET_VEHICLE_USE_PLAYER_LIGHT_SETTINGS(mVehicle, true);
    }
    else if (!mSettings.Main.EnableNPC && mPlayerModeActive) {
        VEHICLE::SET_VEHICLE_USE_PLAYER_LIGHT_SETTINGS(mVehicle, false);
    }
}

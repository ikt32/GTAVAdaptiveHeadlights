#include "HeadlightsScript.hpp"

#include "Constants.hpp"

#include "Util/Math.hpp"
#include "Util/Paths.hpp"
#include "Util/Game.hpp"
#include "Util/UI.hpp"
#include "Util/String.hpp"
#include "Util/Logger.hpp"

#include "Memory/Offsets.hpp"
#include "Memory/VehicleExtensions.hpp"

#include <inc/enums.h>
#include <inc/natives.h>
#include <fmt/format.h>
#include <filesystem>
#include <algorithm>

using VExt = VehicleExtensions;

CHeadlightsScript::CHeadlightsScript(CScriptSettings& settings, std::vector<CConfig>& configs)
    : mSettings(settings)
    , mConfigs(configs)
    , mDefaultConfig(configs[0])
    , mVehicle(0)
    , mActiveConfig(nullptr)
    , mIsNPC(false) {
}

CHeadlightsScript::~CHeadlightsScript() = default;

void CHeadlightsScript::Tick() {
    Vehicle playerVehicle = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), false);

    // Update active vehicle and config
    if (playerVehicle != mVehicle) {
        mVehicle = playerVehicle;

        UpdateActiveConfig(true);
    }

    if (mActiveConfig && Util::VehicleAvailable(mVehicle, PLAYER::PLAYER_PED_ID(), false)) {
        update();
    }
}

void CHeadlightsScript::UpdateActiveConfig(bool playerCheck) {
    if (!ENTITY::DOES_ENTITY_EXIST(mVehicle))
        return;

    if (playerCheck) {
        if (!Util::VehicleAvailable(mVehicle, PLAYER::PLAYER_PED_ID(), false)) {
            mActiveConfig = nullptr;
            return;
        }
    }

    Hash model = ENTITY::GET_ENTITY_MODEL(mVehicle);
    std::string plate = VEHICLE::GET_VEHICLE_NUMBER_PLATE_TEXT(mVehicle);

    // First pass - match model and plate
    auto foundConfig = std::find_if(mConfigs.begin(), mConfigs.end(), [&](const CConfig& config) {
        bool modelMatch = config.ModelHash == model;
        bool plateMatch = Util::strcmpwi(config.Plate, plate);
        return modelMatch && plateMatch;
        });

    // second pass - match model with any plate
    if (foundConfig == mConfigs.end()) {
        foundConfig = std::find_if(mConfigs.begin(), mConfigs.end(), [&](const CConfig& config) {
            bool modelMatch = config.ModelHash == model;
            bool plateMatch = config.Plate.empty();
            return modelMatch && plateMatch;
            });
    }

    // third pass - use default
    if (foundConfig == mConfigs.end()) {
        mActiveConfig = &mDefaultConfig;
    }
    else {
        mActiveConfig = &*foundConfig;
    }
}

void CHeadlightsScript::ApplyConfig(const CConfig& config) {
    if (!mActiveConfig)
        return;

    // TODO: Apply config data
}

void CHeadlightsScript::update() {
    if (!ENTITY::DOES_ENTITY_EXIST(mVehicle) ||
        !VEHICLE::GET_IS_VEHICLE_ENGINE_RUNNING(mVehicle) ||
        !mActiveConfig ||
        mActiveConfig == &mDefaultConfig)
        return;

    // TODO: Implement it.
}

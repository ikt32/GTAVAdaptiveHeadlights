#include "HeadlightsScript.hpp"

#include "Constants.hpp"

#include "Util/Math.hpp"
#include "Util/Paths.hpp"
#include "Util/Game.hpp"
#include "Util/UI.hpp"
#include "Util/String.hpp"
#include "Util/Logger.hpp"

#include "Memory/Offsets.hpp"
#include "Memory/VehicleBone.hpp"
#include "Memory/VehicleEnums.hpp"
#include "Memory/VehicleExtensions.hpp"

#include <inc/enums.h>
#include <inc/natives.h>
#include <fmt/format.h>

#include <algorithm>
#include <filesystem>
#include <ranges>

using VExt = VehicleExtensions;

CHeadlightsScript::CHeadlightsScript(Vehicle vehicle, std::vector<CConfig>& configs)
    : mConfigs(configs)
    , mDefaultConfig(configs[0])
    , mVehicle(vehicle) {
}

CHeadlightsScript::~CHeadlightsScript() = default;

void CHeadlightsScript::Tick() {
    if (mActiveConfig) {
        update();
    }
}

void CHeadlightsScript::UpdateActiveConfig() {
    if (!ENTITY::DOES_ENTITY_EXIST(mVehicle)) {
        mActiveConfig = &mDefaultConfig;
        return;
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

    mBoneIdxRotationMap.clear();
    mLowpassBodyPitch = 0.0f;
    mLowpassSuspPitch = 0.0f;

    registerBoneMatrices(mActiveConfig->Bones.LowLeft, mLowLeftBones);
    registerBoneMatrices(mActiveConfig->Bones.LowRight, mLowRightBones);
    registerBoneMatrices(mActiveConfig->Bones.HighLeft, mHighLeftBones);
    registerBoneMatrices(mActiveConfig->Bones.HighRight, mHighRightBones);
}

void CHeadlightsScript::ApplyConfig(const CConfig& config) {
    if (!mActiveConfig)
        return;

    mActiveConfig->Correction = config.Correction;
    mActiveConfig->Level = config.Level;
    mActiveConfig->Steer = config.Steer;
}

void CHeadlightsScript::update() {
    if (!ENTITY::DOES_ENTITY_EXIST(mVehicle) ||
        !mActiveConfig) {
        mStartupCalibrationDone = false;
        mStartupCalibrationActive = false;
        mLastHeadlightOn = false;
        return;
    }

    bool engineOnState = VEHICLE::GET_IS_VEHICLE_ENGINE_RUNNING(mVehicle);
    if (mEngineOnState && !engineOnState) {
        mLastEngineOffTime = MISC::GET_GAME_TIMER();
    }
    mEngineOnState = engineOnState;

    // If engine has been off for >1 second, skip this vehicle
    // Also reset calibration stuff.
    // >1 frame required so the "keep engine on" thing doesn't trip it.
    if (!mEngineOnState &&
        MISC::GET_GAME_TIMER() > mLastEngineOffTime + 1000) {
        mStartupCalibrationDone = false;
        mStartupCalibrationActive = false;
        mLastHeadlightOn = false;
        return;
    }

    for (auto& [boneIdx, rotations] : mBoneIdxRotationMap) {
        (void)boneIdx; // No [[maybe_unused]] for structured bindings...
        rotations.clear();
    }

    if (mActiveConfig->Correction.Enable) {
        getCorrectionRotation(mBoneIdxRotationMap);
    }

    if (mActiveConfig->Level.Enable) {
        auto [lowBeams, highBeams] = getBeamsActive(mVehicle);

        if (!mStartupCalibrationDone &&
            !mLastHeadlightOn &&
            (lowBeams || highBeams)) {
            mStartupCalibrationStarted = MISC::GET_GAME_TIMER();
            mStartupCalibrationActive = true;
        }
        mLastHeadlightOn = lowBeams || highBeams;
    }
    else {
        mStartupCalibrationDone = false;
        mStartupCalibrationActive = false;
        mLastHeadlightOn = false;
    }

    if (mActiveConfig->Steer.Enable && mStartupCalibrationDone) {
        getSteerRotation(mBoneIdxRotationMap);
    }

    if (!mStartupCalibrationDone && mStartupCalibrationActive) {
        getStartupCalibration(mBoneIdxRotationMap);
    }

    if (mActiveConfig->Level.Enable && mStartupCalibrationDone) {
        getLevelRotation(mBoneIdxRotationMap);
    }

    for (const auto& [boneIdx, rotations] : mBoneIdxRotationMap) {
        if (!rotations.empty()) {
            VehicleBones::RotateAxisAbsolute(mVehicle, boneIdx, rotations);
        }
        else {
            // Reset rotation with a basic valid axis and no rotation
            VehicleBones::RotateAxisAbsolute(mVehicle, boneIdx, { { Vector3{0.0f, 0.0f, 1.0f}, 0.0f } });
        }
    }
}

void CHeadlightsScript::registerBoneMatrices(const std::vector<std::string> bones, std::vector<BoneInfo>& boneInfos) {
    boneInfos.clear();
    for (const auto& boneName : bones) {
        if (auto boneIdx = ENTITY::GET_ENTITY_BONE_INDEX_BY_NAME(mVehicle, boneName.c_str());
            boneIdx != -1) {
            VehicleBones::RegisterMatrix(mVehicle, boneName.c_str());
            mBoneIdxRotationMap.insert({ boneIdx, std::vector<VehicleBones::SAxisRotation>()});
            boneInfos.push_back({ boneIdx, getDamageFlag(boneName)});
        }
        else {
            LOG(WARN, "{}: Bone index for '{}' not found", mActiveConfig->Name, boneName);
        }
    }
}

void CHeadlightsScript::updateAngle(const std::vector<BoneInfo>& boneInfos,
                                    VehicleBones::SAxisRotation rotation,
                                    BoneIdxRotationMap& rotationMap,
                                    std::vector<int>& modifiedBoneIdxs) const {
    for (const auto& [boneIdx, boneDmgFlag] : boneInfos) {
        // Only set angle once per bone per feature. If high beams, they take priority.
        if (std::ranges::find(modifiedBoneIdxs, boneIdx) != modifiedBoneIdxs.end())
            continue;

        // Don't set damaged light bones
        if ((VExt::GetLightsBroken(mVehicle) & boneDmgFlag) > 0)
            continue;

        rotationMap.at(boneIdx).push_back(rotation);
        modifiedBoneIdxs.push_back(boneIdx);
    }
}

void CHeadlightsScript::getCorrectionRotation(BoneIdxRotationMap& rotationMap) const {
    const Vector3 axis = { 1.0f, 0.0f, 0.0f };
    auto [lowBeams, highBeams] = getBeamsActive(mVehicle);
    std::vector<int> modifiedBoneIdxs;
    if (highBeams) {
        auto pitch = deg2rad(mActiveConfig->Correction.PitchAdjustHighBeam);
        updateAngle(mHighLeftBones, { axis, pitch }, rotationMap, modifiedBoneIdxs);
        updateAngle(mHighRightBones, { axis, pitch }, rotationMap, modifiedBoneIdxs);
    }
    if (lowBeams) {
        auto pitch = deg2rad(mActiveConfig->Correction.PitchAdjustLowBeam);
        updateAngle(mLowLeftBones, { axis, pitch }, rotationMap, modifiedBoneIdxs);
        updateAngle(mLowRightBones, { axis, pitch }, rotationMap, modifiedBoneIdxs);
    }
}

void CHeadlightsScript::getLevelRotation(BoneIdxRotationMap& rotationMap) {
    const Vector3 axis = { 1.0f, 0.0f, 0.0f };
    const float upperLimitRad = deg2rad(mActiveConfig->Level.UpperLimit);
    const float lowerLimitRad = deg2rad(mActiveConfig->Level.LowerLimit);

    auto comp = VExt::GetWheelCompressions(mVehicle);

    // Suspension component only works for 4-wheel vehicles.
    if (comp.size() == 4) {
        float compFrontAxle = (comp[0] + comp[1]) / 2.0f;
        float compRearAxle = (comp[2] + comp[3]) / 2.0f;
        auto offsets = VExt::GetWheelOffsets(mVehicle);
        float wheelBase = abs(offsets[0].y - offsets[2].y);

        float pitchSuspRad = 0.0f;
        pitchSuspRad = atan((compFrontAxle - compRearAxle) / wheelBase);
        pitchSuspRad = std::clamp(pitchSuspRad, lowerLimitRad, upperLimitRad);

        const float horCenterSpeed = 1.0f;
        float rate = MISC::GET_FRAME_TIME() * horCenterSpeed;
        mLowpassSuspPitch = rate * rad2deg(pitchSuspRad) + (1.0f - rate) * mLowpassSuspPitch;
    }

    // Body component works for all vehicles
    auto pitchBodyDeg = ENTITY::GET_ENTITY_PITCH(mVehicle);

    const float horCenterSpeed = 3.5f;
    float rate = MISC::GET_FRAME_TIME() * horCenterSpeed;
    mLowpassBodyPitch = rate * pitchBodyDeg + (1.0f - rate) * mLowpassBodyPitch;
    float dynamicBodyPitch = pitchBodyDeg - mLowpassBodyPitch;

    float pitchBodyCompRad = deg2rad(dynamicBodyPitch);
    float pitchSuspCompRad = deg2rad(mLowpassSuspPitch);
    float levelPitch = std::clamp(-pitchBodyCompRad + pitchSuspCompRad, lowerLimitRad, upperLimitRad);

    auto [lowBeams, highBeams] = getBeamsActive(mVehicle);
    std::vector<int> modifiedBoneIdxs;
    if (highBeams) {
        updateAngle(mHighLeftBones, { axis, levelPitch }, rotationMap, modifiedBoneIdxs);
        updateAngle(mHighRightBones, { axis, levelPitch }, rotationMap, modifiedBoneIdxs);
    }
    if (lowBeams) {
        updateAngle(mLowLeftBones, { axis, levelPitch }, rotationMap, modifiedBoneIdxs);
        updateAngle(mLowRightBones, { axis, levelPitch }, rotationMap, modifiedBoneIdxs);
    }
}

void CHeadlightsScript::getSteerRotation(BoneIdxRotationMap& rotationMap) const {
    const Vector3 axis = { 0.0f, 0.0f, 1.0f };

    const float mult = mActiveConfig->Steer.SteeringMultiplier;
    const float limitRad = deg2rad(mActiveConfig->Steer.Limit);

    float yaw = std::clamp(VExt::GetSteeringAngle(mVehicle) * mult, -limitRad, limitRad);

    auto [lowBeams, highBeams] = getBeamsActive(mVehicle);
    std::vector<int> modifiedBoneIdxs;
    if (highBeams) {
        if (yaw > 0.0f) {
            updateAngle(mHighLeftBones, { axis, yaw }, rotationMap, modifiedBoneIdxs);
            updateAngle(mHighRightBones, { axis, yaw * 0.5f }, rotationMap, modifiedBoneIdxs);
        }
        if (yaw < 0.0f) {
            updateAngle(mHighLeftBones, { axis, yaw * 0.5f }, rotationMap, modifiedBoneIdxs);
            updateAngle(mHighRightBones, { axis, yaw }, rotationMap, modifiedBoneIdxs);
        }
    }
    if (lowBeams) {
        if (yaw > 0.0f) {
            updateAngle(mLowLeftBones, { axis, yaw }, rotationMap, modifiedBoneIdxs);
            updateAngle(mLowRightBones, { axis, yaw * 0.5f }, rotationMap, modifiedBoneIdxs);
        }
        if (yaw < 0.0f) {
            updateAngle(mLowLeftBones, { axis, yaw * 0.5f }, rotationMap, modifiedBoneIdxs);
            updateAngle(mLowRightBones, { axis, yaw }, rotationMap, modifiedBoneIdxs);
        }
    }
}

void CHeadlightsScript::getStartupCalibration(BoneIdxRotationMap& rotationMap) {
    const Vector3 axisPitch = { 1.0f, 0.0f, 0.0f };
    const Vector3 axisYaw = { 0.0f, 0.0f, 1.0f };
    const float upperLimitRad = deg2rad(mActiveConfig->Level.UpperLimit);
    const float lowerLimitRad = deg2rad(mActiveConfig->Level.LowerLimit);
    const float yawLimitRad = deg2rad(mActiveConfig->Steer.Limit);

    const float stage1Ratio = 0.25f; // Off
    const float stage2Ratio = 0.40f; // To lower position
    const float stage3Ratio = 0.80f; // Test yaw
    const float stage4Ratio = 1.00f; // To Normal position

    // From stage2 to stage3, we're lowered and can test yaw
    const float stage1RatioYaw = 0.50f; // Move to out
    const float stage2RatioYaw = 0.60f; // Move back to 0
    const float stage3RatioYaw = 0.70f; // Wait

    float calibrationPitch = 0.0f;
    float calibrationYaw = 0.0f;

    float progress = map(static_cast<float>(MISC::GET_GAME_TIMER()),
        static_cast<float>(mStartupCalibrationStarted),
        static_cast<float>(mStartupCalibrationStarted + mStartupDurationMs),
        0.0f,
        1.0f);

    if (progress < stage1Ratio) {
        calibrationPitch = 0.0f;
    }
    else if (progress < stage2Ratio) {
        calibrationPitch = map(progress, stage1Ratio, stage2Ratio, 0.0f, lowerLimitRad);
    }
    else if (progress < stage3Ratio) {
        calibrationPitch = lowerLimitRad;
    }
    else if (progress < stage4Ratio) {
        calibrationPitch = map(progress, stage3Ratio, stage4Ratio, lowerLimitRad, calibrationPitch);
    }

    if (progress < stage1RatioYaw) {
        calibrationYaw = 0.0f;
    }
    else if (progress < stage2RatioYaw) {
        calibrationYaw = map(progress, stage1RatioYaw, stage2RatioYaw, 0.0f, yawLimitRad);
    }
    else if (progress < stage3RatioYaw) {
        calibrationYaw = map(progress, stage2RatioYaw, stage3RatioYaw, yawLimitRad, 0.0f);
    }

    if (mStartupCalibrationActive && progress > 1.0f) {
        mStartupCalibrationActive = false;
        mStartupCalibrationDone = true;
        mLowpassBodyPitch = ENTITY::GET_ENTITY_PITCH(mVehicle);
        mLowpassSuspPitch = 0.0f;
    }
    else {
        std::vector<int> modifiedBoneIdxs;
        // Update for all lights simultaneously
        updateAngle(mHighLeftBones, { axisPitch, calibrationPitch }, rotationMap, modifiedBoneIdxs);
        updateAngle(mHighRightBones, { axisPitch, calibrationPitch }, rotationMap, modifiedBoneIdxs);
        updateAngle(mLowLeftBones, { axisPitch, calibrationPitch }, rotationMap, modifiedBoneIdxs);
        updateAngle(mLowRightBones, { axisPitch, calibrationPitch }, rotationMap, modifiedBoneIdxs);

        // ... and yaw, too.
        if (mActiveConfig->Steer.Enable) {
            modifiedBoneIdxs.clear();
            updateAngle(mHighLeftBones, { axisYaw, calibrationYaw }, rotationMap, modifiedBoneIdxs);
            updateAngle(mLowLeftBones, { axisYaw, calibrationYaw }, rotationMap, modifiedBoneIdxs);
            updateAngle(mHighRightBones, { axisYaw, -calibrationYaw }, rotationMap, modifiedBoneIdxs);
            updateAngle(mLowRightBones, { axisYaw, -calibrationYaw }, rotationMap, modifiedBoneIdxs);
        }
    }
}

std::pair<bool, bool> CHeadlightsScript::getBeamsActive(Vehicle vehicle) const {
    auto lightStates = VExt::GetLightStates(mVehicle);
    bool lowBeams = (lightStates & EVehicleLightState::LightStateLowBeam) > 0;
    bool highBeams = (lightStates & EVehicleLightState::LightStateHighBeam) > 0;
    return std::pair<bool, bool>(lowBeams, highBeams);
}

uint32_t CHeadlightsScript::getDamageFlag(const std::string& boneName) const {
    using namespace Util;
    switch (joaat(boneName.c_str())) {
        case joaat("headlight_l"):
            return EVehicleLightDamage::LeftHeadlight;
        case joaat("headlight_r"):
            return EVehicleLightDamage::RightHeadlight;
        case joaat("extralight_1"):
            return EVehicleLightDamage::ExtraLight1;
        case joaat("extralight_2"):
            return EVehicleLightDamage::ExtraLight2;
        case joaat("extralight_3"):
            return EVehicleLightDamage::ExtraLight3;
        case joaat("extralight_4"):
            return EVehicleLightDamage::ExtraLight4;
        default:
            return 0;
    }
}

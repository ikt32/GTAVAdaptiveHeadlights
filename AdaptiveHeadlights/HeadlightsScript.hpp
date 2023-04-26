#pragma once
#include "ScriptSettings.hpp"
#include "Config.hpp"
#include "Memory/VehicleBone.hpp"

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

class CHeadlightsScript {
public:
    CHeadlightsScript(Vehicle vehicle, std::vector<CConfig>& configs);
    virtual ~CHeadlightsScript();
    virtual void Tick();

    CConfig* ActiveConfig() {
        return mActiveConfig;
    }

    void UpdateActiveConfig();

    // Applies the passed config onto the current active config.
    void ApplyConfig(const CConfig& config);

    Vehicle GetVehicle() {
        return mVehicle;
    }

protected:
    typedef std::unordered_map<int, std::vector<VehicleBones::SAxisRotation>> BoneIdxRotationMap;
    struct SBoneInfo {
        int BoneIdx;
        uint32_t BoneDmgFlag;
    };

    struct SSuspensionGeometry {
        float CompFront;
        float CompRear;
        float Wheelbase;
    };

    void update();

    void registerBoneMatrices(const std::vector<std::string> bones, std::vector<SBoneInfo>& boneInfos);
    void updateAngle(const std::vector<SBoneInfo>& boneInfos,
                     VehicleBones::SAxisRotation rotation,
                     BoneIdxRotationMap& rotationMap,
                     std::vector<int>& modifiedBoneIdxs) const;
    void getCorrectionRotation(BoneIdxRotationMap& rotationMap) const;
    void getLevelRotation(BoneIdxRotationMap& rotationMap);
    void getSteerRotation(BoneIdxRotationMap& rotationMap) const;
    void getStartupCalibration(BoneIdxRotationMap& rotationMap);

    std::pair<bool, bool> getBeamsActive(Vehicle vehicle) const;
    uint32_t getDamageFlag(const std::string& boneName) const;

    std::optional<SSuspensionGeometry> GetSuspensionGeometry(Vehicle vehicle) const;

    std::vector<CConfig>& mConfigs;
    CConfig mDefaultConfig;

    Vehicle mVehicle;
    CConfig* mActiveConfig = nullptr;

    BoneIdxRotationMap mBoneIdxRotationMap;
    std::vector<SBoneInfo> mLowLeftBones;
    std::vector<SBoneInfo> mLowRightBones;
    std::vector<SBoneInfo> mHighLeftBones;
    std::vector<SBoneInfo> mHighRightBones;

    float mLowpassBodyPitch = 0.0f; // degrees
    float mLowpassSuspPitch = 0.0f; // degrees

    const int mStartupDurationMs = 5000;

    bool mStartupCalibrationDone = false;
    bool mStartupCalibrationActive = false;
    bool mLastHeadlightOn = false;
    int mStartupCalibrationStarted = 0;

    bool mEngineOnState = false;
    int mLastEngineOffTime = 0;
};

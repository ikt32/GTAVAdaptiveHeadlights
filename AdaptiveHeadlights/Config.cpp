#include "Config.hpp"
#include "SettingsCommon.hpp"

#include "Constants.hpp"
#include "Util/AddonSpawnerCache.hpp"
#include "Util/Paths.hpp"
#include "Util/Logger.hpp"
#include "Util/String.hpp"

#include <fmt/format.h>
#include <simpleini/SimpleIni.h>
#include <filesystem>
#include <format>
#include <sstream>

#define CHECK_LOG_SI_ERROR(result, operation, file) \
    if ((result) < 0) { \
        LOG(ERROR, "[Config] {} Failed to {}, SI_Error [{}]", \
        file, operation, result); \
    }

#define SAVE_VAL(section, key, option) \
    { \
        SetValue(ini, section, key, option); \
    }

#define LOAD_VAL(section, key, option) \
    { \
        option = (GetValue(ini, section, key, option)); \
    }

namespace {
    void parseBone(const CSimpleIniA& ini,
                   const char* section,
                   const char* key,
                   std::vector<std::string>& boneList) {
        boneList.clear();
        std::string bonesAll = ini.GetValue(section, key, "");
        auto boneNames = Util::split(bonesAll, ' ');
        for (const auto& boneName : boneNames) {
            if (!boneName.empty())
                boneList.push_back(boneName);
        }
    }
}

CConfig CConfig::Read(const std::string& configFile) {
    CConfig config{};

    CSimpleIniA ini;
    ini.SetUnicode();
    ini.SetMultiLine(true);
    SI_Error result = ini.LoadFile(configFile.c_str());
    CHECK_LOG_SI_ERROR(result, "load", configFile.c_str());

    config.Name = std::filesystem::path(configFile).stem().string();

    // [ID]
    std::string modelNamesAll = ini.GetValue("ID", "Models", "");

    std::string modelHashStr = ini.GetValue("ID", "ModelHash", "");
    std::string modelName = ini.GetValue("ID", "ModelName", "");


    if (modelHashStr.empty() && modelName.empty()) {
        // This is a no-vehicle config. Nothing to be done.
    }
    else if (modelHashStr.empty()) {
        // This config only has a model name.
        config.ModelHash = Util::joaat(modelName.c_str());
        config.ModelName = modelName;
    }
    else {
        // This config only has a hash.
        Hash modelHash = 0;
        int found = sscanf_s(modelHashStr.c_str(), "%X", &modelHash);

        if (found == 1) {
            config.ModelHash = modelHash;

            auto& asCache = ASCache::Get();
            auto it = asCache.find(modelHash);
            std::string modelName = it == asCache.end() ? std::string() : it->second;
            config.ModelName = modelName;
        }
    }

    config.Plate = ini.GetValue("ID", "Plate", "");

    // [Bones]
    parseBone(ini, "Bones", "LowLeft",    config.Bones.LowLeft);
    parseBone(ini, "Bones", "LowRight",   config.Bones.LowRight);
    parseBone(ini, "Bones", "HighLeft",   config.Bones.HighLeft);
    parseBone(ini, "Bones", "HighRight",  config.Bones.HighRight);
    parseBone(ini, "Bones", "Mods",       config.Bones.Mods);

    // [Correction]
    LOAD_VAL("Correction", "Enable", config.Correction.Enable);
    LOAD_VAL("Correction", "PitchAdjustLowBeam", config.Correction.PitchAdjustLowBeam);
    LOAD_VAL("Correction", "PitchAdjustHighBeam", config.Correction.PitchAdjustHighBeam);
    LOAD_VAL("Correction", "PitchAdjustMods", config.Correction.PitchAdjustMods);

    // [Level]
    LOAD_VAL("Level", "EnableSuspension", config.Level.EnableSuspension);
    LOAD_VAL("Level", "SpeedSuspension", config.Level.SpeedSuspension);
    LOAD_VAL("Level", "EnableGyroscope", config.Level.EnableGyroscope);
    LOAD_VAL("Level", "SpeedGyroscope", config.Level.SpeedGyroscope);
    LOAD_VAL("Level", "UpperLimit", config.Level.UpperLimit);
    LOAD_VAL("Level", "LowerLimit", config.Level.LowerLimit);

    // [Steer]
    LOAD_VAL("Steer", "Enable", config.Steer.Enable);
    LOAD_VAL("Steer", "SteeringMultiplier", config.Steer.SteeringMultiplier);
    LOAD_VAL("Steer", "Limit", config.Steer.Limit);

    return config;
}

void CConfig::Write(ESaveType saveType) {
    Write(Name, 0, std::string(), saveType);
}

bool CConfig::Write(const std::string& newName, Hash model, std::string plate, ESaveType saveType) {
    const auto configsPath = Paths::GetModPath() / "Configs";
    const auto configFile = configsPath / std::format("{}.ini", newName);

    CSimpleIniA ini;
    ini.SetUnicode();
    ini.SetMultiLine(true);

    // This here MAY fail on first save, in which case, it can be ignored.
    // _Not_ having this just nukes the entire file, including any comments.
    SI_Error result = ini.LoadFile(configFile.c_str());
    if (result < 0) {
        LOG(WARN, "[Config] {} Failed to load, SI_Error [{}]. (No problem if no file exists yet)",
            configFile.string(), result);
    }

    // [ID]
    if (saveType != ESaveType::GenericNone) {
        if (model != 0) {
            ModelHash = model;
        }

        ini.SetValue("ID", "ModelHash", std::format("{:X}", ModelHash).c_str());

        auto& asCache = ASCache::Get();
        auto it = asCache.find(ModelHash);
        std::string modelName = it == asCache.end() ? std::string() : it->second;
        if (!modelName.empty()) {
            ModelName = modelName;
            ini.SetValue("ID", "ModelName", modelName.c_str());
        }

        if (saveType == ESaveType::Specific) {
            Plate = plate;
            ini.SetValue("ID", "Plate", plate.c_str());
        }
    }

    // [Bones]
    // No stl-format alternative for fmt::join yet?
    SAVE_VAL("Bones", "LowLeft",    fmt::format("{}", fmt::join(Bones.LowLeft, " ")));
    SAVE_VAL("Bones", "LowRight",   fmt::format("{}", fmt::join(Bones.LowRight, " ")));
    SAVE_VAL("Bones", "HighLeft",   fmt::format("{}", fmt::join(Bones.HighLeft, " ")));
    SAVE_VAL("Bones", "HighRight",  fmt::format("{}", fmt::join(Bones.HighRight, " ")));
    SAVE_VAL("Bones", "Mods",       fmt::format("{}", fmt::join(Bones.Mods, " ")));

    // [Correction]
    SAVE_VAL("Correction", "Enable", Correction.Enable);
    SAVE_VAL("Correction", "PitchAdjustLowBeam", Correction.PitchAdjustLowBeam);
    SAVE_VAL("Correction", "PitchAdjustHighBeam", Correction.PitchAdjustHighBeam);
    SAVE_VAL("Correction", "PitchAdjustMods", Correction.PitchAdjustMods);

    // [Level]
    SAVE_VAL("Level", "EnableSuspension", Level.EnableSuspension);
    SAVE_VAL("Level", "SpeedSuspension", Level.SpeedSuspension);
    SAVE_VAL("Level", "EnableGyroscope", Level.EnableGyroscope);
    SAVE_VAL("Level", "SpeedGyroscope", Level.SpeedGyroscope);
    SAVE_VAL("Level", "UpperLimit", Level.UpperLimit);
    SAVE_VAL("Level", "LowerLimit", Level.LowerLimit);

    // [Steer]
    SAVE_VAL("Steer", "Enable", Steer.Enable);
    SAVE_VAL("Steer", "SteeringMultiplier", Steer.SteeringMultiplier);
    SAVE_VAL("Steer", "Limit", Steer.Limit);

    result = ini.SaveFile(configFile.c_str());
    CHECK_LOG_SI_ERROR(result, "save", configFile.string());
    if (result < 0)
        return false;
    return true;
}

#include "Config.hpp"
#include "SettingsCommon.hpp"

#include "Constants.hpp"
#include "Util/AddonSpawnerCache.hpp"
#include "Util/Paths.hpp"
#include "Util/Logger.hpp"
#include "Util/String.hpp"

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

    // TODO: Settings

    return config;
}

void CConfig::Write(ESaveType saveType) {
    Write(Name, 0, std::string(), saveType);
}

bool CConfig::Write(const std::string& newName, Hash model, std::string plate, ESaveType saveType) {
    const std::string configsPath =
        Paths::GetModuleFolder(Paths::GetOurModuleHandle()) +
        Constants::ScriptFolder +
        "\\Configs";
    const std::string configFile = std::format("{}\\{}.ini", configsPath, newName);

    CSimpleIniA ini;
    ini.SetUnicode();
    ini.SetMultiLine(true);

    // This here MAY fail on first save, in which case, it can be ignored.
    // _Not_ having this just nukes the entire file, including any comments.
    SI_Error result = ini.LoadFile(configFile.c_str());
    if (result < 0) {
        LOG(WARN, "[Config] {} Failed to load, SI_Error [{}]. (No problem if no file exists yet)",
            configFile, result);
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

    // TODO

    result = ini.SaveFile(configFile.c_str());
    CHECK_LOG_SI_ERROR(result, "save", configFile.c_str());
    if (result < 0)
        return false;
    return true;
}

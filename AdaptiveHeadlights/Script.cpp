#include "Script.hpp"

#include "HeadlightsScript.hpp"
#include "HeadlightsScriptNPC.hpp"
#include "ScriptMenu.hpp"
#include "Constants.hpp"

#include "Util/Logger.hpp"
#include "Util/Paths.hpp"
#include "Util/String.hpp"
#include "Memory/VehicleExtensions.hpp"

#include <inc/natives.h>
#include <inc/main.h>
#include <fmt/format.h>
#include <memory>
#include <filesystem>

namespace {
    std::shared_ptr<CScriptSettings> settings;
    std::shared_ptr<CHeadlightsScript> playerScriptInst;
    std::vector<std::shared_ptr<CHeadlightsScriptNPC>> npcScriptInsts;
    std::unique_ptr<CScriptMenu<CHeadlightsScript>> scriptMenu;

    std::vector<CConfig> configs;

    bool initialized = false;
}

void AdaptiveHeadlights::ScriptMain() {
    if (!initialized) {
        LOG(INFO, "Script started");
        ScriptInit();
        initialized = true;
    }
    else {
        LOG(INFO, "Script restarted");
    }
    ScriptTick();
}

void AdaptiveHeadlights::ScriptInit() {
    const auto settingsGeneralPath = Paths::GetModPath() / "settings_general.ini";
    const auto settingsMenuPath = Paths::GetModPath() / "settings_menu.ini";

    settings = std::make_shared<CScriptSettings>(settingsGeneralPath.string());
    settings->Load();
    LOG(INFO, "Settings loaded");

    AdaptiveHeadlights::LoadConfigs();

    playerScriptInst = std::make_shared<CHeadlightsScript>(*settings, configs);

    VehicleExtensions::Init();

    scriptMenu = std::make_unique<CScriptMenu<CHeadlightsScript>>(settingsMenuPath.string(),
        []() {
            // OnInit
            settings->Load();
            AdaptiveHeadlights::LoadConfigs();
        },
        []() {
            // OnExit
            settings->Save();
            AdaptiveHeadlights::SaveConfigs();
        },
        BuildMenu()
    );
}

void AdaptiveHeadlights::ScriptTick() {
    while (true) {
        playerScriptInst->Tick();

        if (settings->Main.EnableNPC)
            UpdateNPC();

        scriptMenu->Tick(*playerScriptInst);
        WAIT(0);
    }
}

void AdaptiveHeadlights::UpdateNPC() {
    std::vector<std::shared_ptr<CHeadlightsScriptNPC>> instsToDelete;
    
    std::vector<Vehicle> allVehicles(1024);
    int actualSize = worldGetAllVehicles(allVehicles.data(), 1024);
    allVehicles.resize(actualSize);
    
    for (const auto& vehicle : allVehicles) {
        if (ENTITY::IS_ENTITY_DEAD(vehicle, 0) ||
            vehicle == playerScriptInst->GetVehicle() ||
            !VEHICLE::GET_IS_VEHICLE_ENGINE_RUNNING(vehicle))
            continue;
    
        auto it = std::find_if(npcScriptInsts.begin(), npcScriptInsts.end(), [vehicle](const auto& inst) {
            return inst->GetVehicle() == vehicle;
            });
    
        if (it == npcScriptInsts.end()) {
            npcScriptInsts.push_back(std::make_shared<CHeadlightsScriptNPC>(vehicle, *settings, configs));
            auto npcScriptInst = npcScriptInsts.back();
    
            npcScriptInst->UpdateActiveConfig(false);
        }
    }
    
    for (const auto& inst : npcScriptInsts) {
        if (!ENTITY::DOES_ENTITY_EXIST(inst->GetVehicle()) ||
            ENTITY::IS_ENTITY_DEAD(inst->GetVehicle(), 0) ||
            inst->GetVehicle() == playerScriptInst->GetVehicle()) {
            instsToDelete.push_back(inst);
        }
        else {
            inst->Tick();
        }
    }
    
    for (const auto& inst : instsToDelete) {
        npcScriptInsts.erase(std::remove(npcScriptInsts.begin(), npcScriptInsts.end(), inst), npcScriptInsts.end());
    }
}

void AdaptiveHeadlights::UpdateActiveConfigs() {
    if (playerScriptInst)
        playerScriptInst->UpdateActiveConfig(true);

    for (const auto& inst : npcScriptInsts) {
        inst->UpdateActiveConfig(false);
    }
}

CScriptSettings& AdaptiveHeadlights::GetSettings() {
    return *settings;
}

CHeadlightsScript* AdaptiveHeadlights::GetScript() {
    return playerScriptInst.get();
}

uint64_t AdaptiveHeadlights::GetNPCScriptCount() {
    uint64_t numActive = 0;
    for (auto& npcInstance : npcScriptInsts) {
        if (!npcInstance->ActiveConfig()->Name.empty())
            ++numActive;
    }
    return numActive;
}

void AdaptiveHeadlights::ClearNPCScripts() {
    npcScriptInsts.clear();
}

const std::vector<CConfig>& AdaptiveHeadlights::GetConfigs() {
    return configs;
}

uint32_t AdaptiveHeadlights::LoadConfigs() {
    namespace fs = std::filesystem;

    const auto configsPath = Paths::GetModPath() / "Configs";

    LOG(DEBUG, "Clearing and reloading configs");

    configs.clear();

    if (!(fs::exists(configsPath) && fs::is_directory(configsPath))) {
        LOG(ERROR, "Directory [{}] not found!", configsPath.string());
        configs.insert(configs.begin(), CConfig{});
        AdaptiveHeadlights::UpdateActiveConfigs();
        return 0;
    }

    for (const auto& file : fs::directory_iterator(configsPath)) {
        if (Util::to_lower(fs::path(file).extension().string()) != ".ini") {
            LOG(DEBUG, "Skipping [{}] - not .ini", file.path().stem().string());
            continue;
        }

        CConfig config = CConfig::Read(fs::path(file).string());
        if (Util::strcmpwi(config.Name, "Default")) {
            configs.insert(configs.begin(), config);
            continue;
        }

        configs.push_back(config);
        LOG(DEBUG, "Loaded vehicle config [{}]", config.Name);
    }

    if (configs.empty() ||
        !configs.empty() && !Util::strcmpwi(configs[0].Name, "Default")) {
        LOG(WARN, "No default config found, generating a default one and saving it...");
        CConfig defaultConfig;
        defaultConfig.Name = "Default";
        configs.insert(configs.begin(), defaultConfig);
        defaultConfig.Write(CConfig::ESaveType::GenericNone);
    }

    LOG(INFO, "Configs loaded: {}", configs.size());

    AdaptiveHeadlights::UpdateActiveConfigs();
    return static_cast<unsigned>(configs.size());
}

void AdaptiveHeadlights::SaveConfigs() {
    namespace fs = std::filesystem;

    const auto configsPath = Paths::GetModPath() / "Configs";

    LOG(DEBUG, "Saving all configs");

    if (!(fs::exists(configsPath) && fs::is_directory(configsPath))) {
        LOG(ERROR, "Directory [{}] not found!", configsPath.string());
        return;
    }

    for (auto& config : configs) {
        if (!config.ModelName.empty()) {
            auto saveType = config.Plate.empty() ?
                CConfig::ESaveType::GenericModel :
                CConfig::ESaveType::Specific;
            config.Write(saveType);
        }
    }
}

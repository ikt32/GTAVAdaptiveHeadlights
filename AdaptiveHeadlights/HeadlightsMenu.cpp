#include "ScriptMenu.hpp"
#include "Script.hpp"
#include "HeadlightsScript.hpp"
#include "Constants.hpp"

#include "ScriptMenuUtils.hpp"

#include "Util/UI.hpp"
#include "Util/Math.hpp"

#include "Memory/VehicleExtensions.hpp"

#include <format>
#include <optional>

using VExt = VehicleExtensions;

namespace AdaptiveHeadlights {
    std::vector<std::string> FormatConfig(CHeadlightsScript& context, const CConfig& config);

    bool PromptSave(CHeadlightsScript& context, CConfig& config, Hash model, std::string plate, CConfig::ESaveType saveType);
}

std::vector<CScriptMenu<CHeadlightsScript>::CSubmenu> AdaptiveHeadlights::BuildMenu() {
    std::vector<CScriptMenu<CHeadlightsScript>::CSubmenu> submenus;
    /* mainmenu */
    submenus.emplace_back("mainmenu", [](NativeMenu::Menu& mbCtx, CHeadlightsScript& context) {
        mbCtx.Title(Constants::ScriptName);
        mbCtx.Subtitle(std::string("~b~") + Constants::DisplayVersion);

        Ped playerPed = PLAYER::PLAYER_PED_ID();
        Vehicle playerVehicle = PED::GET_VEHICLE_PED_IS_IN(playerPed, false);

        if (!playerVehicle || !ENTITY::DOES_ENTITY_EXIST(playerVehicle)) {
            mbCtx.Option("No vehicle", { "Get in a vehicle." });
            mbCtx.MenuOption("Developer options", "developermenu");
            return;
        }

        // activeConfig can always be assumed if in any vehicle.
        CConfig* activeConfig = context.ActiveConfig();

        if (mbCtx.MenuOption("Load configuration", "loadmenu",
            { "Load another configuration into the current config." })) {
            AdaptiveHeadlights::LoadConfigs();
        }

        mbCtx.MenuOption("Developer options", "developermenu");
    });

    /* mainmenu -> loadmenu */
    submenus.emplace_back("loadmenu", [](NativeMenu::Menu& mbCtx, CHeadlightsScript& context) {
        mbCtx.Title("Load configurations");

        CConfig* config = context.ActiveConfig();
        mbCtx.Subtitle(std::format("Current: {}", config ? config->Name : "None"));

        if (config == nullptr) {
            mbCtx.Option("No active configuration");
            return;
        }

        if (AdaptiveHeadlights::GetConfigs().empty()) {
            mbCtx.Option("No saved configs");
        }

        for (const auto& config : AdaptiveHeadlights::GetConfigs()) {
            bool selected;
            bool triggered = mbCtx.OptionPlus(config.Name, {}, &selected);

            if (selected) {
                mbCtx.OptionPlusPlus(FormatConfig(context, config), config.Name);
            }

            if (triggered) {
                context.ApplyConfig(config);
                UI::Notify(std::format("Applied config {}.", config.Name), true);
            }
        }
    });

    /* mainmenu -> developermenu */
    submenus.emplace_back("developermenu", [](NativeMenu::Menu& mbCtx, CHeadlightsScript& context) {
        mbCtx.Title("Developer options");
        mbCtx.Subtitle("");

        mbCtx.Option("Placehodler");
    });

    return submenus;
}

std::vector<std::string> AdaptiveHeadlights::FormatConfig(CHeadlightsScript& context, const CConfig& config) {
    std::vector<std::string> extras{
        std::format("Name: {}", config.Name),
        std::format("Model: {}", config.ModelName.empty() ? "None (Generic)" : config.ModelName),
        std::format("Plate: {}", config.Plate.empty() ? "None" : std::format("[{}]", config.Plate)),
    };

    // TODO: Headlight-specific stuff

    return extras;
}

bool AdaptiveHeadlights::PromptSave(CHeadlightsScript& context, CConfig& config, Hash model, std::string plate, CConfig::ESaveType saveType) {
    UI::Notify("Enter new config name.", true);
    std::string newName = UI::GetKeyboardResult();

    if (newName.empty()) {
        UI::Notify("No config name entered. Not saving anything.", true);
        return false;
    }

    if (config.Write(newName, model, plate, saveType))
        UI::Notify("Saved as new configuration", true);
    else
        UI::Notify("Failed to save as new configuration", true);
    AdaptiveHeadlights::LoadConfigs();

    return true;
}

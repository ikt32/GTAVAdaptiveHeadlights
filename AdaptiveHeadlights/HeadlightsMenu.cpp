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

#include "Util/Logger.hpp"

using VExt = VehicleExtensions;

namespace AdaptiveHeadlights {
    std::vector<std::string> FormatConfig(CHeadlightsScript& context, const CConfig& config);

    bool PromptSave(CHeadlightsScript& context, CConfig& config, Hash model, std::string plate);
}

std::vector<CScriptMenu<CHeadlightsScript>::CSubmenu> AdaptiveHeadlights::BuildMenu() {
    std::vector<CScriptMenu<CHeadlightsScript>::CSubmenu> submenus;
    /* mainmenu */
    submenus.emplace_back("mainmenu",
        [](NativeMenu::Menu& mbCtx, CHeadlightsScript& context) {
            mbCtx.Title(Constants::ScriptName);
            mbCtx.Subtitle(std::string("~b~") + Constants::DisplayVersion);

            if (mbCtx.MenuOption("Manage configurations", "configsmenu",
                { "Create a new configuration, or delete one." })) {
                AdaptiveHeadlights::LoadConfigs();
            }

            Ped playerPed = PLAYER::PLAYER_PED_ID();
            Vehicle playerVehicle = PED::GET_VEHICLE_PED_IS_IN(playerPed, false);

            if (!playerVehicle || !ENTITY::DOES_ENTITY_EXIST(playerVehicle)) {
                mbCtx.Option("No active configuration",
                    { "Get in a vehicle to edit its configuration." });
            }
            else {
                mbCtx.MenuOption("Pitch correction", "correctionmenu",
                    { "Fix wrongly aimed headlights." });

                mbCtx.MenuOption("Self-leveling", "levelmenu",
                    { "Level the headlights from cargo and stabilize on unstable roads." });

                mbCtx.MenuOption("Cornering", "steermenu",
                    { "Illuminate the inside of corners." });
            }

            mbCtx.MenuOption("NPC options", "npcmenu");

            mbCtx.MenuOption("Developer options", "developermenu");
        });

    /* mainmenu -> loadmenu */
    submenus.emplace_back("configsmenu",
        [](NativeMenu::Menu& mbCtx, CHeadlightsScript& context) {
            mbCtx.Title("Manage configurations");

            CConfig* config = context.ActiveConfig();
            mbCtx.Subtitle(std::format("Current: {}", config ? config->Name : "None"));

            if (config == nullptr) {
                mbCtx.Option("No active configuration");
                return;
            }

            if (mbCtx.Option("Create configuration",
                { "Create a new configuration file from the current settings.",
                  "Changes made within a configuration are saved to that configuration only.",
                  "The submenu subtitles indicate which configuration is being edited." })) {
                PromptSave(context,
                           *config,
                           ENTITY::GET_ENTITY_MODEL(context.GetVehicle()),
                           VEHICLE::GET_VEHICLE_NUMBER_PLATE_TEXT(context.GetVehicle()));
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

    submenus.emplace_back("correctionmenu",
        [](NativeMenu::Menu& mbCtx, CHeadlightsScript& context) {
            mbCtx.Title("Correction");

            CConfig* config = context.ActiveConfig();
            mbCtx.Subtitle(std::format("Current: {}", config ? config->Name : "None"));

            if (config == nullptr) {
                mbCtx.Option("No active configuration");
                return;
            }

            mbCtx.BoolOption("Enable pitch correction", config->Correction.Enable);

            mbCtx.FloatOptionCb("Low beam correction", config->Correction.PitchAdjustLowBeam,
                -90.0f, 90.0f, 0.1f, MenuUtils::GetKbFloat);

            mbCtx.FloatOptionCb("High beam correction", config->Correction.PitchAdjustHighBeam,
                -90.0f, 90.0f, 0.1f, MenuUtils::GetKbFloat);
        });

    submenus.emplace_back("levelmenu",
        [](NativeMenu::Menu& mbCtx, CHeadlightsScript& context) {
            mbCtx.Title("Self-leveling");

            CConfig* config = context.ActiveConfig();
            mbCtx.Subtitle(std::format("Current: {}", config ? config->Name : "None"));

            if (config == nullptr) {
                mbCtx.Option("No active configuration");
                return;
            }

            mbCtx.BoolOption("Enable", config->Level.Enable);

            mbCtx.FloatOptionCb("Lower limit", config->Level.LowerLimit,
                -15.0f, 15.0f, 0.1f, MenuUtils::GetKbFloat);

            mbCtx.FloatOptionCb("Upper limit", config->Level.UpperLimit,
                -15.0f, 15.0f, 0.1f, MenuUtils::GetKbFloat);
        });

    submenus.emplace_back("steermenu",
        [](NativeMenu::Menu& mbCtx, CHeadlightsScript& context) {
            mbCtx.Title("Correction");

            CConfig* config = context.ActiveConfig();
            mbCtx.Subtitle(std::format("Current: {}", config ? config->Name : "None"));

            if (config == nullptr) {
                mbCtx.Option("No active configuration");
                return;
            }

            mbCtx.BoolOption("Enable", config->Steer.Enable);

            mbCtx.FloatOptionCb("Limit", config->Steer.Limit,
                                0.0f, 60.0f, 0.1f, MenuUtils::GetKbFloat);

            mbCtx.FloatOptionCb("Sensitivity", config->Steer.SteeringMultiplier,
                                0.0f, 2.0f, 0.1f, MenuUtils::GetKbFloat);
        });

    /* mainmenu -> npcmenu */
    submenus.emplace_back("npcmenu",
        [](NativeMenu::Menu& mbCtx, CHeadlightsScript& context) {
            mbCtx.Title("NPC options");
            mbCtx.Subtitle("");

            mbCtx.BoolOption("Use player light settings", AdaptiveHeadlights::GetSettings().Main.EnableNPC,
                { "Use the players' brightness, range, etc. from visualsettings.dat, for the NPCs." });
        });

    /* mainmenu -> developermenu */
    submenus.emplace_back("developermenu",
        [](NativeMenu::Menu& mbCtx, CHeadlightsScript& context) {
            mbCtx.Title("Developer options");
            mbCtx.Subtitle("");

            mbCtx.Option("Placeholder");
        });

    return submenus;
}

std::vector<std::string> AdaptiveHeadlights::FormatConfig(CHeadlightsScript& context, const CConfig& config) {
    std::vector<std::string> extras{
        std::format("Name: {}", config.Name),
        std::format("Model: {}", config.ModelName.empty() ? "None (Generic)" : config.ModelName),
        std::format("Plate: {}", config.Plate.empty() ? "None" : std::format("[{}]", config.Plate)),
        std::format("Correction: {}", config.Correction.Enable ?
            std::format("Yes: Low {:+.2f} / High {:+.2f} degrees",
                config.Correction.PitchAdjustLowBeam, config.Correction.PitchAdjustHighBeam) :
            "No"),
        std::format("Self-leveling: {}", config.Level.Enable ?
            std::format("Yes: Between {:+.2f} and {:+.2f} degrees",
                config.Level.LowerLimit, config.Level.UpperLimit) :
            "No"),
        std::format("Cornering: {}", config.Steer.Enable ?
            std::format("Yes: Sensitivity {:.2f}x, limit {:+.2f} degrees",
                config.Steer.SteeringMultiplier, config.Steer.Limit) :
            "No"),
    };
    return extras;
}

bool AdaptiveHeadlights::PromptSave(CHeadlightsScript& context, CConfig& config, Hash model, std::string plate) {
    UI::ShowHelpText("Enter new configuration name.");
    std::string newName = UI::GetKeyboardResult();

    if (newName.empty()) {
        UI::ShowHelpText("No configuration name entered, save cancelled.");
        return false;
    }

    UI::ShowHelpText("Enter '1' for a generic model configuration.\n"
                     "Enter '2' for a model and plate matched configuration.");
    std::string configMode = UI::GetKeyboardResult();

    CConfig::ESaveType saveType;
    if (configMode == "1") {
        saveType = CConfig::ESaveType::GenericModel;
    }
    else if (configMode == "2") {
        saveType = CConfig::ESaveType::Specific;
    }
    else {
        UI::ShowHelpText("No supported configuration type entered, save cancelled.");
        return false;
    }

    if (config.Write(newName, model, plate, saveType))
        UI::Notify("New configuration saved.", true);
    else
        UI::Notify("~r~An error occurred~s~, failed to save new configuration.\n"
                   "Check the log file for further details.", true);
    AdaptiveHeadlights::LoadConfigs();

    return true;
}

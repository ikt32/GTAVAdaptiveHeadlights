#include "Script.hpp"
#include "Constants.hpp"

#include "Util/FileVersion.hpp"
#include "Util/Paths.hpp"
#include "Util/Logger.hpp"
#include "Memory/Versions.h"
#include "Memory/VehicleExtensions.hpp"

#include <inc/main.h>

#include <filesystem>

namespace fs = std::filesystem;

void resolveVersion() {
    int shvVersion = getGameVersion();

    LOG(INFO, "SHV API Game version: {} ({})", eGameVersionToString(shvVersion), shvVersion);
    SVersion exeVersion = getExeInfo();

    // Version we *explicitly* support
    std::vector<int> exeVersionsSupp = findNextLowest(ExeVersionMap, exeVersion);
    if (exeVersionsSupp.empty() || exeVersionsSupp.size() == 1 && exeVersionsSupp[0] == -1) {
        LOG(ERROR, "Failed to find a corresponding game version.");
        LOG(WARN, "    Using SHV API version [{}] ({})",
            eGameVersionToString(shvVersion), shvVersion);
        VehicleExtensions::SetVersion(shvVersion);
        return;
    }

    int highestSupportedVersion = *std::max_element(std::begin(exeVersionsSupp), std::end(exeVersionsSupp));
    if (shvVersion > highestSupportedVersion) {
        LOG(WARN, "Game newer than last supported version");
        LOG(WARN, "    You might experience instabilities or crashes");
        LOG(WARN, "    Using SHV API version [{}] ({})",
            eGameVersionToString(shvVersion), shvVersion);
        VehicleExtensions::SetVersion(shvVersion);
        return;
    }

    int lowestSupportedVersion = *std::min_element(std::begin(exeVersionsSupp), std::end(exeVersionsSupp));
    if (shvVersion < lowestSupportedVersion) {
        LOG(WARN, "SHV API reported lower version than actual EXE version.");
        LOG(WARN, "    EXE version     [{}] ({})",
            eGameVersionToString(lowestSupportedVersion), lowestSupportedVersion);
        LOG(WARN, "    SHV API version [{}] ({})",
            eGameVersionToString(shvVersion), shvVersion);
        LOG(WARN, "    Using EXE version, or highest supported version [{}] ({})",
            eGameVersionToString(lowestSupportedVersion), lowestSupportedVersion);
        VehicleExtensions::SetVersion(lowestSupportedVersion);
        return;
    }

    LOG(DEBUG, "Using offsets based on SHV API version [{}] ({})",
        eGameVersionToString(shvVersion), shvVersion);
    VehicleExtensions::SetVersion(shvVersion);
}

void initializePaths(HMODULE hInstance) {
    Paths::SetOurModuleHandle(hInstance);

    auto localAppDataPath = Paths::GetLocalAppDataPath();
    auto localAppDataModPath = localAppDataPath / Constants::iktFolder / Constants::ScriptFolder;
    std::string originalModPath = Paths::GetModuleFolder(hInstance) + std::string("\\") + Constants::ScriptFolder;
    Paths::SetModPath(originalModPath);

    bool useAltModPath = false;
    if (std::filesystem::exists(localAppDataModPath)) {
        useAltModPath = true;
    }

    std::string modPath;
    std::string logFile;

    // Use LocalAppData if it already exists.
    if (useAltModPath) {
        modPath = localAppDataModPath.string();
        logFile = (localAppDataModPath / (Paths::GetModuleNameWithoutExtension(hInstance) + ".log")).string();
    }
    else {
        modPath = originalModPath;
        logFile = modPath + std::string("\\") + Paths::GetModuleNameWithoutExtension(hInstance) + ".log";
    }

    Paths::SetModPath(modPath);

    if (!fs::is_directory(modPath) || !fs::exists(modPath)) {
        fs::create_directories(modPath);
    }

    g_Logger.SetFile(logFile);
    g_Logger.Clear();

    if (g_Logger.Error()) {
        modPath = localAppDataModPath.string();
        logFile = (localAppDataModPath / (Paths::GetModuleNameWithoutExtension(hInstance) + ".log")).string();
        fs::create_directories(modPath);

        Paths::SetModPath(modPath);
        g_Logger.SetFile(logFile);

        fs::copy(fs::path(originalModPath), localAppDataModPath, fs::copy_options::update_existing | fs::copy_options::recursive);

        // Fix perms
        for (auto& path : fs::recursive_directory_iterator(localAppDataModPath)) {
            try {
                fs::permissions(path, fs::perms::all);
            }
            catch (std::exception& e) {
                LOG(ERROR, "Failed to set permissions on [{}]: {}.", path.path().string(), e.what());
            }
        }

        g_Logger.ClearError();
        g_Logger.Clear();
        LOG(WARN, "Copied to [{}] from [{}] due to read/write issues.", modPath, originalModPath);
    }
}

BOOL APIENTRY DllMain(HMODULE hInstance, DWORD reason, LPVOID lpReserved) {
    const std::string modPath = Paths::GetModuleFolder(hInstance) + Constants::ScriptFolder;
    const std::string logFile = modPath + "\\" + Paths::GetModuleNameWithoutExtension(hInstance) + ".log";

    if (!fs::is_directory(modPath) || !fs::exists(modPath)) {
        fs::create_directory(modPath);
    }

    g_Logger.SetFile(logFile);
    Paths::SetOurModuleHandle(hInstance);

    switch (reason) {
        case DLL_PROCESS_ATTACH: {
            initializePaths(hInstance);
            LOG(INFO, "{} {} (built {} {})", Constants::ScriptName, Constants::DisplayVersion, __DATE__, __TIME__);
            resolveVersion();

            scriptRegister(hInstance, AdaptiveHeadlights::ScriptMain);
            LOG(INFO, "Script registered");
            break;
        }
        case DLL_PROCESS_DETACH: {
            scriptUnregister(hInstance);
            break;
        }
        default: {
            break;
        }
    }
    return TRUE;
}

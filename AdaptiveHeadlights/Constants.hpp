#pragma once
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define VERSION_MAJOR 1
#define VERSION_MINOR 0
#define VERSION_PATCH 1

namespace Constants {
    static const char* const ScriptName = "Adaptive Headlights";
    static const char* const NotificationPrefix = "~b~Adaptive Headlights~w~";
    static const char* const DisplayVersion = "v" STR(VERSION_MAJOR) "."  STR(VERSION_MINOR) "." STR(VERSION_PATCH);
    static const char* const iktFolder = "ikt";
    static const char* const ScriptFolder = "AdaptiveHeadlights";
}

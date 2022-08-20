#include "TuningBonesManip.hpp"
#include <MinHook.h>
namespace {
    bool hooked = false;
}
void ToggleHook(bool hook) {
    if (hook) {
        if (hooked) {
            return;
        }
        MH_Initialize();
        MH_EnableHook(MH_ALL_HOOKS);
        hooked = true;
    }
    else {
        if (!hooked) {
            return;
        }
        MH_DisableHook(MH_ALL_HOOKS);
        MH_RemoveHook(fnAddr);
        hooked = false;
    }
}

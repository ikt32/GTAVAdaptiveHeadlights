#pragma once

#include <cstdint>

enum EVehicleLightDamage : uint32_t {
    LeftHeadlight = 1 << 0,
    RightHeadlight = 1 << 1,

    LeftTailLight = 1 << 2,
    RightTailLight = 1 << 3,

    LeftFrontIndicatorLight = 1 << 4,
    RightFrontIndicatorLight = 1 << 5,
    LeftRearIndicatorLight = 1 << 6,
    RightRearIndicatorLight = 1 << 7,

    LeftBrakeLight = 1 << 8,
    RightBrakeLight = 1 << 9,
    MiddleBrakeLight = 1 << 10,

    LeftReverseLight = 1 << 11,
    RightReverseLight = 1 << 12,

    ExtraLight1 = 1 << 21,
    ExtraLight2 = 1 << 22,
    ExtraLight3 = 1 << 23,
    ExtraLight4 = 1 << 24,
};

enum EVehicleLightState : uint32_t {
    LightStateLowBeam = 1 << 6,
    LightStateHighBeam = 1 << 7,
    LightStateIndicatorLeft = 1 << 8,
    LightStateIndicatorRight = 1 << 9,
};

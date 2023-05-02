#include "VehicleExtensions.hpp"

#include "NativeMemory.hpp"
#include "Offsets.hpp"

#include "VehicleFlags.hpp"

#include "../Util/Logger.hpp"

#include <inc/main.h>

#include <vector>
#include <functional>

// <= b1493: 8  (Top gear = 7)
// >= b1604: 11 (Top gear = 10)
uint8_t g_numGears = 8;

eGameVersion g_gameVersion = getGameVersion();

namespace {
    template <typename T> T Sign(T val) {
        return static_cast<T>((T{} < val) - (val < T{}));
    }

    int lightsBrokenOffset = 0;
    int lightsBrokenVisuallyOffset = 0;
    int handlingOffset = 0;
    int lightStatesOffset = 0;
    int indicatorTimingOffset = 0;
    int steeringAngleInputOffset = 0;
    int steeringAngleOffset = 0;
    int wheelsPtrOffset = 0;
    int numWheelsOffset = 0;

    int wheelSuspensionCompressionOffset = 0;
}

/*
 * Offsets/patterns done by me might need revision, but they've been checked
 * against b1180.2 and b877.1 and are okay.
 */
void VehicleExtensions::Init() {
    mem::init();

    // Figuring out indicator timing: LieutenantDan
    uintptr_t addr = mem::FindPattern("\x44\x0F\xB7\x91\xDC\x00\x00\x00\x0F\xB7\x81\xB0\x0A\x00\x00\x41\xB9\x01\x00\x00\x00\x44\x03\x15\x8C\x63\xDF\x01",
        "xxxx????xxx????xxxxxxxxx????");
    indicatorTimingOffset = addr == 0 ? 0 : *(int*)(addr + 4);
    LOG(indicatorTimingOffset == 0 ? WARN : DEBUG, "Indicator timing offset: 0x{:03X}", indicatorTimingOffset);

    // 86C -> bulb
    addr = mem::FindPattern("F6 87 ? ? ? ? 02 75 06 C6 45 80 01");
    lightsBrokenOffset = addr == 0 ? 0 : *(int*)(addr + 2);;
    LOG(lightsBrokenOffset == 0 ? WARN : DEBUG, "Lights Broken Offset: 0x{:X}", lightsBrokenOffset);

    // 874 -> visibly
    lightsBrokenVisuallyOffset = addr == 0 ? 0 : lightsBrokenOffset + 8;
    LOG(lightsBrokenVisuallyOffset == 0 ? WARN : DEBUG, "Lights Visually Broken Offset: 0x{:X}", lightsBrokenVisuallyOffset);

    addr = mem::FindPattern("\x3C\x03\x0F\x85\x00\x00\x00\x00\x48\x8B\x41\x20\x48\x8B\x88",
        "xxxx????xxxxxxx");
    handlingOffset = addr == 0 ? 0 : *(int*)(addr + 0x16);
    LOG(handlingOffset == 0 ? WARN : DEBUG, "Handling Offset: 0x{:X}", handlingOffset);

    addr = mem::FindPattern("FD 02 DB 08 98 ? ? ? ? 48 8B 5C 24 30");
    lightStatesOffset = addr == 0 ? 0 : *(int*)(addr - 4) - 1;
    LOG(lightStatesOffset == 0 ? WARN : DEBUG, "Light States Offset: 0x{:X}", lightStatesOffset);
    // Or "8A 96 ? ? ? ? 0F B6 C8 84 D2 41", +10 or something (+31 is the engine starting bit), (0x928 starting addr)

    addr = mem::FindPattern("\x74\x0A\xF3\x0F\x11\xB3\x1C\x09\x00\x00\xEB\x25", "xxxxxx????xx");
    steeringAngleInputOffset = addr == 0 ? 0 : *(int*)(addr + 6);
    LOG(steeringAngleInputOffset == 0 ? WARN : DEBUG, "Steering Input Offset: 0x{:X}", steeringAngleInputOffset);

    steeringAngleOffset = addr == 0 ? 0 : *(int*)(addr + 6) + 8;
    LOG(steeringAngleOffset == 0 ? WARN : DEBUG, "Steering Angle Offset: 0x{:X}", steeringAngleOffset);

    addr = mem::FindPattern("\x3B\xB7\x48\x0B\x00\x00\x7D\x0D", "xx????xx");
    wheelsPtrOffset = addr == 0 ? 0 : *(int*)(addr + 2) - 8;
    LOG(wheelsPtrOffset == 0 ? WARN : DEBUG, "Wheels Pointer Offset: 0x{:X}", wheelsPtrOffset);

    numWheelsOffset = addr == 0 ? 0 : *(int*)(addr + 2);
    LOG(numWheelsOffset == 0 ? WARN : DEBUG, "Wheel Count Offset: 0x{:X}", numWheelsOffset);

    addr = mem::FindPattern("\x45\x0f\x57\xc9\xf3\x0f\x11\x83\x60\x01\x00\x00\xf3\x0f\x5c", "xxx?xxx???xxxxx");
    wheelSuspensionCompressionOffset = addr == 0 ? 0 : *(int*)(addr + 8);
    LOG(wheelSuspensionCompressionOffset == 0 ? WARN : DEBUG, "Wheel Suspension Compression Offset: 0x{:X}", wheelSuspensionCompressionOffset);
}

BYTE* VehicleExtensions::GetAddress(Vehicle handle) {
    return reinterpret_cast<BYTE*>(mem::GetAddressOfEntity(handle));
}

uint32_t VehicleExtensions::GetLightsBroken(Vehicle handle) {
    if (lightsBrokenOffset == 0) return 0;
    auto address = GetAddress(handle);
    return *reinterpret_cast<uint32_t*>(address + lightsBrokenOffset);
}

void VehicleExtensions::SetLightsBroken(Vehicle handle, uint32_t value) {
    if (lightsBrokenOffset == 0) return;
    auto address = GetAddress(handle);
    *reinterpret_cast<uint32_t*>(address + lightsBrokenOffset) = value;
}

uint32_t VehicleExtensions::GetLightsBrokenVisual(Vehicle handle) {
    if (lightsBrokenVisuallyOffset == 0) return 0;
    auto address = GetAddress(handle);
    return *reinterpret_cast<uint32_t*>(address + lightsBrokenVisuallyOffset);
}

void VehicleExtensions::SetLightsBrokenVisual(Vehicle handle, uint32_t value) {
    if (lightsBrokenVisuallyOffset == 0) return;
    auto address = GetAddress(handle);
    *reinterpret_cast<uint32_t*>(address + lightsBrokenVisuallyOffset) = value;
}

uint64_t VehicleExtensions::GetHandlingPtr(Vehicle handle) {
    if (handlingOffset == 0) return 0;
    auto address = GetAddress(handle);
    return *reinterpret_cast<uint64_t*>(address + handlingOffset);
}

void VehicleExtensions::SetHandlingPtr(Vehicle handle, uint64_t value) {
    if (handlingOffset == 0) return;
    auto address = GetAddress(handle);
    if (address == 0) return;
    *reinterpret_cast<uint64_t*>(address + handlingOffset) = value;
}

uint32_t VehicleExtensions::GetLightStates(Vehicle handle) {
    if (lightStatesOffset == 0) return 0;
    auto address = GetAddress(handle);
    return *reinterpret_cast<uint32_t*>(address + lightStatesOffset);
}

void VehicleExtensions::SetLightStates(Vehicle handle, uint32_t value) {
    if (lightStatesOffset == 0) return;
    auto address = GetAddress(handle);
    *reinterpret_cast<uint32_t*>(address + lightStatesOffset) = value;
}

bool VehicleExtensions::GetIndicatorHigh(Vehicle handle, int gameTime) {
    if (indicatorTimingOffset == 0) return false;
    auto address = GetAddress(handle);

    auto a = *reinterpret_cast<uint32_t*>(address + indicatorTimingOffset);
    a += (uint32_t)gameTime;
    a = a >> 9;
    a = a & 1;
    return a == 1;
}

float VehicleExtensions::GetSteeringInputAngle(Vehicle handle) {
    if (steeringAngleInputOffset == 0) return 0;
    auto address = GetAddress(handle);
    return *reinterpret_cast<float*>(address + steeringAngleInputOffset);
}

void VehicleExtensions::SetSteeringInputAngle(Vehicle handle, float value) {
    if (steeringAngleInputOffset == 0) return;
    auto address = GetAddress(handle);
    *reinterpret_cast<float*>(address + steeringAngleInputOffset) = value;
}

float VehicleExtensions::GetSteeringAngle(Vehicle handle) {
    if (steeringAngleOffset == 0) return 0;
    auto address = GetAddress(handle);
    return *reinterpret_cast<float*>(address + steeringAngleOffset);
}

void VehicleExtensions::SetSteeringAngle(Vehicle handle, float value) {
    if (steeringAngleOffset == 0) return;
    auto address = GetAddress(handle);
    *reinterpret_cast<float*>(address + steeringAngleOffset) = value;
}

uint64_t VehicleExtensions::GetWheelsPtr(Vehicle handle) {
    if (wheelsPtrOffset == 0) return 0;
    auto address = GetAddress(handle);
    return *reinterpret_cast<uint64_t*>(address + wheelsPtrOffset);
}

uint8_t VehicleExtensions::GetNumWheels(Vehicle handle) {
    if (numWheelsOffset == 0) return 0;
    auto address = GetAddress(handle);
    if (address == 0) return 0;
    return *reinterpret_cast<int*>(address + numWheelsOffset);
}

std::vector<Vector3> VehicleExtensions::GetWheelOffsets(Vehicle handle) {
    auto wheelPtr = GetWheelsPtr(handle);
    std::vector<Vector3> positions(GetNumWheels(handle));

    int offPosX = 0x20;
    int offPosY = 0x24;
    int offPosZ = 0x28;

    for (uint8_t i = 0; i < positions.size(); ++i) {
        auto wheelAddr = *reinterpret_cast<uint64_t*>(wheelPtr + 0x008 * i);
        positions.emplace_back(Vector3{
            *reinterpret_cast<float*>(wheelAddr + offPosX),
            *reinterpret_cast<float*>(wheelAddr + offPosY),
            *reinterpret_cast<float*>(wheelAddr + offPosZ),
            });
    }
    return positions;
}

std::vector<float> VehicleExtensions::GetWheelCompressions(Vehicle handle) {
    auto wheelPtr = GetWheelsPtr(handle);
    auto numWheels = GetNumWheels(handle);

    std::vector<float> compressions(numWheels);

    if (wheelSuspensionCompressionOffset == 0) return compressions;

    for (auto i = 0; i < numWheels; i++) {
        auto wheelAddr = *reinterpret_cast<uint64_t*>(wheelPtr + 0x008 * i);
        compressions[i] = *reinterpret_cast<float*>(wheelAddr + wheelSuspensionCompressionOffset);
    }
    return compressions;
}

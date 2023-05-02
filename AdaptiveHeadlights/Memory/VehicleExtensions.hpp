#pragma once
#include <inc/types.h>
#include <vector>
#include <cstdint>

class VehicleExtensions {
public:
    static void Init();

    static BYTE* GetAddress(Vehicle handle);

    static uint32_t GetLightsBroken(Vehicle handle);
    static void SetLightsBroken(Vehicle handle, uint32_t value);

    static uint32_t GetLightsBrokenVisual(Vehicle handle);
    static void SetLightsBrokenVisual(Vehicle handle, uint32_t value);

    static uint64_t GetHandlingPtr(Vehicle handle);
    static void SetHandlingPtr(Vehicle handle, uint64_t value);

    static uint32_t GetLightStates(Vehicle handle);
    static void SetLightStates(Vehicle handle, uint32_t value);

    static bool GetIndicatorHigh(Vehicle handle, int gameTime);

    // Steering input angle, steering lock independent
    static float GetSteeringInputAngle(Vehicle handle);
    static void SetSteeringInputAngle(Vehicle handle, float value);

    // Wheel angle, steering lock dependent
    static float GetSteeringAngle(Vehicle handle);
    static void SetSteeringAngle(Vehicle handle, float value);

    static uint64_t GetWheelsPtr(Vehicle handle);
    static uint8_t GetNumWheels(Vehicle handle);

    static std::vector<Vector3> GetWheelOffsets(Vehicle handle);
    static std::vector<float> GetWheelCompressions(Vehicle handle);
private:
    VehicleExtensions() = default;
};

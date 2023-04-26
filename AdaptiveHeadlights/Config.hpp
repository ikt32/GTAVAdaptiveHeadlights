#pragma once

#include <inc/types.h>
#include <string>
#include <vector>
#include <map>

class CConfig {
public:
    enum class ESaveType {
        Specific,       // [ID] writes Model + Plate
        GenericModel,   // [ID] writes Model
        GenericNone,    // [ID] writes none
    };

    CConfig() = default;
    static CConfig Read(const std::string& configFile);

    void Write(ESaveType saveType);
    bool Write(const std::string& newName, Hash model, std::string plate, ESaveType saveType);

    std::string Name;

    Hash ModelHash = 0;
    std::string ModelName;
    std::string Plate;

    // Bones
    struct {
        std::vector<std::string> LowLeft    = { "headlight_l" };
        std::vector<std::string> LowRight   = { "headlight_r" };
        std::vector<std::string> HighLeft   = { "headlight_l" };
        std::vector<std::string> HighRight  = { "headlight_r" };
        std::vector<std::string> Mods = { "extralight_1", "extralight_2" };
    } Bones;

    // Correction
    struct {
        bool Enable = false;
        float PitchAdjustLowBeam = 0.0f;
        float PitchAdjustHighBeam = 0.0f;
        float PitchAdjustMods = 0.0f;
    } Correction;

    // Level
    struct {
        bool EnableSuspension = false;
        float SpeedSuspension = 5.0f;

        bool EnableGyroscope = false;
        float SpeedGyroscope = 3.5f;

        float UpperLimit = 5.0f;
        float LowerLimit = -5.0f;
    } Level;

    // Steer
    struct {
        bool Enable = false;
        float SteeringMultiplier = 1.0f;
        float Limit = 10.0f;
    } Steer;
};

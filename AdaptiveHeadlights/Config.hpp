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

    Hash ModelHash;
    std::string ModelName;
    std::string Plate;

    // Main
    struct {
        bool Enable = false;
    } Main;

    // Correction
    struct {
        bool Enable = false;
        float PitchAdjust = 0.0f;
    } Correction;

    // Level
    struct {
        bool Enable = false;
        float UpperLimit = 2.0f;
        float LowerLimit = -2.0f;
    } Level;

    // Steer
    struct {
        bool Enable = false;
        float SteeringMultiplier = 1.0f;
        float Limit = 10.0f;
    };
};

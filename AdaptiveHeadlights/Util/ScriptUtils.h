#pragma once
#include "inc/types.h"
#include <string>

namespace Util {
    bool PlayerAvailable(Player player, Ped playerPed);
    bool VehicleAvailable(Vehicle vehicle, Ped playerPed);
    bool IsPedOnSeat(Vehicle vehicle, Ped ped, int seat);

    std::string GetFormattedModelName(Hash modelHash);
    std::string GetFormattedVehicleModelName(Vehicle vehicle);
}

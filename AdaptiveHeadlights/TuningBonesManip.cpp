/*
 * Most of this is by AlexGuirre!
 * Thanks a bunch!
 */
#include "TuningBonesManip.hpp"
#include "Memory/NativeMemory.hpp"
#include "Memory/VehicleBone.hpp"
#include "Util/Logger.hpp"
#include "Util/Math.hpp"
#include "Script.hpp"

#include <MinHook.h>
#include <DirectXMath.h>
#include <mutex>
#include <unordered_map>

using namespace DirectX;
static void* CVehicleStreamRenderGfx_GetModFragType_detour(void* thisStreamRenderGfx, uint8_t modType);
struct ClonedFragType {
    ~ClonedFragType() {
        if (boneTransformations) {
            delete boneTransformations;
            boneTransformations = nullptr;
        }
        if (originalBoneTransformations) {
            delete originalBoneTransformations;
            originalBoneTransformations = nullptr;
        }
    }
    alignas(16) uint8_t gtaFragType_Buffer[0x130];
    alignas(16) uint8_t fragDrawable_Buffer[0x150];
    alignas(16) uint8_t crSkeletonData_Buffer[0x70];
    void* origFragType = nullptr;
    XMMATRIX* boneTransformations = nullptr;
    // "original" as in "when we cloned it" and not the original object.
    XMMATRIX* originalBoneTransformations = nullptr;
};

struct VehicleModRenderData {
    Vehicle Vehicle;
    void* StreamRenderGfx;
    uint8_t ModType;

    bool operator==(const VehicleModRenderData& p) const {
        return Vehicle == p.Vehicle &&
               StreamRenderGfx == p.StreamRenderGfx &&
               ModType == p.ModType;
    }
};

namespace std {
    template <>
    struct hash<VehicleModRenderData> {
        std::size_t operator()(const VehicleModRenderData& k) const {
            return ((std::hash<int>()(k.Vehicle)
                ^ (std::hash<void*>()(k.StreamRenderGfx) << 1)) >> 1)
                ^ (std::hash<uint8_t>()(k.ModType) << 1);
        }
    };
}

namespace {
    bool hooked = false;
    static void* (*CVehicleStreamRenderGfx_GetModFragType_orig)(void* This, uint8_t modType);
    void* fnAddr = nullptr;

    std::mutex mapMutex;
    // Vehicle, streamRenderGfx, modType
    std::unordered_map<VehicleModRenderData, std::shared_ptr<ClonedFragType>> clonedFragTypes;
}

std::shared_ptr<ClonedFragType> cloneFragType(void* origFragType) {
    auto pShared = std::make_shared<ClonedFragType>();
    ClonedFragType& clonedFragType = *pShared;
    // copy the original data
    uint8_t* origFragDrawable = *(uint8_t**)((uint8_t*)origFragType + 0x30);
    uint8_t* origCrSkeletonData = *(uint8_t**)(origFragDrawable + 0x18);
    uint8_t* origBoneTransformations = *(uint8_t**)(origCrSkeletonData + 0x30);

    clonedFragType.origFragType = origFragType;
    std::memcpy(clonedFragType.gtaFragType_Buffer, origFragType, sizeof(clonedFragType.gtaFragType_Buffer));
    std::memcpy(clonedFragType.fragDrawable_Buffer, origFragDrawable, sizeof(clonedFragType.fragDrawable_Buffer));
    std::memcpy(clonedFragType.crSkeletonData_Buffer, origCrSkeletonData, sizeof(clonedFragType.crSkeletonData_Buffer));

    uint16_t numBones = *(uint16_t*)(origCrSkeletonData + 0x5E);
    clonedFragType.boneTransformations = new XMMATRIX[numBones];
    std::memcpy(clonedFragType.boneTransformations, origBoneTransformations, numBones * sizeof(XMMATRIX));

    clonedFragType.originalBoneTransformations = new XMMATRIX[numBones];
    std::memcpy(clonedFragType.originalBoneTransformations, origBoneTransformations, numBones * sizeof(XMMATRIX));

    // fixup the pointers to point to the modified bone transformations
    *(void**)(clonedFragType.crSkeletonData_Buffer + 0x30) = clonedFragType.boneTransformations;
    *(void**)(clonedFragType.fragDrawable_Buffer + 0x18) = clonedFragType.crSkeletonData_Buffer;
    *(void**)(clonedFragType.gtaFragType_Buffer + 0x30) = clonedFragType.fragDrawable_Buffer;
    return pShared;
}

void* GetStreamRenderGfx(CHeadlightsScript* script, void* thisStreamRenderGfx) {
    uint8_t* vehicle = reinterpret_cast<uint8_t*>(mem::GetAddressOfEntity(script->GetVehicle()));
    if (!vehicle) {
        return nullptr;
    }

    uint8_t* drawHandler = *(uint8_t**)(vehicle + 0x48); // cvehicle->drawHandler
    if (!drawHandler) {
        return nullptr;
    }

    void* streamRenderGfx = *(void**)(drawHandler + 0x40 + 0x330); // drawHandler->vehicleModMgr.streamRenderGfx
    // is this the CVehicleStreamRenderGfx of the target vehicle?
    if (!streamRenderGfx || thisStreamRenderGfx != streamRenderGfx) {
        return nullptr;
    }
    return streamRenderGfx;
}

static void* CVehicleStreamRenderGfx_GetModFragType_detour(void* thisStreamRenderGfx, uint8_t modType) {
    void* origFragType = CVehicleStreamRenderGfx_GetModFragType_orig(thisStreamRenderGfx, modType);

    if (modType != 1 /*VMT_BUMPER_F*/ || !origFragType) {
        return origFragType;
    }

    auto& scripts = AdaptiveHeadlights::GetScripts();

    if (scripts.empty()) {
        return origFragType;
    }

    CHeadlightsScript* applicableScript = nullptr;
    void* streamRenderGfx = nullptr;

    for (const auto& npcScript : scripts) {
        if (auto* foundStreamRenderGfx = GetStreamRenderGfx(&*npcScript, thisStreamRenderGfx)) {
            applicableScript = &*npcScript;
            streamRenderGfx = foundStreamRenderGfx;
            break;
        }
    }

    if (!applicableScript || !streamRenderGfx) {
        return origFragType;
    }

    if (!applicableScript->ActiveConfig()->Correction.Enable)
        return origFragType;

    ClonedFragType* clonedFragType = nullptr;
    {
        VehicleModRenderData renderData{ applicableScript->GetVehicle(), streamRenderGfx, modType};
        std::scoped_lock lock(mapMutex);

        if (auto it = clonedFragTypes.find(renderData);
            it == clonedFragTypes.end()) {
            clonedFragType = clonedFragTypes.insert({ renderData, cloneFragType(origFragType) }).first->second.get();
            LOG(DEBUG, "Inserted ({} {} {})", applicableScript->GetVehicle(), streamRenderGfx, modType);
        }
        else {
            clonedFragType = it->second.get();
        }
    }

    if (!clonedFragType) {
        return origFragType;
    }

    uint8_t* origFragDrawable = *(uint8_t**)((uint8_t*)origFragType + 0x30);
    uint8_t* origCrSkeletonData = *(uint8_t**)(origFragDrawable + 0x18);
    uint8_t* origBoneTransformations = *(uint8_t**)(origCrSkeletonData + 0x30);
    uint16_t numBones = *(uint16_t*)(origCrSkeletonData + 0x5E);

    // modify the bone transforms
    auto adjust = deg2rad(applicableScript->ActiveConfig()->Correction.PitchAdjustMods);
    XMMATRIX rot = XMMatrixRotationX(adjust);
    for (int i = 0; i < numBones; i++) {
        auto boneName = ((VehicleBones::crSkeletonData*)origCrSkeletonData)->GetBoneNameForIndex(i);
        const auto& tuningBones = applicableScript->ActiveConfig()->Bones.Mods;
        if (std::find(tuningBones.begin(), tuningBones.end(), boneName) != tuningBones.end()) {
            clonedFragType->boneTransformations[i] = rot * clonedFragType->originalBoneTransformations[i];
        }
    }

    return clonedFragType->gtaFragType_Buffer;
}

void TuningBones::ToggleHook(bool hook) {
    if (!fnAddr)
        fnAddr = (void*)mem::FindPattern("80 FA 33 73 57 0F B6 C2 48 8B 8C C1 ? ? ? ? 48 85 C9 74 47 8B 01");

    if (!fnAddr) {
        LOG(ERROR, "Failed to find CVehicleStreamRenderGfx_GetModFragType pattern.");
        return;
    }

    if (hook) {
        if (hooked) {
            return;
        }
        MH_Initialize();
        MH_CreateHook(fnAddr, &CVehicleStreamRenderGfx_GetModFragType_detour, (void**)&CVehicleStreamRenderGfx_GetModFragType_orig);
        MH_EnableHook(MH_ALL_HOOKS);
        hooked = true;
        LOG(DEBUG, "Hooked CVehicleStreamRenderGfx_GetModFragType");
    }
    else {
        if (!hooked) {
            return;
        }
        MH_DisableHook(MH_ALL_HOOKS);
        MH_RemoveHook(fnAddr);
        hooked = false;
        LOG(DEBUG, "Unhooked CVehicleStreamRenderGfx_GetModFragType");
    }
}

void TuningBones::ClearStaleEntries() {
    std::scoped_lock lock(mapMutex);
    for (auto instIt = clonedFragTypes.begin(); instIt != clonedFragTypes.end();) {
        if (!mem::GetAddressOfEntity(instIt->first.Vehicle)) {
            instIt = clonedFragTypes.erase(instIt);
        }
        else {
            ++instIt;
        }
    }
}

#pragma once
#include <cstdint>
#include <winnt.h>
// Unknown Modders' work

enum eVehicleFlag1 : uint32_t {
    FLAG_SMALL_WORKER = 0x00000001,
    FLAG_BIG = 0x00000002,
    FLAG_NO_BOOT = 0x00000004,
    FLAG_0x133F18EF = 0x00000008,
    FLAG_BOOT_IN_FRONT = 0x00000010,
    FLAG_IS_VAN = 0x00000020,
    FLAG_AVOID_TURNS = 0x00000040,
    FLAG_HAS_LIVERY = 0x00000080,
    FLAG_0x75674EB4 = 0x00000100,
    FLAG_SPORTS = 0x00000200,
    FLAG_DELIVERY = 0x00000400,
    FLAG_0xB5A93F62 = 0x00000800,
    FLAG_0x4F9A82E2 = 0x00001000,
    FLAG_TALL_SHIP = 0x00002000,
    FLAG_SPAWN_ON_TRAILER = 0x00004000,
    FLAG_SPAWN_BOAT_ON_TRAILER = 0x00008000,
    FLAG_EXTRAS_GANG = 0x00010000,
    FLAG_EXTRAS_CONVERTIBLE = 0x00020000,
    FLAG_EXTRAS_TAXI = 0x00040000,
    FLAG_EXTRAS_RARE = 0x00080000,
    FLAG_EXTRAS_REQUIRE = 0x00100000,
    FLAG_EXTRAS_STRONG = 0x00200000,
    FLAG_EXTRAS_ONLY_BREAK_WHEN_DESTROYED = 0x00400000,
    FLAG_EXTRAS_SCRIPT = 0x00800000,
    FLAG_EXTRAS_ALL = 0x01000000,
    FLAG_0x44F7BDAB = 0x02000000,
    FLAG_DONT_ROTATE_TAIL_ROTOR = 0x04000000,
    FLAG_PARKING_SENSORS = 0x08000000,
    FLAG_PEDS_CAN_STAND_ON_TOP = 0x10000000,
    FLAG_0x77C9F804 = 0x20000000,
    FLAG_GEN_NAVMESH = 0x40000000,
    FLAG_LAW_ENFORCEMENT = 0x80000000
};
DEFINE_ENUM_FLAG_OPERATORS(eVehicleFlag1);

enum eVehicleFlag2 : uint32_t {
    FLAG_EMERGENCY_SERVICE = 0x00000001,
    FLAG_DRIVER_NO_DRIVE_BY = 0x00000002,
    FLAG_NO_RESPRAY = 0x00000004,
    FLAG_IGNORE_ON_SIDE_CHECK = 0x00000008,
    FLAG_RICH_CAR = 0x00000010,
    FLAG_AVERAGE_CAR = 0x00000020,
    FLAG_POOR_CAR = 0x00000040,
    FLAG_ALLOWS_RAPPEL = 0x00000080,
    FLAG_DONT_CLOSE_DOOR_UPON_EXIT = 0x00000100,
    FLAG_USE_HIGHER_DOOR_TORQUE = 0x00000200,
    FLAG_DISABLE_THROUGH_WINDSCREEN = 0x00000400,
    FLAG_IS_ELECTRIC = 0x00000800,
    FLAG_NO_BROKEN_DOWN_SCENARIO = 0x00001000,
    FLAG_IS_JETSKI = 0x00002000,
    FLAG_DAMPEN_STICKBOMB_DAMAGE = 0x00004000,
    FLAG_DONT_SPAWN_IN_CARGEN = 0x00008000,
    FLAG_IS_OFFROAD_VEHICLE = 0x00010000,
    FLAG_INCREASE_PED_COMMENTS = 0x00020000,
    FLAG_EXPLODE_ON_CONTACT = 0x00040000,
    FLAG_USE_FAT_INTERIOR_LIGHT = 0x00080000,
    FLAG_HEADLIGHTS_USE_ACTUAL_BONE_POS = 0x00100000,
    FLAG_FAKE_EXTRALIGHTS = 0x00200000,
    FLAG_CANNOT_BE_MODDED = 0x00400000,
    FLAG_DONT_SPAWN_AS_AMBIENT = 0x00800000,
    FLAG_IS_BULKY = 0x01000000,
    FLAG_BLOCK_FROM_ATTRACTOR_SCENARIO = 0x02000000,
    FLAG_IS_BUS = 0x04000000,
    FLAG_USE_STEERING_PARAM_FOR_LEAN = 0x08000000,
    FLAG_CANNOT_BE_DRIVEN_BY_PLAYER = 0x10000000,
    FLAG_SPRAY_PETROL_BEFORE_EXPLOSION = 0x20000000,
    FLAG_ATTACH_TRAILER_ON_HIGHWAY = 0x40000000,
    FLAG_ATTACH_TRAILER_IN_CITY = 0x80000000
};
DEFINE_ENUM_FLAG_OPERATORS(eVehicleFlag2);

enum eVehicleFlag3 : uint32_t {
    FLAG_HAS_NO_ROOF = 0x00000001,
    FLAG_0xFC6563DB = 0x00000002,
    FLAG_RECESSED_HEADLIGHT_CORONAS = 0x00000004,
    FLAG_RECESSED_TAILLIGHT_CORONAS = 0x00000008,
    FLAG_0xEF78E6B2 = 0x00000010,
    FLAG_HEADLIGHTS_ON_LANDINGGEAR = 0x00000020,
    FLAG_CONSIDERED_FOR_VEHICLE_ENTRY_WHEN_STOOD_ON = 0x00000040,
    FLAG_GIVE_SCUBA_GEAR_ON_EXIT = 0x00000080,
    FLAG_0xEF1764E1 = 0x00000100,
    FLAG_IS_TANK = 0x00000200,
    FLAG_USE_COVERBOUND_INFO_FOR_COVERGEN = 0x00000400,
    FLAG_CAN_BE_DRIVEN_ON = 0x00000800,
    FLAG_HAS_BULLETPROOF_GLASS = 0x00001000,
    FLAG_CANNOT_TAKE_COVER_WHEN_STOOD_ON = 0x00002000,
    FLAG_INTERIOR_BLOCKED_BY_BOOT = 0x00004000,
    FLAG_DONT_TIMESLICE_WHEELS = 0x00008000,
    FLAG_FLEE_FROM_COMBAT = 0x00010000,
    FLAG_0xE1AB06F0 = 0x00020000,
    FLAG_DRIVER_SHOULD_BE_MALE = 0x00040000,
    FLAG_COUNT_AS_FACEBOOK_DRIVEN = 0x00080000,
    FLAG_BIKE_CLAMP_PICKUP_LEAN_RATE = 0x00100000,
    FLAG_PLANE_WEAR_ALTERNATIVE_HELMET = 0x00200000,
    FLAG_USE_STRICTER_EXIT_COLLISION_TESTS = 0x00400000,
    FLAG_TWO_DOORS_ONE_SEAT = 0x00800000,
    FLAG_USE_LIGHTING_INTERIOR_OVERRIDE = 0x01000000,
    FLAG_USE_RESTRICTED_DRIVEBY_HEIGHT = 0x02000000,
    FLAG_CAN_HONK_WHEN_FLEEING = 0x04000000,
    FLAG_PEDS_INSIDE_CAN_BE_SET_ON_FIRE_MP = 0x08000000,
    FLAG_REPORT_CRIME_IF_STANDING_ON = 0x10000000,
    FLAG_HELI_USES_FIXUPS_ON_OPEN_DOOR = 0x20000000,
    FLAG_FORCE_ENABLE_CHASSIS_COLLISION = 0x40000000,
    FLAG_CANNOT_BE_PICKUP_BY_CARGOBOB = 0x80000000
};
DEFINE_ENUM_FLAG_OPERATORS(eVehicleFlag3);

enum eVehicleFlag4 : uint32_t {
    FLAG_CAN_HAVE_NEONS = 0x00000001,
    FLAG_HAS_INTERIOR_EXTRAS = 0x00000002,
    FLAG_HAS_TURRET_SEAT_ON_VEHICLE = 0x00000004,
    FLAG_0xC5C455F1 = 0x00000008,
    FLAG_DISABLE_AUTO_VAULT_ON_VEHICLE = 0x00000010,
    FLAG_USE_TURRET_RELATIVE_AIM_CALCULATION = 0x00000020,
    FLAG_USE_FULL_ANIMS_FOR_MP_WARP_ENTRY_POINTS = 0x00000040,
    FLAG_HAS_DIRECTIONAL_SHUFFLES = 0x00000080,
    FLAG_DISABLE_WEAPON_WHEEL_IN_FIRST_PERSON = 0x00000100,
    FLAG_USE_PILOT_HELMET = 0x00000200,
    FLAG_USE_WEAPON_WHEEL_WITHOUT_HELMET = 0x00000400,
    FLAG_PREFER_ENTER_TURRET_AFTER_DRIVER = 0x00000800,
    FLAG_USE_SMALLER_OPEN_DOOR_RATIO_TOLERANCE = 0x00001000,
    FLAG_USE_HEADING_ONLY_IN_TURRET_MATRIX = 0x00002000,
    FLAG_DONT_STOP_WHEN_GOING_TO_CLIMB_UP_POINT = 0x00004000,
    FLAG_HAS_REAR_MOUNTED_TURRET = 0x00008000,
    FLAG_DISABLE_BUSTING = 0x00010000,
    FLAG_IGNORE_RWINDOW_COLLISION = 0x00020000,
    FLAG_HAS_GULL_WING_DOORS = 0x00040000,
    FLAG_CARGOBOB_HOOK_UP_CHASSIS = 0x00080000,
    FLAG_0x8A4A78C6 = 0x00100000,
    FLAG_ALLOW_HATS_NO_ROOF = 0x00200000,
    FLAG_HAS_REAR_SEAT_ACTIVITIES = 0x00400000,
    FLAG_HAS_LOWRIDER_HYDRAULICS = 0x00800000,
    FLAG_HAS_BULLET_RESISTANT_GLASS = 0x01000000,
    FLAG_HAS_INCREASED_RAMMING_FORCE = 0x02000000,
    FLAG_HAS_CAPPED_EXPLOSION_DAMAGE = 0x04000000,
    FLAG_HAS_LOWRIDER_DONK_HYDRAULICS = 0x08000000,
    FLAG_HELICOPTER_WITH_LANDING_GEAR = 0x10000000,
    FLAG_JUMPING_CAR = 0x20000000,
    FLAG_HAS_ROCKET_BOOST = 0x40000000,
    FLAG_RAMMING_SCOOP = 0x80000000
};
DEFINE_ENUM_FLAG_OPERATORS(eVehicleFlag4);

enum eVehicleFlag5 : uint32_t {
    FLAG_HAS_PARACHUTE = 0x00000001,
    FLAG_RAMP = 0x00000002,
    FLAG_HAS_EXTRA_SHUFFLE_SEAT_ON_VEHICLE = 0x00000004,
    FLAG_0xFD328787 = 0x00000008,
    FLAG_0x809DD26E = 0x00000010,
    FLAG_RESET_TURRET_SEAT_HEADING = 0x00000020,
    FLAG_TURRET_MODS_ON_ROOF = 0x00000040,
    FLAG_UPDATE_WEAPON_BATTERY_BONES = 0x00000080,
    FLAG_DONT_HOLD_LOW_GEARS_WHEN_ENGINE_UNDER_LOAD = 0x00000100,
    FLAG_HAS_GLIDER = 0x00000200,
    FLAG_INCREASE_LOW_SPEED_TORQUE = 0x00000400,
    FLAG_USE_AIRCRAFT_STYLE_WEAPON_TARGETING = 0x00000800,
    FLAG_KEEP_ALL_TURRETS_SYNCHRONISED = 0x00001000,
    FLAG_SET_WANTED_FOR_ATTACHED_VEH = 0x00002000,
    FLAG_TURRET_ENTRY_ATTACH_TO_DRIVER_SEAT = 0x00004000,
    FLAG_USE_STANDARD_FLIGHT_HELMET = 0x00008000,
    FLAG_SECOND_TURRET_MOD = 0x00010000,
    FLAG_0x62CC4C3E = 0x00020000,
    FLAG_HAS_EJECTOR_SEATS = 0x00040000,
    FLAG_0x2028D687 = 0x00080000,
    FLAG_HAS_JATO_BOOST_MOD = 0x00100000,
    FLAG_IGNORE_TRAPPED_HULL_CHECK = 0x00200000,
    FLAG_HOLD_TO_SHUFFLE = 0x00400000,
    FLAG_TURRET_MOD_WITH_NO_STOCK_TURRET = 0x00800000,
    FLAG_EQUIP_UNARMED_ON_ENTER = 0x01000000,
    FLAG_DISABLE_CAMERA_PUSH_BEYOND = 0x02000000,
    FLAG_HAS_VERTICAL_FLIGHT_MODE = 0x04000000,
    FLAG_HAS_OUTRIGGER_LEGS = 0x08000000,
    FLAG_CAN_NAVIGATE_TO_ON_VEHICLE_ENTRY = 0x10000000,
    FLAG_DROP_SUSPENSION_WHEN_STOPPED = 0x20000000,
    FLAG_DONT_CRASH_ABANDONED_NEAR_GROUND = 0x40000000,
    FLAG_USE_INTERIOR_RED_LIGHT = 0x80000000
};
DEFINE_ENUM_FLAG_OPERATORS(eVehicleFlag5);

enum eVehicleFlag6 : uint32_t {
    FLAG_HAS_HELI_STRAFE_MODE = 0x00000001,
    FLAG_HAS_VERTICAL_ROCKET_BOOST = 0x00000002,
    FLAG_CREATE_WEAPON_MANAGER_ON_SPAWN = 0x00000004,
    FLAG_USE_ROOT_AS_BASE_LOCKON_POS = 0x00000008,
    FLAG_HEADLIGHTS_ON_TAP_ONLY = 0x00000010,
    FLAG_CHECK_WARP_TASK_FLAG_DURING_ENTER = 0x00000020,
    FLAG_USE_RESTRICTED_DRIVEBY_HEIGHT_HIGH = 0x00000040,
    FLAG_INCREASE_CAMBER_WITH_SUSPENSION_MOD = 0x00000080,
    FLAG_NO_HEAVY_BRAKE_ANIMATION = 0x00000100,
};
DEFINE_ENUM_FLAG_OPERATORS(eVehicleFlag6);

enum eWheelFlag : uint32_t {
    FLAG_UNK0 = 0x00000001,
    FLAG_ORIENT_LEFT = 0x00000002,
    FLAG_HAS_HANDBRAKE = 0x00000004,
    FLAG_IS_STEERED = 0x00000008,
    FLAG_IS_DRIVEN = 0x00000010,
};
DEFINE_ENUM_FLAG_OPERATORS(eWheelFlag);

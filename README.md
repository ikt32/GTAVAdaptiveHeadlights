# Adaptive Headlights

Adaptive Headlights brings helpful headlight features into the game.

* Self-leveling: headlights stay level with the road
  * With a start-up animation when turning on for the first time
* Cornering assist: headlights shine into the steering direction
* Correct wrongly aimed headlights
  * With separate low and high beam settings

## Requirements and installation

* Grand Theft Auto V (b2372+)
* [ScriptHookV](http://www.dev-c.com/gtav/scripthookv/)

To install, drag and drop the `AdaptiveHeadlights.asi` file and
`AdaptiveHeadlights` folder to the GTA V folder.

`%localappdata%/ikt/AdaptiveHeadlights` may also be used, if the GTA V folder
is not writeable.

Open the management menu with the `ah` cheat (use tilde (`~`) to open the cheat
console).

Cheat and hotkeys can be set in `settings_menu.ini`.

## Configuration files

Configuration files are placed in the `Configs` folder.

The file name: Anything goes, as long as it ends on `.ini`.
The file name without the extension is used as "Name" in the script.

```ini
[ID]
ModelHash = 87A21275
ModelName = rs4avant

; The headlight bones which the script changes.
; Multiple bones can be assigned to a headlight:
; Use comma separation.
[Bones]
; Low beam
LowLeft = headlight_l
LowRight = headlight_r
; High beam
HighLeft = headlight_l
HighRight = headlight_r
; For aim corrections for light-casting modified parts
Mods = extralight_1,extralight_2


[Correction]
Enable = true
PitchAdjustLowBeam = 2.9
PitchAdjustHighBeam = 10.5
PitchAdjustMods = -42.5


[Level]
EnableSuspension = true
SpeedSuspension = 5.0
EnableGyroscope = true
SpeedGyroscope = 3.5
UpperLimit = 5.0
LowerLimit = -5.0


[Steer]
Enable = false
SteeringMultiplier = 0.6
Limit = 17.5
```

## Notes

* The headlight bone moves around, which may result in clipping
  or movement of the headlight housing for a number of vehicles.

## Recommended

* [InfamousSabre's Headlights++](https://www.gta5-mods.com/misc/headlights-infamoussabre)

## Credits

Special thanks to AlexGuirre for the bone manipulation bits.

<sub>
© 2023 ikt. All rights reserved.
</sub>

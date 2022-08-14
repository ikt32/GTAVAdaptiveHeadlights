# Adaptive Headlights

Adaptive Headlights is a GTA V script that...

## Requirements and installation

* Grand Theft Auto V
* [ScriptHookV](http://www.dev-c.com/gtav/scripthookv/)

To install, drag and drop the `AdaptiveHeadlights.asi` file and `AdaptiveHeadlights` folder to the GTA V folder or `%localappdata%/ikt/AdaptiveHeadlights`.

Stuff...

Open the management menu with the `adaphead` cheat (use tilde (`~`) to open the cheat console).
Other hotkeys may be assigned in `settings_menu.ini`.

## Configuration files

Configuration files are to be placed in the `Configs` folder. These are the things to pay attention to:

File name: Anything goes, as long as it ends on `.ini`.
The file name without the extension is used as "Name" in the script.

```ini
[ID]
; Model name of vehicles this configuration applies for.
ModelName = r32

; Optional: Hash of model name.
; Only used if specified, may be omitted entirely.
ModelHash = 0x8C34D838

; Optional: License plate this configuration applies for.
; If both model and plate match, the plate variant of the configuration is prioritized.
; Only used if specified, may be omitted entirely.
Plate = 46EEK572

[TODO]
Key = Value
```

## Download

N/A

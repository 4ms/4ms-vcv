# CHANGE LOG

## v2.1.0

- Add Basic Sample Player module

- Made AltParams display directly in the context menu, instead of in a submenu

- Added color logo to non-hardware modules, and tweaked artwork so screws don't cover any art/words

- Updated MetaModule SDK to v2.1, and using the entire SDK as a submodule (not just core-interface)


## v2.0.15

- Add "Include Modules from:" right-click menu option that allows including only certain modules in the exported patch file. 
  Thanks to @rjsmith.
   - All: include all modules (current behavior)
   - Left Only: only modules touching each other to the left of the Hub
   - Right Only: only modules touching each other to the right of the Hub
   - Left & Right Only: only modules touching each other to the left or the right of the Hub
   - Connected: only modules connected to the Hub by cables, directly or indirectly through any number of other modules.

- Fix Wi-Fi URL being stored in patch file and overriding local settings

- Don't show Alt Parameters menu item if it would be empty

- Fix not exporting MIDI channel in patch file if no Split module was used

- Allow multiple MIDI modules of the same kind in a patch. Mappings are merged (different channels can be used on each)

- Update lots of parameter and jack labels:
  - Add units and useful ranges
  - All snapped knob position labels (e.g. QCD, PEG, ...)

- Fix Drum module CV jacks not working, and envelope ranges not set correctly

## v2.0.14

- Knob alias name is removed if knob no longer has any mappings

## v2.0.13

- Removed debug output to console

## v2.0.12

- Fix and improve some 4ms modules:
  - MultiLFO: add Fast/Slow switch and tempo LED
  - QCD: save and restore tempo in patch file. Defaults to running when adding a new module
  - EnOsc: fix Alt Params not being saved or loaded from patch files on hardware
  - KPLS: fix pitch jack holding voltage after cable is unplugged

- When all maps are removed from a panel knob, the alias name is cleared

- It's possible to use other modules to "remote control" the Wifi and Save buttons on the Hub

- Wifi timeout response is explicitly reported (instead of just "Failed")

- Allow Scope and Oscilloscope modules in patch files

- Save MIDI channel, poly mode, and pitchwheel range in patch file

- Don't change case of brand slugs HetrickCV and NonlinearCircuits when saving a patch file

## v2.0.11

- Fix patch name and wifi result text displaying badly when sending a patch via wifi

## v2.0.10

- Add MetaAIO audio/CV expander hub

- Fix DLD clock issues

- Fix CLKD occasionally getting stuck high

- Make Wifi button glow a little if a Wifi expander address is set

## v2.0.9

- Fixed default values for AltParams (e.g. Tapo Seq mode, or EnOsc Stereo Split)

- Fixed Tapographic Delay CV jacks being inverted

- Added Wi-Fi button on hub


## v2.0.8

- Fix plugin not loading on older Linux systems

## v2.0.7

- Add Bypass routes for FX modules (DLD, Tapo, BPF, Verb, PitchShift, LPF, HPF, Freeverb, Detune, ENVVCA, SHEV, DEV)

- Fixed Verb module: fixed CV jacks, added DC blocking, clamped output

- Fix labels on Verb and SHEV module (small text by switches missing on some systems)

- Fix Djembe locking up when Pitch CV is a high frequency

- plugin.json: Add some tags to previously un-tagged modules

## v2.0.6

- Removed unused Hold jack from Verb module

- When saving a MM patch, sort modules by row, then column.

- When saving a MM patch, typing a filename in the Save diaog box will
  automatically be copied to the patch name. Remove slashes, colons, quotes,
  and replace dots with underscores

- Make knob alias name text color legible, and make box wider in the context menu

- Use better color for cursor in text boxes on the Hub

- Add "Unmap" to Hub knob menu for each mapped knob

- Hide "Wrote Patch File:" text at top of module

- Fix crash when loading a preset on the Hub

- Doing Initialize on the Hub removes all mappings


## v2.0.5

- Fix knob alias names not always saving into MM patch file. See [Issue 20](https://github.com/4ms/4ms-vcv/issues/20)

## v2.0.4

- Fix issue where you couldn't map the same knob in two different knob sets,
  if the hub knob was the same in each. 
  See [Issue 16](https://github.com/4ms/4ms-vcv/issues/16)

- Ensemble Osc: Updated CV range of Pitch and Root jacks to be -8V to +8V.
  Previously was -5V to +5V, which did not fully cover the original hardware
  range of -2V to +6V.

- Refactor many jack inputs handling whether they are patched or not. Fixes
  issues on hardware MetaModule with MIDI jacks (non-streaming signals), but
  should not change behaviors when used from within VCV Rack.


## v2.0.3

- Fix noise when modulating Root CV on Ensemble Osc, see [Issue 13](https://github.com/4ms/4ms-vcv/issues/13)

## v2.0.2

- Fix crash on QPLFO module (and potential, but not reported crash on
  PitchShift). See [Issue 11](https://github.com/4ms/4ms-vcv/issues/11).

## v2.0.1

- Fix PEG and MPEG crashing on some platforms, see [Issue 9](https://github.com/4ms/4ms-vcv/issues/9)
- Using versioning required of Rack plugins


## v0.20.1 

Fixed compatibility with macOS 10.13, see [Issue 6](https://github.com/4ms/4ms-vcv/issues/6)

## v0.20.0 (aka Rack v2.0.0)

First release as a separate repository.



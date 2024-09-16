# CHANGE LOG

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



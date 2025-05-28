SVG info file Specifications
----------------------------
In general, all elements are specified with a circle. But some objects that require a horizontal/vertical orientation are specified by a rectangle.


**Red: Knob or Slider**
Circle = Knob
Default position specified by amount of blue:
0% (fully CCW): color is '#FF0000' or '#f00' or 'red'
25%: #FF0020
50%: #FF0040
75%: #FF0060
100%: #FF0080
[and any default value can be specified with anything between Hex 00 - Hex 80 for the last two digits]
See below for circle sizes

Rectangle = Slider
Same colors and default position as knob
2mm x 27mm = standard 4ms slider
If height > width then it's vertical. Otherwise it's horizontal

**Deep Magenta: Rotary Encoder**
color is '#C000C0'

**Green: Input jack, analog:**
color is '#00FF00' or '#0f0' or 'lime'

**Light Green: Input jack, gate**
color is '#80FF80'

**Blue: Output jack, analog**
color is '#0000FF' or '#00f' or 'blue'

**Light Blue: Output jack, gate**
color is '#8080FF'

**Magenta: LED**
color is #FF00FF
Append object name with `#classname`:

  - `#RedGreenBlueLight (default)`
  - `#RedBlueLight`
  - `#OrangeLight`
  - `#WhiteLight`
  - `#GreenLight`
  - `#RedLight`
  - `#BlueLight`

**Orange: Button - Latching**
color is '#FF8000' if default UP (not pressed/engaged)
color is '#FF8001' if default DOWN (pressed/engaged)

**Light Orange: Button - Momentary**
color is '#FFC000'

**Light Coral: Switch - 2pos**
color is '#FF8080' (default = 0), '#FF8081' (default = 1)
If height > width then it's vertical. Otherwise it's horizontal

**Faded Orange: Switch - 3pos**
color is '#FFC080' (default = 0), '#FFC081' (default = 1), or '#FFC082' (default = 2)
If height > width then it's vertical. Otherwise it's horizontal

**Yellow: TextDisplay
color is '#FFFF00' or '#ff0' or 'yellow'
Must be a rectangle, the width and height will be used.
Must specify a class name (which sets the font, color, and default text)

**Grey: AltParam**
color is '#808080'

By default this is a continuous range parameter (any value 0.0 to 1.0).

If the object name contains a single `@` followed by a number, then this is a discrete-valued choice without labels (AltParamChoice). 
Example: 2 choices is `Name@2`, 3 choices is `Name@3`, etc.

To provide names for the choices (AltParamChoiceLabeled), append them to the object name using the `@` separator. 
There must be at least two `@` symbols in the name.
Examples: `Speed@Low@Medium@High` or `Channels@1@2@3@4@5`
The number of choices will be deduced from the number of labels given.

The default choice can be specified by adding it to the blue color (zero-based, so `80` means the first choice).
Example: A 3-choice AltParam (`Name@3` or `Name@low@med@high`) with the default choice of 2 (the last choice) would have a color of `#808082` 
For continuous values, the default value is interpreted as a fixed-point number between 00 and FF. So 0x808080 is a default of 0.5. (0xFF is interpreted as 1.0, not 255/256)

If the shape is a rectangle then it will be linked to the region of the faceplate that it covers (position and size).
Otherwise, if the center X,Y position matches a knob, switch, button, or slider object exactly, then the AltParam is linked to that object.
Otherwise, it will be not linked to any other parameter.

-------------

**Knob sizes:**

Small (SMR Freq, EnOsc Detune, etc):
Diameter = 6mm
(or anything < 7mm)
Code: radius < 10px

Medium (normal Davies):
Diameter = 12mm
(or anything between 7mm and 14.1mm)
Code: 10px < radius < 20px

Large (EnOsc Root):
Diameter = 22mm
(or anything > 14.1mm)
Code: 20px < radius < 40px

----------

**Button sizes:**

Big button cap (EnOsc, SMR):
Diameter = 8mm
(or anything > 7mm)
Code: radius > 10

Small button cap (Tapo):
Diameter = 5mm
(or anything < 7mm)
Code: radius < 10

------------

**Switch positions:**

A text name for each switch position can be given using the `@` symbol as a separator.

Example:

A Switch 3-pos object (Faded Orange rectangle) with the name:
`Warp@Segment@Cheb@Fold`
The switch will be displayed as "Warp", and the three positions will be "Segment", "Cheb", and "Fold"


------------

**Class names:**

You can force a particular C++ class to be used for the element by appending `#ClassName` to the object name.
This is normally only done for LEDs, but it works for any object. The script does not check if the class name is 
valid and/or if you provided the right parameters to construct the element.

------------

**Element Names:**

Set the name of each object in AI or Inkscape to the display name.
Example: “Scale CV” (without the quotes).

As described above, you can append switch or AltParam position names to the object name using the `@` separator.
Also as described above, you can force a particular class to be used, by appending `#ClassName`.

There are two text elements required. They both must *not* be converted to outlines (must be editable text):
 
 - Text object with the name “slug”. Text content should be a short identifier (e.g. “SMR” or “EnOsc”)
 - Text object with the name “modulename”. Content is full module name (“Spectral Multiband Resonator”)

--------------

**Object Order:**

The order of the components in the layer in the SVG file is the order that they will be scrolled through on hardware 
Therefore, make sure the order makes sense to a user.
Generally, go from top-left to bottom-right, but with common-sense groupings applied.

For example, on something like the SMR's six channel sliders and buttons, either go through all buttons then all sliders,
or go button1-slider1-button2-slider2-etc. But don't go button-FreqKnob-slider1-button2-LFOCV-slider2.

------------

**Export from AI settings:**

File > Export > Export As… > select SVG, uncheck “Use Artboards”

Styling: Inline Style

Font: SVG

Images: Link (doesn't matter)

Object IDs: Layer Names

Decimal: 2

[unchecked: Minify] [unchecked: Responsive] 


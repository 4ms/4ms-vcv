import logging
import re
from helpers.util import *

ns = {
    "svg": "http://www.w3.org/2000/svg",
    "inkscape": "http://www.inkscape.org/namespaces/inkscape",
}

def get_knob_class_from_radius(radius):
    r = float(radius)
    if r < 10 and r > 3:
        return "Knob9mm" #<10: 8.5 typical
    if r < 20:
        return "Davies1900hBlackKnob" #10-20: 17.01 typical
    if r < 40:
        return "DaviesLargeKnob" #15-40: 31.18 typical

    return "Davies1900hBlackKnob" #under 3 or over 40 is not a known knob


def get_encoder_class_from_radius(radius):
    r = float(radius)
    if r < 10 and r > 3:
        return "Encoder" #<10: 8.5 typical
    if r < 20:
        return "EncoderRGB" #10-20: 17.01 typical
    if r < 40:
        return "EncoderWhiteLight" #15-40: 31.18 typical

    return "Encoder" #under 3 or over 40 is not a known knob


def get_button_style_from_radius(radius):
    r = float(radius)
    if r < 10 and r > 3:
        return "small" #<10: 7.09 typical?
    if r < 40:
        return "medium" #10-40: 11.34 typical

    return "unknown" #under 3 or over 40 is not a known style


def get_slider_class(c):
    cls = "Slider25mmHorizLED"
    #TODO: differentiate LED vs no LED
    
    if c['height'] > c['width']:
        cls = "Slider25mmVertLED"  
    else:
        cls = "Slider25mmHorizLED"

    return cls

def get_toggle2pos_class(c):
    if c['height'] > c['width']:
        return "Toggle2pos"
    else:
        return "Toggle2posHoriz"

def get_toggle3pos_class(c):
    if c['height'] > c['width']:
        return "Toggle3pos"
    else:
        return "Toggle3posHoriz"

def get_led_class_from_selector(selector: int):
    cls = "WhiteLight"

    if selector == 0xFF:
        cls = "RedGreenBlueLight"
    elif selector == 0xFE:
        cls = "RedBlueLight"
    elif selector == 0xFD:
        cls = "WhiteLight" 
    elif selector == 0xFC:
        cls = "RedLight" 
    elif selector == 0xFB:
        cls = "OrangeLight" 
    elif selector == 0xFA:
        cls = "GreenLight" 
    elif selector == 0xF9:
        cls = "BlueLight" 

    return cls

def expand_color_synonyms(color):
    if color == 'red' or color == '#f00': color = '#ff0000'
    if color == 'lime' or color == '#0f0': color = '#00ff00'
    if color == 'blue' or color == '#00f': color = '#0000ff'
    if color == 'yellow' or color == '#ff0': color = '#ffff00'
    if color == 'magenta' or color == '#f0f': color = '#ff00ff'
    if color == 'cyan' or color == '#0ff': color = '#00ffff'
    if color == 'black' or color == '#000': color = '#000000'
    if color == 'white' or color == '#fff': color = '#ffffff'
    if color == 'gray' or color == '#888': color = '#808080'
    if color == 'grey' or color == '#888': color = '#808080'
    return color


def get_components_group(root):
    groups = root.findall(".//svg:g[@inkscape:label='components']", ns)
    # Illustrator uses `id` for the group name.
    if len(groups) < 1:
        groups = root.findall(".//svg:g[@id='components']", ns)
    if len(groups) < 1:
        raise UserException("ERROR: Could not find \"components\" layer on panel")
    return groups[0]


def find_slug_and_modulename(components_group):
    texts = components_group.findall(".//svg:text", ns)
    slug = "Unnamed"
    moduleName = "Unnamed"
    for t in texts:
        name = t.get('{http://www.inkscape.org/namespaces/inkscape}label')
        if name is None:
            name = t.get('id')
        if name is None:
            name = t.get('data-name')
        if name is None:
            continue

        if name == "slug":
            slug = ""
            for m in t.itertext():
                slug += m

        if name == "modulename":
            moduleName = ""
            for m in t.itertext():
                moduleName += m

    subgroups = components_group.findall(".//svg:g", ns)
    if slug == "Unnamed":
        for t in subgroups:
            name = t.get('{http://www.inkscape.org/namespaces/inkscape}label')
            if name is None:
                name = t.get('id')
            if name is None:
                name = t.get('data-name')
            if name is None:
                continue
            if name == "slug":
                slug = ""
                for m in t.itertext():
                    slug += m

    if moduleName == "Unnamed":
        for t in subgroups:
            name = t.get('{http://www.inkscape.org/namespaces/inkscape}label')
            if name is None:
                name = t.get('id')
            if name is None:
                name = t.get('data-name')
            if name is None:
                continue
            if name == "modulename":
                moduleName = ""
                for m in t.itertext():
                    moduleName += m

    slug = re.sub(r'\W+', '', slug).strip().rstrip()
    moduleName = re.sub(r'[^a-zA-Z0-9_.-]+', ' ', moduleName).strip().rstrip()
    return slug, moduleName


def deduce_dpi(root):
    widthInches = get_dim_inches(root.get('width'))
    heightInches = get_dim_inches(root.get('height'))
    viewBox = root.get('viewBox')
    if widthInches == 0 or heightInches == 0 or viewBox is None:
        logging.warning("WARNING: DPI cannot be deduced, width, height, or viewBox is missing or 0 in root node. Using 72dpi")
        return 72

    viewBoxDims = viewBox.split(" ")
    viewWidth = float(viewBoxDims[2]) - float(viewBoxDims[0])
    viewHeight = float(viewBoxDims[3]) - float(viewBoxDims[1])
    hDPI = round(viewWidth / widthInches)
    vDPI = round(viewHeight / heightInches)
    if vDPI is not hDPI:
        logging.warning(f"WARNING: Horizontal DPI is {hDPI} and Vertical DPI is {vDPI}, which are not equal. Using horizontal value")
        vDPI = hDPI

    if hDPI == 1:
        logging.info(f"File is possibly already in 72DPI pixels for the root object units. Using 72 for DPI")
        hDPI = 72

    logging.info(f"DPI deduced as {hDPI}")

    return hDPI


def get_dim_inches(dimString):
    if dimString is None:
        return 0
    dimInches = float(re.sub("[^0-9.-]","", dimString))
    if dimInches is None:
        return 0
    if re.sub("[0-9.-]","", dimString) == "mm":
        dimInches /= 25.4
    return dimInches

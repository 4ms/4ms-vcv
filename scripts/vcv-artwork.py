#!/usr/bin/env python3

import os
import argparse
import xml.etree.ElementTree
import logging
from helpers.xml_helper import register_all_namespaces
from helpers.svg_parse_helpers import *

def extractForVcv(svgFilename, artworkFilename = None, slug = ""):
    if artworkFilename == None:
        outputpath = os.getenv('METAMODULE_ARTWORK_DIR')
        if outputpath is None:
            outputpath = pathFromHere("../../../res")
        artworkFilename = os.path.join(outputpath, slug + ".svg")

    logging.debug(f"reading from {svgFilename}, writing to {artworkFilename}")
    register_all_namespaces(svgFilename)

    tree = xml.etree.ElementTree.parse(svgFilename)
    root = tree.getroot()
    components_group = get_components_group(root)

    components_group.clear()
    logging.debug("Removed components layer")
    tree.write(artworkFilename)
    logging.info(f"Wrote artwork svg file for vcv: {artworkFilename}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser("Convert info SVG file to VCV SVG file")
    parser.add_argument("--input", required=True, help="Name of info SVG file (*.svg: typically XXXX_info.svg)")
    parser.add_argument("--output", required=True, help="Output file path (typically res/XXXX.svg)")
    parser.add_argument("-v", dest="verbose", help="Verbose logging", action="store_true")
    args = parser.parse_args()

    if args.verbose:
        logging.basicConfig(level=logging.DEBUG, format='%(message)s')
    else:
        logging.basicConfig(level=logging.INFO, format='%(message)s')

    if os.path.isdir(args.output):
        logging.error(f"Output {args.output} is not a file name")

    elif not os.path.isfile(args.input):
        logging.error(f"Input {args.input} is not a file name")

    else:
       extractForVcv(args.input, args.output)



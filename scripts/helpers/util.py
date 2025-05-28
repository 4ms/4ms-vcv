import os
import re

class UserException(Exception):
    pass

def pathFromHere(path):
    return os.path.join(os.path.dirname(os.path.realpath(__file__)),path)

def is_valid_slug(slug):
    return re.match(r'^[a-zA-Z0-9_\-]+$', slug) != None


def str_to_identifier(s):
    if not s:
        return "_"
    # Capitalize first letter
    s = s.title()
    # Replace + with P
    s = re.sub(r'\+', 'P', s)
    # Replace - with N
    s = re.sub(r'\-', 'N', s)
    # Replace | with OR 
    s = re.sub(r'\|', '_OR_', s)
    # Remove spaces
    s = re.sub(r' ', '', s)
    # Replace other special characters with underscore
    s = re.sub(r'\W', '_', s)
    # Identifiers can't start with a number
    if s[0].isdigit():
        s = "_" + s
    return s


def remove_trailing_dash_number(name):
    #Chop off -[\d\d] or -[\d]
    if name[-3:-2] == '-' and name[-2:].isdigit() and name[-1:].isdigit():
        name = name[:-3]
    if name[-2:-1] == '-' and name[-1:].isdigit():
        name = name[:-2]
    return name


def format_for_display(comp_name):
    return comp_name.replace('_',' ')


def format_as_enum_item(comp_name):
    return str_to_identifier(comp_name)

def format_as_legacy_enum_item(s):
    if not s:
        return "_"
    # Identifiers can't start with a number
    if s[0].isdigit():
        s = "_" + s
    # Capitalize first letter
    s = s.title()
    # Replace + with P
    s = re.sub(r'\+', 'P', s)
    # Replace - with N
    s = re.sub(r'\-', 'N', s)
    # Replace | with OR 
    s = re.sub(r'\|', '_OR_', s)
    # Spaces=>_
    s = re.sub(r' ', '_', s)
    # Replace other special characters with underscore
    s = re.sub(r'\W', '_', s)
    return s

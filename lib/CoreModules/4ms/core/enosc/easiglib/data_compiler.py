#!/usr/bin/env python

import numpy as np

class s1_15(np.float64): pass

def type_of(value):
    return {
        s1_15: lambda: "s1_15",
        int: lambda: "s32",
        np.int32: lambda: "s32",
        np.int16: lambda: "s16",
        np.uint32: lambda: "uint32_t",
        np.uint16: lambda: "u16",
        bool: lambda: "bool",
        float: lambda: "f",
        np.float64: lambda: "f",
        str: lambda: "string",
        tuple: lambda: ("tuple<%s>" % (", ".join(map(type_of, value)))),
        list: lambda: ("Buffer<%s, %d>" % (type_of(value[0]), len(value))),
        np.ndarray: lambda: ("Buffer<%s, %d>" % (type_of(value[0]), len(value))),
    }[type(value)]()

def is_base_type(value):
    t = type(value)
    if (t is int or
        t is bool or
        t is s1_15 or
        t is np.int16 or
        t is np.int32 or
        t is np.uint16 or
        t is np.uint32 or
        t is np.float64 or
        t is np.float32):
        return True;
    else:
        return False;

def write_value_of(file, value, indent_level):
    t = type(value)
    if (t is int or t is np.int32): file.write("%d_s32" % (value))
    elif (t is np.int16): file.write("%d_s16" % (value))
    elif (t is np.uint16): file.write("%d_u16" % (value))
    elif (t is np.uint32): file.write("%d" % (value))
    elif (t is s1_15): file.write("s1_15::inclusive(f(%f))" % (value))
    elif (t is bool): file.write("true" if value else "false")
    elif (t is float or t is np.float64): file.write("%.32f_f" % (value))
    elif (t is str): file.write("\"%s\"" % (value))
    elif (t is tuple):
        file.write("{")
        for v in value:
            write_value_of(file, v, indent_level+1)
            file.write(",")
        file.write("}")
    elif (t is list or t is np.ndarray):
        file.write("{{{\n")
        for v in value:
            file.write(" " * indent_level * 2)
            write_value_of(file, v, indent_level+1)
            file.write(",\n")
        file.write(" " * (indent_level-1) * 2)
        file.write("}}}")
    else: raise Exception("unknown type " + str(t))

def write_definition(file, name, value):
    file.write("const ");
    file.write(type_of(value))
    file.write(" " + name + " = ")
    write_value_of(file, value, 1)
    file.write(";\n")

def write_implementation_file(file_name, class_name, file, data):
    file.write("#include \""+file_name+".hh\"\n\n")
    file.write("using namespace std;\n\n")
    for name, value in data.items():
        if (not is_base_type(value)):
            file.write("/* %s */\n" % (name))
            write_definition(file, class_name+"::"+name, value)
            file.write("\n")

def write_header_file(class_name, file, data):
    file.write("#include \"lib/easiglib/numtypes.hh\"\n")
    file.write("#include \"lib/easiglib/buffer.hh\"\n\n")
    file.write("#pragma once\n\n")
    file.write("using namespace std;\n\n")
    file.write("struct "+class_name+" {\n")
    for name, value in data.items():
        if (is_base_type(value)):
            file.write("  static constexpr %s %s = " % (type_of(value), name))
            write_value_of(file, value, 0)
            file.write(";\n")
        else:
            file.write("  static const %s %s;\n" % (type_of(value), name))
    file.write("};")

def compile(file_name, class_name, data):
    with open(file_name+".hh", "w") as header_file:
        write_header_file(class_name, header_file, data)
    with open(file_name+".cc", "w") as implem_file:
        write_implementation_file(file_name, class_name, implem_file, data)

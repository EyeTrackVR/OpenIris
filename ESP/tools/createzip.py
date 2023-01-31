#!/usr/bin/env python3
Import("env")

import sys
from os import getcwd
from ntpath import basename
from zipfile import ZipFile
import json

my_flags = env.ParseFlags(env["BUILD_FLAGS"])
defines = dict()
for x in my_flags.get("CPPDEFINES"):
    if type(x) is tuple:
        (k, v) = x
        defines[k] = v
        # print("Type Tuple: %s" % x)
    elif type(x) is list:
        k = x[0]
        v = x[1]
        defines[k] = v
        # print("Type List: %s" % x)
    else:
        defines[x] = ""  # empty value
        # print("Warning: unknown type for %s" % x)

# strip quotes needed for shell escaping
s = lambda x: x.replace('"', "")


n = 2
partitions_arg = sys.argv[1:]
partitions = final = [
    partitions_arg[i * n : (i + 1) * n]
    for i in range((len(partitions_arg) + n - 1) // n)
]

# file_name = "build/" + str(env["PIOENV"]) + "/" + env["PROGNAME"] + ".zip"

file_name = "build/{1}/{2}.zip".format(str(env["PIOENV"]), env["PROGNAME"])

with ZipFile(file_name, "w") as archive:
    print('Creating "' + archive.filename + '"', end="\n")
    parts = []

    for [offset, path] in partitions:
        filename = basename(path)
        archive.write(path, filename)
        partition = {
            "path": filename,
            "offset": int(offset, 16),
        }
        parts.append(partition)

    manifest = {
        "chipFamily": "ESP32",
        "parts": parts,
    }
    archive.writestr("manifest.json", json.dumps(manifest))

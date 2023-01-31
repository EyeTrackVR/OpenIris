#!/usr/bin/env python3
Import("env")

import sys
import os
from ntpath import basename
from zipfile import ZipFile
import json


def createZip(source, target, env):
    if os == "linux":
        print("Program has been built, creating zip archive!")
        my_flags = env.ParseFlags(env["BUILD_FLAGS"])
        defines = dict()
        for x in my_flags.get("CPPDEFINES"):
            if type(x) is tuple:
                (k, v) = x
                defines[k] = v
            elif type(x) is list:
                k = x[0]
                v = x[1]
                defines[k] = v
            else:
                defines[x] = ""  # empty value
        s = lambda x: x.replace('"', "")

        print("FLASH_EXTRA_IMAGES: %s\n" % env["FLASH_EXTRA_IMAGES"])
        print("ESP32_APP_OFFSET: %s\n" % env["ESP32_APP_OFFSET"])
        array_args = [env["FLASH_EXTRA_IMAGES"], env["ESP32_APP_OFFSET"]]

        for (offset, image) in env["FLASH_EXTRA_IMAGES"]:
            print("\nImage: %s" % str(image))
            array_args.append(str(image))
            array_args.append(str(offset))

        for _source in source:
            print("\nSource: %s" % str(_source))
            array_args.append(str(_source))
        n = 2
        partitions_arg = array_args[1:]
        partitions = final = [
            partitions_arg[i * n : (i + 1) * n]
            for i in range((len(partitions_arg) + n - 1) // n)
        ]
        file_name = "./build/{0}/{1}.zip".format(str(env["PIOENV"]), env["PROGNAME"])
        with ZipFile(file_name, "w") as archive:
            print('\nCreating "' + archive.filename + '"', end="\n")
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
    else:
       print("Not running on Linux, skipping zip creation")

env.AddPostAction("$PROGPATH", createZip)

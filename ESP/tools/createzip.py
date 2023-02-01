#!/usr/bin/env python3
Import("env")

import sys
from ntpath import basename
from zipfile import ZipFile
import json


def createZip(source, target, env):
    if (
        sys.platform.startswith("ubuntu")
        or sys.platform.startswith("Linux")
        or sys.platform.startswith("linux")
    ):
        print("Program has been built, creating zip archive!")
        program_path = target[0].get_abspath()
        #print("binary path", program_path)
        #print("FLASH_EXTRA_IMAGES: %s\n" % env["FLASH_EXTRA_IMAGES"])
        #print("ESP32_APP_OFFSET: %s\n" % env["ESP32_APP_OFFSET"])
        
        array_args = [env["FLASH_EXTRA_IMAGES"]]

        for [offset, image] in env["FLASH_EXTRA_IMAGES"]:
            print("\nImage: %s" % str(image))
            array_args.append(str(offset))
            array_args.append(str(image))

        array_args.append(env["ESP32_APP_OFFSET"])
        array_args.append(program_path)

        n = 2
        partitions_arg = array_args[1:]
        print("partitions_arg: %s\n" % partitions_arg)
        partitions = final = [
            partitions_arg[i * n : (i + 1) * n]
            for i in range((len(partitions_arg) + n - 1) // n)
        ]
        print("partitions: %s\n" % partitions)
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


env.AddPostAction("$BUILD_DIR/${PROGNAME}.bin", createZip)

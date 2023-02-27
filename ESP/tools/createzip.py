#!/usr/bin/env python3
Import("env")

import sys
from colors import *
import os
from ntpath import basename
from zipfile import ZipFile
import json


def createZip(source, target, env):
    if os.getenv("OPENIRIS_CI_BUILD", False):
        if sys.platform.lower().startswith(("ubuntu", "linux")):
            sys.stdout.write(GREEN)
            print("Program has been built, creating zip archive!")
            program_path = target[0].get_abspath()

            array_args = [env["FLASH_EXTRA_IMAGES"]]

            for offset, image in env["FLASH_EXTRA_IMAGES"]:
                print("\nImage: %s" % str(image))
                array_args.extend([str(offset), str(image)])

            array_args.append(env["ESP32_APP_OFFSET"])
            array_args.append(program_path)

            n = 2
            partitions_arg = array_args[1:]
            sys.stdout.write(CYAN)
            print(f"partitions_args: {partitions_arg}")
            partitions = final = [
                partitions_arg[i * n : (i + 1) * n]
                for i in range((len(partitions_arg) + n - 1) // n)
            ]
            print(f"partitions: {partitions}")
            file_name = "./build/{0}/{1}.zip".format(
                str(env["PIOENV"]), env["PROGNAME"]
            )
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
                sys.stdout.write(RESET)
        else:
            sys.stdout.write(BLUE)
            print("Not running on Linux, skipping zip creation")
            sys.stdout.write(RESET)
    else:
        sys.stdout.write(BLUE)
        print("CI build not detected, skipping zip creation")
        sys.stdout.write(RESET)

env.AddPostAction("$BUILD_DIR/${PROGNAME}.bin", createZip)

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

                name = "OpenIris"
                version = env["PROGNAME"].split("-")[1]
                new_install_prompt_erase = True

                print("Creating manifest.json")

                temp = []
                for [offset, path] in partitions:
                    # join the offset and path into a space separated string
                    temp.append("{} {}".format(offset, path))

                # detect if the PIOENV has QIO flash mode
                flash_mode = env["BOARD_FLASH_MODE"]

                flash_freq = env["BOARD_F_FLASH"]

                if flash_freq == "80000000L":
                    flash_freq = "80m"
                elif flash_freq == "40000000L":
                    flash_freq = "40m"

                # detect the chip type
                chip_type = env["BOARD_MCU"]

                flash_size = '4'

                # TODO: detect the flash size from the board manifest
                if chip_type == "esp32s3":
                    flash_size = '8'

                # capitalize the chip type
                chip_type = chip_type.upper()

                # add hyphen between ESP32 and the suffix (WROOM, WROVER, etc)
                chip_type = chip_type.replace("ESP32", "ESP32-")

                print("Flash Mode: %s" % flash_mode)
                print("Chip Type: %s" % chip_type)
                print("Flash Frequency: %s" % flash_freq)
                print("Flash Size: %s" % flash_size)

                """
                python esptool.py --chip ESP32 merge_bin -o merged-firmware.bin --flash_mode dio --flash_freq 40m --flash_size 4MB
                0x1000 bootloader.bin 0x8000 partitions.bin 0xe000 boot.bin 0x10000 OpenIris-v1.3.0-esp32AIThinker-8229a3a-master.bin
                """
                execute_cmd = "$PYTHONEXE $PROJECT_PACKAGES_DIR/tool-esptoolpy/esptool.py --chip {chip} merge_bin -o merged-firmware.bin --flash_mode {flash_mode} --flash_freq {flash_freq} --flash_size {flash_size}MB %s" % (
                    " ".join(temp)
                )

                print("Executing: %s" % execute_cmd)

                env.Execute(
                    execute_cmd.format(
                        chip=chip_type, flash_mode=flash_mode, flash_freq=flash_freq, flash_size=flash_size)
                )

                filename = basename("merged-firmware.bin")
                archive.write("merged-firmware.bin", filename)

                partition = {
                    "path": filename,
                    "offset": 0,
                }
                parts.append(partition)

                manifest = {
                    "name": name,
                    "version": version,
                    "new_install_prompt_erase": new_install_prompt_erase,
                    "builds": [
                        {
                            "chipFamily": chip_type,
                            "parts": parts,
                        }
                    ],
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

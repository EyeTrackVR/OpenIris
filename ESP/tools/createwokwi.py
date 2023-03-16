#!/usr/bin/env python3
# https://docs.wokwi.com/vscode/getting-started

# TODO: Add generation of diagram.json file per board environment

Import("env")
try:

    def createTOML(source, target, env):
        try:
            firmware_name = env.subst("$BUILD_DIR\${PROGNAME}")
            firmware_name = firmware_name.split(".pio")[1]

            firmware_name = firmware_name.replace("\\", "/")

            print("Creating wokwi.toml for %s" % firmware_name)

            wokwi_string = """\
[wokwi]
version = 1
elf = ".pio{name}.elf"
firmware = ".pio{name}.bin"
[[net.forward]]
from = "localhost:8180"
to = "target:80"
"""
            toml_string = wokwi_string.format(name=firmware_name)
            print(toml_string)
            with open("wokwi.toml", "w") as f:
                f.write(toml_string)
                f.close()

            print("wokwi.toml created \n")

        except Exception as e:
            print("Error creating wokwi.toml: %s" % e)

    env.AddPostAction("$BUILD_DIR\${PROGNAME}.bin", createTOML)

except Exception as e:
    print("Error creating wokwi.toml: %s" % e)

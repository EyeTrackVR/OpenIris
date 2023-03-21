# Description: Custom name for firmware

Import("env")
import subprocess
import sys
from colors import *

project = ""
version = ""
commit = ""
branch = ""


def onError():
    print("Please install Git for Windows and add it to your PATH")
    print(
        "Continuing with default values for PIO_SRC_NAM, PIO_SRC_TAG, PIO_SRC_REV, PIO_SRC_BRH"
    )
    sys.stdout.write(RESET)

    project = "PIO"
    version = "0.0.0"
    commit = "0000000"
    branch = "main"

    customName(project, version, commit, branch)


def handleGit():
    try:
        checkgit = "git rev-parse --is-inside-work-tree"

        if subprocess.check_output(checkgit, shell=True).decode().strip() != "true":
            sys.stdout.write(RED)
            onError()

        sys.stdout.write(GREEN)
        print("Git is installed and we are in a Git repository,  continuing...")
        sys.stdout.write(RESET)

        # Get Git project name
        projcmd = "git rev-parse --show-toplevel"
        project = subprocess.check_output(projcmd, shell=True).decode().strip()
        project = project.split("/")
        project = project[len(project) - 1]

        # Get 0.0.0 version from latest Git tag
        tagcmd = "git describe --tags --always --abbrev=0"
        version = subprocess.check_output(tagcmd, shell=True).decode().strip()

        # Get latest commit short from Git
        revcmd = "git log --pretty=format:'%h' -n 1"
        commit = subprocess.check_output(revcmd, shell=True).decode().strip()

        # Get branch name from Git
        branchcmd = "git rev-parse --abbrev-ref HEAD"
        branch = subprocess.check_output(branchcmd, shell=True).decode().strip()

        print("Project: %s" % project)
        print("Version: %s" % version)
        print("Commit: %s" % commit)
        print("Branch: %s" % branch)

        # Make all available for use in the macros
        customName(project, version, commit, branch)

        sys.stdout.write(GREEN)
        print("Git information has been added to the build flags")
        # print(env.Dump())
        sys.stdout.write(RESET)

    except subprocess.CalledProcessError as e:
        sys.stdout.write(RED)
        print("Error: %s" % e)
        onError()


def customName(project, version, commit, branch):

    my_flags = env.ParseFlags(env["BUILD_FLAGS"])
    defines = dict()

    # add the git information to the build flags
    my_flags.get("CPPDEFINES").append(("PIO_SRC_NAM", project))
    my_flags.get("CPPDEFINES").append(("PIO_SRC_TAG", version))
    my_flags.get("CPPDEFINES").append(("PIO_SRC_REV", commit))
    my_flags.get("CPPDEFINES").append(("PIO_SRC_BRH", branch))

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

    # print("Project: %s" % defines)
    # strip quotes needed for shell escaping
    s = lambda x: x.replace('"', "")
    s = lambda x: x.replace("'", "")

    """ env.Replace(
        PROGNAME="%s-%s-%s-%s-%s"
        % (
            s(defines.get("PIO_SRC_NAM")),
            s(defines.get("PIO_SRC_TAG")),
            str(env["PIOENV"]),
            s(defines.get("PIO_SRC_REV")),
            s(defines.get("PIO_SRC_BRH")),
        )
    ) """

    env.Replace(
        PROGNAME="%s-%s-%s"
        % (
            str(env["PIOENV"]),
            env.GetProjectOption("custom_firmware_version"),
            s(defines.get("PIO_SRC_BRH")),
        )
    )


    # detect if there is a forward slash in the PROGNAME and replace it with an underscore
    if "/" in env["PROGNAME"]:
        env.Replace(PROGNAME="%s" % (env["PROGNAME"].replace("/", "-")))

    # create a file with the name of the firmware
    env.Execute(
        "echo %s > %s" % (env["PROGNAME"], env.subst("./tools/firmware_name.txt"))
    )


try:
    flags = env["BUILD_FLAGS"]
    my_flags = env.ParseFlags(flags)

    # Dump global construction environment (for debug purpose)
    # write env.Dump() to a file
    #with open("env_dump.txt", "w") as f:
    #    f.write(env.Dump())

    handleGit()
except ValueError as ex:
    # look for apostrophes and warn the user
    sys.stdout.write(RED)
    print(
        "[Warning]: Apostrophes are not allowed in the build flags. Please remove them from the \033[;1m\033[1;36m`user-config.ini` \033[1;31mfile and try again."
    )
    raise Exception(
        "Could not parse BUILD_FLAGS - Possible apostrophy used in user configuration",
        ex,
    )

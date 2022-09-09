import subprocess

# Get Git project name
projcmd = "git rev-parse --show-toplevel"
project = subprocess.check_output(projcmd, shell=True).decode().strip()
project = project.split("/")
project = project[len(project)-1]

# Get 0.0.0 version from latest Git tag
# tagcmd = "git describe --tags --abbrev=0"
# version = subprocess.check_output(tagcmd, shell=True).decode().strip()

# Get latest commit short from Git
revcmd = "git log --pretty=format:'%h' -n 1"
commit = subprocess.check_output(revcmd, shell=True).decode().strip()

# Get branch name from Git
branchcmd = "git rev-parse --abbrev-ref HEAD"
branch = subprocess.check_output(branchcmd, shell=True).decode().strip()

# Make all available for use in the macros
print("-DPIO_SRC_NAM={0}".format(project))
# print("-DPIO_SRC_TAG={0}".format(version))
print("-DPIO_SRC_REV={0}".format(commit))
print("-DPIO_SRC_BRH={0}".format(branch))
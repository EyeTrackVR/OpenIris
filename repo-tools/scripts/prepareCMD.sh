#!/bin/bash

# create a vairable to hold a passed in argument
# this argument is the next release version
# this is passed in from the .releaserc file

sudo apt-get install -y jq

nextReleaseVersion=$1

# parse all letters a-z and A-Z and replace with nothing
# this will remove all letters from the version string
# this is to ensure that the version string is a valid semver

# check if there is a letter in the version string
# if there is a letter, then remove it
# if there is no letter, then do nothing
if [[ $nextReleaseVersion =~ [a-zA-Z] ]]; then
    nextReleaseVersion=$(echo $nextReleaseVersion | sed 's/[a-zA-Z]//g')
    
    # check if there is a dash in the version string
    # if there is a dash, then replace it with a dot
    # if there is no dash, then do nothing
    if [[ $nextReleaseVersion =~ "-" ]]; then
        # parse all dashes and replace with dots
        # this is to ensure that the version string is a valid semver
        nextReleaseVersion=$(echo $nextReleaseVersion | sed 's/-/./g')
        
        # remove everything after the third dot and the dot itself
        # this is to ensure that the version string is a valid semver
        nextReleaseVersion=$(echo $nextReleaseVersion | sed 's/\.[0-9]*$//g')
        # remove the last dot
        nextReleaseVersion=$(echo $nextReleaseVersion | sed 's/\.$//g')
    fi
fi

# print the next release version

printf "[prepareCMD.sh]: Next version: ${nextReleaseVersion}\n"

# This script is used to execute the prepareCMD.sh script on the remote host
printf "[prepareCMD.sh]: Executing prepareCMD.sh on remote host \n"

printf "[prepareCMD.sh]: Updating the version in the library.json file \n"

# make a temp file
tmp=$(mktemp)

jq --arg a "$nextReleaseVersion" '.version = $a' ./ESP/lib/library.json > "$tmp" && mv "$tmp" ./ESP/lib/library.json -f

printf "[prepareCMD.sh]: Done \n"

printf "[prepareCMD.sh]: Installing the dependencies for the ini file \n"

pip3 install yq

export PATH="~/.local/bin:$PATH"
source ~/.bashrc

tmp=$(mktemp)
tomlq -t --arg version "$nextReleaseVersion" '.env.custom_firmware_version |= $version' ./ESP/ini/dev_config.ini > "$tmp" && mv "$tmp" ./ESP/ini/dev_config.ini -f

printf "[prepareCMD.sh]: Done, continuing with release. \n"

# mass rename files in the ./build sub folders
printf "[prepareCMD.sh]: Mass renaming files in the ./build sub folders \n"

#create an array of all the sub folders in the build folder
buildPaths=($(ls ./build))


# loop through all the sub folders in the build folder
for buildPath in "${buildPaths[@]}"
do
    printf "[prepareCMD.sh]: Build Path: ${buildPath} \n"
    # create a variable to hold the path to the sub folder
    buildPath="./build/${buildPath}"
    
    # create a variable to hold the path to the sub folder's files
    buildPathFiles=($(ls ${buildPath}))
    #create a variable that holds the current directory
    currentDir=$(pwd)
    
    #parse out the parent folder name and store it in a variable
    buildPathFileSubFolder=$(basename $(dirname ${buildPathFiles}))
    
    # append the sub folder name to the next release version
    nextReleaseVersion="${buildPathFileSubFolder}-v${nextReleaseVersion}-master"
    
    mv ${buildPathFile} ${buildPath}/${nextReleaseVersion}.zip
done

printf "[prepareCMD.sh]: Done \n"


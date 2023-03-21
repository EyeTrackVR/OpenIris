#!/bin/bash

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

printf "[prepareCMD.sh]: Next version: ${nextReleaseVersion}\n"
printf "[prepareCMD.sh]: Executing prepareCMD.sh on remote host \n"
printf "[prepareCMD.sh]: Updating the version in the library.json file \n"

# make a temp file
tmp=$(mktemp)

jq --arg a "$nextReleaseVersion" '.version = $a' ./ESP/lib/library.json > "$tmp" && mv "$tmp" ./ESP/lib/library.json -f

printf "[prepareCMD.sh]: Done, moving on to next files \n"

sed -i -e "/^\[env\]/,/^\[.*\]/ s|^\(custom_firmware_version[ \t]*=[ \t]*\).*$|\1$nextReleaseVersion|" "./ESP/ini/dev_config.ini"

printf "[prepareCMD.sh]: Updating the version in the dev_config.ini file \n"
printf "[prepareCMD.sh]: Mass renaming files in the ./build sub folders \n"

buildPaths=($(ls ./build))
# loop through all the sub folders in the build folder
for buildPath in "${buildPaths[@]}"
do
    printf "[prepareCMD.sh]: Build Path: ${buildPath} \n"
    fileToRename=$(ls ./build/${buildPath})
    newFileName=$(echo $fileToRename | sed "s/v[0-9]*\.[0-9]*\.[0-9]*/v${nextReleaseVersion}/g")
    printf "[prepareCMD.sh]: Renaming file: ${fileToRename} to ${newFileName} \n"
    mv ./build/${buildPath}/${fileToRename} ./build/${buildPath}/${newFileName}
done

printf "[prepareCMD.sh]: Done, continuing with release. \n"


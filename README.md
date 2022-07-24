# OpenIris

OpenIris is the firmware part of the [EyeTrackVR Project](https://github.com/RedHawk989/EyeTrackVR).

The aim of this project is to provide a fast and performant firmware for streaming the eye data back to the PC for further processing and actual tracking.

# **NOTE**

This is a work in progress project, things in the main branch are tested and should work just fine, but as it is in its very early stages it might not "just work".

Nonetheless, pull requests and issues are welcome, let's get this cracking!

# Features

### Working right now

- [x] Basic stream in 60FPS at 248x248px in MJPEG in greyscale
- [x] A basic HTTP server with API
- [x] Basic control of the camera though API
- [x] Health checks
- [x] OTA updates
- [x] ROI selection for eye area
- [x] MDNS - so that the server itself will detect and communicate with the tracker without you doing anything. No need to configure IPs and stuff, it's automagic

### TODO

- [ ] Implementation of Preferences Lib for saving camera and ROI settings
- [ ] LED status patterns - so that you know what's going on without plugging the tracker in to the PC
- [ ] FEC encoding with packet injection for even faster streams!
- [ ] Better OTA so that updates can be downloaded from github and pushed by the server to the tracker
- [ ] CI/CD with github actions - so we can more seamlessly update the trackers

# Accompanying projects

[EyeTrackVR](https://github.com/RedHawk989/EyeTrackVR) - The core project, made by Prohurtz

[IndexEyeTrackVR](https://github.com/rrazgriz/IndexEyeTrackVR) - An OpenCV version of the EyeTrackVR with a mount for Valve Index made by one of the community members - Razgriz!

[VrEyeToolbox](https://github.com/SummerSigh/VrEyeToolbox) - A compilation of different AI/ML methods and models for eye tracking in VR, made by Summer from CFVR!

# Additional equipment and software:

Below is a list of parts, mounts, etc

## Electronics

Currently, the following hardware is being used for testing. There is no guarantee these will be the final parts, and everything is heavily in flux. Links to AliExpress and Digikey listings for some items are included - these are for reference, and do not represent endorsements.

- [ESP32CAM Development board with ESP32-CAM-MB programmer](https://www.aliexpress.com/item/1005001900359624.html)
- [OV2640 Camera Module, 75mm Cable, 160 degree lens, 850nm "Night Vision" - no IR cut filter](https://www.aliexpress.com/item/1005003040149873.html)
- [IN-P32ZTIR IR Diode](https://www.digikey.com/en/products/detail/inolux/IN-P32ZTIR/10384796), run at 35 mA - 5V w/ 100ohm resistor

## Mounts

[IndexEyeTrackVR](https://github.com/rrazgriz/IndexEyeTrackVR) - A PoC version of an index mount made by Razgriz

Mounts for the Oculus Quest 2.

# Now, how do I get it running?

In the future, all you would have to do would be to push a flash/update button in the server and everything would be set up.

But we're not there yet so here's how to get it running manually:

1. Clone this repo
2. [Download VSCode](https://code.visualstudio.com/download)
3. [Install platformio IDE plugin for VSC](https://marketplace.visualstudio.com/items?itemName=platformio.platformio-ide)
4. [Install the drivers for CH341SER from here](https://cdn.sparkfun.com/assets/learn_tutorials/8/4/4/CH341SER.EXE)
5. Open the cloned project in VSC
6. Set your own credentials in ESP/platformio.ini in the [wifi] section. **NOTE: special characters are not supported by the ESP**
7. Connect the esp to the pc via a micro usb cable. Make sure that it can transfer data, otherwise this will **not** work
8. Click on the Ant's head icon on the left and from the menu select a build environment and then click on **build**, wait till it finishes and then click on **upload and monitor**

# Hold on, an environment, what do you mean?

Thanks to [DaOfficialWizard](https://github.com/ZanzyTHEbar) we now have a couple of options letting us decide how we want to build the firmaware.

- Debug - Has a lot of logging, useful for getting everything setup for the first time and to see what is going on. I recommed starting with this one.
- Release - Has a lot less debugging, may also be missing some things availible only in debug for debug purposes, this should be flashed when everything is working.
- OTA - Just like release, but it will upload your firmare to your esp wirelessly. NOTE: You need to change to the IP to which it should upload, it's the upload_port option under [env:OTA]

# Aight, I want to help, what should I do?

That's great to hear, I'm not a cpp dev, nor am I good with embedded so help is more than welcome.
That being said:

1. Fork and clone the project
2. Set up Visual Studio Code or Clion with the platformio plugin
3. Take a look at the project board - that's where I note everything I'm working on, I've finished and what I'm planning to do
4. Make a PR and / or ping me in the EyeTrackVR discord

# Questions, help, bugs, suggestions, blames?

Ping me in the EyeTrackVR discord, I'm - lorow#6600

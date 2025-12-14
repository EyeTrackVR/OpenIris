# OpenIris

OpenIris is the firmware part of the [EyeTrackVR Project](https://github.com/RedHawk989/EyeTrackVR).

The aim of this project is to provide a fast and performant firmware for streaming the eye data back to the PC for further processing and actual tracking.

# **NOTE**

This project is now archived and in upkeep mode. A new version, built from ground up, is being worked on here: https://github.com/lorow/openiris-espidf 

# Features

### Working right now

- [x] Basic stream in 60FPS at 248x248px in MJPEG in greyscale
- [x] A basic HTTP server with API
- [x] Basic control of the camera though API
- [x] Health checks
- [x] OTA updates
- [x] ROI selection for eye area
- [x] MDNS - so that the server itself will detect and communicate with the tracker without you doing anything. No need to configure IPs and stuff, it's automagic
- [x] Implementation of Preferences Lib for saving device settings (camera , MDNS, wifi configs etc )
- [x] CI/CD with github actions - so we can more seamlessly update the trackers
- [x] LED status patterns - so that you know what's going on without plugging the tracker in to the PC
- [x] Better OTA so that updates can be downloaded from github and pushed by the server to the tracker
- [x] Streaming over USB on boards that support it (ESP32S3 / XIAO ESP32S3 Sense thanks to XadE#2410 and Seaweed#4353

### TODO
- [ ] streaming over sockets instead of HTTP MJPEG for faster streams!
- [ ] better LED patterns 

# Docs: 
The "documentation" that was once present here was very old and outdated, we've moved from it being spread out in multiple repos to one place while also massively improving and expanding it, for the current info check this out:

https://docs.eyetrackvr.dev/

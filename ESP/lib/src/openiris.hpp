#ifndef OPENIRIS_HPP
#define OPENIRIS_HPP

#include <Arduino.h>

#include <data/config/project_config.hpp>
#include <io/LEDManager/LEDManager.hpp>
#include <io/camera/cameraHandler.hpp>
#include <logo/logo.hpp>

#ifndef ETVR_EYE_TRACKER_USB_API
#include <network/api/webserverHandler.hpp>
#include <network/mDNS/MDNSManager.hpp>
#include <network/stream/streamServer.hpp>
#include <network/wifihandler/wifihandler.hpp>
#else
#include <usb/etvr_eye_tracker_usb.hpp>
#endif  // ETVR_EYE_TRACKER_WEB_API

#endif  // OPENIRIS_HPP

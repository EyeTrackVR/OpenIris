#ifndef OPENIRIS_HPP
#define OPENIRIS_HPP

#include <Arduino.h>

#include <local/data/api/api.hpp>
#include <local/data/config/config.hpp>
#include <local/io/led_manager/led_manager.hpp>
#include <local/logo/logo.hpp>

#ifndef ETVR_EYE_TRACKER_USB_API
#include <local/network/api_server/api_server.hpp>
#include <local/network/stream/stream_server.hpp>
#else
#include <local/usb/etvr_eye_tracker_usb.hpp>
#endif  // ETVR_EYE_TRACKER_WEB_API

#endif  // OPENIRIS_HPP

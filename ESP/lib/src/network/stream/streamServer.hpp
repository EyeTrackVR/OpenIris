#pragma once
#ifndef STREAM_SERVER_HPP
#define STREAM_SERVER_HPP
#define PART_BOUNDARY "123456789000000000000987654321"
#include <Arduino.h>
#include <WiFi.h>
// Used to disable brownout detection
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

// Camera includes
#include "esp_camera.h"
#include "esp_http_server.h"
#include "esp_timer.h"
#include "fb_gfx.h"
#include "img_converters.h"

#include "io/camera/cameraHandler.hpp"
//#include "fd_forward.h"
namespace StreamHelpers
{
	esp_err_t stream_wrapper(httpd_req_t *req);
}

class StreamServer
{

private:
	CameraHandler *cameraHandler;
	httpd_handle_t camera_stream = nullptr;
	int STREAM_SERVER_PORT;

public:
	StreamServer(CameraHandler *cameraHandler, int STREAM_PORT);
	int startStreamServer();
	esp_err_t stream(httpd_req_t *req);
};

#endif // STREAM_SERVER_HPP

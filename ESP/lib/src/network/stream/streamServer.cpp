#include "streamServer.hpp"

constexpr static const char *STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
constexpr static const char *STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
constexpr static const char *STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\nX-Timestamp: %d.%06d\r\n\r\n";

StreamServer *streamServerLocal;

esp_err_t StreamHelpers::stream_wrapper(httpd_req_t *req)
{
	return streamServerLocal->stream(req);
}

StreamServer::StreamServer(CameraHandler *cameraHandler,
						   int STREAM_PORT) : cameraHandler(cameraHandler),
											  STREAM_SERVER_PORT(STREAM_PORT) {}

esp_err_t StreamServer::stream(httpd_req_t *req)
{
	long last_request_time = 0;
	camera_fb_t *fb = NULL;
	struct timeval _timestamp;

	esp_err_t res = ESP_OK;

	size_t _jpg_buf_len = 0;
	uint8_t *_jpg_buf = NULL;

	char *part_buf[256];

	static int64_t last_frame = 0;
	if (!last_frame)
		last_frame = esp_timer_get_time();

	res = httpd_resp_set_type(req, STREAM_CONTENT_TYPE);
	if (res != ESP_OK)
		return res;

	httpd_resp_set_hdr(req, "Access-Control-Allow-Origin; Content-Type: multipart/x-mixed-replace; boundary=123456789000000000000987654321\r\n", "*");
	httpd_resp_set_hdr(req, "X-Framerate", "60");

	while (true)
	{
		fb = esp_camera_fb_get();

		if (cameraHandler->light == 0)
		{
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x47, 0xff, 0x40); // Frame Length Adjustment MSBs
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2a, 0xf0, 0xf0); // line adjust MSB
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2b, 0xff, 0xff); // line adjust
		}
		else if (cameraHandler->light == 1)
		{
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x47, 0xff, 0x40); // Frame Length Adjustment MSBs
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2a, 0xf0, 0xd0); // line adjust MSB
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2b, 0xff, 0xff); // line adjust
		}
		else if (cameraHandler->light == 2)
		{
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x47, 0xff, 0x40); // Frame Length Adjustment MSBs
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2a, 0xf0, 0xb0); // line adjust MSB
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2b, 0xff, 0xff); // line adjust
		}
		else if (cameraHandler->light == 3)
		{
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x47, 0xff, 0x40); // Frame Length Adjustment MSBs
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2a, 0xf0, 0x70); // line adjust MSB
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2b, 0xff, 0xff); // line adjust
		}
		else if (cameraHandler->light == 4)
		{
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x47, 0xff, 0x40); // Frame Length Adjustment MSBs
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2a, 0xf0, 0x40); // line adjust MSB
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2b, 0xff, 0xff); // line adjust
		}
		else if (cameraHandler->light == 5)
		{
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x47, 0xff, 0x20); // Frame Length Adjustment MSBs
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2a, 0xf0, 0x80); // line adjust MSB
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2b, 0xff, 0xff); // line adjust
		}
		else if (cameraHandler->light == 6)
		{
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x47, 0xff, 0x20); // Frame Length Adjustment MSBs
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2a, 0xf0, 0x40); // line adjust MSB
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2b, 0xff, 0xff); // line adjust
		}
		else if (cameraHandler->light == 7)
		{
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x47, 0xff, 0x20); // Frame Length Adjustment MSBs
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2a, 0xf0, 0x30); // line adjust MSB
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2b, 0xff, 0xff); // line adjust
		}
		else if (cameraHandler->light == 8)
		{
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x47, 0xff, 0x20); // Frame Length Adjustment MSBs
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2a, 0xf0, 0x20); // line adjust MSB
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2b, 0xff, 0xff); // line adjust
		}
		else if (cameraHandler->light == 9)
		{
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x47, 0xff, 0x20); // Frame Length Adjustment MSBs
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2a, 0xf0, 0x10); // line adjust MSB
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2b, 0xff, 0xff); // line adjust
		}
		else if (cameraHandler->light == 10)
		{
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x47, 0xff, 0x10); // Frame Length Adjustment MSBs
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2a, 0xf0, 0x70); // line adjust MSB
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2b, 0xff, 0xff); // line adjust
		}
		else if (cameraHandler->light <= 12)
		{
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x47, 0xff, 0x10); // Frame Length Adjustment MSBs
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2a, 0xf0, 0x60); // line adjust MSB
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2b, 0xff, 0xff); // line adjust
		}
		else if (cameraHandler->light <= 14)
		{
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x47, 0xff, 0x10); // Frame Length Adjustment MSBs
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2a, 0xf0, 0x40); // line adjust MSB
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2b, 0xff, 0xff); // line adjust
		}
		else if (cameraHandler->light <= 18)
		{
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x47, 0xff, 0x08); // Frame Length Adjustment MSBs
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2a, 0xf0, 0xb0); // line adjust MSB
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2b, 0xff, 0xff); // line adjust
		}
		else if (cameraHandler->light <= 20)
		{
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x47, 0xff, 0x08); // Frame Length Adjustment MSBs
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2a, 0xf0, 0x80); // line adjust MSB
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2b, 0xff, 0xff); // line adjust
		}
		else if (cameraHandler->light <= 23)
		{
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x47, 0xff, 0x08); // Frame Length Adjustment MSBs
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2a, 0xf0, 0x60); // line adjust MSB
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2b, 0xff, 0xff); // line adjust
		}
		else if (cameraHandler->light <= 27)
		{
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x47, 0xff, 0x04); // Frame Length Adjustment MSBs
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2a, 0xf0, 0xd0); // line adjust MSB
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2b, 0xff, 0xff); // line adjust
		}
		else if (cameraHandler->light <= 31)
		{
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x47, 0xff, 0x04); // Frame Length Adjustment MSBs
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2a, 0xf0, 0x80); // line adjust MSB
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2b, 0xff, 0xff); // line adjust
		}
		else if (cameraHandler->light <= 35)
		{
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x47, 0xff, 0x04); // Frame Length Adjustment MSBs
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2a, 0xf0, 0x60); // line adjust MSB
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2b, 0xff, 0xff); // line adjust
		}
		else if (cameraHandler->light <= 40)
		{
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x47, 0xff, 0x02); // Frame Length Adjustment MSBs
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2a, 0xf0, 0x70); // line adjust MSB
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2b, 0xff, 0xff); // line adjust
		}
		else if (cameraHandler->light < 45)
		{
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x47, 0xff, 0x02); // Frame Length Adjustment MSBs
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2a, 0xf0, 0x40); // line adjust MSB
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2b, 0xff, 0xff); // line adjust
		}
		// after this the frame rate is higher, so we need to compensate
		else if (cameraHandler->light < 50)
		{
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x47, 0xff, 0x04); // Frame Length Adjustment MSBs
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2a, 0xf0, 0xa0); // line adjust MSB
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2b, 0xff, 0xff); // line adjust
		}
		else if (cameraHandler->light < 55)
		{
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x47, 0xff, 0x04); // Frame Length Adjustment MSBs
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2a, 0xf0, 0x70); // line adjust MSB
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2b, 0xff, 0xff); // line adjust
		}
		else if (cameraHandler->light < 65)
		{
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x47, 0xff, 0x04); // Frame Length Adjustment MSBs
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2a, 0xf0, 0x30); // line adjust MSB
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2b, 0xff, 0xff); // line adjust
		}
		else if (cameraHandler->light < 75)
		{
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x47, 0xff, 0x02); // Frame Length Adjustment MSBs
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2a, 0xf0, 0x80); // line adjust MSB
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2b, 0xff, 0xff); // line adjust
		}
		else if (cameraHandler->light < 90)
		{
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x47, 0xff, 0x02); // Frame Length Adjustment MSBs
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2a, 0xf0, 0x50); // line adjust MSB
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2b, 0xff, 0xbf); // line adjust
		}
		else if (cameraHandler->light < 100)
		{
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x47, 0xff, 0x02); // Frame Length Adjustment MSBs
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2a, 0xf0, 0x20); // line adjust MSB
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2b, 0xff, 0x8f); // line adjust
		}
		else if (cameraHandler->light < 110)
		{
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x47, 0xff, 0x02); // Frame Length Adjustment MSBs
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2a, 0xf0, 0x10); // line adjust MSB
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2b, 0xff, 0x7f); // line adjust
		}
		else if (cameraHandler->light < 120)
		{
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x47, 0xff, 0x01); // Frame Length Adjustment MSBs
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2a, 0xf0, 0x10); // line adjust MSB
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2b, 0xff, 0x5f); // line adjust
		}
		else if (cameraHandler->light < 130)
		{
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x47, 0xff, 0x00); // Frame Length Adjustment MSBs
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2a, 0xf0, 0x0);  // line adjust MSB
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2b, 0xff, 0x2f); // line adjust
		}
		else if (cameraHandler->light < 140)
		{
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x47, 0xff, 0x00); // Frame Length Adjustment MSBs
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2a, 0xf0, 0x0);  // line adjust MSB
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x2b, 0xff, 0x0);  // line adjust
		}

		if (cameraHandler->light < cameraHandler->day_switch_value)
			cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x43, 0xff, 0x40); // magic value to give us the frame faster (bit 6 must be 1)

		// fb = esp_camera_fb_get();

		cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0xff, 0xff, 0x00); // banksel
		cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0xd3, 0xff, 0x8);  // clock

		cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x42, 0xff, 0x2f); // image quality (lower is bad)
		cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x44, 0xff, 3);	   // quality

		// no sharpening
		cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x92, 0xff, 0x1);
		cameraHandler->camera_sensor->set_reg(cameraHandler->camera_sensor, 0x93, 0xff, 0x0);

		if (!fb)
		{
			log_e("Camera capture failed");
			res = ESP_FAIL;
		}
		else
		{
			_timestamp.tv_sec = fb->timestamp.tv_sec;
			_timestamp.tv_usec = fb->timestamp.tv_usec;
			_jpg_buf_len = fb->len;
			_jpg_buf = fb->buf;
		}

		if (res == ESP_OK)
			res = httpd_resp_send_chunk(req, STREAM_BOUNDARY, strlen(STREAM_BOUNDARY));

		if (res == ESP_OK)
		{
			size_t hlen = snprintf((char *)part_buf, 128, STREAM_PART, _jpg_buf_len, _timestamp.tv_sec, _timestamp.tv_usec);
			res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
		}
		if (res == ESP_OK)
			res = httpd_resp_send_chunk(req, (const char *)_jpg_buf, _jpg_buf_len);

		if (fb)
		{
			esp_camera_fb_return(fb);
			fb = NULL;
			_jpg_buf = NULL;
		}
		else if (_jpg_buf)
		{
			free(_jpg_buf);
			_jpg_buf = NULL;
		}

		if (res != ESP_OK)
			break;

		long request_end = millis();
		long latency = (request_end - last_request_time);
		last_request_time = request_end;
		log_d("Size: %uKB, Time: %ums (%ifps)\n", _jpg_buf_len / 1024, latency, 1000 / latency);
	}

	last_frame = 0;

	return res;
}

int StreamServer::startStreamServer()
{
	// WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //! Turn-off the 'brownout detector'
	httpd_config_t config = HTTPD_DEFAULT_CONFIG();
	config.stack_size = 20480;
	config.max_uri_handlers = 1;
	config.server_port = this->STREAM_SERVER_PORT;
	config.ctrl_port = this->STREAM_SERVER_PORT;
	config.stack_size = 20480;

	httpd_uri_t stream_page = {
		.uri = "/",
		.method = HTTP_GET,
		.handler = &StreamHelpers::stream_wrapper,
		.user_ctx = nullptr};

	int status = httpd_start(&camera_stream, &config);

	if (status != ESP_OK)
		return -1;
	else
	{
		httpd_register_uri_handler(camera_stream, &stream_page);
		Serial.println("Stream server initialized");
		Serial.print("\n\rThe stream is under: http://");
		Serial.print(WiFi.localIP());
		Serial.printf(":%i\n\r", this->STREAM_SERVER_PORT);
		return 0;
	}
}

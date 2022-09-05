#include "cameraHandler.hpp"

bool CameraHandler::setupCamera()
{
	log_d("Setting up camera \r\n");

	config.ledc_channel = LEDC_CHANNEL_0;
	config.ledc_timer = LEDC_TIMER_0;
	config.grab_mode = CAMERA_GRAB_LATEST;
	config.pin_d0 = Y2_GPIO_NUM;
	config.pin_d1 = Y3_GPIO_NUM;
	config.pin_d2 = Y4_GPIO_NUM;
	config.pin_d3 = Y5_GPIO_NUM;
	config.pin_d4 = Y6_GPIO_NUM;
	config.pin_d5 = Y7_GPIO_NUM;
	config.pin_d6 = Y8_GPIO_NUM;
	config.pin_d7 = Y9_GPIO_NUM;
	config.pin_xclk = XCLK_GPIO_NUM;
	config.pin_pclk = PCLK_GPIO_NUM;
	config.pin_vsync = VSYNC_GPIO_NUM;
	config.pin_href = HREF_GPIO_NUM;
	config.pin_sscb_sda = SIOD_GPIO_NUM;
	config.pin_sscb_scl = SIOC_GPIO_NUM;
	config.pin_pwdn = PWDN_GPIO_NUM;
	config.pin_reset = RESET_GPIO_NUM;
	config.xclk_freq_hz = 16500000; // 10000000 stable,
									// 16500000 optimal,
									// 20000000 max fps
	config.pixel_format = PIXFORMAT_JPEG;

	if (psramFound())
	{
		log_d("Found psram, setting the 240x240 image quality");
		config.frame_size = FRAMESIZE_240X240;
		config.jpeg_quality = 7; // 0-63 lower number = higher quality, more latency and less fps   7 for most fps, 5 for best quality
		config.fb_count = 3;
	}
	else
	{
		log_e("Did not find psram, setting svga quality");
		config.frame_size = FRAMESIZE_SVGA;
		config.jpeg_quality = 1;
		config.fb_count = 1;
	}

	esp_err_t err = esp_camera_init(&config);

	if (err != ESP_OK)
	{
		log_e("Camera initialization failed with error: 0x%x \r\n", err);
		log_e("Camera most likely not seated properly in the socket. Please fix the camera and reboot the device.\r\n");
		//! TODO add led blinking here
		return false;
	}

	log_d("Successfully initialized the camera!");
	//! TODO add led blinking here

	camera_sensor = esp_camera_sensor_get();
	// fixes corrupted jpegs, https://github.com/espressif/esp32-camera/issues/203
	camera_sensor->set_reg(camera_sensor, 0xff, 0xff, 0x00);		 // banksel
	camera_sensor->set_reg(camera_sensor, 0xd3, 0xff, 5);			 // clock
	camera_sensor->set_brightness(camera_sensor, 2);				 // -2 to 2   I see no difference between numbers..
	camera_sensor->set_contrast(camera_sensor, 2);					 // -2 to 2
	camera_sensor->set_saturation(camera_sensor, -2);				 // -2 to 2
	camera_sensor->set_whitebal(camera_sensor, 1);					 // 0 = disable , 1 = enable
	camera_sensor->set_awb_gain(camera_sensor, 1);					 // 0 = disable , 1 = enable
	camera_sensor->set_wb_mode(camera_sensor, 0);					 // 0 to 4 - if awb_gain enabled (0 - Auto, 1 - Sunny, 2 - Cloudy, 3 - Office, 4 - Home)
	camera_sensor->set_exposure_ctrl(camera_sensor, 1);				 // 0 = disable , 1 = enable
	camera_sensor->set_aec2(camera_sensor, 0);						 // 0 = disable , 1 = enable
	camera_sensor->set_gain_ctrl(camera_sensor, 0);					 // 0 = disable , 1 = enable
	camera_sensor->set_agc_gain(camera_sensor, 2);					 // 0 to 30  brightness of sorts? higher = brighter with more lag
	camera_sensor->set_gainceiling(camera_sensor, (gainceiling_t)6); // 0 to 6
	camera_sensor->set_bpc(camera_sensor, 1);						 // 0 = disable , 1 = enable
	camera_sensor->set_wpc(camera_sensor, 1);						 // 0 = disable , 1 = enable
	camera_sensor->set_raw_gma(camera_sensor, 1);					 // 0 = disable , 1 = enable (makes much lighter and noisy)
	camera_sensor->set_lenc(camera_sensor, 0);						 // 0 = disable , 1 = enable                // 0 = disable , 1 = enable
	camera_sensor->set_dcw(camera_sensor, 0);						 // 0 = disable , 1 = enable
	camera_sensor->set_colorbar(camera_sensor, 0);					 // 0 = disable , 1 = enable
	camera_sensor->set_special_effect(camera_sensor, 2);			 // 0 to 6 (0 - No Effect, 1 - Negative, 2 - Grayscale, 3 - Red Tint, 4 - Green Tint, 5 - Blue Tint, 6 - Sepia)

	return true;
}

void CameraHandler::update(ObserverEvent::Event event)
{
	if (event == ObserverEvent::cameraConfigUpdated)
	{
		ProjectConfig::CameraConfig_t *cameraConfig = configManager->getCameraConfig();
		this->setHFlip(cameraConfig->href);
		this->setVFlip(cameraConfig->vflip);
		this->setCameraResolution((framesize_t)cameraConfig->framesize);
		camera_sensor->set_quality(camera_sensor, cameraConfig->quality);
	}
}

int CameraHandler::setCameraResolution(framesize_t frameSize)
{
	if (camera_sensor->pixformat == PIXFORMAT_JPEG)
	{
		try
		{
			return camera_sensor->set_framesize(camera_sensor, frameSize);
		}
		catch (...)
		{
			// they sent us a malformed or unsupported frameSize - rather than crash - tell them about it
			return -1;
		}
	}
	return -1;
}

int CameraHandler::setVFlip(int direction)
{
	return camera_sensor->set_vflip(camera_sensor, direction);
}

int CameraHandler::setHFlip(int direction)
{
	return camera_sensor->set_hmirror(camera_sensor, direction);
}

//! either hardware(1) or software(0)
// TODO: Add to API
void CameraHandler::resetCamera(bool type)
{
	if (type)
	{
		// power cycle the camera module (handy if camera stops responding)
		digitalWrite(PWDN_GPIO_NUM, HIGH); // turn power off to camera module
		Network_Utilities::my_delay(0.3); // a for loop with a delay of 300ms
		digitalWrite(PWDN_GPIO_NUM, LOW);
		Network_Utilities::my_delay(0.3);
		setupCamera();
	}
	else
	{
		// reset via software (handy if you wish to change resolution or image type etc. - see test procedure)
		esp_camera_deinit();
		Network_Utilities::my_delay(0.05);
		setupCamera();
	}
}
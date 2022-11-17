#include "cameraHandler.hpp"

CameraHandler::CameraHandler(ProjectConfig *configManager,
							 StateManager<LEDStates_e> *stateManager) : configManager(configManager),
																		stateManager(stateManager),
																		light(0),
																		day_switch_value(140) {}

void CameraHandler::setupCameraPinout()
{
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
}

void CameraHandler::setupBasicResolution()
{
	config.pixel_format = PIXFORMAT_JPEG;
	config.frame_size = FRAMESIZE_240X240;
	if (!psramFound())
	{
		log_e("Did not find psram, setting lower image quality");
		config.jpeg_quality = 12;
		config.fb_count = 1;
		return;
	}

	log_d("Found psram, setting the higher image quality");
	config.jpeg_quality = 7; // 0-63 lower number = higher quality, more latency and less fps   7 for most fpcamera_sensor, 5 for best quality
	config.fb_count = 3;
}

void CameraHandler::setupCameraSensor()
{
	camera_sensor = esp_camera_sensor_get();
	// fixes corrupted jpegcamera_sensor, https://github.com/espressif/esp32-camera/issues/203
	camera_sensor->set_reg(camera_sensor, 0xff, 0xff, 0x01); // banksel
	camera_sensor->set_reg(camera_sensor, 0xd3, 0xff, 5);	 // clock
	camera_sensor->set_brightness(camera_sensor, 2);		 // -2 to 2   I see no difference between numbers..
	camera_sensor->set_contrast(camera_sensor, 2);			 // -2 to 2
	camera_sensor->set_saturation(camera_sensor, -2);		 // -2 to 2
	camera_sensor->set_whitebal(camera_sensor, 1);			 // 0 = disable , 1 = enable
	camera_sensor->set_awb_gain(camera_sensor, 1);			 // 0 = disable , 1 = enable
	camera_sensor->set_wb_mode(camera_sensor, 0);			 // 0 to 4 - if awb_gain enabled (0 - Auto, 1 - Sunny, 2 - Cloudy, 3 - Office, 4 - Home)
	// camera_sensor->set_exposure_ctrl(camera_sensor, 1);		     // 0 = disable , 1 = enable
	// camera_sensor->set_aec2(camera_sensor, 0);
	// camera_sensor->set_ae_level(camera_sensor, 2);       								 // -2 to 2
	// camera_sensor->set_aec_value(camera_sensor, 1200);   								 // 0 to 1200						 // 0 = disable , 1 = enable
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

	//! Below are experimental values for setting proper exposure and quality based on lightmeter register values

	light = camera_sensor->get_reg(camera_sensor, 0x2f, 0xff);
	log_d("First light is: %d", light);
	log_d("Old 0x0 value is: %d", camera_sensor->get_reg(camera_sensor, 0x0, 0xff));

	// light=120+cur_pic*10;
	// light=0+cur_pic*5;

	// TODO: if this works, make it a function and turn it into a map for better performance
	if (light < day_switch_value)
	{
		// here we are in night mode
		if (light < 45)
			camera_sensor->set_reg(camera_sensor, 0x11, 0xff, 1); // frame rate (1 means longer exposure)
		camera_sensor->set_reg(camera_sensor, 0x13, 0xff, 0);	  // manual everything
		camera_sensor->set_reg(camera_sensor, 0x0c, 0x6, 0x8);	  // manual banding
		camera_sensor->set_reg(camera_sensor, 0x45, 0x3f, 0x3f);  // really long exposure (but it doesn't really work)
	}
	else
	{
		// here we are in daylight mode
		camera_sensor->set_reg(camera_sensor, 0x2d, 0xff, 0x0); // extra lines
		camera_sensor->set_reg(camera_sensor, 0x2e, 0xff, 0x0); // extra lines

		camera_sensor->set_reg(camera_sensor, 0x47, 0xff, 0x0); // Frame Length Adjustment MSBs

		if (light < 150)
		{
			camera_sensor->set_reg(camera_sensor, 0x46, 0xff, 0xd0); // Frame Length Adjustment LSBs
			camera_sensor->set_reg(camera_sensor, 0x2a, 0xff, 0xff); // line adjust MSB
			camera_sensor->set_reg(camera_sensor, 0x2b, 0xff, 0xff); // line adjust
			camera_sensor->set_reg(camera_sensor, 0x45, 0xff, 0xff); // exposure (doesn't seem to work)
		}
		else if (light < 160)
		{
			camera_sensor->set_reg(camera_sensor, 0x46, 0xff, 0xc0); // Frame Length Adjustment LSBs
			camera_sensor->set_reg(camera_sensor, 0x2a, 0xff, 0xb0); // line adjust MSB
			camera_sensor->set_reg(camera_sensor, 0x2b, 0xff, 0xff); // line adjust
			camera_sensor->set_reg(camera_sensor, 0x45, 0xff, 0x10); // exposure (doesn't seem to work)
		}
		else if (light < 170)
		{
			camera_sensor->set_reg(camera_sensor, 0x46, 0xff, 0xb0); // Frame Length Adjustment LSBs
			camera_sensor->set_reg(camera_sensor, 0x2a, 0xff, 0x80); // line adjust MSB
			camera_sensor->set_reg(camera_sensor, 0x2b, 0xff, 0xff); // line adjust
			camera_sensor->set_reg(camera_sensor, 0x45, 0xff, 0x10); // exposure (doesn't seem to work)
		}
		else if (light < 180)
		{
			camera_sensor->set_reg(camera_sensor, 0x46, 0xff, 0xa8); // Frame Length Adjustment LSBs
			camera_sensor->set_reg(camera_sensor, 0x2a, 0xff, 0x80); // line adjust MSB
			camera_sensor->set_reg(camera_sensor, 0x2b, 0xff, 0xff); // line adjust
			camera_sensor->set_reg(camera_sensor, 0x45, 0xff, 0x10); // exposure (doesn't seem to work)
		}
		else if (light < 190)
		{
			camera_sensor->set_reg(camera_sensor, 0x46, 0xff, 0xa6); // Frame Length Adjustment LSBs
			camera_sensor->set_reg(camera_sensor, 0x2a, 0xff, 0x80); // line adjust MSB
			camera_sensor->set_reg(camera_sensor, 0x2b, 0xff, 0xff); // line adjust
			camera_sensor->set_reg(camera_sensor, 0x45, 0xff, 0x90); // exposure (doesn't seem to work)
		}
		else if (light < 200)
		{
			camera_sensor->set_reg(camera_sensor, 0x46, 0xff, 0xa4); // Frame Length Adjustment LSBs
			camera_sensor->set_reg(camera_sensor, 0x2a, 0xff, 0x80); // line adjust MSB
			camera_sensor->set_reg(camera_sensor, 0x2b, 0xff, 0xff); // line adjust
			camera_sensor->set_reg(camera_sensor, 0x45, 0xff, 0x10); // exposure (doesn't seem to work)
		}
		else if (light < 210)
		{
			camera_sensor->set_reg(camera_sensor, 0x46, 0xff, 0x98); // Frame Length Adjustment LSBs
			camera_sensor->set_reg(camera_sensor, 0x2a, 0xff, 0x60); // line adjust MSB
			camera_sensor->set_reg(camera_sensor, 0x2b, 0xff, 0xff); // line adjust
			camera_sensor->set_reg(camera_sensor, 0x45, 0xff, 0x10); // exposure (doesn't seem to work)
		}
		else if (light < 220)
		{
			camera_sensor->set_reg(camera_sensor, 0x46, 0xff, 0x80); // Frame Length Adjustment LSBs
			camera_sensor->set_reg(camera_sensor, 0x2a, 0xff, 0x20); // line adjust MSB
			camera_sensor->set_reg(camera_sensor, 0x2b, 0xff, 0xff); // line adjust
			camera_sensor->set_reg(camera_sensor, 0x45, 0xff, 0x10); // exposure (doesn't seem to work)
		}
		else if (light < 230)
		{
			camera_sensor->set_reg(camera_sensor, 0x46, 0xff, 0x70); // Frame Length Adjustment LSBs
			camera_sensor->set_reg(camera_sensor, 0x2a, 0xff, 0x20); // line adjust MSB
			camera_sensor->set_reg(camera_sensor, 0x2b, 0xff, 0xff); // line adjust
			camera_sensor->set_reg(camera_sensor, 0x45, 0xff, 0x10); // exposure (doesn't seem to work)
		}
		else if (light < 240)
		{
			camera_sensor->set_reg(camera_sensor, 0x46, 0xff, 0x60); // Frame Length Adjustment LSBs
			camera_sensor->set_reg(camera_sensor, 0x2a, 0xff, 0x20); // line adjust MSB
			camera_sensor->set_reg(camera_sensor, 0x2b, 0xff, 0x80); // line adjust
			camera_sensor->set_reg(camera_sensor, 0x45, 0xff, 0x10); // exposure (doesn't seem to work)
		}
		else if (light < 253)
		{
			camera_sensor->set_reg(camera_sensor, 0x46, 0xff, 0x10); // Frame Length Adjustment LSBs
			camera_sensor->set_reg(camera_sensor, 0x2a, 0xff, 0x0);	 // line adjust MSB
			camera_sensor->set_reg(camera_sensor, 0x2b, 0xff, 0x40); // line adjust
			camera_sensor->set_reg(camera_sensor, 0x45, 0xff, 0x10); // exposure (doesn't seem to work)
		}
		else
		{
			camera_sensor->set_reg(camera_sensor, 0x46, 0xff, 0x0); // Frame Length Adjustment LSBs
			camera_sensor->set_reg(camera_sensor, 0x2a, 0xff, 0x0); // line adjust MSB
			camera_sensor->set_reg(camera_sensor, 0x2b, 0xff, 0x0); // line adjust
			camera_sensor->set_reg(camera_sensor, 0x45, 0xff, 0x0); // exposure (doesn't seem to work)
			camera_sensor->set_reg(camera_sensor, 0x10, 0xff, 0x0); // exposure (doesn't seem to work)
		}

		camera_sensor->set_reg(camera_sensor, 0x0f, 0xff, 0x4b); // no idea
		camera_sensor->set_reg(camera_sensor, 0x03, 0xff, 0xcf); // no idea
		camera_sensor->set_reg(camera_sensor, 0x3d, 0xff, 0x34); // changes the exposure somehow, has to do with frame rate

		camera_sensor->set_reg(camera_sensor, 0x11, 0xff, 0x0);	 // frame rate
		camera_sensor->set_reg(camera_sensor, 0x43, 0xff, 0x11); // 11 is the default value
	}
}

bool CameraHandler::setupCamera()
{

	this->setupCameraPinout();
	this->setupBasicResolution();
#ifdef CAMERA_MODEL_ESP_EYE
	pinMode(13, INPUT_PULLUP);
	pinMode(14, INPUT_PULLUP);
#endif
	esp_err_t hasCameraBeenInitialized = esp_camera_init(&config);

	if (hasCameraBeenInitialized != ESP_OK)
	{
		log_e("Camera initialization failed with error: 0x%x \r\n", hasCameraBeenInitialized);
		log_e("Camera most likely not seated properly in the socket. Please fix the camera and reboot the device.\r\n");
		stateManager->setState(LEDStates_e::_Camera_Error);
		return false;
	}

	this->setupCameraSensor();
	stateManager->setState(LEDStates_e::_Camera_Success);
	return true;
}

void CameraHandler::loadConfigData()
{
	ProjectConfig::CameraConfig_t *cameraConfig = configManager->getCameraConfig();
	this->setHFlip(cameraConfig->href);
	this->setVFlip(cameraConfig->vflip);
	this->setCameraResolution((framesize_t)cameraConfig->framesize);
	camera_sensor->set_quality(camera_sensor, cameraConfig->quality);
	camera_sensor->set_agc_gain(camera_sensor, cameraConfig->brightness);
}

void CameraHandler::update(ObserverEvent::Event event)
{
	switch (event)
	{
	case ObserverEvent::Event::configLoaded:
		this->setupCamera();
		this->loadConfigData();
		break;
	case ObserverEvent::Event::cameraConfigUpdated:
		this->loadConfigData();
		break;
	default:
		break;
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
void CameraHandler::resetCamera(bool type)
{
	if (type)
	{
		// power cycle the camera module (handy if camera stops responding)
		digitalWrite(PWDN_GPIO_NUM, HIGH); // turn power off to camera module
		Network_Utilities::my_delay(0.3);  // a for loop with a delay of 300ms
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
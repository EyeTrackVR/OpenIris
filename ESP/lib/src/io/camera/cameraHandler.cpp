#include "cameraHandler.hpp"

CameraHandler::CameraHandler(ProjectConfig& configManager)
    : configManager(configManager) {}

void CameraHandler::setupCameraPinout() {
  // Workaround for espM5SStack not having a defined camera
#ifdef CAMERA_MODULE_NAME
  log_i("Camera module is %s", CAMERA_MODULE_NAME);
#else
  log_i("Camera module is undefined");
#endif

  // camera external clock signal frequencies
  // 10000000 stable
  // 16500000 optimal freq on ESP32-CAM (default)
  // 20000000 max freq on ESP32-CAM
  // 24000000 optimal freq on ESP32-S3
  int xclk_freq_hz = 16500000;

#if CONFIG_CAMERA_MODULE_ESP_EYE
  /* IO13, IO14 is designed for JTAG by default,
   * to use it as generalized input,
   * firstly declare it as pullup input
   **/
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
  log_i("ESP_EYE");
#elif CONFIG_CAMERA_MODULE_CAM_BOARD
  /* IO13, IO14 is designed for JTAG by default,
   * to use it as generalized input,
   * firstly declare it as pullup input
   **/
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
  log_i("CAM_BOARD");
#endif
#if ETVR_EYE_TRACKER_USB_API
  /* ESP32-S3 is capable of using higher freqs */
  xclk_freq_hz = 24000000;
#endif

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
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = xclk_freq_hz;
}

void CameraHandler::setupBasicResolution() {
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_240X240;

  if (!psramFound()) {
    log_e("Did not find psram, setting lower image quality");
    config.fb_location = CAMERA_FB_IN_DRAM;
    config.jpeg_quality = 9;
    config.fb_count = 2;
    return;
  }

  log_d("Found psram, setting the higher image quality");
  config.jpeg_quality = 7;  // 0-63 lower number = higher quality, more latency
                            // and less fps   7 for most fps, 5 for best quality
  config.fb_count = 3;
  log_d("Setting fb_location to CAMERA_FB_IN_PSRAM");
}

void CameraHandler::setupCameraSensor() {
  log_d("Setting up camera sensor");

  camera_sensor = esp_camera_sensor_get();
  // fixes corrupted jpegs, https://github.com/espressif/esp32-camera/issues/203
  // documentation https://www.uctronics.com/download/cam_module/OV2640DS.pdf
  camera_sensor->set_reg(
      camera_sensor, 0xff, 0xff,
      0x00);  // banksel, here we're directly writing to the registers.
              // 0xFF==0x00 is the first bank, there's also 0xFF==0x01
  camera_sensor->set_reg(camera_sensor, 0xd3, 0xff, 5);  // clock
  camera_sensor->set_brightness(camera_sensor, 2);       // -2 to 2
  camera_sensor->set_contrast(camera_sensor, 2);         // -2 to 2
  camera_sensor->set_saturation(camera_sensor, -2);      // -2 to 2

  // white balance control
  camera_sensor->set_whitebal(camera_sensor, 1);  // 0 = disable , 1 = enable
  camera_sensor->set_awb_gain(camera_sensor, 0);  // 0 = disable , 1 = enable
  camera_sensor->set_wb_mode(camera_sensor,
                             0);  // 0 to 4 - if awb_gain enabled (0 - Auto, 1 -
                                  // Sunny, 2 - Cloudy, 3 - Office, 4 - Home)

  // controls the exposure
  camera_sensor->set_exposure_ctrl(camera_sensor,
                                   0);               // 0 = disable , 1 = enable
  camera_sensor->set_aec2(camera_sensor, 0);         // 0 = disable , 1 = enable
  camera_sensor->set_ae_level(camera_sensor, 0);     // -2 to 2
  camera_sensor->set_aec_value(camera_sensor, 300);  // 0 to 1200

  // controls the gain
  camera_sensor->set_gain_ctrl(camera_sensor, 0);  // 0 = disable , 1 = enable

  // automatic gain control gain, controls by how much the resulting image
  // should be amplified
  camera_sensor->set_agc_gain(camera_sensor, 2);                    // 0 to 30
  camera_sensor->set_gainceiling(camera_sensor, (gainceiling_t)6);  // 0 to 6

  // black and white pixel correction, averages the white and black spots
  camera_sensor->set_bpc(camera_sensor, 1);  // 0 = disable , 1 = enable
  camera_sensor->set_wpc(camera_sensor, 1);  // 0 = disable , 1 = enable
  // digital clamp white balance
  camera_sensor->set_dcw(camera_sensor, 0);  // 0 = disable , 1 = enable

  // gamma correction
  camera_sensor->set_raw_gma(
      camera_sensor,
      1);  // 0 = disable , 1 = enable (makes much lighter and noisy)

  camera_sensor->set_lenc(camera_sensor, 0);  // 0 = disable , 1 = enable // 0 =
                                              // disable , 1 = enable

  camera_sensor->set_colorbar(camera_sensor, 0);  // 0 = disable , 1 = enable

  camera_sensor->set_special_effect(
      camera_sensor,
      2);  // 0 to 6 (0 - No Effect, 1 - Negative, 2 - Grayscale, 3 - Red Tint,
           // 4 - Green Tint, 5 - Blue Tint, 6 - Sepia)

  log_d("Setting up camera sensor done");
}

bool CameraHandler::setupCamera() {
  log_d("Setting up camera pinout");
  this->setupCameraPinout();
  log_d("Setting up camera with resolution");
  this->setupBasicResolution();
  log_d("Initializing camera...");

  esp_err_t hasCameraBeenInitialized = esp_camera_init(&config);
  log_d("Camera initialized: 0x%x \r\n", hasCameraBeenInitialized);
  if (hasCameraBeenInitialized != ESP_OK) {
    log_e("Camera initialization failed with error: 0x%x \r\n",
          hasCameraBeenInitialized);
    log_e(
        "Camera most likely not seated properly in the socket. Please fix the "
        "camera and reboot the device.\r\n");
    ledStateManager.setState(LEDStates_e::_Camera_Error);
    return false;
  }

  this->setupCameraSensor();
  return true;
}

void CameraHandler::loadConfigData() {
  ProjectConfig::CameraConfig_t cameraConfig = configManager.getCameraConfig();
  this->setHFlip(cameraConfig.href);
  this->setVFlip(cameraConfig.vflip);
  this->setCameraResolution((framesize_t)cameraConfig.framesize);
  camera_sensor->set_quality(camera_sensor, cameraConfig.quality);
  camera_sensor->set_agc_gain(camera_sensor, cameraConfig.brightness);
}

int CameraHandler::setCameraResolution(framesize_t frameSize) {
  if (camera_sensor->pixformat == PIXFORMAT_JPEG) {
    try {
      return camera_sensor->set_framesize(camera_sensor, frameSize);
    } catch (...) {
      // they sent us a malformed or unsupported frameSize - rather than crash -
      // tell them about it
      return -1;
    }
  }
  return -1;
}

int CameraHandler::setVFlip(int direction) {
  return camera_sensor->set_vflip(camera_sensor, direction);
}

int CameraHandler::setHFlip(int direction) {
  return camera_sensor->set_hmirror(camera_sensor, direction);
}

int CameraHandler::setVieWindow(int offsetX,
                                int offsetY,
                                int outputX,
                                int outputY) {
  return 0;
}

//! either hardware(1) or software(0)
void CameraHandler::resetCamera(bool type) {
  if (type) {
    // power cycle the camera module (handy if camera stops responding)
    digitalWrite(PWDN_GPIO_NUM, HIGH);  // turn power off to camera module
    Network_Utilities::my_delay(0.3);   // a for loop with a delay of 300ms
    digitalWrite(PWDN_GPIO_NUM, LOW);
    Network_Utilities::my_delay(0.3);
    setupCamera();
  } else {
    // reset via software (handy if you wish to change resolution or image type
    // etc. - see test procedure)
    esp_camera_deinit();
    Network_Utilities::my_delay(0.05);
    setupCamera();
  }
}

void CameraHandler::update(ConfigState_e event) {
  switch (event) {
    case ConfigState_e::configLoaded:
      this->setupCamera();
      this->loadConfigData();
      break;
    case ConfigState_e::cameraConfigUpdated:
      this->loadConfigData();
      break;
    default:
      break;
  }
}

std::string CameraHandler::getName() {
  return "CameraHandler";
}

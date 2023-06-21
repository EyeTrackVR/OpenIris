#include "api.hpp"

#ifndef SIM_ENABLED
CameraHandler &camera,
#endif  // SIM_ENABLED

//*********************************************************************************************
//!                                     Camera Command Functions
//*********************************************************************************************
#ifndef SIM_ENABLED
    void API::setCamera(AsyncWebServerRequest* request) {
  switch (_networkMethodsMap_enum[request->method()]) {
    case GET: {
      // create temporary variables to store the values
      uint8_t temp_camera_framesize = 0;
      uint8_t temp_camera_vflip = 0;
      uint8_t temp_camera_hflip = 0;
      uint8_t temp_camera_quality = 0;
      uint8_t temp_camera_brightness = 0;

      int params = request->params();
      //! Using the else if statements to ensure that the values do not need to
      //! be set in a specific order This means the order of the URL params does
      //! not matter
      for (int i = 0; i < params; i++) {
        AsyncWebParameter* param = request->getParam(i);
        if (param->name() == "framesize") {
          temp_camera_framesize = (uint8_t)param->value().toInt();
        } else if (param->name() == "vflip") {
          temp_camera_vflip = (uint8_t)param->value().toInt();
        } else if (param->name() == "hflip") {
          temp_camera_hflip = (uint8_t)param->value().toInt();
        } else if (param->name() == "quality") {
          temp_camera_quality = (uint8_t)param->value().toInt();
        } else if (param->name() == "brightness") {
          temp_camera_brightness = (uint8_t)param->value().toInt();
        }
      }
      // note: We're passing empty params by design, this is done to reset
      // specific fields
      projectConfig.setCameraConfig(temp_camera_vflip, temp_camera_framesize,
                                    temp_camera_hflip, temp_camera_quality,
                                    temp_camera_brightness, true);

      request->send(200, MIMETYPE_JSON,
                    "{\"msg\":\"Done. Camera Settings have been set.\"}");
      break;
    }
    default: {
      request->send(400, MIMETYPE_JSON, "{\"msg\":\"Invalid Request\"}");
      request->redirect("/");
      break;
    }
  }
}

void API::restartCamera(AsyncWebServerRequest* request) {
  bool mode = (bool)atoi(request->arg("mode").c_str());
  camera.resetCamera(mode);

  request->send(200, MIMETYPE_JSON,
                "{\"msg\":\"Done. Camera had been restarted.\"}");
}
#endif  // SIM_ENABLED

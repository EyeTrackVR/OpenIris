#include "GlobalVars.h"
#include "webServer/asyncJPGResponse.h"
#include "webServer/webserverHandler.h"

OpenIris::HTTPDHandler::HTTPDHandler()
{
  this->server = new AsyncWebServer(STREAM_SERVER_PORT);
}

void OpenIris::HTTPDHandler::startStreamServer()
{
  this->server->on(
      "/",
      HTTP_GET,
      std::bind(&OpenIris::HTTPDHandler::stream_handler, this, std::placeholders::_1));
  this->server->on(
      "/control",
      HTTP_GET,
      std::bind(&OpenIris::HTTPDHandler::command_handler, this, std::placeholders::_1));

  this->server->on(
      "/crop-roi",
      HTTP_GET,
      std::bind(&OpenIris::HTTPDHandler::roi_crop_command_handler, this, std::placeholders::_1));

  Serial.println("Initializing web server");
  this->server->begin();
}

void OpenIris::HTTPDHandler::roi_crop_command_handler(AsyncWebServerRequest *request)
{
  int offsetX = 0;
  int offsetY = 0;
  int outputX = 0;
  int outputY = 0;

  if (request->hasParam("offsetX"))
  {
    AsyncWebParameter *offsetX_param = request->getParam("offsetX");
    offsetX = atoi(offsetX_param->value().c_str());
  }
  if (request->hasParam("offsetY"))
  {
    AsyncWebParameter *offsetY_param = request->getParam("offsetY");
    offsetY = atoi(offsetY_param->value().c_str());
  }
  if (request->hasParam("outputX"))
  {
    AsyncWebParameter *outputX_param = request->getParam("outputX");
    outputX = atoi(outputX_param->value().c_str());
  }
  if (request->hasParam("outputY"))
  {
    AsyncWebParameter *outputY_param = request->getParam("outputY");
    outputY = atoi(outputY_param->value().c_str());
  }

  if (offsetX != 0 && offsetY != 0 && outputX != 0 && outputY != 0)
    cameraHandler.setVieWindow(offsetX, offsetY, outputX, outputY);
  request->send(200);
}

void OpenIris::HTTPDHandler::command_handler(AsyncWebServerRequest *request)
{
  if (request->hasParam("framesize"))
  {
    AsyncWebParameter *framesize_param = request->getParam("framesize");
    cameraHandler.setCameraResolution((framesize_t)atoi(framesize_param->value().c_str()));
  }
  if (request->hasParam("hmirror"))
  {
    AsyncWebParameter *hmirror_param = request->getParam("hmirror");
    cameraHandler.setHFlip(atoi(hmirror_param->value().c_str()));
  }
  if (request->hasParam("vflip"))
  {
    AsyncWebParameter *vflip_param = request->getParam("vflip");
    cameraHandler.setVFlip(atoi(vflip_param->value().c_str()));
  }

  request->send(200);
}

void OpenIris::HTTPDHandler::stream_handler(AsyncWebServerRequest *request)
{
  OpenIris::AsyncJpegStreamResponse *response = new OpenIris::AsyncJpegStreamResponse();
  if (!response)
  {
    request->send(501);
    return;
  }
  response->addHeader("Access-Control-Allow-Origin", "*");
  request->send(response);
}
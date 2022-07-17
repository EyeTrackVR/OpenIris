#include "webserverHandler.hpp"

APIServer::APIServer(int CONTROL_PORT, CameraHandler *cameraHandler)
{
  this->server = new AsyncWebServer(CONTROL_PORT);
  this->cameraHandler = cameraHandler;
}

void APIServer::startAPIServer()
{
  this->server->on(
      "/control",
      HTTP_GET,
      std::bind(&APIServer::command_handler, this, std::placeholders::_1));

  log_d("Initializing web server");
  this->server->begin();
}

void APIServer::command_handler(AsyncWebServerRequest *request)
{
  if (request->hasParam("framesize"))
  {
    AsyncWebParameter *framesize_param = request->getParam("framesize");
    cameraHandler->setCameraResolution((framesize_t)atoi(framesize_param->value().c_str()));
  }
  if (request->hasParam("hmirror"))
  {
    AsyncWebParameter *hmirror_param = request->getParam("hmirror");
    cameraHandler->setHFlip(atoi(hmirror_param->value().c_str()));
  }
  if (request->hasParam("vflip"))
  {
    AsyncWebParameter *vflip_param = request->getParam("vflip");
    cameraHandler->setVFlip(atoi(vflip_param->value().c_str()));
  }

  request->send(200);
}
#include "webserverHandler.hpp"

/* Constructor with unique_ptr */
APIServer::APIServer(int CONTROL_PORT, CameraHandler *cameraHandler, WiFiHandler *network) : network(network),
                                                                                             server(new AsyncWebServer(CONTROL_PORT)),
                                                                                             cameraHandler(cameraHandler) {}

void APIServer::startAPIServer()
{
  /* this->server->on(
      "/control",
      HTTP_GET,
      std::bind(&APIServer::command_handler, this, std::placeholders::_1)); */

  //! i have changed this to use lambdas instead of std::bind to avoid the overhead. Lambdas are always more preferable.
  this->server->on(
      "/control",
      HTTP_GET, [&](AsyncWebServerRequest *request)
      { command_handler(request); 
        request->send(200); });

  log_d("Initializing web server");
  this->server->begin();
}

//! To do - change this to use proper Hash Map to remove overhead of conditionals.
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
#if ENABLE_ADHOC
  if (request->hasParam("ap_ssid"))
  {
    AsyncWebParameter *ap_ssid_param = request->getParam("ap_ssid");
    network->setWiFiConf(ap_ssid_param->value().c_str(), network->conf->ap.ssid, &*network->conf);
  }
  if (request->hasParam("ap_password"))
  {
    AsyncWebParameter *ap_password_param = request->getParam("ap_password");
    network->setWiFiConf(ap_password_param->value().c_str(), network->conf->ap.password, &*network->conf);
  }
  if (request->hasParam("ap_channel"))
  {
    AsyncWebParameter *ap_channel_param = request->getParam("ap_channel");
    network->setWiFiConf(ap_channel_param->value().c_str(), &network->conf->ap.channel, &*network->conf);
  }
#else
  if (request->hasParam("ssid"))
  {
    AsyncWebParameter *ssid_param = request->getParam("ssid");
    network->setWiFiConf(ssid_param->value().c_str(), network->conf->sta.ssid, &*network->conf);
  }
  if (request->hasParam("password"))
  {
    AsyncWebParameter *password_param = request->getParam("password");
    network->setWiFiConf(password_param->value().c_str(), network->conf->sta.password, &*network->conf);
  }
  if (request->hasParam("channel"))
  {
    AsyncWebParameter *channel_param = request->getParam("channel");
    network->setWiFiConf(channel_param->value().c_str(), &network->conf->sta.channel, &*network->conf);
  }
#endif // ENABLE_ADHOC
}
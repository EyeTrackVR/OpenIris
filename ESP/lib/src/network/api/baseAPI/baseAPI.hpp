#ifndef BASEAPI_HPP
#define BASEAPI_HPP

#include <unordered_map>
#include <string>

#define WEBSERVER_H

/* #define XHTTP_GET 0b00000001;
#define XHTTP_POST 0b00000010;
#define XHTTP_DELETE 0b00000100;
#define XHTTP_PUT 0b00001000;
#define XHTTP_PATCH 0b00010000;
#define XHTTP_HEAD 0b00100000;
#define XHTTP_OPTIONS 0b01000000;
#define XHTTP_ANY 0b01111111; */

#define HTTP_ANY 0b01111111
#define HTTP_GET 0b00000001

#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>

// #include "network/api/utilities/apiUtilities.hpp" //! Only needed for the shaEncoder function (for now)
#include "data/utilities/network_utilities.hpp"

#include "data/config/project_config.hpp"
#include "data/StateManager/StateManager.hpp"
#include "io/camera/cameraHandler.hpp"

class BaseAPI
{
protected:
	std::string api_url;

	static const char *MIMETYPE_HTML;
	/* static const char *MIMETYPE_CSS; */
	/* static const char *MIMETYPE_JS; */
	/* static const char *MIMETYPE_PNG; */
	/* static const char *MIMETYPE_JPG; */
	/* static const char *MIMETYPE_ICO; */
	static const char *MIMETYPE_JSON;

protected:
	/* Commands */
	void setWiFi(AsyncWebServerRequest *request);
	void setWiFiTXPower(AsyncWebServerRequest *request);
	void getJsonConfig(AsyncWebServerRequest *request);
	void factoryReset(AsyncWebServerRequest *request);
	void initOTAHandler(AsyncWebServerRequest *request);
	void setDeviceConfig(AsyncWebServerRequest *request);
	void rebootDevice(AsyncWebServerRequest *request);
	void ping(AsyncWebServerRequest *request);
	void save(AsyncWebServerRequest *request);

	/* Camera Handlers */
	void setCamera(AsyncWebServerRequest *request);
	void restartCamera(AsyncWebServerRequest *request);

	/* Route Command types */
	using route_method = void (BaseAPI::*)(AsyncWebServerRequest *);
	typedef std::unordered_map<std::string, route_method> route_t;
	typedef std::unordered_map<std::string, route_t> route_map_t;

	route_t routes;
	route_map_t route_map;

	std::unordered_map<int, std::string> _networkMethodsMap = {
		{0b00000001, "GET"},
		{0b00000010, "POST"},
		{0b00001000, "PUT"},
		{0b00000100, "DELETE"},
		{0b00010000, "PATCH"},
		{0b01000000, "OPTIONS"},
	};

	enum RequestMethods
	{
		GET,
		POST,
		PUT,
		DELETE,
		PATCH,
		OPTIONS,
	};

	std::unordered_map<int, RequestMethods> _networkMethodsMap_enum = {
		{0b00000001, GET},
		{0b00000010, POST},
		{0b00001000, PUT},
		{0b00000100, DELETE},
		{0b00010000, PATCH},
		{0b01000000, OPTIONS},
	};

	typedef std::unordered_map<std::string, WebRequestMethodComposite> networkMethodsMap_t;

	ProjectConfig *projectConfig;
	AsyncWebServer *server;
	CameraHandler *camera;
	StateManager<WiFiState_e> *WiFiStateManager;

public:
	BaseAPI(int CONTROL_PORT,
			ProjectConfig *projectConfig,
			CameraHandler *camera,
			StateManager<WiFiState_e> *WiFiStateManager,
			const std::string &api_url);

	virtual ~BaseAPI();
	virtual void begin();
	void notFound(AsyncWebServerRequest *request) const;
};

#endif // BASEAPI_HPP

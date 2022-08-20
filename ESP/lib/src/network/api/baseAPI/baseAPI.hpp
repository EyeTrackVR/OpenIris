#ifndef BASEAPI_HPP
#define BASEAPI_HPP
#include "network/wifihandler/wifiHandler.hpp"
#include "network/api/utilities/apiUtilities.hpp"

class BaseAPI : public API_Utilities
{
protected:
	struct LocalWifiConfig
	{
		std::string ssid;
		std::string pass;
		uint8_t channel;
		bool adhoc;
	};

	LocalWifiConfig localWifiConfig;

	struct LocalAPWifiConfig
	{
		std::string ssid;
		std::string pass;
		uint8_t channel;
	};

	LocalWifiConfig localAPWifiConfig;

	enum JSON_TYPES
	{
		CONFIG,
		SETTINGS,
		DATA,
		STATUS,
		COMMANDS,
		WIFI,
		WIFIAP,
	};

	std::unordered_map<std::string, JSON_TYPES> json_TypesMap = {
		{"config", CONFIG},
		{"settings", SETTINGS},
		{"data", DATA},
		{"status", STATUS},
		{"commands", COMMANDS},
		{"wifi", WIFI},
		{"wifiap", WIFIAP},
	};

protected:
	/* Commands */
	void setWiFi(AsyncWebServerRequest *request);
	void handleJson(AsyncWebServerRequest *request);
	void factoryReset(AsyncWebServerRequest *request);
	void rebootDevice(AsyncWebServerRequest *request);
	void deleteRoute(AsyncWebServerRequest *request);

	/* Camera Handler */
	void setCamera(AsyncWebServerRequest *request);

	using call_back_function_t = void (BaseAPI::*)(AsyncWebServerRequest *);
	typedef call_back_function_t (*call_back_function_ptr)(AsyncWebServerRequest *);

	/* Route Command types */
	using route_method = void (BaseAPI::*)(AsyncWebServerRequest *);
	// typedef void (*callback)(AsyncWebServerRequest *);
	typedef std::unordered_map<std::string, route_method> route_t;
	typedef std::unordered_map<std::string, route_t> route_map_t;

	route_t routes;
	route_map_t route_map;

public:
	BaseAPI(int CONTROL_PORT,
			WiFiHandler *network,
			CameraHandler *camera,
			StateManager<WiFiState_e> *stateManager,
			std::string api_url);
	virtual ~BaseAPI();
	virtual void begin();
	virtual void setupServer();
	void triggerWifiConfigWrite();
};

#endif // BASEAPI_HPP
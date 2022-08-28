#ifndef BASEAPI_HPP
#define BASEAPI_HPP
#include "network/wifihandler/wifiHandler.hpp"
#include "network/api/utilities/apiUtilities.hpp"
#include "data/utilities/network_utilities.hpp"

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

	/* Camera Handlers */
	void getCameraStatus(AsyncWebServerRequest *request);
	void setCameraVar(AsyncWebServerRequest *request);
	void setCamera(AsyncWebServerRequest *request);

	/* Route Command types */
	using route_method = void (BaseAPI::*)(AsyncWebServerRequest *);
	typedef std::unordered_map<std::string, route_method> route_t;
	typedef std::unordered_map<std::string, route_t> route_map_t;

	route_t routes;
	route_map_t route_map;

public:
	BaseAPI();
			
	virtual ~BaseAPI();
	virtual void begin();
	virtual void setupServer();
	void triggerWifiConfigWrite();
};

#endif // BASEAPI_HPP
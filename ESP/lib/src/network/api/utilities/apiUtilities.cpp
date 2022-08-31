#include "apiUtilities.hpp"

//! These have to be called before the constructor of the class because they are static
//! C++ 11 does not have inline variables, sadly. So we have to do this.
const char *API_Utilities::MIMETYPE_HTML{"text/html"};
// const char *BaseAPI::MIMETYPE_CSS{"text/css"};
// const char *BaseAPI::MIMETYPE_JS{"application/javascript"};
// const char *BaseAPI::MIMETYPE_PNG{"image/png"};
// const char *BaseAPI::MIMETYPE_JPG{"image/jpeg"};
// const char *BaseAPI::MIMETYPE_ICO{"image/x-icon"};
const char *API_Utilities::MIMETYPE_JSON{"application/json"};

//*********************************************************************************************
//!                                     API Utilities
//*********************************************************************************************

API_Utilities::API_Utilities(int CONTROL_PORT,
							 WiFiHandler *network,
							 CameraHandler *camera,
							 StateManager<WiFiState_e> *stateManager,
							 const std::string &api_url) : server(new AsyncWebServer(CONTROL_PORT)),
														   stateManager(stateManager),
														   network(network),
														   camera(camera),
														   api_url(api_url) {}

API_Utilities::~API_Utilities() {}

std::string API_Utilities::shaEncoder(std::string data)
{
	const char *data_c = data.c_str();
	int size = 64;
	uint8_t hash[size];
	mbedtls_md_context_t ctx;
	mbedtls_md_type_t md_type = MBEDTLS_MD_SHA512;

	const size_t len = strlen(data_c);
	mbedtls_md_init(&ctx);
	mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 0);
	mbedtls_md_starts(&ctx);
	mbedtls_md_update(&ctx, (const unsigned char *)data_c, len);
	mbedtls_md_finish(&ctx, hash);
	mbedtls_md_free(&ctx);

	std::string hash_string = "";
	for (uint16_t i = 0; i < size; i++)
	{
		std::string hex = String(hash[i], HEX).c_str();
		if (hex.length() < 2)
		{
			hex = "0" + hex;
		}
		hash_string += hex;
	}
	return hash_string;
}

void API_Utilities::notFound(AsyncWebServerRequest *request) const
{
	if (_networkMethodsMap.find(request->method()) != _networkMethodsMap.end())
	{
		log_i("%s: http://%s%s/\n", _networkMethodsMap.at(request->method()).c_str(), request->host().c_str(), request->url().c_str());
		char buffer[100];
		snprintf(buffer, sizeof(buffer), "Request %s Not found: %s", _networkMethodsMap.at(request->method()).c_str(), request->url().c_str());
		request->send(404, "text/plain", buffer);
	}
	else
	{
		request->send(404, "text/plain", "Request Not found using unknown method");
	}
}

void API_Utilities::printASCII()
{
	Serial.println(F("                                                      : === WELCOME === TO === : "));
	Serial.println(F(" <===========================================================================================================================> "));
	Serial.println(F("                                        ██████╗ ██████╗ ███████╗███╗   ██╗██╗██████╗ ██╗███████╗   "));
	Serial.println(F("                                       ██╔═══██╗██╔══██╗██╔════╝████╗  ██║██║██╔══██╗██║██╔════╝   "));
	Serial.println(F("                                       ██║   ██║██████╔╝█████╗  ██╔██╗ ██║██║██████╔╝██║███████╗   "));
	Serial.println(F("                                       ██║   ██║██╔═══╝ ██╔══╝  ██║╚██╗██║██║██╔══██╗██║╚════██║   "));
	Serial.println(F("                                       ╚██████╔╝██║     ███████╗██║ ╚████║██║██║  ██║██║███████║   "));
	Serial.println(F("                                        ╚═════╝ ╚═╝     ╚══════╝╚═╝  ╚═══╝╚═╝╚═╝  ╚═╝╚═╝╚══════╝   "));
	Serial.println(F("                                                                                                          "));
	Serial.println(F("                                                             ██████████████                               "));
	Serial.println(F("                                                       ██▓▓▓▓▓▓▒▒▒▒▒▒▒▒░░▒▒▓▓▓▓██                         "));
	Serial.println(F("                                                   ████▓▓░░░░▒▒░░░░░░▒▒░░░░░░▒▒░░████                     "));
	Serial.println(F("                                               ██▓▓▒▒▓▓▓▓▒▒▒▒░░░░░░▒▒░░▒▒░░░░░░▒▒░░▒▒▓▓▓▓                 "));
	Serial.println(F("                                             ██▓▓▒▒▒▒▒▒▒▒░░▒▒░░░░░░░░░░░░▒▒░░░░▒▒░░░░▒▒░░██               "));
	Serial.println(F("                                           ██▓▓▓▓░░░░▒▒░░░░▒▒▒▒░░░░░░░░░░▒▒░░  ░░░░░░░░▒▒░░██             "));
	Serial.println(F("                                         ██▓▓▓▓▓▓▓▓▓▓░░░░░░▒▒░░░░░░░░░░░░░░      ░░░░░░░░░░░░██           "));
	Serial.println(F("                                       ██▓▓▒▒▒▒▒▒▒▒▒▒▒▒░░░░░░▒▒░░░░░░░░░░░░  ░░            ▒▒▒▒██         "));
	Serial.println(F("                                       ██▓▓▒▒▒▒▒▒▒▒░░░░▒▒░░░░░░░░░░░░░░░░░░                ░░░░██         "));
	Serial.println(F("                                     ▓▓▓▓▒▒▒▒▒▒▒▒▒▒░░░░░░▒▒░░░░░░░░░░░░░░░░░░▓▓▓▓▓▓▓▓▓▓    ░░  ▒▒▓▓       "));
	Serial.println(F("                                     ██▒▒▒▒▒▒▒▒▒▒░░░░░░░░░░░░░░░░░░░░░░░░▓▓▓▓▒▒▒▒▒▒▒▒▓▓▓▓▓▓    ░░██       "));
	Serial.println(F("                                   ▓▓▒▒▒▒▒▒▒▒▒▒░░░░░░░░░░░░░░░░░░░░    ▓▓▒▒▒▒▒▒▒▒░░░░▒▒▒▒▓▓▒▒  ░░▒▒▓▓     "));
	Serial.println(F("                                   ██▓▓▒▒░░░░░░▒▒▒▒▒▒░░░░░░░░░░░░░░░░▓▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒░░░░▒▒▓▓▓▓  ░░██     "));
	Serial.println(F("                                   ██▓▓▓▓▒▒▒▒▒▒▒▒░░░░▒▒░░░░░░░░░░░░  ▒▒▒▒▒▒▒▒▒▒▒▒████▓▓░░░░▒▒▓▓  ░░██     "));
	Serial.println(F("                                 ██▓▓▒▒▒▒▒▒▓▓▒▒▓▓░░░░░░░░░░░░░░░░░░▓▓▒▒▒▒░░▒▒▒▒████  ▒▒██░░▒▒▓▓▓▓  ░░██   "));
	Serial.println(F("                                 ██▒▒▒▒▒▒▒▒▒▒▒▒▒▒░░░░░░░░░░░░░░░░░░▓▓░░▒▒▒▒▒▒██████▒▒  ▓▓▓▓░░▓▓▓▓  ░░██   "));
	Serial.println(F("                                 ██▓▓▒▒▒▒░░▒▒▒▒▒▒░░░░░░░░░░░░░░░░░░▓▓▒▒░░▒▒░░████████▓▓  ██▓▓▒▒▓▓  ░░██   "));
	Serial.println(F("                                 ██▓▓▓▓▓▓▓▓▒▒▒▒▒▒░░░░░░▒▒░░░░░░░░░░▒▒░░▒▒░░▒▒██████████▒▒██▒▒▒▒▓▓  ░░██   "));
	Serial.println(F("                                 ██▓▓▒▒▒▒▒▒░░░░░░░░▒▒▒▒░░░░░░░░░░░░▒▒▒▒░░░░▒▒██▒▒██████  ██▒▒▒▒▓▓  ░░██   "));
	Serial.println(F("                                 ██▒▒▒▒▒▒░░▓▓▓▓▓▓▒▒░░░░░░░░░░░░░░░░▒▒░░  ▒▒░░██  ▒▒████▒▒██▒▒▒▒▓▓  ░░██   "));
	Serial.println(F("                                 ██▓▓▓▓▓▓▓▓░░▒▒▒▒░░▒▒░░░░░░░░░░░░░░▒▒▒▒░░░░░░▒▒██  ██████▒▒▒▒▒▒▓▓░░░░██   "));
	Serial.println(F("                               ██▓▓██▓▓▒▒▒▒▓▓░░░░▒▒░░░░░░░░░░░░░░░░░░▒▒▒▒░░▒▒░░▒▒██████▒▒▒▒▒▒▓▓  ░░██     "));
	Serial.println(F("                               ██▓▓██▒▒▒▒▒▒▒▒▓▓▓▓▒▒░░░░░░░░░░░░░░░░░░▒▒░░▒▒░░▒▒░░▒▒░░▒▒▒▒▒▒▒▒▓▓░░░░██     "));
	Serial.println(F("                             ██▒▒▓▓██▓▓▒▒▒▒▒▒▒▒▒▒▒▒░░░░░░░░░░░░░░░░░░░░▒▒░░▒▒░░░░░░▒▒▒▒▒▒▒▒▓▓  ░░░░██     "));
	Serial.println(F("                             ██▒▒▒▒▓▓██▓▓▓▓▒▒▒▒▒▒▒▒░░░░░░░░▒▒░░░░░░░░░░░░▒▒▒▒▒▒░░▒▒▒▒▒▒▓▓▓▓  ░░░░██       "));
	Serial.println(F("                           ▓▓▓▓▒▒▒▒▓▓██▓▓▒▒▒▒▒▒░░░░▒▒▒▒░░▒▒░░░░░░░░░░░░░░░░░░▒▒▒▒▓▓▓▓▓▓░░░░░░░░▒▒██       "));
	Serial.println(F("                           ██▒▒▒▒▓▓▒▒▓▓██▒▒▒▒▒▒▒▒▓▓▒▒▒▒▒▒░░░░░░░░░░░░░░░░░░░░░░░░░░░░  ░░  ░░░░██         "));
	Serial.println(F("                         ██▒▒▒▒▓▓░░▒▒▒▒██▓▓▓▓▓▓▒▒▒▒▒▒▒▒▓▓▓▓░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░▒▒░░▒▒██         "));
	Serial.println(F("                         ██▒▒▒▒▓▓▒▒░░▓▓▒▒██▓▓▒▒▒▒▒▒▒▒░░▓▓▒▒▓▓▒▒░░░░▒▒░░░░░░░░░░░░░░░░░░░░░░▓▓██           "));
	Serial.println(F("                         ██▒▒▒▒▒▒░░▒▒▒▒▒▒▓▓██▓▓▒▒▒▒▒▒▓▓▒▒▒▒▒▒▒▒▒▒▓▓░░░░░░░░░░▒▒░░░░▒▒░░▒▒▒▒██             "));
	Serial.println(F("                         ██▒▒▒▒░░▒▒░░▒▒████  ██▓▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒░░▓▓▒▒▒▒▒▒▓▓▒▒██               "));
	Serial.println(F("                         ██▒▒▓▓░░▒▒░░▓▓        ████▓▓▒▒▓▓▒▒▒▒▒▒░░▒▒▒▒▒▒▒▒▓▓▒▒▒▒▒▒▒▒▒▒██▓▓                 "));
	Serial.println(F("                         ██▒▒▒▒▒▒▒▒  ██            ████▓▓▒▒▓▓▓▓▓▓▒▒▒▒▒▒▒▒▓▓▒▒▒▒▒▒████                     "));
	Serial.println(F("                           ██▒▒▒▒▒▒▒▒░░██              ██████▓▓▒▒▒▒▒▒▒▒▓▓▓▓██████                         "));
	Serial.println(F("                           ██▒▒▒▒▒▒▒▒░░  ████                ██████████████                               "));
	Serial.println(F("                             ██▒▒▒▒▒▒▒▒░░  ░░████                                                         "));
	Serial.println(F("                               ████▒▒▒▒▒▒░░░░  ░░████                                                     "));
	Serial.println(F("                                   ████▒▒▒▒▒▒░░░░  ░░██                                                   "));
	Serial.println(F("                                       ████▒▒▒▒▒▒░░  ░░██                                                 "));
	Serial.println(F("                                           ██▓▓▒▒▒▒░░  ▒▒▓▓                                               "));
	Serial.println(F("                                               ████▒▒░░  ▒▒██                                             "));
	Serial.println(F("                                                   ▓▓▒▒░░░░██                                             "));
	Serial.println(F("                                                     ██░░  ██                                             "));
	Serial.println(F("                                               ▓▓██  ██░░░░██                                             "));
	Serial.println(F("                                             ██░░██  ██░░░░██                                             "));
	Serial.println(F("                                           ██░░██    ██░░▒▒██                                             "));
	Serial.println(F("                                           ██░░▒▒████░░▒▒██                                               "));
	Serial.println(F("                                             ▓▓▒▒▒▒▒▒▒▒▓▓                                                 "));
	Serial.println(F("                                               ████████                                                   "));
	Serial.println(F("                                                                                                          "));
	Serial.println(F(" <============================================================================================================================> "));
}

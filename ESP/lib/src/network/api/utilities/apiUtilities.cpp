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

bool API_Utilities::ssid_write = false;
bool API_Utilities::pass_write = false;
bool API_Utilities::channel_write = false;

//*********************************************************************************************
//!                                     API Utilities
//*********************************************************************************************

API_Utilities::API_Utilities(int CONTROL_PORT,
							 WiFiHandler *network,
							 CameraHandler *camera,
							 StateManager<WiFiState_e> *stateManager,
							 std::string api_url) : server(new AsyncWebServer(CONTROL_PORT)),
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

// Read File from SPIFFS
/* String API_Utilities::readFile(fs::FS &fs, std::string path)
{
	log_i("Reading file: %s\r\n", path.c_str());

	File file = fs.open(path.c_str());
	if (!file || file.isDirectory())
	{
		log_e("[INFO]: Failed to open file for reading");
		return String();
	}

	String fileContent;
	while (file.available())
	{
		fileContent = file.readStringUntil('\n');
		break;
	}
	return fileContent;
}

// Write file to SPIFFS
void API_Utilities::writeFile(fs::FS &fs, std::string path, std::string message)
{
	log_i("[Writing File]: Writing file: %s\r\n", path);
	Network_Utilities::my_delay(0.1L);

	File file = fs.open(path.c_str(), FILE_WRITE);
	if (!file)
	{
		log_i("[Writing File]: failed to open file for writing");
		return;
	}
	if (file.print(message.c_str()))
	{
		log_i("[Writing File]: file written");
	}
	else
	{
		log_i("[Writing File]: file write failed");
	}
} */

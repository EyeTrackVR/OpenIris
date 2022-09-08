#include "apiUtilities.hpp"

//*********************************************************************************************
//!                                     API Utilities
//*********************************************************************************************

API_Utilities::API_Utilities() {};

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

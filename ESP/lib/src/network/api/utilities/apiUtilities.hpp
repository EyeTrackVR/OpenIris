#ifndef APIUTILITIES_HPP
#define APIUTILITIES_HPP

#include <string>
//#include <SPIFFS.h>
#include "mbedtls/md.h"
#include "data/utilities/network_utilities.hpp"

class API_Utilities
{
public:
	API_Utilities();
	virtual ~API_Utilities();
	static std::string shaEncoder(std::string data);

public:
	
};

#endif // APIUTILITIES_HPP
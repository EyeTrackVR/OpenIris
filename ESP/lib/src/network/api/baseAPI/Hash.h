#ifndef HASH_H_
#define HASH_H_

#include <stdint.h>
#include <string>

// #define DEBUG_SHA1

void sha1(const uint8_t* data, uint32_t size, uint8_t hash[20]);
void sha1(const char* data, uint32_t size, uint8_t hash[20]);
void sha1(const std::string& data, uint8_t hash[20]);

std::string sha1(const uint8_t* data, uint32_t size);
std::string sha1(const char* data, uint32_t size);
std::string sha1(const std::string& data);

#endif /* HASH_H_ */

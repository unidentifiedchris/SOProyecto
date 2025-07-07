#pragma once
#include <string>
struct Hash32{ unsigned char bytes[32]; };
Hash32 hash_sha256(const std::string& data);
std::string hash_to_hex(const Hash32& h);

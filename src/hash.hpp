#pragma once
#include <array>
#include <string>
std::array<unsigned char,32> hash_sha256(const std::string& data);
std::string hash_to_hex(const std::array<unsigned char,32>& h);

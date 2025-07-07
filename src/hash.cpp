#include "hash.hpp"
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>

std::array<unsigned char,32> hash_sha256(const std::string& data){
    std::array<unsigned char,32> h{};
    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, data.data(), data.size());
    SHA256_Final(h.data(), &ctx);
    return h;
}
std::string hash_to_hex(const std::array<unsigned char,32>& h){
    std::ostringstream oss;
    for(std::size_t i = 0; i < h.size(); ++i){
        unsigned char b = h[i];
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)b;
    }
    return oss.str();
}

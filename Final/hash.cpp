#include "hash.hpp"
#include <sstream>
#include <iomanip>

Hash32 hash_sha256(const std::string& data){
    Hash32 h;
    for(int i=0;i<32;++i) h.bytes[i]=0;
    for(std::size_t i=0;i<data.size();++i){
        unsigned char b = static_cast<unsigned char>(data[i]);
        h.bytes[i%32] = static_cast<unsigned char>((h.bytes[i%32] + b + i) * 31u);
    }
    return h;
}
std::string hash_to_hex(const Hash32& h){
    std::ostringstream oss;
    for(int i = 0; i < 32; ++i){
        unsigned char b = h.bytes[i];
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)b;
    }
    return oss.str();
}

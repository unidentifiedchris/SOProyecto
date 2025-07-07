#include "verificar.hpp"
#include <fstream>
#include <cstring>       

bool verificar_archivos(const std::string& orig, const std::string& copia){
    std::ifstream f1(orig.c_str(), std::ios::binary);
    std::ifstream f2(copia.c_str(), std::ios::binary);
    if(!f1 || !f2) return false;
    static const std::size_t BUF = 8192;
    char b1[BUF], b2[BUF];
    while(f1 && f2){
        f1.read(b1, BUF);
        f2.read(b2, BUF);
        if(f1.gcount()!=f2.gcount()) return false;
        if(std::memcmp(b1, b2, f1.gcount())!=0) return false;
    }
    return true;
}

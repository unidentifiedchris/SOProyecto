#pragma once
#include <string>
#include <cstdio>
#include <fstream>
#include <sstream>
#if defined(_WIN32)
#include <direct.h>
#else
#include <sys/stat.h>
#endif
inline void fs_create_dir(const std::string& p){
#if defined(_WIN32)
    _mkdir(p.c_str());
#else
    mkdir(p.c_str(), 0755);
#endif
}
inline bool fs_copy_file(const std::string& from, const std::string& to){
    std::ifstream in(from.c_str(), std::ios::binary);
    std::ofstream out(to.c_str(), std::ios::binary);
    if(!in || !out) return false;
    char buffer[4096];
    while(in.read(buffer, sizeof(buffer))) out.write(buffer, sizeof(buffer));
    out.write(buffer, in.gcount());
    return true;
}
inline void fs_remove(const std::string& p){
    std::remove(p.c_str());
}
inline std::string int_to_string(int v){
    std::ostringstream oss;
    oss<<v;
    return oss.str();
}
inline int string_to_int(const std::string& s){
    std::istringstream iss(s);
    int v=0; iss>>v; return v;
}

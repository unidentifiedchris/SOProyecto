#include <filesystem>
#include <fstream>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <ctime>
#include "cifrado.hpp"
#include "hash.hpp"

using Steady = std::chrono::steady_clock;
using Sys = std::chrono::system_clock;
namespace fs = std::filesystem;

static std::string fmt_time(Sys::time_point tp){
    std::time_t t = Sys::to_time_t(tp);
    std::tm tm = *std::localtime(&t);
    char buf[9];
    std::strftime(buf, sizeof(buf), "%H:%M:%S", &tm);
    return buf;
}

static std::string fmt_dur(Steady::duration d){
    using namespace std::chrono;
    auto h = duration_cast<hours>(d).count();
    auto m = duration_cast<minutes>(d).count() % 60;
    auto s = duration_cast<seconds>(d).count() % 60;
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << h << ':'
        << std::setw(2) << m << ':' << std::setw(2) << s;
    return oss.str();
}

static std::string idx2(int i){
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << i;
    return oss.str();
}

int main(int argc,char* argv[]){
    if(argc<3){
        std::cerr<<"Uso: flujo <original> <N>\n";
        return 1;
    }
    fs::path original = argv[1];
    int N = std::stoi(argv[2]);

    fs::create_directory("copias");
    fs::create_directory("cifrados");
    fs::create_directory("sha");

    std::ifstream in(original, std::ios::binary);
    std::string orig_data((std::istreambuf_iterator<char>(in)),
                           std::istreambuf_iterator<char>());

    std::cout << "PROCESO BASE\n";
    auto TI_sys = Sys::now();
    auto TI = Steady::now();
    std::cout << "TI: " << fmt_time(TI_sys) << "\n";

    bool any_fail = false;
    for(int i=1;i<=N;++i){
        auto start = Steady::now();
        std::string name = idx2(i);
        // copy original file
        fs::copy_file(original, fs::path("copias")/(name+".txt"),
                      fs::copy_options::overwrite_existing);
        // encrypt and write
        std::string cif = cifrar(orig_data);
        {
            std::ofstream out(fs::path("cifrados")/(name+".txt"), std::ios::binary);
            out.write(cif.data(), cif.size());
        }
        // hash
        auto h = hash_sha256(cif);
        std::string hex = hash_to_hex(h);
        {
            std::ofstream sh(fs::path("sha")/(name+".sha"));
            sh << hex;
        }
        // verify hash and decrypt
        auto h2 = hash_sha256(cif);
        std::string dec = descifrar(cif);
        if(hex != hash_to_hex(h2) || dec != orig_data)
            any_fail = true;
        auto end = Steady::now();
        std::cout << "Tiempo " << name << " : " << fmt_dur(end-start) << "\n";
    }

    auto TFIN = Steady::now();
    auto TFIN_sys = Sys::now();
    auto TT = TFIN - TI;
    auto TPPA = TT / N;

    std::cout << "TFIN : " << fmt_time(TFIN_sys) << "\n";
    std::cout << "TPPA : " << fmt_dur(TPPA) << "\n";
    std::cout << "TT   : " << fmt_dur(TT) << "\n";
    if(any_fail)
        std::cout << "❌ Error de verificación\n";
    else
        std::cout << "✅ Verificación OK\n";
}

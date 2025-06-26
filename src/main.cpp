#include <filesystem>
#include <fstream>
#include <iostream>
#include <chrono>
#include "cifrado.hpp"
#include "hash.hpp"
#include "verificar.hpp"

using Clock = std::chrono::steady_clock;
namespace fs = std::filesystem;

// ---------- Paso 1: Copiado ----------
void copiar_archivos(const fs::path& original, int N){
    fs::create_directory("copias");
    for(int i=1;i<=N;++i){
        fs::copy_file(original,
            fs::path("copias")/(std::to_string(i)+".txt"),
            fs::copy_options::overwrite_existing);
    }
}

// ---------- Paso 2: Cifrado + Hash ----------
void cifrar_y_hashear(int N){
    fs::create_directory("cifrados");
    fs::create_directory("sha");
    for(int i=1;i<=N;++i){
        std::ifstream in("copias/"+std::to_string(i)+".txt",
                         std::ios::binary);
        std::string data((std::istreambuf_iterator<char>(in)),
                          std::istreambuf_iterator<char>());
        std::string cif = cifrar(data);

        // escribe cifrado
        std::ofstream out("cifrados/"+std::to_string(i)+".txt",
                          std::ios::binary);
        out.write(cif.data(), cif.size());

        // hash
        auto h = hash_sha256(cif);
        std::ofstream sh("sha/"+std::to_string(i)+".sha");
        sh << hash_to_hex(h);
    }
}

// ---------- Paso 3: Verificar ----------
bool verificar(int N, const fs::path& original){
    for(int i=1;i<=N;++i){
        // descifrar
        std::ifstream in("cifrados/"+std::to_string(i)+".txt",
                         std::ios::binary);
        std::string cif((std::istreambuf_iterator<char>(in)),
                         std::istreambuf_iterator<char>());
        std::string dec = descifrar(cif);

        // compara con original
        std::ofstream tmp("tmp.txt", std::ios::binary);
        tmp.write(dec.data(), dec.size());
        tmp.close();

        if(!verificar_archivos(original.string(), "tmp.txt")){
            fs::remove("tmp.txt");
            std::cerr << "Fallo en copia "<<i<<"\n";
            return false;
        }
        fs::remove("tmp.txt");
    }
    return true;
}

int main(int argc,char* argv[]){
    if(argc<3){
        std::cerr<<"Uso: flujo <original> <N>\n";
        return 1;
    }
    fs::path original = argv[1];
    int N = std::stoi(argv[2]);

    auto TI = Clock::now();
    copiar_archivos(original, N);
    auto t_copia = Clock::now();

    cifrar_y_hashear(N);
    auto t_cif = Clock::now();

    bool ok = verificar(N, original);
    auto TFIN = Clock::now();

    auto ms_copia = std::chrono::duration_cast<std::chrono::milliseconds>(t_copia-TI).count();
    auto ms_cif   = std::chrono::duration_cast<std::chrono::milliseconds>(t_cif-t_copia).count();
    auto ms_ver   = std::chrono::duration_cast<std::chrono::milliseconds>(TFIN-t_cif).count();
    auto TT       = std::chrono::duration_cast<std::chrono::milliseconds>(TFIN-TI).count();
    double TPPA   = double(TT)/N;

    std::cout << "Copiado: " << ms_copia <<" ms\n";
    std::cout << "Cif+Hash: "<< ms_cif <<" ms\n";
    std::cout << "Verif: "   << ms_ver <<" ms\n";
    std::cout << "TPPA: "    << TPPA <<" ms\n";
    std::cout << "TT: "      << TT <<" ms\n";
    std::cout << (ok?"✅ Verificación OK\n":"❌ Error de verificación\n");
}

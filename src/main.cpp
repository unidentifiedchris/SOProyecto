#include <fstream>
#include <iostream>
#include <ctime>
#include <sstream>
#include "fs_compat.hpp"
#include "cifrado.hpp"
#include "hash.hpp"
#include "verificar.hpp"

// ---------- Paso 1: Copiado ----------
static void copiar_archivos(const std::string& original, int N){
    fs_create_dir("copias");
    for(int i=1;i<=N;++i){
        fs_copy_file(original, std::string("copias/")+int_to_string(i)+".txt");
    }
}

// ---------- Paso 2: Cifrado + Hash ----------
static void cifrar_y_hashear(int N){
    fs_create_dir("cifrados");
    fs_create_dir("sha");
    for(int i=1;i<=N;++i){
        std::string copy_name = std::string("copias/")+int_to_string(i)+".txt";
        std::ifstream in(copy_name.c_str(), std::ios::binary);
        std::string data((std::istreambuf_iterator<char>(in)),
                          std::istreambuf_iterator<char>());
        std::string cif = cifrar(data);
        std::string out_name = std::string("cifrados/")+int_to_string(i)+".txt";
        std::ofstream out(out_name.c_str(), std::ios::binary);
        out.write(cif.data(), cif.size());
        Hash32 h = hash_sha256(cif);
        std::string sh_name = std::string("sha/")+int_to_string(i)+".sha";
        std::ofstream sh(sh_name.c_str());
        sh << hash_to_hex(h);
    }
}

// ---------- Paso 3: Verificar ----------
static bool verificar(int N, const std::string& original){
    for(int i=1;i<=N;++i){
        std::string cif_name = std::string("cifrados/")+int_to_string(i)+".txt";
        std::ifstream in(cif_name.c_str(), std::ios::binary);
        std::string cif((std::istreambuf_iterator<char>(in)),
                         std::istreambuf_iterator<char>());
        std::string dec = descifrar(cif);
        std::ofstream tmp("tmp.txt", std::ios::binary);
        tmp.write(dec.data(), dec.size());
        tmp.close();
        if(!verificar_archivos(original, "tmp.txt")){
            fs_remove("tmp.txt");
            std::cerr << "Fallo en copia "<<i<<"\n";
            return false;
        }
        fs_remove("tmp.txt");
    }
    return true;
}

static void run_base(const std::string& original, int N){
    clock_t TI = clock();
    copiar_archivos(original, N);
    clock_t t_copia = clock();
    cifrar_y_hashear(N);
    clock_t t_cif = clock();
    bool ok = verificar(N, original);
    clock_t TFIN = clock();
    long ms_copia = (long)((t_copia - TI) * 1000 / CLOCKS_PER_SEC);
    long ms_cif   = (long)((t_cif   - t_copia) * 1000 / CLOCKS_PER_SEC);
    long ms_ver   = (long)((TFIN    - t_cif)   * 1000 / CLOCKS_PER_SEC);
    long TT       = (long)((TFIN    - TI)      * 1000 / CLOCKS_PER_SEC);
    double TPPA   = double(TT)/N;
    std::cout << "Copiado: " << ms_copia <<" ms\n";
    std::cout << "Cif+Hash: "<< ms_cif <<" ms\n";
    std::cout << "Verif: "   << ms_ver <<" ms\n";
    std::cout << "TPPA: "    << TPPA <<" ms\n";
    std::cout << "TT: "      << TT <<" ms\n";
    std::cout << (ok?"\xE2\x9C\x85 Verificaci\xC3\xB3n OK\n":"\xE2\x9D\x8C Error de verificaci\xC3\xB3n\n");
}

// -------- Optimizado (memoria) ---------
static void process_file_opt(int i, const std::string& orig, const std::string& orig_data, bool& any_fail){
    try{
        fs_copy_file(orig, std::string("copias/")+int_to_string(i)+".txt");
        std::string cif = cifrar(orig_data);
        std::string out_name = std::string("cifrados/")+int_to_string(i)+".txt";
        std::ofstream out(out_name.c_str(), std::ios::binary);
        out.write(cif.data(), cif.size());
        Hash32 h = hash_sha256(cif);
        std::string sh_name = std::string("sha/")+int_to_string(i)+".sha";
        std::ofstream sh(sh_name.c_str());
        sh << hash_to_hex(h);
        std::string dec = descifrar(cif);
        if(dec != orig_data){
            any_fail = true;
        }
    }catch(...){
        any_fail = true;
    }
}

static void run_opt(const std::string& original, int N){
    fs_create_dir("copias");
    fs_create_dir("cifrados");
    fs_create_dir("sha");
    std::ifstream in(original.c_str(), std::ios::binary);
    std::string orig_data((std::istreambuf_iterator<char>(in)),
                           std::istreambuf_iterator<char>());
    bool any_fail = false;
    clock_t TI = clock();
    for(int i=1;i<=N;++i){
        process_file_opt(i, original, orig_data, any_fail);
    }
    clock_t TFIN = clock();
    long TT = (long)((TFIN - TI) * 1000 / CLOCKS_PER_SEC);
    double TPPA = double(TT)/N;
    std::cout << "TPPA: " << TPPA << " ms\n";
    std::cout << "TT: " << TT << " ms\n";
    std::cout << (any_fail?"\xE2\x9D\x8C Error de verificaci\xC3\xB3n\n":"\xE2\x9C\x85 Verificaci\xC3\xB3n OK\n");
}

int main(int argc, char* argv[]){
    if(argc<3){
        std::cerr<<"Uso: flujo [opt] <original> <N>\n";
        return 1;
    }
    int idx=1;
    bool opt = false;
    if(std::string(argv[idx]) == "opt"){
        opt = true;
        ++idx;
    }
    if(argc - idx < 2){
        std::cerr<<"Uso: flujo [opt] <original> <N>\n";
        return 1;
    }
    std::string original = argv[idx];
    int N = string_to_int(argv[idx+1]);
    if(opt) run_opt(original,N); else run_base(original,N);
}

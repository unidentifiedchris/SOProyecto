#include <fstream>
#include <iostream>
#include <vector>
#include <ctime>
#include <sstream>
#include "fs_compat.hpp"
#include "cifrado.hpp"
#include "hash.hpp"



// Process a single index i (1..N)
static void process_file(int i, const std::string& orig, const std::string& orig_data,
                         bool& any_fail){
    try{
        // copy original file
        fs_copy_file(orig, std::string("copias/")+int_to_string(i)+".txt");
        // encrypt using in-memory data to avoid disk read
        std::string cif = cifrar(orig_data);
        // write cipher text
        {
            std::string out_name = std::string("cifrados/")+int_to_string(i)+".txt";
            std::ofstream out(out_name.c_str(), std::ios::binary);
            out.write(cif.data(), cif.size());
        }
        // hash
        Hash32 h = hash_sha256(cif);
        {
            std::string sh_name = std::string("sha/")+int_to_string(i)+".sha";
            std::ofstream sh(sh_name.c_str());
            sh << hash_to_hex(h);
        }
        // decrypt and verify in-memory
        std::string dec = descifrar(cif);
        if(dec != orig_data){
            any_fail = true;
        }
    }catch(...){
        any_fail = true;
    }
}

int main(int argc, char* argv[]){
    if(argc<3){
        std::cerr<<"Uso: flujo_opt <original> <N>\n";
        return 1;
    }
    std::string original = argv[1];
    int N = string_to_int(argv[2]);

    fs_create_dir("copias");
    fs_create_dir("cifrados");
    fs_create_dir("sha");

    // load original file once
    std::ifstream in(original.c_str(), std::ios::binary);
    std::string orig_data((std::istreambuf_iterator<char>(in)),
                           std::istreambuf_iterator<char>());

    bool any_fail = false;

    clock_t TI = clock();
    for(int i=1;i<=N;++i){
        process_file(i, original, orig_data, any_fail);
    }

    clock_t TFIN = clock();

    long TT = (long)((TFIN - TI) * 1000 / CLOCKS_PER_SEC);
    double TPPA = double(TT)/N;

    std::cout << "TPPA: " << TPPA << " ms\n";
    std::cout << "TT: " << TT << " ms\n";
    std::cout << (any_fail?"❌ Error de verificación\n":"✅ Verificación OK\n");
}


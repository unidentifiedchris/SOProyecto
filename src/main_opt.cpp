#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>
#include "cifrado.hpp"
#include "hash.hpp"

using Clock = std::chrono::steady_clock;
namespace fs = std::filesystem;

struct ThreadResult{
    bool ok{true};
};

// Process a single index i (1..N)
static void process_file(int i, const fs::path& orig, const std::string& orig_data,
                         std::atomic<bool>& any_fail){
    try{
        // copy original file
        fs::copy_file(orig, fs::path("copias")/(std::to_string(i)+".txt"),
                      fs::copy_options::overwrite_existing);
        // encrypt using in-memory data to avoid disk read
        std::string cif = cifrar(orig_data);
        // write cipher text
        {
            std::ofstream out(fs::path("cifrados")/(std::to_string(i)+".txt"),
                              std::ios::binary);
            out.write(cif.data(), cif.size());
        }
        // hash
        auto h = hash_sha256(cif);
        {
            std::ofstream sh(fs::path("sha")/(std::to_string(i)+".sha"));
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
    fs::path original = argv[1];
    int N = std::stoi(argv[2]);

    fs::create_directory("copias");
    fs::create_directory("cifrados");
    fs::create_directory("sha");

    // load original file once
    std::ifstream in(original, std::ios::binary);
    std::string orig_data((std::istreambuf_iterator<char>(in)),
                           std::istreambuf_iterator<char>());

    std::atomic<bool> any_fail(false);

    auto TI = Clock::now();
    std::vector<std::thread> threads;
    threads.reserve(N);
    for(int i=1;i<=N;++i){
        threads.emplace_back(process_file, i, std::cref(original),
                             std::cref(orig_data), std::ref(any_fail));
    }
    for(auto& t: threads) t.join();
    auto TFIN = Clock::now();

    auto TT = std::chrono::duration_cast<std::chrono::milliseconds>(TFIN-TI).count();
    double TPPA = double(TT)/N;

    std::cout << "TPPA: " << TPPA << " ms\n";
    std::cout << "TT: " << TT << " ms\n";
    std::cout << (any_fail?"❌ Error de verificación\n":"✅ Verificación OK\n");
}


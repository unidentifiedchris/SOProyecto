#include <filesystem>
#include <iostream>
#include <chrono>
#include <fstream>

using Clock = std::chrono::steady_clock;

void copiarArchivos(const std::filesystem::path& original, int N) {
    std::filesystem::create_directory("copias");
    auto ti = Clock::now();

    for (int i = 1; i <= N; ++i) {
        std::filesystem::path destino = "copias/" + std::to_string(i) + ".txt";
        std::filesystem::copy_file(
            original, destino,
            std::filesystem::copy_options::overwrite_existing
        );
    }

    auto tf = Clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(tf - ti).count();
    std::cout << "Tiempo (copiado): " << ms << " ms\n";
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Uso: flujo <archivo_original> <N>\n";
        return 1;
    }
    std::filesystem::path original = argv[1];
    int N = std::stoi(argv[2]);

    // ----- Fase 1: copiado (aún sin código) -----
    copiarArchivos(original, N);

    std::cout << "Plantilla lista. Compila OK.\n";
    return 0;
}

#include <filesystem>
#include <iostream>
#include <chrono>

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Uso: flujo <archivo_original> <N>\n";
        return 1;
    }
    std::filesystem::path original = argv[1];
    int N = std::stoi(argv[2]);

    // ----- Fase 1: copiado (aún sin código) -----
    // TODO copiarArchivos(original, N);

    std::cout << "Plantilla lista. Compila OK.\n";
    return 0;
}

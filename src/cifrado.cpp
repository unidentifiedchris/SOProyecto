#include "cifrado.hpp"
#include <cctype>

static char cifraChar(char c) {
    if (std::isalpha(static_cast<unsigned char>(c))) {
        char base = std::islower(c) ? 'a' : 'A';
        return char(base + (c - base + 3) % 26);
    }
    if (std::isdigit(static_cast<unsigned char>(c))) {
        return char('9' - (c - '0'));            // simetría numérica
    }
    return c;
}
static char descifraChar(char c) {
    if (std::isalpha(static_cast<unsigned char>(c))) {
        char base = std::islower(c) ? 'a' : 'A';
        return char(base + (c - base + 23) % 26); // −3 ≡ +23 mod 26
    }
    if (std::isdigit(static_cast<unsigned char>(c))) {
        return char('9' - (c - '0'));            // simetría es involutiva
    }
    return c;
}
std::string cifrar(const std::string& in){
    std::string out = in;
    for(char& c: out) c = cifraChar(c);
    return out;
}
std::string descifrar(const std::string& in){
    std::string out = in;
    for(char& c: out) c = descifraChar(c);
    return out;
}

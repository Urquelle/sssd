#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "spanne.hpp"
#include "diagnostik.hpp"
#include "glied.hpp"

namespace Sss::Kmp {

class Lexer
{
public:
    Lexer(std::string quelle, std::string inhalt);

    std::vector<Glied *> starten(int32_t position = 0);
    Diagnostik diagnostik();

private:
    void melden(Spanne spanne, Fehler *fehler);
    Zeichen zeichen(int16_t versatz = 0);
    Zeichen weiter(int32_t anzahl = 1);
    void leerzeichen_einlesen();

    uint32_t _index;
    std::string _quelle;
    std::string _inhalt;
    Diagnostik _diagnostik;
};

}

#pragma once

#include <cstdint>

#include "vm/laufwerk.hpp"

namespace Sss::Vm {

class Hauptspeicher : public Laufwerk
{
public:
    Hauptspeicher(size_t größe);

    std::optional<uint8_t> lesen1(uint16_t adresse);
    bool schreiben1(uint16_t adresse, uint8_t wert);

private:
    size_t _größe;
    uint8_t *_daten;
};

}

#include "vm/hauptspeicher.hpp"

namespace Sss::Vm {

Hauptspeicher::Hauptspeicher(size_t größe)
    : _größe(größe)
{
   _daten = (uint8_t *) calloc(_größe, sizeof(uint8_t));
}

std::optional<uint8_t>
Hauptspeicher::lesen1(uint16_t adresse)
{
    if (adresse < 0 || adresse >= _größe)
    {
        return std::nullopt;
    }

    auto erg = _daten[adresse];

    return erg;
}

bool
Hauptspeicher::schreiben1(uint16_t adresse, uint8_t wert)
{
    if (adresse < 0 || adresse >= _größe)
    {
        return false;
    }

    _daten[adresse] = wert;

    return true;
}

}

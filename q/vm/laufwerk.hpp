#pragma once

#include <optional>

namespace Sss::Vm {

class Laufwerk
{
public:
    virtual std::optional<uint8_t>  lesen1(uint16_t adresse) = 0;
    virtual bool schreiben1(uint16_t adresse, uint8_t wert) = 0;

    std::optional<uint16_t> lesen2(uint16_t adresse)
    {
        auto unter = lesen1(adresse + 1);

        if (!unter.has_value())
        {
            return std::nullopt;
        }

        auto ober = lesen1(adresse);

        if (!ober.has_value())
        {
            return std::nullopt;
        }

        uint16_t erg = (ober.value() << 8) | unter.value();

        return erg;
    }

    bool schreiben2(uint16_t adresse, uint16_t wert)
    {
        // oberes byte
        if (!schreiben1(adresse, wert >> 8))
        {
            return false;
        }

        // unteres byte
        if (!schreiben1(adresse + 1, wert & 0xff))
        {
            return false;
        }

        return true;
    }
};

}

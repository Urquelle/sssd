#pragma once

#include "zeichen.hpp"

class Spanne
{
public:
    Spanne();
    Spanne(const Spanne& s);
    Spanne(const Zeichen& von, const Zeichen& bis);

    Zeichen von() const;
    Zeichen bis() const;
    uint32_t länge() const;
    bool ist_leer() const;

    Spanne& operator=(const Spanne &s);

private:
    void länge_berechnen();

    Zeichen _von;
    Zeichen _bis;
    uint32_t _länge;
};

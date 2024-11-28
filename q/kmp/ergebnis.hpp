#pragma once

#include "fehler.hpp"

template <typename TWert, typename FWert = Fehler *>
class Ergebnis
{
public:
    Ergebnis(TWert wert);
    Ergebnis(FWert fehler);

    TWert wert();
    FWert fehler();

    bool gut();
    bool schlecht();

private:
    bool _gut;

    TWert _wert;
    FWert _fehler;
};

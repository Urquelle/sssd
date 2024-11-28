#include "ergebnis.hpp"

template <typename TWert, typename FWert>
Ergebnis<TWert, FWert>::Ergebnis(TWert wert)
    : _wert(wert), _gut(true)
{
}

template <typename TWert, typename FWert>
Ergebnis<TWert, FWert>::Ergebnis(FWert fehler)
    : _fehler(fehler), _gut(false)
{
}

template <typename TWert, typename FWert>
TWert Ergebnis<TWert, FWert>::wert()
{
    return _wert;
}

template <typename TWert, typename FWert>
FWert Ergebnis<TWert, FWert>::fehler()
{
    return _fehler;
}

template <typename TWert, typename FWert>
bool Ergebnis<TWert, FWert>::gut()
{
    return _gut;
}

template <typename TWert, typename FWert>
bool Ergebnis<TWert, FWert>::schlecht()
{
    return !_gut;
}

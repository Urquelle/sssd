#include "spanne.hpp"

#include <ostream>

Spanne::Spanne()
    : _von()
    , _bis()
    , _länge(0)
{
}

Spanne::Spanne(const Spanne& s)
    : _von(s.von())
    , _bis(s.bis())
{
    länge_berechnen();
}

Spanne::Spanne(const Spanne& von, const Spanne& bis)
    : _von(von.von())
    , _bis(bis.bis())
{
    länge_berechnen();
}

Spanne::Spanne(const Zeichen& von, const Zeichen& bis)
    : _von(von)
    , _bis(bis)
{
    länge_berechnen();
}

void
Spanne::länge_berechnen()
{
    uint32_t länge = 1;
    auto *anfang = _von.utf8().data();

    while (anfang != _bis.utf8().data())
    {
        länge += 1;
        anfang += 1;
    }

    _länge = länge;
}

Spanne&
Spanne::operator=(const Spanne& s)
{
    if (this != &s)
    {
        _von = s._von;
        _bis = s._bis;
    }

    return *this;
}

Zeichen
Spanne::von() const
{
    return _von;
}

Zeichen
Spanne::bis() const
{
    return _bis;
}

uint32_t
Spanne::länge() const
{
    return _länge;
}

bool
Spanne::ist_leer() const
{
    // AUFGABE: implementieren

    return true;
}

std::ostream&
operator<<(std::ostream& ausgabe, const Spanne& spanne)
{
    if (spanne.ist_leer())
    {
        return ausgabe;
    }

    if (spanne.länge() > 0)
    {
        auto daten = spanne.von().utf8().data();

        if (daten != nullptr)
        {
            ausgabe.write(daten, spanne.länge());
        }
        else
        {
            ausgabe.setstate(std::ios_base::failbit);
        }
    }

    return ausgabe;
}

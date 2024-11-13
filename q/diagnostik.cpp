#include "diagnostik.hpp"

#include <iostream>

Diagnostik::Diagnostik()
{
}

bool
Diagnostik::hat_meldungen()
{
    auto erg = _meldungen.size() > 0;

    return erg;
}

void
Diagnostik::melden(Diagnostik::Meldung meldung)
{
    _meldungen.push_back(meldung);
}

void
Diagnostik::melden(Spanne spanne, Fehler *fehler)
{
    _meldungen.push_back({
        .spanne = spanne,
        .fehler = fehler
    });
}

std::vector<Diagnostik::Meldung>
Diagnostik::meldungen()
{
    return _meldungen;
}

std::ostream& operator<<(std::ostream& ausgabe, const Diagnostik::Meldung& m)
{
    ausgabe << m.spanne.von().utf8();
    ausgabe << " fehler: " << m.fehler->text << std::endl;

    return ausgabe;
}

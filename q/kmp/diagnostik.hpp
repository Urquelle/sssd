#pragma once

#include <vector>

#include "fehler.hpp"
#include "spanne.hpp"

class Diagnostik
{
public:
    struct Meldung
    {
        Spanne spanne;
        Fehler * fehler;
    };

    Diagnostik();

    bool hat_meldungen();
    void melden(Meldung meldung);
    void melden(Spanne spanne, Fehler *fehler);
    std::vector<Meldung> meldungen();

private:
    std::vector<Meldung> _meldungen;
};

std::ostream& operator<<(std::ostream& ausgabe, const Diagnostik::Meldung& m);

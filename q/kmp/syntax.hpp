#pragma once

#include <vector>

#include "fehler.hpp"
#include "ergebnis.hpp"
#include "diagnostik.hpp"
#include "glied.hpp"
#include "asb.hpp"

namespace Sss::Kmp {

class Syntax
{
public:
    Syntax(std::vector<Glied *> glieder);

    Asb starten(int32_t index = 0);

    Ergebnis<Ausdruck *> ausdruck_einlesen();
    Ergebnis<Ausdruck *> reihe_ausdruck_einlesen();
    Ergebnis<Ausdruck *> bitschieben_ausdruck_einlesen();
    Ergebnis<Ausdruck *> vergleich_ausdruck_einlesen();
    Ergebnis<Ausdruck *> add_ausdruck_einlesen();
    Ergebnis<Ausdruck *> mult_ausdruck_einlesen();
    Ergebnis<Ausdruck *> aufruf_ausdruck_einlesen();
    Ergebnis<Ausdruck *> eigenschaft_ausdruck_einlesen();
    Ergebnis<Ausdruck *> index_ausdruck_einlesen();
    Ergebnis<Ausdruck *> basis_ausdruck_einlesen();

    Spezifizierung * spezifizierung_einlesen();

    Deklaration * deklaration_einlesen(bool mit_abschluss = true);

    Ergebnis<Anweisung *> anweisung_einlesen();
    Anweisung * block_anweisung_einlesen();
    Anweisung * wenn_anweisung_einlesen();
    Anweisung * für_anweisung_einlesen();
    Anweisung * solange_anweisung_einlesen();
    Anweisung * weiche_anweisung_einlesen();
    Anweisung * deklaration_anweisung_einlesen();
    Anweisung * final_anweisung_einlesen();
    Anweisung * res_anweisung_einlesen();
    Anweisung * weiter_anweisung_einlesen();
    Anweisung * sprung_anweisung_einlesen();
    Anweisung * raus_anweisung_einlesen();
    Anweisung * markierung_anweisung_einlesen();

    std::vector<Marke *> marken_einlesen();

    Glied * glied(int32_t versatz = 0) const;
    Glied * weiter(int32_t anzahl = 1);
    Glied * erwarte(Glied::Art art);
    bool    passt(Glied::Art art);
    bool    ist(Glied::Art art, int32_t versatz = 0);
    bool    dateiende();

    Diagnostik diagnostik() const;
    void melden(Spanne spanne, Fehler *fehler);
    void melden(Anweisung * anweisung, Fehler *fehler);
    void melden(Ausdruck * ausdruck, Fehler *fehler);
    void melden(Glied * glied, Fehler *fehler);

private:
    Diagnostik _diagnostik;
    std::vector<Glied *> _glieder;
    int32_t _glied_index;
};

}

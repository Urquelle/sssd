#pragma once

#include <map>

#include "kmp/bestimmter_asb.hpp"
#include "vm/hauptspeicher.hpp"

namespace Sss::Kmp
{

class Interpret
{
public:
    Interpret();

    void starten(Bestimmte_Deklaration_Funktion *hauptmethode);

    void anweisung_kodieren(Bestimmte_Anweisung *anweisung);

private:
    int _adresse;
    std::map<std::string, int> _markierungen;
    Vm::Hauptspeicher *_speicher;
};

}

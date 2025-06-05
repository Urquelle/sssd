#include "kmp/interpret.hpp"

#include <cassert>

#include "kmp/bestimmter_asb.hpp"
#include "vm/anweisung.hpp"
#include "vm/operand.hpp"

namespace Sss::Kmp
{

Interpret::Interpret()
    : _adresse(0)
{
}

void
Interpret::starten(Bestimmte_Deklaration_Funktion *hauptmethode)
{
    _speicher = new Vm::Hauptspeicher(256*256);

    anweisung_kodieren(hauptmethode->rumpf());
}

void
Interpret::anweisung_kodieren(Bestimmte_Anweisung *anweisung)
{
    switch (anweisung->art())
    {
        case Asb_Knoten::ANWEISUNG_BLOCK:
        {
            for (auto *a : anweisung->als<Bestimmte_Anweisung_Block *>()->anweisungen())
            {
                anweisung_kodieren(a);
            }
        } break;

        case Asb_Knoten::ANWEISUNG_MARKIERUNG:
        {
            _markierungen[anweisung->als<Bestimmte_Anweisung_Markierung *>()->markierung()] = _adresse;
        } break;

        case Asb_Knoten::ANWEISUNG_SPRUNG:
        {
            auto adresse = _markierungen[anweisung->als<Bestimmte_Anweisung_Sprung *>()->markierung()];
            auto a = Vm::Anweisung_Jmp(new Vm::Operand_Adr(adresse));
            a.kodieren(_speicher, {}, _adresse);
        } break;

        case Asb_Knoten::ANWEISUNG_BEDINGTER_SPRUNG:
        {
            auto *sprung = anweisung->als<Bestimmte_Anweisung_Bedingter_Sprung *>();

            auto adresse = _markierungen[sprung->markierung()];
        } break;

        default:
        {
            assert(!"unbekannte anweisung");
        } break;
    }
}

}

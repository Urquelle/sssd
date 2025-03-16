#pragma once

#include "kmp/bestimmter_asb.hpp"

namespace Sss::Kmp
{

class Transformator
{
public:
    virtual Bestimmte_Anweisung   * anweisung_transformieren(Bestimmte_Anweisung *anweisung);
    virtual Bestimmte_Anweisung   * für_anweisung_transformieren(Bestimmte_Anweisung_Für *anweisung);
    virtual Bestimmte_Anweisung   * solange_anweisung_transformieren(Bestimmte_Anweisung_Solange *anweisung);
    virtual Bestimmte_Anweisung   * wenn_anweisung_transformieren(Bestimmte_Anweisung_Wenn *anweisung);
    virtual Bestimmte_Anweisung   * bedingter_sprung_anweisung_transformieren(Bestimmte_Anweisung_Bedingter_Sprung *anweisung);
    virtual Bestimmte_Anweisung   * block_anweisung_transformieren(Bestimmte_Anweisung_Block *anweisung);
    virtual Bestimmte_Anweisung   * deklaration_anweisung_transformieren(Bestimmte_Anweisung_Deklaration *anweisung);

    virtual Bestimmter_Ausdruck   * ausdruck_transformieren(Bestimmter_Ausdruck *ausdruck);

    virtual Bestimmte_Deklaration * deklaration_transformieren(Bestimmte_Deklaration *deklaration);
    virtual Bestimmte_Deklaration * funktion_deklaration_transformieren(Bestimmte_Deklaration_Funktion *deklaration);
    virtual Bestimmte_Deklaration * variable_deklaration_transformieren(Bestimmte_Deklaration *deklaration);

    static Bestimmte_Anweisung * Block(Asb_Knoten *asb_knoten, std::vector<Bestimmte_Anweisung *> rumpf);
    static Bestimmte_Anweisung * Variable_Deklaration(Asb_Knoten *asb_knoten, Symbol *symbol, Bestimmter_Ausdruck *ausdruck);
    static Bestimmte_Anweisung * Konstante_Deklaration(Asb_Knoten *asb_knoten, std::string name, Bestimmter_Ausdruck *ausdruck);
    static Bestimmter_Ausdruck * Variable(Asb_Knoten *asb_knoten, Bestimmter_Ausdruck *ausdruck);
    static Bestimmte_Anweisung * Solange(Asb_Knoten *asb_knoten, Bestimmter_Ausdruck *bedingung, Bestimmte_Anweisung *rumpf, std::string weiter_markierung, std::string raus_markierung);
    static Bestimmter_Ausdruck * Kleiner_Gleich(Asb_Knoten *asb_knoten, Bestimmter_Ausdruck *links, Bestimmter_Ausdruck *rechts);
    static Bestimmte_Anweisung * Markierung(Asb_Knoten *asb_knoten, std::string markierung);
    static Bestimmte_Anweisung * Hochzählen(Asb_Knoten *asb_knoten, Bestimmter_Ausdruck *ausdruck);
    static Bestimmte_Anweisung * Sprung(Asb_Knoten *asb_knoten, std::string markierung);
    static Bestimmte_Anweisung * Sprung_Wahr(Asb_Knoten *asb_knoten, std::string markierung, Bestimmter_Ausdruck *bedingung);
    static Bestimmte_Anweisung * Sprung_Unwahr(Asb_Knoten *asb_knoten, std::string markierung, Bestimmter_Ausdruck *bedingung);
};

}

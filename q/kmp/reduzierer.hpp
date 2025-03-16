#pragma once

#include "kmp/transformator.hpp"

namespace Sss::Kmp
{

class Reduzierer : public Transformator
{
public:
    Bestimmte_Anweisung * für_anweisung_transformieren(Bestimmte_Anweisung_Für *anweisung) override;
    Bestimmte_Anweisung * solange_anweisung_transformieren(Bestimmte_Anweisung_Solange *anweisung) override;
    Bestimmte_Anweisung * wenn_anweisung_transformieren(Bestimmte_Anweisung_Wenn *anweisung) override;
    Bestimmte_Anweisung * bedingter_sprung_anweisung_transformieren(Bestimmte_Anweisung_Bedingter_Sprung *anweisung) override;
    /*Bestimmte_Anweisung * block_transformieren(Bestimmte_Anweisung_Block *anweisung);*/
    /*Bestimmter_Ausdruck * transformieren(Bestimmter_Ausdruck *ausdruck) override;*/
};

}

#pragma once

#include "kmp/datentyp.hpp"
#include "kmp/symbol.hpp"

#include <variant>

namespace Sss::Kmp {

#define Operand_Merkmal_Liste \
    X(LITERAL, 1, "Literal") \
    X(ARITHMETISCH, 2, "Arithmetisch")

class Operand
{
public:
    enum Merkmal
    {
    #define X(N, W, ...) N = W,
        Operand_Merkmal_Liste
    #undef X
    };

    typedef std::variant<int, float, bool, std::string> Wert;

    Operand(Datentyp *datentyp, uint32_t merkmale = 0);
    Operand(Symbol *symbol, uint32_t merkmale = 0);

    Datentyp * datentyp() const;

    Symbol   * symbol() const;
    void       symbol_setzen(Symbol *symbol);

    uint32_t   merkmale() const;
    void       merkmale_setzen(uint32_t merkmale);
    bool       merkmal_gesetzt(Merkmal merkmal) const;
    void       merkmal_setzen(Merkmal merkmal);
    void       merkmal_löschen(Merkmal merkmal);

    void       wert_setzen(Wert *wert);
    Wert     * wert() const;

    bool       ist_arithmetisch() const;

private:
    Datentyp *_datentyp;
    Symbol *_symbol;
    uint32_t _merkmale;
    Wert *_wert;
};

}

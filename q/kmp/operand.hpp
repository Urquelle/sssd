#pragma once

#include "datentyp.hpp"
#include "symbol.hpp"

namespace Sss::Kmp {

#define Operand_Merkmal_Liste \
    X(LITERAL, 1, "Literal")

class Operand
{
public:
    enum Merkmal
    {
    #define X(N, W, ...) N = W,
        Operand_Merkmal_Liste
    #undef X
    };

    Operand(Datentyp *datentyp, uint32_t merkmale = 0);
    Operand(Symbol *symbol, uint32_t merkmale = 0);

    Datentyp *datentyp() const;

    Symbol *symbol() const;
    void symbol_setzen(Symbol *symbol);

    uint32_t merkmale() const;
    void merkmale_setzen(uint32_t merkmale);
    bool merkmal_gesetzt(Merkmal merkmal) const;
    void merkmal_setzen(Merkmal merkmal);
    void merkmal_löschen(Merkmal merkmal);

private:
    Datentyp *_datentyp;
    Symbol *_symbol;
    uint32_t _merkmale;
};

}

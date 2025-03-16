#include "operand.hpp"

namespace Sss::Kmp {

Operand::Operand(Datentyp *datentyp, uint32_t merkmale)
    : _datentyp(datentyp)
    , _merkmale(merkmale)
    , _symbol(nullptr)
    , _wert(nullptr)
{
}

Operand::Operand(Symbol *symbol, uint32_t merkmale)
    : _datentyp(nullptr)
    , _symbol(symbol)
    , _merkmale(merkmale)
    , _wert(nullptr)
{
    if (symbol != nullptr)
    {
        _datentyp = symbol->datentyp();
    }
}

Datentyp *
Operand::datentyp() const
{
    return _datentyp;
}

Symbol *
Operand::symbol() const
{
    return _symbol;
}

void
Operand::symbol_setzen(Symbol *symbol)
{
    _symbol = symbol;
}

uint32_t
Operand::merkmale() const
{
    return _merkmale;
}

void
Operand::merkmale_setzen(uint32_t merkmale)
{
    _merkmale = merkmale;
}

bool
Operand::merkmal_gesetzt(Merkmal merkmal) const
{
    auto erg = (_merkmale & merkmal) > 0;

    return erg;
}

void
Operand::merkmal_setzen(Merkmal merkmal)
{
    _merkmale = _merkmale | merkmal;
}

void
Operand::merkmal_löschen(Merkmal merkmal)
{
    _merkmale = _merkmale & ~merkmal;
}

void
Operand::wert_setzen(Wert *wert)
{
    _wert = wert;
}

Operand::Wert *
Operand::wert() const
{
    return _wert;
}

bool
Operand::ist_arithmetisch() const
{
    bool erg = merkmal_gesetzt(Operand::ARITHMETISCH);

    return erg;
}

}

#include "kmp/symbol.hpp"
#include "kmp/datentyp.hpp"
#include "kmp/zone.hpp"

namespace Sss::Kmp {

Symbol::Symbol(Spanne spanne, Symbol::Art art, Symbol::Status status, std::string name, Datentyp *datentyp)
    : _art(art)
    , _status(status)
    , _name(name)
    , _datentyp(datentyp)
    , _zone(new Zone())
    , _spanne(spanne)
{
}

Symbol::Symbol(Spanne spanne, Symbol::Art art, Symbol::Status status, std::string name, Zone *zone)
    : _art(art)
    , _status(status)
    , _name(name)
    , _datentyp(nullptr)
    , _zone(zone)
    , _spanne(spanne)
{
}

Symbol::Symbol(Spanne spanne, Symbol::Art art, Symbol::Status status, std::string name)
    : Symbol(spanne, art, status, name, (Datentyp *) nullptr)
{
}

Symbol::Art
Symbol::art() const
{
    return _art;
}

Symbol::Status
Symbol::status() const
{
    return _status;
}

Spanne
Symbol::spanne() const
{
    return _spanne;
}

void
Symbol::status_setzen(Symbol::Status status)
{
    _status = status;
}

std::string
Symbol::name() const
{
    return _name;
}

Zone *
Symbol::zone() const
{
    return _zone;
}

Datentyp *
Symbol::datentyp() const
{
    return _datentyp;
}

void
Symbol::datentyp_setzen(Datentyp *datentyp)
{
    _datentyp = datentyp;
}

}

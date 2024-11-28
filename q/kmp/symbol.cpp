#include "kmp/symbol.hpp"
#include "kmp/zone.hpp"

namespace Sss::Kmp {

Symbol::Symbol(Symbol::Art art, Symbol::Status status, std::string name, Datentyp *datentyp)
    : _art(art)
    , _status(status)
    , _name(name)
    , _datentyp(datentyp)
    , _zone(new Zone())
{
}

Symbol::Symbol(Symbol::Art art, Symbol::Status status, std::string name, Zone *zone)
    : _art(art)
    , _status(status)
    , _name(name)
    , _datentyp(nullptr)
    , _zone(zone)
{
}

Symbol::Symbol(Symbol::Art art, Symbol::Status status, std::string name)
    : Symbol(art, status, name, (Datentyp *) nullptr)
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

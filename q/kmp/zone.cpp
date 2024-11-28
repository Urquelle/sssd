#include "zone.hpp"

namespace Sss::Kmp {

Zone::Zone()
    : _über(nullptr)
    , _name()
{
}

Zone::Zone(std::string name, Zone *über)
    : _über(über)
    , _name(name)
{
}

std::map<std::string, Symbol *>
Zone::symbole() const
{
    return _symbole;
}

std::string
Zone::name() const
{
    return _name;
}

Zone *
Zone::über() const
{
    return _über;
}

void
Zone::über_setzen(Zone *zone)
{
    _über = zone;
}

bool
Zone::registrieren(Symbol *symbol)
{
    auto erg = registrieren(symbol->name(), symbol);

    return erg;
}

bool
Zone::registrieren(std::string name, Symbol *symbol)
{
    if (ist_registriert(name))
    {
        return false;
    }

    _symbole[name] = symbol;

    return true;
}

bool
Zone::ist_registriert(std::string name)
{
    Symbol *sym = _symbole[name];

    bool erg = sym != nullptr;

    return erg;
}

Symbol *
Zone::suchen(std::string name, bool über_suche)
{
    Zone *z = this;

    while (z != nullptr)
    {
        Symbol *sym = z->_symbole[name];

        if (sym != nullptr)
        {
            return sym;
        }

        z = z->über();
    }

    return nullptr;
}

}

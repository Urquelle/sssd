#include "zone.hpp"

#include "kmp/datentyp.hpp"

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

bool
Zone::registrieren_funktion(Symbol *symbol)
{
    auto* neue_funktion = symbol->datentyp()->als<Datentyp_Funktion *>();
    auto& überladungen = _überladungen[symbol->name()];

    // Prüfen ob eine identische Signatur bereits existiert
    for (auto* existierend : überladungen)
    {
        auto* existierende_funktion =
            existierend->datentyp()->als<Datentyp_Funktion *>();

        if (sind_signaturen_gleich(neue_funktion, existierende_funktion))
        {
            return false;  // Identische Signatur bereits vorhanden
        }
    }

    überladungen.push_back(symbol);

    return true;
}

bool
Zone::sind_signaturen_gleich(Datentyp_Funktion* f1, Datentyp_Funktion* f2)
{
    if (f1->parameter().size() != f2->parameter().size())
    {
        return false;
    }

    for (size_t i = 0; i < f1->parameter().size(); ++i)
    {
        if (!Datentyp::datentypen_kompatibel(f1->parameter()[i], f2->parameter()[i]))
        {
            return false;
        }
    }

    return true;
}

}

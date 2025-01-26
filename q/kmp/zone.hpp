#pragma once

#include <string>
#include <map>
#include <vector>

#include "kmp/symbol.hpp"

namespace Sss::Kmp {

class Datentyp_Funktion;

class Zone
{
public:
    Zone();
    Zone(std::string name, Zone *über = nullptr);

    std::map<std::string, Symbol *> symbole() const;
    std::string name() const;

    Zone * über() const;
    void über_setzen(Zone *zone);

    bool registrieren(Symbol *symbol);
    bool registrieren(std::string name, Symbol *symbol);
    bool ist_registriert(std::string name);
    Symbol * suchen(std::string name, bool über_suche = true);

private:
    bool registrieren_funktion(Symbol *symbol);

    std::map<std::string, std::vector<Symbol *>> _überladungen;
    std::map<std::string, Symbol *> _symbole;

    bool sind_signaturen_gleich(Datentyp_Funktion *f1, Datentyp_Funktion *f2);

    std::string _name;
    Zone *_über;
};

}

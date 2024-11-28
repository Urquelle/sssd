#pragma once

#include <string>
#include <map>

#include "kmp/symbol.hpp"

namespace Sss::Kmp {

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
    std::map<std::string, Symbol *> _symbole;
    std::string _name;
    Zone *_über;
};

}

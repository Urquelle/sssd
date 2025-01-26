#pragma once

#include <string>

#include "kmp/spanne.hpp"

namespace Sss::Kmp {

class Datentyp;
class Zone;

#define Symbol_Art_Liste \
    X(VARIABLE,  1, "Variable") \
    X(FUNKTION,  2, "Funktion") \
    X(DATENTYP,  3, "Datentyp") \
    X(MODUL,     4, "Modul")

#define Symbol_Status_Liste \
    X(UNBEHANDELT,   0, "Unbehandelt") \
    X(IN_BEHANDLUNG, 1, "In Behandlung") \
    X(BEHANDELT,     2, "Behandelt")

class Symbol
{
public:
    enum Art
    {
    #define X(N, W, ...) N = W,
        Symbol_Art_Liste
    #undef X
    };

    enum Status
    {
    #define X(N, W, ...) N = W,
        Symbol_Status_Liste
    #undef X
    };

    Symbol(Spanne spanne, Art art, Status status, std::string name, Datentyp *datentyp);
    Symbol(Spanne spanne, Art art, Status status, std::string name);
    Symbol(Spanne spanne, Art art, Status status, std::string name, Zone *zone);

    Art art() const;
    Status status() const;
    void status_setzen(Status status);
    std::string name() const;
    Spanne spanne() const;

    Zone *zone() const;

    Datentyp *datentyp() const;
    void datentyp_setzen(Datentyp *datentyp);

private:
    Art _art;
    Status _status;
    std::string _name;
    Datentyp *_datentyp;
    Zone *_zone;
    Spanne _spanne;
};

}

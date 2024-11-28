#include "datentyp.hpp"

namespace Sss::Kmp {

Datentyp::Datentyp(Datentyp::Art art, size_t größe)
    : _art(art)
    , _größe(größe)
{
}

Datentyp::Art
Datentyp::art() const
{
    return _art;
}

Datentyp::Status
Datentyp::status() const
{
    return _status;
}

bool
Datentyp::abgeschlossen() const
{
    return _abgeschlossen;
}

void
Datentyp::abschließen()
{
    _abgeschlossen = true;
}

size_t
Datentyp::größe() const
{
    return _größe;
}

void
Datentyp::größe_setzen(size_t größe)
{
    _größe = größe;
}

template<typename T>
T Datentyp::als()
{
    return static_cast<T> (this);
}

void
Datentyp::ausgeben(int32_t tiefe, std::ostream &ausgabe)
{
    switch (_art)
    {
    #define X(N, W, B) case (N): ausgabe << std::string(tiefe*4, ' ') << B; break;
        Datentyp_Art_Liste
    #undef X
    }
}

Datentyp_Feld::Datentyp_Feld(Datentyp *basis, Datentyp *index)
    : Datentyp(Datentyp::FELD, 8)
    , _basis(basis)
    , _index(index)
{
}

Datentyp *
Datentyp_Feld::basis() const
{
    return _basis;
}

Datentyp *
Datentyp_Feld::index() const
{
    return _index;
}

Datentyp_Zeiger::Datentyp_Zeiger(Datentyp *basis)
    : Datentyp(Datentyp::ZEIGER, 8)
    , _basis(basis)
{
}

Datentyp *
Datentyp_Zeiger::basis() const
{
    return _basis;
}

Datentyp_Schablone::Datentyp_Schablone(Deklaration_Schablone *deklaration)
    : Datentyp(Datentyp::SCHABLONE, 0)
    , _deklaration(deklaration)
{
}

Deklaration_Schablone *
Datentyp_Schablone::deklaration() const
{
    return _deklaration;
}

std::vector<Datentyp_Schablone::Eigenschaft *>
Datentyp_Schablone::eigenschaften() const
{
    return _eigenschaften;
}

void
Datentyp_Schablone::eigenschaft_hinzufügen(Datentyp *datentyp, size_t versatz, std::string name)
{
    auto *eigenschaft = new Datentyp_Schablone::Eigenschaft(datentyp, versatz, name);
    _eigenschaften.push_back(eigenschaft);
}

Datentyp_Schablone::Eigenschaft::Eigenschaft(Datentyp *datentyp, size_t versatz, std::string name)
    : _datentyp(datentyp)
    , _versatz(versatz)
    , _name(name)
{
}

Datentyp *
Datentyp_Schablone::Eigenschaft::datentyp() const
{
    return _datentyp;
}

size_t
Datentyp_Schablone::Eigenschaft::versatz() const
{
    return _versatz;
}

std::string
Datentyp_Schablone::Eigenschaft::name() const
{
    return _name;
}

Datentyp_Funktion::Datentyp_Funktion()
    : Datentyp(Datentyp::FUNKTION, 8)
    , _parameter()
    , _rückgabe(nullptr)
{
}

Datentyp_Funktion::Datentyp_Funktion(std::vector<Datentyp *> parameter, Datentyp *rückgabe)
    : Datentyp(Datentyp::FUNKTION, 8)
    , _parameter(parameter)
    , _rückgabe(rückgabe)
{
}

std::vector<Datentyp *>
Datentyp_Funktion::parameter() const
{
    return _parameter;
}

void
Datentyp_Funktion::parameter_hinzufügen(Datentyp *datentyp)
{
    _parameter.push_back(datentyp);
}

void
Datentyp_Funktion::rückgabe_setzen(Datentyp *datentyp)
{
    _rückgabe = datentyp;
}

Datentyp *
Datentyp_Funktion::rückgabe() const
{
    return _rückgabe;
}

Datentyp_Opt::Datentyp_Opt(Datentyp *basis)
    : Datentyp(Datentyp::OPT, 8)
    , _basis(basis)
{
}

}

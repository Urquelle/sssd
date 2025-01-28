#include "datentyp.hpp"

#include <iostream>
#include <cassert>

#include "kmp/symbol.hpp"
#include "kmp/zone.hpp"

namespace Sss::Kmp {

Datentyp::Kompatibilität
Datentyp::datentypen_kompatibel(Datentyp *ziel, Datentyp *quelle, bool implizit)
{
    // INFO: wenn einer davon null ist => ungültig
    if (ziel == nullptr || quelle == nullptr)
    {
        return Datentyp::INKOMPATIBEL;
    }

    // INFO: wenn beide vom gleichen typ sind => dann kompatibel
    if (ziel == quelle)
    {
        return Datentyp::KOMPATIBEL;
    }

    // INFO: wenn die art gleich ist und der rechte datentyp in den linken
    //       reinpasst => dann kompatibel
    if (ziel->art() == quelle->art() && ziel->größe() >= quelle->größe())
    {
        return Datentyp::KOMPATIBEL;
    }

    switch (ziel->art())
    {
        case Datentyp::DEZIMAL_ZAHL:
        {
            // Implizite Konvertierung von ganzen Zahlen zu Dezimalzahlen erlauben
            if (quelle->art() == Datentyp::GANZE_ZAHL)
            {
                return Datentyp::KOMPATIBEL;
            }
        } break;

        case Datentyp::GANZE_ZAHL:
        {
            if (quelle->art() == Datentyp::GANZE_ZAHL)
            {
                // INFO: Kleinere ganze Zahlen in größere konvertieren
                return ziel->größe() >= quelle->größe() ? Datentyp::KOMPATIBEL : Datentyp::INKOMPATIBEL;
            }

            // INFO: Von bool nach ganzer Zahl nur wenn implizit erlaubt
            if (quelle->art() == Datentyp::BOOL && implizit)
            {
                return Datentyp::KOMPATIBEL;
            }

            if (quelle->art() == Datentyp::DEZIMAL_ZAHL && implizit)
            {
                std::cout << "Bei einer Umwandlung einer Dezimalzahl in eine Ganzzahl gehen Daten verloren!" << std::endl;

                return Datentyp::KOMPATIBEL_MIT_DATENVERLUST;
            }
        } break;

        case Datentyp::BOOL:
        {
            // INFO: Ganze Zahlen nach bool nur wenn implizit erlaubt
            if (quelle->art() == Datentyp::GANZE_ZAHL && implizit)
            {
                return Datentyp::KOMPATIBEL;
            }

        } break;

        case Datentyp::ZEIGER:
        {
            if (quelle->art() == Datentyp::ZEIGER)
            {
                auto *ziel_basis = ziel->als<Datentyp_Zeiger*>()->basis();
                auto *quelle_basis = quelle->als<Datentyp_Zeiger*>()->basis();

                // Nullzeiger erlauben
                if (quelle_basis->art() == Datentyp::NIHIL)
                {
                    return Datentyp::KOMPATIBEL;
                }

                // Zeiger-Hierarchie prüfen
                return datentypen_kompatibel(ziel_basis, quelle_basis, false);
            }
        } break;

        case Datentyp::SCHABLONE:
        {
            if (quelle->art() != Datentyp::SCHABLONE)
            {
                return Datentyp::INKOMPATIBEL;
            }

            if (quelle->symbol() == ziel->symbol())
            {
                return Datentyp::KOMPATIBEL;
            }

            for (auto *eigenschaft : quelle->als<Datentyp_Schablone *>()->eigenschaften())
            {
                if (!ziel->symbol()->zone()->suchen(eigenschaft->name()))
                {
                    return Datentyp::INKOMPATIBEL;
                }
            }
        } break;
    }

    return Datentyp::INKOMPATIBEL;
}

Datentyp::Datentyp(Datentyp::Art art, size_t größe, bool abgeschlossen, uint32_t merkmale)
    : _art(art)
    , _größe(größe)
    , _symbol(nullptr)
    , _deklaration(nullptr)
    , _abgeschlossen(abgeschlossen)
    , _merkmale(merkmale)
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

void
Datentyp::status_setzen(Datentyp::Status status)
{
    _status = status;
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

bool
Datentyp::ist_arithmetisch() const
{
    auto erg = (_merkmale & Datentyp::ARITHMETISCH) == Datentyp::ARITHMETISCH;

    return erg;
}

Symbol *
Datentyp::symbol() const
{
    return _symbol;
}

void
Datentyp::symbol_setzen(Symbol *symbol)
{
    _symbol = symbol;
}

Deklaration *
Datentyp::deklaration() const
{
    return _deklaration;
}

void
Datentyp::deklaration_setzen(Deklaration *deklaration)
{
    _deklaration = deklaration;
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

bool
Datentyp_Schablone::eigenschaft_hinzufügen(Datentyp *datentyp, size_t versatz, std::string name)
{
    auto *eigenschaft = new Datentyp_Schablone::Eigenschaft(datentyp, versatz, name);

    for (auto *e : _eigenschaften)
    {
        if (e->name() == name)
        {
            return false;
        }
    }
    _eigenschaften.push_back(eigenschaft);

    return true;
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

Datentyp_Anon::Eigenschaft::Eigenschaft(Datentyp *datentyp, size_t versatz, std::string name)
    : _datentyp(datentyp)
    , _versatz(versatz)
    , _name(name)
{
}

Datentyp_Option::Datentyp_Option(Datentyp *basis)
    : Datentyp(Datentyp::OPTION, 8)
    , _basis(basis)
{
}

Datentyp_Option::Eigenschaft::Eigenschaft(Datentyp *datentyp, Operand *operand, std::string& name)
    : _datentyp(datentyp)
    , _operand(operand)
    , _name(std::move(name))
{
}

void
Datentyp_Option::eigenschaft_hinzufügen(Datentyp *datentyp, Operand *operand, std::string& name)
{
    Datentyp_Option::Eigenschaft *eigenschaft = new Datentyp_Option::Eigenschaft(datentyp, operand, name);

    _eigenschaften.push_back(eigenschaft);
}

}

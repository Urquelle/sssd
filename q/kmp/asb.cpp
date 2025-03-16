#include "asb.hpp"

#include <ostream>
#include <format>

#include "kmp/datentyp.hpp"

namespace Sss::Kmp {

int32_t global_id = 0;

// asb_knoten {{{
Asb_Knoten::Asb_Knoten(Asb_Knoten::Art art, Spanne spanne)
    : _art(art)
    , _spanne(spanne)
    , _id(++global_id)
{
}

Asb_Knoten::Art
Asb_Knoten::art() const
{
    return _art;
}

int32_t
Asb_Knoten::id() const
{
    return _id;
}

Spanne
Asb_Knoten::spanne() const
{
    return _spanne;
}

template<typename T>
T Asb_Knoten::als()
{
    return static_cast<T> (this);
}

void
Asb_Knoten::ausgeben(int32_t tiefe, std::ostream& ausgabe)
{
    switch (_art)
    {
    #define X(N, W, B) case Asb_Knoten::N: ausgabe << std::string(tiefe*4, ' ') << std::string(B); break;
        Asb_Knoten_Art
    #undef X
    }
}
// }}}
// muster {{{
Muster::Muster(Spanne spanne, Ausdruck *muster, Anweisung *anweisung)
    : _spanne(spanne)
    , _muster(muster)
    , _anweisung(anweisung)
{
}

Spanne
Muster::spanne() const
{
    return _spanne;
}

Ausdruck *
Muster::muster() const
{
    return _muster;
}

Anweisung *
Muster::anweisung() const
{
    return _anweisung;
}
// }}}
// ausdruck {{{
Ausdruck::Ausdruck(Asb_Knoten::Art art, Spanne spanne)
    : Asb_Knoten(art, spanne)
{
}

Ausdruck_Ganzzahl::Ausdruck_Ganzzahl(Spanne spanne, int32_t wert)
    : Ausdruck(Asb_Knoten::AUSDRUCK_GANZZAHL, spanne)
    , _wert(wert)
{
}

int32_t
Ausdruck_Ganzzahl::wert() const
{
    return _wert;
}

void
Ausdruck_Ganzzahl::ausgeben(int32_t tiefe, std::ostream &ausgabe)
{
    ausgabe << std::string(tiefe*4, ' ') << std::format("Ganzzahl ({})", _wert);
}

Ausdruck_Dezimalzahl::Ausdruck_Dezimalzahl(Spanne spanne, float wert)
    : Ausdruck(Asb_Knoten::AUSDRUCK_DEZIMALZAHL, spanne)
    , _wert(wert)
{
}

float
Ausdruck_Dezimalzahl::wert() const
{
    return _wert;
}

void
Ausdruck_Dezimalzahl::ausgeben(int32_t tiefe, std::ostream &ausgabe)
{
    ausgabe << std::string(tiefe*4, ' ') << std::format("Dezimalzahl ({})", _wert);
}

Ausdruck_Bezeichner::Ausdruck_Bezeichner(Spanne spanne, std::string wert)
    : Ausdruck(Asb_Knoten::AUSDRUCK_BEZEICHNER, spanne)
    , _wert(wert)
{
}

std::string
Ausdruck_Bezeichner::wert() const
{
    return _wert;
}

void
Ausdruck_Bezeichner::ausgeben(int32_t tiefe, std::ostream &ausgabe)
{
    ausgabe << std::string(tiefe*4, ' ') << std::format("Bezeichner ({})", _wert);
}

Ausdruck_Text::Ausdruck_Text(Spanne spanne, std::string wert)
    : Ausdruck(Ausdruck::AUSDRUCK_TEXT, spanne)
    , _wert(wert)
{
}

std::string
Ausdruck_Text::wert() const
{
    return _wert;
}

void
Ausdruck_Text::ausgeben(int32_t tiefe, std::ostream &ausgabe)
{
    ausgabe << std::string(tiefe*4, ' ') << std::format("Text ({})", _wert);
}

Ausdruck_Binär::Ausdruck_Binär(Spanne spanne, Ausdruck_Binär::Binär_Op op, Ausdruck *links, Ausdruck *rechts)
    : Ausdruck(Asb_Knoten::AUSDRUCK_BINÄR, spanne)
    , _op(op)
    , _links(links)
    , _rechts(rechts)
{
}

Ausdruck_Binär::Binär_Op
Ausdruck_Binär::op() const
{
    return _op;
}

Ausdruck *
Ausdruck_Binär::links() const
{
    return _links;
}

Ausdruck *
Ausdruck_Binär::rechts() const
{
    return _rechts;
}

void
Ausdruck_Binär::ausgeben(int32_t tiefe, std::ostream &ausgabe)
{
    switch (_op)
    {
    #define X(N, W, B) case N: ausgabe << std::string(tiefe*4, ' ') << std::format("Binär ({})", B); break;
        Bin_Op_Liste
    #undef X
    }

    ausgabe.put('\n');
    _links->ausgeben(tiefe+1, ausgabe);
    ausgabe.put('\n');
    _rechts->ausgeben(tiefe+1, ausgabe);
}

Ausdruck_Unär::Ausdruck_Unär(Spanne spanne, Ausdruck_Unär::Unär_Op op, Ausdruck *ausdruck)
    : Ausdruck(Asb_Knoten::AUSDRUCK_UNÄR, spanne)
    , _op(op)
    , _ausdruck(ausdruck)
{
}

Ausdruck_Unär::Unär_Op
Ausdruck_Unär::op() const
{
    return _op;
}

Ausdruck *
Ausdruck_Unär::ausdruck() const
{
    return _ausdruck;
}

void
Ausdruck_Unär::ausgeben(int32_t tiefe, std::ostream &ausgabe)
{
    switch (_op)
    {
    #define X(N, W, B) case N: ausgabe << std::string(tiefe*4, ' ') << std::format("Unär ({})", B); break;
        Unär_Op_Liste
    #undef X
    }

    ausgabe.put('\n');
    _ausdruck->ausgeben(tiefe+1, ausgabe);
}

Ausdruck_Klammer::Ausdruck_Klammer(Spanne spanne, Ausdruck *ausdruck)
    : Ausdruck(Asb_Knoten::AUSDRUCK_KLAMMER, spanne)
    , _ausdruck(ausdruck)
{
}

Ausdruck *
Ausdruck_Klammer::ausdruck() const
{
    return _ausdruck;
}

void
Ausdruck_Klammer::ausgeben(int32_t tiefe, std::ostream &ausgabe)
{
    Asb_Knoten::ausgeben(tiefe, ausgabe);
    ausgabe.put('\n');
    _ausdruck->ausgeben(tiefe+1, ausgabe);
}

Ausdruck_Index::Ausdruck_Index(Spanne spanne, Ausdruck *basis, Ausdruck *index)
    : Ausdruck(Asb_Knoten::AUSDRUCK_INDEX, spanne)
    , _basis(basis)
    , _index(index)
{
}

Ausdruck *
Ausdruck_Index::basis() const
{
    return _basis;
}

Ausdruck *
Ausdruck_Index::index() const
{
    return _index;
}

void
Ausdruck_Index::ausgeben(int32_t tiefe, std::ostream &ausgabe)
{
    ausgabe << std::string(tiefe*4, ' ') << "Index";
    ausgabe.put('\n');
    _basis->ausgeben(tiefe+1, ausgabe);
    ausgabe.put('\n');
    _index->ausgeben(tiefe+1, ausgabe);
}

Ausdruck_Eigenschaft::Ausdruck_Eigenschaft(Spanne spanne, Ausdruck *basis, Ausdruck *eigenschaft)
    : Ausdruck(Asb_Knoten::AUSDRUCK_EIGENSCHAFT, spanne)
    , _basis(basis)
    , _eigenschaft(eigenschaft)
{
}

Ausdruck *
Ausdruck_Eigenschaft::basis() const
{
    return _basis;
}

Ausdruck *
Ausdruck_Eigenschaft::eigenschaft() const
{
    return _eigenschaft;
}

void
Ausdruck_Eigenschaft::ausgeben(int32_t tiefe, std::ostream &ausgabe)
{
    ausgabe << std::string(tiefe*4, ' ') << "Eigenschaft";
    ausgabe.put('\n');
    _basis->ausgeben(tiefe+1, ausgabe);
    ausgabe.put('\n');
    _eigenschaft->ausgeben(tiefe+1, ausgabe);
}

Ausdruck_Aufruf::Ausdruck_Aufruf(Spanne spanne, Ausdruck *basis, std::vector<Ausdruck *> argumente)
    : Ausdruck(Asb_Knoten::AUSDRUCK_AUFRUF, spanne)
    , _basis(basis)
    , _argumente(argumente)
{
}

Ausdruck *
Ausdruck_Aufruf::basis() const
{
    return _basis;
}

std::vector<Ausdruck *>
Ausdruck_Aufruf::argumente() const
{
    return _argumente;
}

void
Ausdruck_Aufruf::ausgeben(int32_t tiefe, std::ostream &ausgabe)
{
    ausgabe << std::string(tiefe*4, ' ') << "Aufruf";
    ausgabe.put('\n');

    ausgabe << std::string((tiefe+1)*4, ' ') << "Basis\n";
    _basis->ausgeben(tiefe+2, ausgabe);

    ausgabe << "\n" << std::string((tiefe+1)*4, ' ') << "Argumente\n";
    for (auto *arg : _argumente)
    {
        ausgabe.put('\n');
        arg->ausgeben(tiefe+2, ausgabe);
    }
}

Ausdruck_Ausführen::Ausdruck_Ausführen(Spanne spanne, Ausdruck *ausdruck)
    : Ausdruck(Asb_Knoten::AUSDRUCK_AUSFÜHREN, spanne)
    , _ausdruck(ausdruck)
{
}

Ausdruck *
Ausdruck_Ausführen::ausdruck() const
{
    return _ausdruck;
}

void
Ausdruck_Ausführen::ausgeben(int32_t tiefe, std::ostream &ausgabe)
{
    Asb_Knoten::ausgeben(tiefe, ausgabe);

    _ausdruck->ausgeben(tiefe+1, ausgabe);
}

Ausdruck_Reihe::Ausdruck_Reihe(Spanne spanne, Ausdruck *min, Ausdruck *max)
    : Ausdruck(Asb_Knoten::AUSDRUCK_REIHE, spanne)
    , _min(min)
    , _max(max)
{
}

Ausdruck *
Ausdruck_Reihe::min() const
{
    return _min;
}

Ausdruck *
Ausdruck_Reihe::max() const
{
    return _max;
}

void
Ausdruck_Reihe::ausgeben(int32_t tiefe, std::ostream &ausgabe)
{
    ausgabe << std::string(tiefe*4, ' ') << "Reihe";
    ausgabe.put('\n');
    _min->ausgeben(tiefe+1, ausgabe);
    ausgabe.put('\n');
    _max->ausgeben(tiefe+1, ausgabe);
}

Ausdruck_Brauche::Ausdruck_Brauche(Spanne spanne, std::string dateiname)
    : Ausdruck(Asb_Knoten::AUSDRUCK_BRAUCHE, spanne)
    , _dateiname(dateiname)
{
}

std::string
Ausdruck_Brauche::dateiname() const
{
    return _dateiname;
}

Ausdruck_Kompositum::Ausdruck_Kompositum(Spanne spanne, std::vector<Ausdruck_Kompositum::Eigenschaft *> eigenschaften,
                                         bool ist_benamt, Spezifizierung *spezifizierung)
    : Ausdruck(Asb_Knoten::AUSDRUCK_KOMPOSITUM, spanne)
    , _eigenschaften(eigenschaften)
    , _ist_benamt(ist_benamt)
    , _spezifizierung(spezifizierung)
{
}

std::vector<Ausdruck_Kompositum::Eigenschaft *>
Ausdruck_Kompositum::eigenschaften() const
{
    return _eigenschaften;
}

Spezifizierung *
Ausdruck_Kompositum::spezifizierung() const
{
    return _spezifizierung;
}

bool
Ausdruck_Kompositum::ist_benamt() const
{
    return _ist_benamt;
}

Ausdruck_Kompositum::Eigenschaft::Eigenschaft(Spanne spanne, std::string name, Ausdruck *ausdruck)
    : _name(name)
    , _spanne(spanne)
    , _ausdruck(ausdruck)
    , _ist_benamt(true)
{
}

Ausdruck_Kompositum::Eigenschaft::Eigenschaft(Spanne spanne, Ausdruck *ausdruck)
    : _name()
    , _spanne(spanne)
    , _ausdruck(ausdruck)
    , _ist_benamt(false)
{
}

std::string
Ausdruck_Kompositum::Eigenschaft::name() const
{
    return _name;
}

Ausdruck *
Ausdruck_Kompositum::Eigenschaft::ausdruck() const
{
    return _ausdruck;
}

bool
Ausdruck_Kompositum::Eigenschaft::ist_benamt() const
{
    return _ist_benamt;
}

Spanne
Ausdruck_Kompositum::Eigenschaft::spanne() const
{
    return _spanne;
}

Ausdruck_Funktion::Ausdruck_Funktion(
        Spanne spanne, std::vector<Deklaration *> parameter,
        Spezifizierung *rückgabe, Anweisung *rumpf)
    : Ausdruck(Asb_Knoten::AUSDRUCK_FUNKTION, spanne)
    , _parameter(parameter)
    , _rückgabe(rückgabe)
    , _rumpf(rumpf)
{
}

std::vector<Deklaration *>
Ausdruck_Funktion::parameter() const
{
    return _parameter;
}

Spezifizierung *
Ausdruck_Funktion::rückgabe() const
{
    return _rückgabe;
}

Anweisung *
Ausdruck_Funktion::rumpf() const
{
    return _rumpf;
}

void Ausdruck_Funktion::ausgeben(int32_t tiefe, std::ostream &ausgabe)
{
    ausgabe << std::string(tiefe*4, ' ') << "Funktion";
    ausgabe.put('\n');

    ausgabe << std::string((tiefe+1)*4, ' ') << "Parameter";
    for (auto *param : _parameter)
    {
        ausgabe.put('\n');
        param->ausgeben(tiefe+2, ausgabe);
    }

    if (_rückgabe)
    {
        ausgabe.put('\n');
        ausgabe << std::string((tiefe+1)*4, ' ') << "Rückgabe";
        ausgabe.put('\n');
        _rückgabe->ausgeben(tiefe+2, ausgabe);
    }

    if (_rumpf)
    {
        ausgabe.put('\n');
        ausgabe << std::string((tiefe+1)*4, ' ') << "Rumpf";
        ausgabe.put('\n');
        _rumpf->ausgeben(tiefe+2, ausgabe);
    }
}

Ausdruck_Schablone::Ausdruck_Schablone(Spanne spanne, std::vector<Deklaration *> eigenschaften)
    : Ausdruck(Asb_Knoten::AUSDRUCK_SCHABLONE, spanne)
    , _eigenschaften(eigenschaften)
{
}

std::vector<Deklaration *>
Ausdruck_Schablone::eigenschaften() const
{
    return _eigenschaften;
}

void Ausdruck_Schablone::ausgeben(int32_t tiefe, std::ostream &ausgabe)
{
    ausgabe << std::string(tiefe*4, ' ') << "Schablone";
    ausgabe.put('\n');
    for (auto *eigenschaft : _eigenschaften)
    {
        ausgabe.put('\n');
        eigenschaft->ausgeben(tiefe+1, ausgabe);
    }
}

Ausdruck_Option::Ausdruck_Option(Spanne spanne, std::vector<Deklaration_Variable *> eigenschaften)
    : Ausdruck(Asb_Knoten::AUSDRUCK_OPTION, spanne)
    , _eigenschaften(eigenschaften)
{
}

std::vector<Deklaration_Variable *> Ausdruck_Option::eigenschaften() const
{
    return _eigenschaften;
}

void Ausdruck_Option::ausgeben(int32_t tiefe, std::ostream &ausgabe)
{
    ausgabe << std::string(tiefe*4, ' ') << "Option";
    ausgabe.put('\n');
    for (auto *eigenschaft : _eigenschaften)
    {
        ausgabe.put('\n');
        eigenschaft->ausgeben(tiefe+1, ausgabe);
    }
}
// }}}
// spezifizierung {{{
Spezifizierung::Spezifizierung(Asb_Knoten::Art art, Spanne spanne)
    : Asb_Knoten(art, spanne)
    , _größe(0)
{
}

int32_t
Spezifizierung::größe() const
{
    return _größe;
}

void
Spezifizierung::größe_setzen(int32_t größe)
{
    _größe = größe;
}

Spezifizierung_Bezeichner::Spezifizierung_Bezeichner(Spanne spanne, std::string name)
    : Spezifizierung(Asb_Knoten::SPEZIFIZIERUNG_BEZEICHNER, spanne)
    , _name(name)
{
}

std::string
Spezifizierung_Bezeichner::name() const
{
    return _name;
}

void
Spezifizierung_Bezeichner::ausgeben(int32_t tiefe, std::ostream &ausgabe)
{
    ausgabe << std::string(tiefe*4, ' ') << std::format("Spezifizierung Name ({})", _name);
}

Spezifizierung_Feld::Spezifizierung_Feld(Spanne spanne, Spezifizierung *basis, Ausdruck *index)
    : Spezifizierung(Asb_Knoten::SPEZIFIZIERUNG_FELD, spanne)
    , _basis(basis)
    , _index(index)
{
}

Spezifizierung *
Spezifizierung_Feld::basis() const
{
    return _basis;
}

Ausdruck *
Spezifizierung_Feld::index() const
{
    return _index;
}

void
Spezifizierung_Feld::ausgeben(int32_t tiefe, std::ostream &ausgabe)
{
    Asb_Knoten::ausgeben(tiefe, ausgabe);
    ausgabe.put('\n');
    _basis->ausgeben(tiefe+1, ausgabe);
    ausgabe.put('\n');
    _index->ausgeben(tiefe+1, ausgabe);
}

Spezifizierung_Zeiger::Spezifizierung_Zeiger(Spanne spanne, Spezifizierung *basis)
    : Spezifizierung(Asb_Knoten::SPEZIFIZIERUNG_ZEIGER, spanne)
    , _basis(basis)
{
}

Spezifizierung *
Spezifizierung_Zeiger::basis() const
{
    return _basis;
}

void
Spezifizierung_Zeiger::ausgeben(int32_t tiefe, std::ostream &ausgabe)
{
    Asb_Knoten::ausgeben(tiefe, ausgabe);
    ausgabe.put('\n');
    _basis->ausgeben(tiefe+1, ausgabe);
}

Spezifizierung_Funktion::Spezifizierung_Funktion(Spanne spanne, std::vector<Spezifizierung *> parameter,
                                                 Spezifizierung *rückgabe)
    : Spezifizierung(Asb_Knoten::SPEZIFIZIERUNG_FUNKTION, spanne)
    , _parameter(parameter)
    , _rückgabe(rückgabe)
{
}

std::vector<Spezifizierung *>
Spezifizierung_Funktion::parameter() const
{
    return _parameter;
}

Spezifizierung *
Spezifizierung_Funktion::rückgabe() const
{
    return _rückgabe;
}

void
Spezifizierung_Funktion::ausgeben(int32_t tiefe, std::ostream &ausgabe)
{
    Asb_Knoten::ausgeben(tiefe, ausgabe);
    ausgabe.put('\n');

    ausgabe << std::string((tiefe+1)*4, ' ') << "Parameter";
    for (auto *param : _parameter)
    {
        ausgabe.put('\n');
        param->ausgeben(tiefe+2, ausgabe);
    }

    if (_rückgabe)
    {
        ausgabe.put('\n');
        ausgabe << std::string((tiefe+1)*4, ' ') << "Rückgabe";
        ausgabe.put('\n');
        _rückgabe->ausgeben(tiefe+2, ausgabe);
    }
}
// }}}
// deklaration {{{
Deklaration::Deklaration(Asb_Knoten::Art art, Spanne spanne, std::vector<std::string> namen)
    : Asb_Knoten(art, spanne)
    , _namen(namen)
    , _spezifizierung(nullptr)
    , _symbol(nullptr)
{
}

Deklaration::Deklaration(Asb_Knoten::Art art, Spanne spanne, std::string name)
    : Asb_Knoten(art, spanne)
    , _namen({name})
{
}

std::vector<std::string>
Deklaration::namen() const
{
    return _namen;
}

Spezifizierung *
Deklaration::spezifizierung() const
{
    return _spezifizierung;
}

Symbol *
Deklaration::symbol() const
{
    return _symbol;
}

void
Deklaration::symbol_setzen(Symbol *symbol)
{
    _symbol = symbol;
}

Deklaration_Variable::Deklaration_Variable(Spanne spanne, std::vector<std::string> namen,
                                           Spezifizierung *spezifizierung, Ausdruck *initialisierung)
    : Deklaration(Asb_Knoten::DEKLARATION_VARIABLE, spanne, namen)
    , _initialisierung(initialisierung)
{
    _spezifizierung = spezifizierung;
}

Ausdruck *
Deklaration_Variable::initialisierung() const
{
    return _initialisierung;
}

void
Deklaration_Variable::ausgeben(int32_t tiefe, std::ostream &ausgabe)
{
    Asb_Knoten::ausgeben(tiefe, ausgabe);
    ausgabe.put('\n');

    for (auto name : namen())
    {
        ausgabe << std::string((tiefe+1)*4, ' ') << name << ", ";
    }

    if (_spezifizierung)
    {
        ausgabe.put('\n');
        _spezifizierung->ausgeben(tiefe+1, ausgabe);
    }

    if (_initialisierung)
    {
        ausgabe.put('\n');
        _initialisierung->ausgeben(tiefe+1, ausgabe);
    }
}

Deklaration_Funktion::Deklaration_Funktion(Spanne spanne, std::string name, Spezifizierung *spezifizierung,
           Ausdruck_Funktion *ausdruck)
    : Deklaration(Asb_Knoten::DEKLARATION_FUNKTION, spanne, name)
    , _ausdruck(ausdruck)
{
    _spezifizierung = spezifizierung;
}

std::vector<Deklaration *>
Deklaration_Funktion::parameter() const
{
    return _ausdruck->parameter();
}

Spezifizierung *
Deklaration_Funktion::rückgabe() const
{
    return _ausdruck->rückgabe();
}

Anweisung *
Deklaration_Funktion::rumpf() const
{
    return _ausdruck->rumpf();
}

void
Deklaration_Funktion::ausgeben(int32_t tiefe, std::ostream &ausgabe)
{
    ausgabe << std::string(tiefe*4, ' ') << std::format("Deklaration Funktion ({})", namen()[0]);

    for (auto *param : _ausdruck->parameter())
    {
        ausgabe.put('\n');
        param->ausgeben(tiefe+1, ausgabe);
    }

    if (_ausdruck->rückgabe())
    {
        ausgabe.put('\n');
        _ausdruck->rückgabe()->ausgeben(tiefe+1, ausgabe);
    }

    if (_ausdruck->rumpf())
    {
        ausgabe.put('\n');
        _ausdruck->rumpf()->ausgeben(tiefe+1, ausgabe);
    }
}

Deklaration_Schablone::Deklaration_Schablone(Spanne spanne, std::string name, std::vector<Deklaration *> eigenschaften)
    : Deklaration(Asb_Knoten::DEKLARATION_SCHABLONE, spanne, name)
    , _eigenschaften(eigenschaften)
{
}

std::vector<Deklaration *>
Deklaration_Schablone::eigenschaften() const
{
    return _eigenschaften;
}

void
Deklaration_Schablone::ausgeben(int32_t tiefe, std::ostream &ausgabe)
{
    ausgabe << std::string(tiefe*4, ' ') << std::format("Deklaration Schablone ({})", namen()[0]);

    for (auto *eigenschaft : _eigenschaften)
    {
        ausgabe.put('\n');
        eigenschaft->ausgeben(tiefe+1, ausgabe);
    }
}

Deklaration_Brauche::Deklaration_Brauche(Spanne spanne, std::string name, Ausdruck_Brauche *ausdruck)
    : Deklaration(Asb_Knoten::DEKLARATION_BRAUCHE, spanne, name)
    , _ausdruck(ausdruck)
{
}

Ausdruck_Brauche *
Deklaration_Brauche::ausdruck() const
{
    return _ausdruck;
}

std::string
Deklaration_Brauche::name() const
{
    auto erg = namen().at(0);

    return erg;
}

Deklaration_Option::Deklaration_Option(Spanne spanne, std::string name, Ausdruck_Option *ausdruck)
    : Deklaration(Asb_Knoten::DEKLARATION_OPTION, spanne, name)
    , _ausdruck(ausdruck)
{
}

std::vector<Deklaration_Variable *>
Deklaration_Option::eigenschaften() const
{
    return _ausdruck->eigenschaften();
}
// }}}
// anweisung {{{
Anweisung::Anweisung(Asb_Knoten::Art art, Spanne spanne)
    : Asb_Knoten(art, spanne)
{
}

void
Anweisung::marken_setzen(std::vector<Marke *> marken)
{
    _marken = marken;
}

void
Anweisung::ausgeben(int32_t tiefe, std::ostream &ausgabe)
{
    if (_marken.size() > 0)
    {
        ausgabe.put('\n');
        ausgabe << std::string((tiefe+1)*4, ' ') << "Marken";

        for (auto *marke : _marken)
        {
            ausgabe.put('\n');
            ausgabe << std::string((tiefe+2)*4, ' ') << marke->wert();
        }
    }

    Asb_Knoten::ausgeben(tiefe, ausgabe);
}

Anweisung_Zuweisung::Anweisung_Zuweisung(Spanne spanne, Ausdruck *links, Glied *op, Ausdruck *rechts)
    : Anweisung(Asb_Knoten::ANWEISUNG_ZUWEISUNG, spanne)
    , _links(links)
    , _op(op)
    , _rechts(rechts)
{
}

Ausdruck *
Anweisung_Zuweisung::links() const
{
    return _links;
}

Glied *
Anweisung_Zuweisung::op() const
{
    return _op;
}

Ausdruck *
Anweisung_Zuweisung::rechts() const
{
    return _rechts;
}

void
Anweisung_Zuweisung::ausgeben(int32_t tiefe, std::ostream &ausgabe)
{
    Anweisung::ausgeben(tiefe, ausgabe);

    ausgabe << std::string(tiefe*4, ' ') << std::format("Zuweisung ({})", _op->text());
    ausgabe.put('\n');
    _links->ausgeben(tiefe+1, ausgabe);
    ausgabe.put('\n');
    _rechts->ausgeben(tiefe+1, ausgabe);

}

Anweisung_Wenn::Anweisung_Wenn(Spanne spanne, Ausdruck *bedingung, Anweisung *rumpf, Anweisung_Wenn *sonst)
    : Anweisung(Asb_Knoten::ANWEISUNG_WENN, spanne)
    , _bedingung(bedingung)
    , _rumpf(rumpf)
    , _sonst(sonst)
{
}

Ausdruck *
Anweisung_Wenn::bedingung() const
{
    return _bedingung;
}

Anweisung *
Anweisung_Wenn::rumpf() const
{
    return _rumpf;
}

Anweisung_Wenn *
Anweisung_Wenn::sonst() const
{
    return _sonst;
}

void
Anweisung_Wenn::ausgeben(int32_t tiefe, std::ostream &ausgabe)
{
    Asb_Knoten::ausgeben(tiefe, ausgabe);
    Anweisung::ausgeben(tiefe, ausgabe);

    ausgabe.put('\n');
    _bedingung->ausgeben(tiefe+1, ausgabe);
    ausgabe.put('\n');
    _rumpf->ausgeben(tiefe+1, ausgabe);

    if (_sonst != nullptr)
    {
        ausgabe.put('\n');
        ausgabe << std::string((tiefe+1)*4, ' ') << "Sonst";
        ausgabe.put('\n');
        _sonst->ausgeben(tiefe+2, ausgabe);
    }
}

Anweisung_Für::Anweisung_Für(Spanne spanne, Ausdruck *index, Ausdruck *bedingung, Anweisung *rumpf)
    : Anweisung(Asb_Knoten::ANWEISUNG_FÜR, spanne)
    , _index(index)
    , _bedingung(bedingung)
    , _rumpf(rumpf)
{
}

Ausdruck *
Anweisung_Für::index() const
{
    return _index;
}

Ausdruck *
Anweisung_Für::bedingung() const
{
    return _bedingung;
}

Anweisung *
Anweisung_Für::rumpf() const
{
    return _rumpf;
}

void
Anweisung_Für::ausgeben(int32_t tiefe, std::ostream &ausgabe)
{
    Asb_Knoten::ausgeben(tiefe, ausgabe);
    Anweisung::ausgeben(tiefe, ausgabe);

    if (_index != nullptr)
    {
        ausgabe.put('\n');
        ausgabe << std::string((tiefe+1)*4, ' ') << "Index";
        ausgabe.put('\n');
        _index->ausgeben(tiefe+2, ausgabe);
    }

    ausgabe.put('\n');
    _bedingung->ausgeben(tiefe+1, ausgabe);
    ausgabe.put('\n');
    _rumpf->ausgeben(tiefe+1, ausgabe);
}

Anweisung_Solange::Anweisung_Solange(Spanne spanne, Ausdruck *bedingung, Anweisung *rumpf)
    : Anweisung(Asb_Knoten::ANWEISUNG_SOLANGE, spanne)
    , _bedingung(bedingung)
    , _rumpf(rumpf)
{
}

Ausdruck *
Anweisung_Solange::bedingung() const
{
    return _bedingung;
}

Anweisung *
Anweisung_Solange::rumpf() const
{
    return _rumpf;
}

void
Anweisung_Solange::ausgeben(int32_t tiefe, std::ostream &ausgabe)
{
    Asb_Knoten::ausgeben(tiefe, ausgabe);
    Anweisung::ausgeben(tiefe, ausgabe);

    ausgabe.put('\n');
    _bedingung->ausgeben(tiefe+1, ausgabe);
    ausgabe.put('\n');
    _rumpf->ausgeben(tiefe+1, ausgabe);
}

Anweisung_Block::Anweisung_Block(Spanne spanne, std::vector<Anweisung *> anweisungen)
    : Anweisung(Asb_Knoten::ANWEISUNG_BLOCK, spanne)
    , _anweisungen(anweisungen)
{
}

std::vector<Anweisung *>
Anweisung_Block::anweisungen() const
{
    return _anweisungen;
}

void
Anweisung_Block::ausgeben(int32_t tiefe, std::ostream &ausgabe)
{
    Asb_Knoten::ausgeben(tiefe, ausgabe);

    for (auto *anweisung : _anweisungen)
    {
        ausgabe.put('\n');
        anweisung->ausgeben(tiefe+1, ausgabe);
    }
}

Anweisung_Deklaration::Anweisung_Deklaration(Spanne spanne, Deklaration *deklaration)
    : Anweisung(Asb_Knoten::ANWEISUNG_DEKLARATION, spanne)
    , _deklaration(deklaration)
{
}

Deklaration *
Anweisung_Deklaration::deklaration() const
{
    return _deklaration;
}

void
Anweisung_Deklaration::ausgeben(int32_t tiefe, std::ostream& ausgabe)
{
    Asb_Knoten::ausgeben(tiefe, ausgabe);

    _deklaration->ausgeben(tiefe+1, ausgabe);
}

Anweisung_Brauche::Anweisung_Brauche(Spanne spanne, std::string dateiname)
    : Anweisung(Asb_Knoten::ANWEISUNG_BRAUCHE, spanne)
    , _dateiname(dateiname)
{
}

std::string
Anweisung_Brauche::dateiname() const
{
    return _dateiname;
}

void
Anweisung_Brauche::ausgeben(int32_t tiefe, std::ostream &ausgabe)
{
    Asb_Knoten::ausgeben(tiefe, ausgabe);
    ausgabe.put('\n');
    ausgabe << std::string((tiefe+1)*4, ' ') << _dateiname;
}

Anweisung_Lade::Anweisung_Lade(Spanne spanne, std::string dateiname)
    : Anweisung(Asb_Knoten::ANWEISUNG_LADE, spanne)
    , _dateiname(dateiname)
{
}

std::string
Anweisung_Lade::dateiname() const
{
    return _dateiname;
}

Anweisung_Ausdruck::Anweisung_Ausdruck(Spanne spanne, Ausdruck *ausdruck)
    : Anweisung(Asb_Knoten::ANWEISUNG_AUSDRUCK, spanne)
    , _ausdruck(ausdruck)
{
}

Ausdruck *
Anweisung_Ausdruck::ausdruck() const
{
    return _ausdruck;
}

void
Anweisung_Ausdruck::ausgeben(int32_t tiefe, std::ostream &ausgabe)
{
    Asb_Knoten::ausgeben(tiefe, ausgabe);
    ausgabe.put('\n');
    _ausdruck->ausgeben(tiefe+1, ausgabe);
}

Anweisung_Final::Anweisung_Final(Spanne spanne, Anweisung *anweisung)
    : Anweisung(Asb_Knoten::ANWEISUNG_FINAL, spanne)
    , _anweisung(anweisung)
{
}

Anweisung *
Anweisung_Final::anweisung() const
{
    return _anweisung;
}

Anweisung_Weiche::Anweisung_Weiche(Spanne spanne, Ausdruck *ausdruck, std::vector<Muster *> muster)
    : Anweisung(Asb_Knoten::ANWEISUNG_WEICHE, spanne)
    , _ausdruck(ausdruck)
    , _muster(muster)
{
}

Ausdruck *
Anweisung_Weiche::ausdruck() const
{
    return _ausdruck;
}

std::vector<Muster *>
Anweisung_Weiche::muster() const
{
    return _muster;
}

Anweisung_Res::Anweisung_Res(Spanne spanne, Ausdruck *ausdruck)
    : Anweisung(Asb_Knoten::ANWEISUNG_RES, spanne)
    , _ausdruck(ausdruck)
{
}

Ausdruck *
Anweisung_Res::ausdruck() const
{
    return _ausdruck;
}

Anweisung_Weiter::Anweisung_Weiter(Spanne spanne)
    : Anweisung(Asb_Knoten::ANWEISUNG_WEITER, spanne)
{
}

Anweisung_Sprung::Anweisung_Sprung(Spanne spanne, Ausdruck_Bezeichner *markierung)
    : Anweisung(Asb_Knoten::ANWEISUNG_SPRUNG, spanne)
    , _markierung(markierung)
{
}

Ausdruck_Bezeichner *
Anweisung_Sprung::markierung() const
{
    return _markierung;
}

Anweisung_Raus::Anweisung_Raus(Spanne spanne)
    : Anweisung(Asb_Knoten::ANWEISUNG_RAUS, spanne)
{
}

Anweisung_Markierung::Anweisung_Markierung(Spanne spanne, std::string markierung)
    : Anweisung(Asb_Knoten::ANWEISUNG_MARKIERUNG, spanne)
    , _markierung(markierung)
{
}

std::string
Anweisung_Markierung::markierung() const
{
    return _markierung;
}
// }}}
}

#include "kmp/bestimmter_asb.hpp"

#include "kmp/asb.hpp"

namespace Sss::Kmp {

Bestimmter_Knoten::Bestimmter_Knoten(Asb_Knoten *asb_knoten, Asb_Knoten::Art art)
    : _asb_knoten(asb_knoten)
    , _art(art)
{
}

Asb_Knoten::Art
Bestimmter_Knoten::art() const
{
    return _art;
}

Asb_Knoten *
Bestimmter_Knoten::asb_knoten() const
{
    return _asb_knoten;
}

Spanne
Bestimmter_Knoten::spanne() const
{
    return _asb_knoten->spanne();
}

template<typename T>
T Bestimmter_Knoten::als()
{
    return static_cast<T> (this);
}

Bestimmtes_Muster::Bestimmtes_Muster(Asb_Knoten *asb_knoten, Bestimmter_Ausdruck *ausdruck, Bestimmte_Anweisung *rumpf)
    : _ausdruck(ausdruck)
    , _rumpf(rumpf)
{
}

Bestimmter_Ausdruck *
Bestimmtes_Muster::ausdruck() const
{
    return _ausdruck;
}

Bestimmte_Anweisung *
Bestimmtes_Muster::rumpf() const
{
    return _rumpf;
}

// ausdrücke {{{
Bestimmter_Ausdruck::Bestimmter_Ausdruck(Ausdruck *asb_knoten, Asb_Knoten::Art art, Operand *operand)
    : Bestimmter_Knoten(asb_knoten, art)
    , _operand(operand)
{
}

Operand *
Bestimmter_Ausdruck::operand() const
{
    return _operand;
}

Bestimmter_Ausdruck_Ganzzahl::Bestimmter_Ausdruck_Ganzzahl(Ausdruck_Ganzzahl *asb_knoten, Operand *operand)
    : Bestimmter_Ausdruck(asb_knoten, Asb_Knoten::AUSDRUCK_GANZZAHL, operand)
{
}

Bestimmter_Ausdruck_Dezimalzahl::Bestimmter_Ausdruck_Dezimalzahl(Ausdruck_Dezimalzahl *asb_knoten, Operand *operand)
    : Bestimmter_Ausdruck(asb_knoten, Asb_Knoten::AUSDRUCK_DEZIMALZAHL, operand)
{
}

Bestimmter_Ausdruck_Bezeichner::Bestimmter_Ausdruck_Bezeichner(Ausdruck_Bezeichner *asb_knoten, Operand *operand)
    : Bestimmter_Ausdruck(asb_knoten, Asb_Knoten::AUSDRUCK_BEZEICHNER, operand)
{
}

Bestimmter_Ausdruck_Text::Bestimmter_Ausdruck_Text(Ausdruck_Text *asb_knoten, Operand *operand)
    : Bestimmter_Ausdruck(asb_knoten, Asb_Knoten::AUSDRUCK_TEXT, operand)
{
}

Bestimmter_Ausdruck_Binär::Bestimmter_Ausdruck_Binär(Ausdruck_Binär *asb_knoten, Operand *operand, Ausdruck_Binär::Binär_Op op,
    Bestimmter_Ausdruck *links, Bestimmter_Ausdruck *rechts)
    : Bestimmter_Ausdruck(asb_knoten, Asb_Knoten::AUSDRUCK_BINÄR, operand)
    , _op(op)
    , _links(links)
    , _rechts(rechts)
{
}

Bestimmter_Ausdruck *
Bestimmter_Ausdruck_Binär::links() const
{
    return _links;
}

Bestimmter_Ausdruck *
Bestimmter_Ausdruck_Binär::rechts() const
{
    return _rechts;
}

Ausdruck_Binär::Binär_Op
Bestimmter_Ausdruck_Binär::op() const
{
    return _op;
}

Bestimmter_Ausdruck_Unär::Bestimmter_Ausdruck_Unär(Ausdruck_Unär *asb_knoten, Operand *operand,
    Ausdruck_Unär::Unär_Op op, Bestimmter_Ausdruck *ausdruck)
    : Bestimmter_Ausdruck(asb_knoten, Asb_Knoten::AUSDRUCK_UNÄR, operand)
    , _op(op)
    , _ausdruck(ausdruck)
{
}

Ausdruck_Unär::Unär_Op
Bestimmter_Ausdruck_Unär::op() const
{
    return _op;
}

Bestimmter_Ausdruck *
Bestimmter_Ausdruck_Unär::ausdruck() const
{
    return _ausdruck;
}

Bestimmter_Ausdruck_Index::Bestimmter_Ausdruck_Index(Ausdruck_Index *asb_knoten, Operand *operand,
    Bestimmter_Ausdruck *basis, Bestimmter_Ausdruck *index)
    : Bestimmter_Ausdruck(asb_knoten, Asb_Knoten::AUSDRUCK_INDEX, operand)
    , _basis(basis)
    , _index(index)
{
}

Bestimmter_Ausdruck *
Bestimmter_Ausdruck_Index::basis() const
{
    return _basis;
}

Bestimmter_Ausdruck *
Bestimmter_Ausdruck_Index::index() const
{
    return _index;
}

Bestimmter_Ausdruck_Eigenschaft::Bestimmter_Ausdruck_Eigenschaft(Ausdruck_Eigenschaft *asb_knoten, Operand *operand,
    Bestimmter_Ausdruck *basis, Symbol *eigenschaft)
    : Bestimmter_Ausdruck(asb_knoten, Asb_Knoten::AUSDRUCK_EIGENSCHAFT, operand)
    , _basis(basis)
    , _eigenschaft(eigenschaft)
{
}

Bestimmter_Ausdruck *
Bestimmter_Ausdruck_Eigenschaft::basis() const
{
    return _basis;
}

Symbol *
Bestimmter_Ausdruck_Eigenschaft::eigenschaft() const
{
    return _eigenschaft;
}

Bestimmter_Ausdruck_Aufruf::Bestimmter_Ausdruck_Aufruf(Ausdruck_Aufruf *asb_knoten, Operand *operand,
    Bestimmter_Ausdruck *basis, std::vector<Bestimmter_Ausdruck *> argumente)
    : Bestimmter_Ausdruck(asb_knoten, Asb_Knoten::AUSDRUCK_AUFRUF, operand)
    , _basis(basis)
    , _argumente(argumente)
{
}

Bestimmter_Ausdruck *
Bestimmter_Ausdruck_Aufruf::basis() const
{
    return _basis;
}

std::vector<Bestimmter_Ausdruck *>
Bestimmter_Ausdruck_Aufruf::argumente() const
{
    return _argumente;
}

Bestimmter_Ausdruck_Ausführen::Bestimmter_Ausdruck_Ausführen(Ausdruck_Ausführen *asb_knoten, Operand *operand,
    Bestimmter_Ausdruck *ausdruck)
    : Bestimmter_Ausdruck(asb_knoten, Asb_Knoten::AUSDRUCK_AUSFÜHREN, operand)
    , _ausdruck(ausdruck)
{
}

Bestimmter_Ausdruck *
Bestimmter_Ausdruck_Ausführen::ausdruck() const
{
    return _ausdruck;
}

Bestimmter_Ausdruck_Reihe::Bestimmter_Ausdruck_Reihe(Ausdruck_Reihe *asb_knoten, Operand *operand,
    Bestimmter_Ausdruck *min, Bestimmter_Ausdruck *max)
    : Bestimmter_Ausdruck(asb_knoten, Asb_Knoten::AUSDRUCK_REIHE, operand)
    , _min(min)
    , _max(max)
{
}

Bestimmter_Ausdruck *
Bestimmter_Ausdruck_Reihe::min() const
{
    return _min;
}

Bestimmter_Ausdruck *
Bestimmter_Ausdruck_Reihe::max() const
{
    return _max;
}

Bestimmter_Ausdruck_Kompositum::Bestimmter_Ausdruck_Kompositum(Ausdruck_Kompositum *asb_knoten, Operand *operand,
    std::vector<Bestimmter_Ausdruck_Kompositum::Bestimmte_Eigenschaft *> eigenschaften)
    : Bestimmter_Ausdruck(asb_knoten, Asb_Knoten::AUSDRUCK_KOMPOSITUM, operand)
    , _eigenschaften(eigenschaften)
{
}

std::vector<Bestimmter_Ausdruck_Kompositum::Bestimmte_Eigenschaft *>
Bestimmter_Ausdruck_Kompositum::eigenschaften() const
{
    return _eigenschaften;
}

Bestimmter_Ausdruck_Kompositum::Bestimmte_Eigenschaft::Bestimmte_Eigenschaft(Operand *operand)
    : _operand(operand)
{
}

Operand *
Bestimmter_Ausdruck_Kompositum::Bestimmte_Eigenschaft::operand() const
{
    return _operand;
}
// }}}
// deklarationen {{{
Bestimmte_Deklaration::Bestimmte_Deklaration(Deklaration *asb_knoten, Asb_Knoten::Art art, Symbol *symbol,
                                             Datentyp *datentyp, Bestimmter_Ausdruck *initialisierung)
    : Bestimmter_Knoten(asb_knoten, art)
    , _symbole({ symbol })
    , _datentyp(datentyp)
    , _initialisierung(initialisierung)
{
}

Bestimmte_Deklaration::Bestimmte_Deklaration(Deklaration *asb_knoten, std::vector<Symbol *> symbole,
                                             Datentyp *datentyp, Bestimmter_Ausdruck *initialisierung)
    : Bestimmter_Knoten(asb_knoten, asb_knoten->art())
    , _symbole(symbole)
    , _datentyp(datentyp)
    , _initialisierung(initialisierung)
{
}

Symbol *
Bestimmte_Deklaration::symbol() const
{
    return _symbole[0];
}

std::vector<Symbol *>
Bestimmte_Deklaration::symbole() const
{
    return _symbole;
}

Bestimmter_Ausdruck *
Bestimmte_Deklaration::initialisierung() const
{
    return _initialisierung;
}

Datentyp *
Bestimmte_Deklaration::datentyp() const
{
    return _datentyp;
}

Bestimmte_Deklaration_Funktion::Bestimmte_Deklaration_Funktion(Deklaration *asb_knoten, Symbol *symbol, Datentyp *datentyp,        Bestimmte_Anweisung *rumpf)
    : Bestimmte_Deklaration(asb_knoten, Asb_Knoten::DEKLARATION_FUNKTION, symbol, datentyp)
    , _rumpf(rumpf)
{
}

Bestimmte_Anweisung *
Bestimmte_Deklaration_Funktion::rumpf() const
{
    return _rumpf;
}

void
Bestimmte_Deklaration_Funktion::rumpf_setzen(Bestimmte_Anweisung *rumpf)
{
    _rumpf = rumpf;
}

Bestimmte_Deklaration_Option::Bestimmte_Deklaration_Option(Deklaration *asb_knoten, Symbol *symbol, Datentyp *datentyp)
    : Bestimmte_Deklaration(asb_knoten, Asb_Knoten::DEKLARATION_OPTION, symbol, datentyp)
{
}

Bestimmte_Deklaration_Schablone::Bestimmte_Deklaration_Schablone(Deklaration *asb_knoten, Symbol *symbol, Datentyp *datentyp)
    : Bestimmte_Deklaration(asb_knoten, Asb_Knoten::DEKLARATION_SCHABLONE, symbol, datentyp)
{
}
// }}}
// anweisungen {{{
Bestimmte_Anweisung::Bestimmte_Anweisung(Anweisung *asb_knoten, Asb_Knoten::Art art)
    : Bestimmter_Knoten(asb_knoten, art)
{
}

Bestimmte_Anweisung_Deklaration::Bestimmte_Anweisung_Deklaration(Anweisung *asb_knoten, Bestimmte_Deklaration *deklaration)
    : Bestimmte_Anweisung(asb_knoten, Asb_Knoten::ANWEISUNG_DEKLARATION)
    , _deklaration(deklaration)
{
}

Bestimmte_Deklaration *
Bestimmte_Anweisung_Deklaration::deklaration() const
{
    return _deklaration;
}

Bestimmte_Anweisung_Ausdruck::Bestimmte_Anweisung_Ausdruck(Anweisung *asb_knoten, Bestimmter_Ausdruck *ausdruck)
    : Bestimmte_Anweisung(asb_knoten, Asb_Knoten::ANWEISUNG_AUSDRUCK)
    , _ausdruck(ausdruck)
{
}

Bestimmter_Ausdruck *
Bestimmte_Anweisung_Ausdruck::ausdruck() const
{
    return _ausdruck;
}

Bestimmte_Anweisung_Block::Bestimmte_Anweisung_Block(Anweisung *asb_knoten, std::vector<Bestimmte_Anweisung *> anweisungen)
    : Bestimmte_Anweisung(asb_knoten, Asb_Knoten::ANWEISUNG_BLOCK)
    , _anweisungen(anweisungen)
{
}

std::vector<Bestimmte_Anweisung *>
Bestimmte_Anweisung_Block::anweisungen() const
{
    return _anweisungen;
}

Bestimmte_Anweisung_Für::Bestimmte_Anweisung_Für(
    Anweisung *asb_knoten, Symbol *index, Bestimmter_Ausdruck *bedingung, Bestimmte_Anweisung *rumpf,
    std::string weiter_markierung, std::string raus_markierung)
    : Bestimmte_Anweisung(asb_knoten, Asb_Knoten::ANWEISUNG_FÜR)
    , _index(index)
    , _bedingung(bedingung)
    , _rumpf(rumpf)
    , _weiter_markierung(weiter_markierung)
    , _raus_markierung(raus_markierung)
{
}

Symbol *
Bestimmte_Anweisung_Für::index() const
{
    return _index;
}

Bestimmter_Ausdruck *
Bestimmte_Anweisung_Für::bedingung() const
{
    return _bedingung;
}

Bestimmte_Anweisung *
Bestimmte_Anweisung_Für::rumpf() const
{
    return _rumpf;
}

std::string
Bestimmte_Anweisung_Für::weiter_markierung() const
{
    return _weiter_markierung;
}

std::string
Bestimmte_Anweisung_Für::raus_markierung() const
{
    return _raus_markierung;
}

Bestimmte_Anweisung_Res::Bestimmte_Anweisung_Res(Anweisung *asb_knoten, Bestimmter_Ausdruck *ausdruck)
    : Bestimmte_Anweisung(asb_knoten, Asb_Knoten::ANWEISUNG_RES)
    , _ausdruck(ausdruck)
{
}

Bestimmter_Ausdruck *
Bestimmte_Anweisung_Res::ausdruck() const
{
    return _ausdruck;
}

Bestimmte_Anweisung_Wenn::Bestimmte_Anweisung_Wenn(Anweisung *asb_knoten, Bestimmter_Ausdruck *bedingung,
                            Bestimmte_Anweisung *rumpf, Bestimmte_Anweisung *sonst)
    : Bestimmte_Anweisung(asb_knoten, Asb_Knoten::ANWEISUNG_WENN)
    , _bedingung(bedingung)
    , _rumpf(rumpf)
    , _sonst(sonst)
{
}

Bestimmter_Ausdruck *
Bestimmte_Anweisung_Wenn::bedingung() const
{
    return _bedingung;
}

Bestimmte_Anweisung *
Bestimmte_Anweisung_Wenn::rumpf() const
{
    return _rumpf;
}

Bestimmte_Anweisung *
Bestimmte_Anweisung_Wenn::sonst() const
{
    return _sonst;
}

Bestimmte_Anweisung_Sprung::Bestimmte_Anweisung_Sprung(Anweisung *asb_knoten, std::string markierung)
    : Bestimmte_Anweisung(asb_knoten, Asb_Knoten::ANWEISUNG_SPRUNG)
    , _markierung(markierung)
{
}

std::string
Bestimmte_Anweisung_Sprung::markierung() const
{
    return _markierung;
}

Bestimmte_Anweisung_Markierung::Bestimmte_Anweisung_Markierung(Anweisung *asb_knoten, std::string markierung)
    : Bestimmte_Anweisung(asb_knoten, Asb_Knoten::ANWEISUNG_MARKIERUNG)
    , _markierung(markierung)
{
}

std::string
Bestimmte_Anweisung_Markierung::markierung() const
{
    return _markierung;
}

Bestimmte_Anweisung_Solange::Bestimmte_Anweisung_Solange(Anweisung *asb_knoten, Bestimmter_Ausdruck *bedingung,
                                                         Bestimmte_Anweisung *rumpf, std::string weiter_markierung, std::string raus_markierung)
    : Bestimmte_Anweisung(asb_knoten, Asb_Knoten::ANWEISUNG_SOLANGE)
    , _bedingung(bedingung)
    , _rumpf(rumpf)
    , _weiter_markierung(weiter_markierung)
    , _raus_markierung(raus_markierung)
{
}

Bestimmter_Ausdruck *
Bestimmte_Anweisung_Solange::bedingung() const
{
    return _bedingung;
}

Bestimmte_Anweisung *
Bestimmte_Anweisung_Solange::rumpf() const
{
    return _rumpf;
}

std::string
Bestimmte_Anweisung_Solange::weiter_markierung() const
{
    return _weiter_markierung;
}

std::string
Bestimmte_Anweisung_Solange::raus_markierung() const
{
    return _raus_markierung;
}

Bestimmte_Anweisung_Bedingter_Sprung::Bestimmte_Anweisung_Bedingter_Sprung(
    Anweisung *asb_knoten, std::string markierung, Bestimmter_Ausdruck *bedingung)
    : Bestimmte_Anweisung(asb_knoten, Asb_Knoten::ANWEISUNG_BEDINGTER_SPRUNG)
    , _markierung(markierung)
    , _bedingung(bedingung)
{
}

std::string
Bestimmte_Anweisung_Bedingter_Sprung::markierung() const
{
    return _markierung;
}

Bestimmter_Ausdruck *
Bestimmte_Anweisung_Bedingter_Sprung::bedingung() const
{
    return _bedingung;
}

Bestimmte_Anweisung_Weiche::Bestimmte_Anweisung_Weiche(Anweisung *asb_knoten, Bestimmter_Ausdruck *ausdruck,
                                                       std::vector<Bestimmtes_Muster *> muster)
    : Bestimmte_Anweisung(asb_knoten, Asb_Knoten::ANWEISUNG_WEICHE)
    , _ausdruck(ausdruck)
    , _muster(muster)
{
}

Bestimmter_Ausdruck *
Bestimmte_Anweisung_Weiche::ausdruck() const
{
    return _ausdruck;
}

std::vector<Bestimmtes_Muster *>
Bestimmte_Anweisung_Weiche::muster() const
{
    return _muster;
}

Bestimmte_Anweisung_Zuweisung::Bestimmte_Anweisung_Zuweisung(Anweisung *asb_knoten, Bestimmter_Ausdruck *links,
                                                             Glied *op, Bestimmter_Ausdruck *rechts)
    : Bestimmte_Anweisung(asb_knoten, Asb_Knoten::ANWEISUNG_ZUWEISUNG)
    , _links(links)
    , _op(op)
    , _rechts(rechts)
{
}

Bestimmter_Ausdruck *
Bestimmte_Anweisung_Zuweisung::links() const
{
    return _links;
}

Bestimmter_Ausdruck *
Bestimmte_Anweisung_Zuweisung::rechts() const
{
    return _rechts;
}

Glied *
Bestimmte_Anweisung_Zuweisung::op() const
{
    return _op;
}

Bestimmte_Anweisung_Final::Bestimmte_Anweisung_Final(Anweisung *asb_knoten, Bestimmte_Anweisung *anweisung)
    : Bestimmte_Anweisung(asb_knoten, Asb_Knoten::ANWEISUNG_FINAL)
    , _anweisung(anweisung)
{
}

Bestimmte_Anweisung *
Bestimmte_Anweisung_Final::anweisung() const
{
    return _anweisung;
}
// }}}
// asb {{{
Bestimmter_Asb::Bestimmter_Asb(Zone *system, Zone *global)
    : _system(system)
    , _global(global)
{
}

Zone *
Bestimmter_Asb::system() const
{
    return _system;
}

Zone *
Bestimmter_Asb::global() const
{
    return _global;
}

std::vector<Bestimmte_Anweisung *>
Bestimmter_Asb::anweisungen() const
{
    return _anweisungen;
}

void
Bestimmter_Asb::anweisungen_setzen(std::vector<Bestimmte_Anweisung *> anweisungen)
{
    _anweisungen = anweisungen;
}
// }}}
}

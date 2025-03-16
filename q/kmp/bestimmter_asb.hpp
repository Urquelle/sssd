#pragma once

#include "kmp/asb.hpp"

namespace Sss::Kmp {

class Bestimmte_Anweisung;
class Bestimmter_Ausdruck;

class Bestimmtes_Muster
{
public:
    Bestimmtes_Muster(Asb_Knoten *asb_knoten, Bestimmter_Ausdruck *muster, Bestimmte_Anweisung *rumpf);

    Bestimmter_Ausdruck * ausdruck() const;
    Bestimmte_Anweisung * rumpf() const;

private:
    Bestimmter_Ausdruck *_ausdruck;
    Bestimmte_Anweisung *_rumpf;
};

class Bestimmter_Knoten
{
public:
    Bestimmter_Knoten(Asb_Knoten *asb_knoten, Asb_Knoten::Art art);

    Spanne spanne() const;
    Asb_Knoten::Art art() const;
    Asb_Knoten * asb_knoten() const;

    template<typename T> T als();

private:
    Asb_Knoten *_asb_knoten;
    Asb_Knoten::Art _art;
};
// ausdrücke {{{
class Bestimmter_Ausdruck : public Bestimmter_Knoten
{
public:
    Bestimmter_Ausdruck(Ausdruck *asb_knoten, Asb_Knoten::Art art, Operand *operand);

    Operand *operand() const;

private:
    Operand *_operand;
};

class Bestimmter_Ausdruck_Ganzzahl : public Bestimmter_Ausdruck
{
public:
    Bestimmter_Ausdruck_Ganzzahl(Ausdruck_Ganzzahl *asb_knoten, Operand *operand);
};

class Bestimmter_Ausdruck_Dezimalzahl : public Bestimmter_Ausdruck
{
public:
    Bestimmter_Ausdruck_Dezimalzahl(Ausdruck_Dezimalzahl *asb_knoten, Operand *operand);
};

class Bestimmter_Ausdruck_Bezeichner : public Bestimmter_Ausdruck
{
public:
    Bestimmter_Ausdruck_Bezeichner(Ausdruck_Bezeichner *asb_knoten, Operand *operand);
};

class Bestimmter_Ausdruck_Text : public Bestimmter_Ausdruck
{
public:
    Bestimmter_Ausdruck_Text(Ausdruck_Text *asb_knoten, Operand *operand);
};

class Bestimmter_Ausdruck_Binär : public Bestimmter_Ausdruck
{
public:
    Bestimmter_Ausdruck_Binär(Ausdruck_Binär *asb_knoten, Operand *operand, Ausdruck_Binär::Binär_Op op,
                               Bestimmter_Ausdruck *links, Bestimmter_Ausdruck *rechts);

    Bestimmter_Ausdruck * links() const;
    Bestimmter_Ausdruck * rechts() const;
    Ausdruck_Binär::Binär_Op op() const;

private:
    Ausdruck_Binär::Binär_Op _op;
    Bestimmter_Ausdruck *_links;
    Bestimmter_Ausdruck *_rechts;
};

class Bestimmter_Ausdruck_Unär : public Bestimmter_Ausdruck
{
public:
    Bestimmter_Ausdruck_Unär(Ausdruck_Unär *asb_knoten, Operand *operand, Ausdruck_Unär::Unär_Op op,
        Bestimmter_Ausdruck *ausdruck);

    Ausdruck_Unär::Unär_Op op() const;
    Bestimmter_Ausdruck * ausdruck() const;

private:
    Ausdruck_Unär::Unär_Op _op;
    Bestimmter_Ausdruck *_ausdruck;
};

class Bestimmter_Ausdruck_Index : public Bestimmter_Ausdruck
{
public:
    Bestimmter_Ausdruck_Index(Ausdruck_Index *asb_knoten, Operand *operand, Bestimmter_Ausdruck *basis, Bestimmter_Ausdruck *index);

    Bestimmter_Ausdruck * basis() const;
    Bestimmter_Ausdruck * index() const;

private:
    Bestimmter_Ausdruck *_basis;
    Bestimmter_Ausdruck *_index;
};

class Bestimmter_Ausdruck_Eigenschaft : public Bestimmter_Ausdruck
{
public:
    Bestimmter_Ausdruck_Eigenschaft(Ausdruck_Eigenschaft *asb_knoten, Operand *operand, Bestimmter_Ausdruck *basis, Symbol *eigenschaft);

    Bestimmter_Ausdruck * basis() const;
    Symbol * eigenschaft() const;

private:
    Bestimmter_Ausdruck *_basis;
    Symbol *_eigenschaft;
};

class Bestimmter_Ausdruck_Aufruf : public Bestimmter_Ausdruck
{
public:
    Bestimmter_Ausdruck_Aufruf(Ausdruck_Aufruf *asb_knoten, Operand *operand, Bestimmter_Ausdruck *basis, std::vector<Bestimmter_Ausdruck *> argumente);

    Bestimmter_Ausdruck * basis() const;
    std::vector<Bestimmter_Ausdruck *> argumente() const;

private:
    Bestimmter_Ausdruck *_basis;
    std::vector<Bestimmter_Ausdruck *> _argumente;
};

class Bestimmter_Ausdruck_Ausführen : public Bestimmter_Ausdruck
{
public:
    Bestimmter_Ausdruck_Ausführen(Ausdruck_Ausführen *asb_knoten, Operand *operand, Bestimmter_Ausdruck *ausdruck);

    Bestimmter_Ausdruck * ausdruck() const;

private:
    Bestimmter_Ausdruck *_ausdruck;
};

class Bestimmter_Ausdruck_Reihe : public Bestimmter_Ausdruck
{
public:
    Bestimmter_Ausdruck_Reihe(Ausdruck_Reihe *asb_knoten, Operand *operand, Bestimmter_Ausdruck *min, Bestimmter_Ausdruck *max);

    Bestimmter_Ausdruck * min() const;
    Bestimmter_Ausdruck * max() const;

private:
    Bestimmter_Ausdruck *_min;
    Bestimmter_Ausdruck *_max;
};

class Bestimmter_Ausdruck_Kompositum : public Bestimmter_Ausdruck
{
public:
    class Bestimmte_Eigenschaft
    {
    public:
        Bestimmte_Eigenschaft(Operand *operand);

        Operand * operand() const;

    private:
        Operand *_operand;
    };

    Bestimmter_Ausdruck_Kompositum(Ausdruck_Kompositum *asb_knoten, Operand *operand, std::vector<Bestimmte_Eigenschaft *> eigenschaften);

    std::vector<Bestimmte_Eigenschaft *> eigenschaften() const;

private:
    std::vector<Bestimmte_Eigenschaft *> _eigenschaften;
};
// }}}
// deklarationen {{{
class Bestimmte_Deklaration : public Bestimmter_Knoten
{
public:
    Bestimmte_Deklaration(Deklaration *asb_knoten, Asb_Knoten::Art art, Symbol *symbol, Datentyp *datentyp, Bestimmter_Ausdruck *initialisierung = nullptr);
    Bestimmte_Deklaration(Deklaration *asb_knoten, std::vector<Symbol *> symbole, Datentyp *datentyp, Bestimmter_Ausdruck *initialisierung = nullptr);

    Symbol * symbol() const;
    std::vector<Symbol *> symbole() const;
    Datentyp * datentyp() const;
    Bestimmter_Ausdruck * initialisierung() const;

private:
    std::vector<Symbol *> _symbole;
    Datentyp *_datentyp;
    Bestimmter_Ausdruck *_initialisierung;
};

class Bestimmte_Deklaration_Funktion : public Bestimmte_Deklaration
{
public:
    Bestimmte_Deklaration_Funktion(Deklaration *deklaration, Symbol *symbol, Datentyp *datentyp, Bestimmte_Anweisung * rumpf);

    Bestimmte_Anweisung * rumpf() const;
    void rumpf_setzen(Bestimmte_Anweisung *rumpf);

private:
    Bestimmte_Anweisung * _rumpf;
};

class Bestimmte_Deklaration_Option : public Bestimmte_Deklaration
{
public:
    Bestimmte_Deklaration_Option(Deklaration *deklaration, Symbol *symbol, Datentyp *datentyp);
};

class Bestimmte_Deklaration_Schablone : public Bestimmte_Deklaration
{
public:
    Bestimmte_Deklaration_Schablone(Deklaration *deklaration, Symbol *symbol, Datentyp *datentyp);
};
// }}}
// anweisungen {{{
class Bestimmte_Anweisung : public Bestimmter_Knoten
{
public:
    Bestimmte_Anweisung(Anweisung *asb_knoten, Asb_Knoten::Art art);
};

class Bestimmte_Anweisung_Deklaration : public Bestimmte_Anweisung
{
public:
    Bestimmte_Anweisung_Deklaration(Anweisung *asb_knoten, Bestimmte_Deklaration *deklaration);

    Bestimmte_Deklaration * deklaration() const;

private:
    Bestimmte_Deklaration * _deklaration;
};

class Bestimmte_Anweisung_Ausdruck : public Bestimmte_Anweisung
{
public:
    Bestimmte_Anweisung_Ausdruck(Anweisung *asb_knoten, Bestimmter_Ausdruck *ausdruck);

    Bestimmter_Ausdruck * ausdruck() const;

private:
    Bestimmter_Ausdruck *_ausdruck;
};

class Bestimmte_Anweisung_Block : public Bestimmte_Anweisung
{
public:
    Bestimmte_Anweisung_Block(Anweisung *asb_knoten, std::vector<Bestimmte_Anweisung *> anweisungen);

    std::vector<Bestimmte_Anweisung *> anweisungen() const;

private:
    std::vector<Bestimmte_Anweisung *> _anweisungen;
};

class Bestimmte_Anweisung_Für : public Bestimmte_Anweisung
{
public:
    Bestimmte_Anweisung_Für(Anweisung *asb_knoten, Symbol *index, Bestimmter_Ausdruck *bedingung, Bestimmte_Anweisung *rumpf, std::string weiter_markierung, std::string raus_markierung);

    Symbol * index() const;
    Bestimmter_Ausdruck * bedingung() const;
    Bestimmte_Anweisung * rumpf() const;

    std::string weiter_markierung() const;
    std::string raus_markierung() const;

private:
    Symbol *_index;
    Bestimmter_Ausdruck *_bedingung;
    Bestimmte_Anweisung *_rumpf;
    std::string _weiter_markierung;
    std::string _raus_markierung;
};

class Bestimmte_Anweisung_Res : public Bestimmte_Anweisung
{
public:
    Bestimmte_Anweisung_Res(Anweisung *asb_knoten, Bestimmter_Ausdruck *ausdruck);

    Bestimmter_Ausdruck * ausdruck() const;

private:
    Bestimmter_Ausdruck *_ausdruck;
};

class Bestimmte_Anweisung_Wenn : public Bestimmte_Anweisung
{
public:
    Bestimmte_Anweisung_Wenn(Anweisung *asb_knoten, Bestimmter_Ausdruck *bedingung, Bestimmte_Anweisung *rumpf, Bestimmte_Anweisung *sonst = nullptr);

    Bestimmter_Ausdruck * bedingung() const;
    Bestimmte_Anweisung * rumpf() const;
    Bestimmte_Anweisung * sonst() const;

private:
    Bestimmter_Ausdruck *_bedingung;
    Bestimmte_Anweisung *_rumpf;
    Bestimmte_Anweisung *_sonst;
};

class Bestimmte_Anweisung_Solange : public Bestimmte_Anweisung
{
public:
    Bestimmte_Anweisung_Solange(Anweisung *asb_knoten, Bestimmter_Ausdruck *bedingung, Bestimmte_Anweisung *rumpf, std::string weiter_markierung, std::string raus_markierung);

    Bestimmter_Ausdruck * bedingung() const;
    Bestimmte_Anweisung * rumpf() const;

    std::string weiter_markierung() const;
    std::string raus_markierung() const;

private:
    Bestimmter_Ausdruck *_bedingung;
    Bestimmte_Anweisung *_rumpf;

    std::string _weiter_markierung;
    std::string _raus_markierung;
};

class Bestimmte_Anweisung_Sprung : public Bestimmte_Anweisung
{
public:
    Bestimmte_Anweisung_Sprung(Anweisung *asb_knoten, std::string markierung);

    std::string markierung() const;

private:
    std::string _markierung;
};

class Bestimmte_Anweisung_Bedingter_Sprung : public Bestimmte_Anweisung
{
public:
    Bestimmte_Anweisung_Bedingter_Sprung(Anweisung *asb_knoten, std::string markierung, Bestimmter_Ausdruck *bedingung);

    std::string markierung() const;
    Bestimmter_Ausdruck * bedingung() const;

private:
    std::string _markierung;
    Bestimmter_Ausdruck *_bedingung;
};

class Bestimmte_Anweisung_Markierung : public Bestimmte_Anweisung
{
public:
    Bestimmte_Anweisung_Markierung(Anweisung *asb_knoten, std::string markierung);

    std::string markierung() const;

private:
    std::string _markierung;
};

class Bestimmte_Anweisung_Weiche : public Bestimmte_Anweisung
{
public:
    Bestimmte_Anweisung_Weiche(Anweisung *asb_knoten, Bestimmter_Ausdruck *ausdruck, std::vector<Bestimmtes_Muster *> muster);

    Bestimmter_Ausdruck * ausdruck() const;
    std::vector<Bestimmtes_Muster *> muster() const;

private:
    Bestimmter_Ausdruck *_ausdruck;
    std::vector<Bestimmtes_Muster *> _muster;
};

class Bestimmte_Anweisung_Zuweisung : public Bestimmte_Anweisung
{
public:
    Bestimmte_Anweisung_Zuweisung(Anweisung *asb_knoten, Bestimmter_Ausdruck *links, Glied *op, Bestimmter_Ausdruck *rechts);

    Bestimmter_Ausdruck * links() const;
    Bestimmter_Ausdruck * rechts() const;
    Glied               * op() const;

private:
    Bestimmter_Ausdruck *_links;
    Bestimmter_Ausdruck *_rechts;
    Glied               *_op;
};

class Bestimmte_Anweisung_Final : public Bestimmte_Anweisung
{
public:
    Bestimmte_Anweisung_Final(Anweisung *asb_knoten, Bestimmte_Anweisung *anweisung);

    Bestimmte_Anweisung * anweisung() const;

private:
    Bestimmte_Anweisung *_anweisung;
};
// }}}
class Bestimmter_Asb
{
public:
    Bestimmter_Asb(Zone *system, Zone *global);

    Zone * global() const;
    Zone * system() const;
    std::vector<Bestimmte_Anweisung *> anweisungen() const;
    void anweisungen_setzen(std::vector<Bestimmte_Anweisung *> anweisungen);

private:
    Zone *_system;
    Zone *_global;
    std::vector<Bestimmte_Anweisung *> _anweisungen;
};

}

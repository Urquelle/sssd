#pragma once

#include <vector>

#include "spanne.hpp"
#include "glied.hpp"
#include "symbol.hpp"

#define Ast_Knoten_Art \
    X(AUSDRUCK_GANZZAHL,             1, "Ausdruck Ganzzahl") \
    X(AUSDRUCK_DEZIMALZAHL,          2, "Ausdruck Dezimalzahl") \
    X(AUSDRUCK_TEXT,                 3, "Ausdruck Text") \
    X(AUSDRUCK_BEZEICHNER,           4, "Ausdruck Bezeichner") \
    X(AUSDRUCK_KLAMMER,              5, "Ausdruck Klammer") \
    X(AUSDRUCK_BINÄR,                6, "Ausdruck Binär") \
    X(AUSDRUCK_UNÄR,                 7, "Ausdruck Unär") \
    X(AUSDRUCK_EIGENSCHAFT,          8, "Ausdruck Eigenschaft") \
    X(AUSDRUCK_INDEX,                9, "Ausdruck Index") \
    X(AUSDRUCK_AUFRUF,              10, "Ausdruck Aufruf") \
    X(AUSDRUCK_REIHE,               11, "Ausdruck Reihe") \
    X(AUSDRUCK_BRAUCHE,             12, "Ausdruck Brauche") \
    X(AUSDRUCK_KOMPOSITUM,          13, "Ausdruck Kompositum") \
    \
    X(SPEZIFIZIERUNG_BEZEICHNER,    20, "Spezifizierung Name") \
    X(SPEZIFIZIERUNG_FELD,          21, "Spezifizierung Feld") \
    X(SPEZIFIZIERUNG_ZEIGER,        22, "Spezifizierung Zeiger") \
    X(SPEZIFIZIERUNG_FUNKTION,      23, "Spezifizierung Funktion") \
    \
    X(DEKLARATION_VARIABLE,         30, "Deklaration Variable") \
    X(DEKLARATION_SCHABLONE,        31, "Deklaration Schablone") \
    X(DEKLARATION_FUNKTION,         32, "Deklaration Funktion") \
    X(DEKLARATION_BRAUCHE,          33, "Deklaration Brauche") \
    X(DEKLARATION_OPTION,           34, "Deklaration Option") \
    \
    X(ANWEISUNG_ZUWEISUNG,          40, "Anweisung Zuweisung") \
    X(ANWEISUNG_WENN,               41, "Anweisung Wenn") \
    X(ANWEISUNG_FÜR,                42, "Anweisung Für") \
    X(ANWEISUNG_BLOCK,              43, "Anweisung Block") \
    X(ANWEISUNG_DEKLARATION,        44, "Anweisung Deklaration") \
    X(ANWEISUNG_BRAUCHE,            45, "Anweisung Brauche") \
    X(ANWEISUNG_LADE,               46, "Anweisung Lade") \
    X(ANWEISUNG_AUSDRUCK,           47, "Anweisung Ausdruck") \
    X(ANWEISUNG_DANACH,             48, "Anweisung Danach") \
    X(ANWEISUNG_WEICHE,             49, "Anweisung Weiche") \
    X(ANWEISUNG_RES,                50, "Anweisung Res")

namespace Sss::Kmp {

class Anweisung;
class Ausdruck;
class Deklaration;
class Spezifizierung;
class Operand;

class Ast_Knoten
{
public:
    enum Art
    {
    #define X(N, W, ...) N = W,
        Ast_Knoten_Art
    #undef X
    };

    int32_t id() const;
    Art art() const;
    Spanne spanne() const;

    template<typename T> T als();

    virtual void ausgeben(int32_t tiefe, std::ostream &ausgabe);

protected:
    Ast_Knoten(Art art, Spanne spanne);

private:
    Spanne _spanne;
    Art _art;
    int32_t _id;
};

struct Ast
{
    std::vector<Ast_Knoten *> anweisungen;
};

class Marke
{
public:
    Marke(Spanne spanne, std::string wert);

    Spanne spanne() const;
    std::string wert() const;

private:
    Spanne _spanne;
    std::string _wert;
};

class Muster
{
public:
    Muster(Spanne spanne, Ausdruck *muster, Anweisung *anweisung);

    Spanne spanne() const;
    Ausdruck *muster() const;
    Anweisung *anweisung() const;

private:
    Spanne _spanne;
    Ausdruck *_muster;
    Anweisung *_anweisung;
};

/* ausdruck {{{ */
class Ausdruck : public Ast_Knoten
{
public:
protected:
    Ausdruck(Ast_Knoten::Art art, Spanne spanne);
};

class Ausdruck_Ganzzahl : public Ausdruck
{
public:
    Ausdruck_Ganzzahl(Spanne spanne, int32_t wert);

    int32_t wert() const;

    void ausgeben(int32_t tiefe, std::ostream &ausgabe) override;

private:
    int32_t _wert;
};

class Ausdruck_Dezimalzahl : public Ausdruck
{
public:
    Ausdruck_Dezimalzahl(Spanne spanne, float wert);

    float wert() const;

    void ausgeben(int32_t tiefe, std::ostream &ausgabe) override;

private:
    float _wert;
};

class Ausdruck_Bezeichner : public Ausdruck
{
public:
    Ausdruck_Bezeichner(Spanne spanne, std::string wert);

    std::string wert() const;

    void ausgeben(int32_t tiefe, std::ostream &ausgabe) override;

private:
    std::string _wert;
};

class Ausdruck_Text : public Ausdruck
{
public:
    Ausdruck_Text(Spanne spanne, std::string wert);

    std::string wert() const;

    void ausgeben(int32_t tiefe, std::ostream &ausgabe) override;

private:
    std::string _wert;
};

#define Bin_Op_Liste \
    X(UNBEKANNT,       0, "Unbekannt") \
    X(ADDITION,        1, "Addition") \
    X(SUBTRAKTION,     2, "Subtraktion") \
    X(MULTIPLIKATION,  3, "Multiplikation") \
    X(DIVISION,        4, "Division") \
    X(MODULO,          5, "Modulo") \
    X(GLEICH,          6, "Gleich") \
    X(UNGLEICH,        7, "Ungleich") \
    X(KLEINER,         8, "Kleiner") \
    X(KLEINER_GLEICH,  9, "Kleinergleich") \
    X(GRÖẞER,         10, "Größer") \
    X(GRÖẞER_GLEICH,  11, "Größergleich") \
    X(BIT_SCHIEB_L,   12, "Bit verschieben links") \
    X(BIT_SCHIEB_R,   13, "Bit verschieben rechts")

class Ausdruck_Binär : public Ausdruck
{
public:
    enum Binär_Op
    {
    #define X(N, W, ...) N = W,
        Bin_Op_Liste
    #undef X
    };

    Ausdruck_Binär(Spanne spanne, Binär_Op op, Ausdruck *links, Ausdruck *rechts);

    Binär_Op     op() const;
    Ausdruck * links() const;
    Ausdruck * rechts() const;

    void ausgeben(int32_t tiefe, std::ostream &ausgabe) override;

private:
    Binär_Op _op;
    Ausdruck *_links;
    Ausdruck *_rechts;
};

#define Unär_Op_Liste \
    X(UNBEKANNT,    0, "Unbekannt") \
    X(MINUS,        1, "Minus") \
    X(NEGIERUNG,    2, "Negierung") \
    X(INVERTIERUNG, 3, "Invertierung")

class Ausdruck_Unär : public Ausdruck
{
public:
    enum Unär_Op
    {
    #define X(N, W, ...) N = W,
        Unär_Op_Liste
    #undef X
    };

    Ausdruck_Unär(Spanne spanne, Unär_Op op, Ausdruck *ausdruck);

    Unär_Op op() const;
    Ausdruck * ausdruck() const;

    void ausgeben(int32_t tiefe, std::ostream &ausgabe) override;

private:
    Unär_Op _op;
    Ausdruck *_ausdruck;
};

class Ausdruck_Klammer : public Ausdruck
{
public:
    Ausdruck_Klammer(Spanne spanne, Ausdruck *ausdruck);

    Ausdruck * ausdruck() const;

    void ausgeben(int32_t tiefe, std::ostream &ausgabe) override;

private:
    Ausdruck *_ausdruck;
};

class Ausdruck_Index : public Ausdruck
{
public:
    Ausdruck_Index(Spanne spanne, Ausdruck *basis, Ausdruck *index);

    Ausdruck * basis() const;
    Ausdruck * index() const;

    void ausgeben(int32_t tiefe, std::ostream &ausgabe) override;

private:
    Ausdruck *_basis;
    Ausdruck *_index;
};

class Ausdruck_Eigenschaft : public Ausdruck
{
public:
    Ausdruck_Eigenschaft(Spanne spanne, Ausdruck *basis, Ausdruck *feld);

    Ausdruck * basis() const;
    Ausdruck * eigenschaft() const;

    void ausgeben(int32_t tiefe, std::ostream &ausgabe) override;

private:
    Ausdruck *_basis;
    Ausdruck * _eigenschaft;
};

class Ausdruck_Aufruf : public Ausdruck
{
public:
    Ausdruck_Aufruf(Spanne spanne, Ausdruck *basis, std::vector<Ausdruck *> argumente);

    Ausdruck * basis() const;
    std::vector<Ausdruck *> argumente() const;

    void ausgeben(int32_t tiefe, std::ostream &ausgabe) override;

private:
    Ausdruck *_basis;
    std::vector<Ausdruck *> _argumente;
};

class Ausdruck_Reihe : public Ausdruck
{
public:
    Ausdruck_Reihe(Spanne spanne, Ausdruck *min, Ausdruck *max);

    Ausdruck * min() const;
    Ausdruck * max() const;

    void ausgeben(int32_t tiefe, std::ostream &ausgabe) override;

private:
    Ausdruck *_min;
    Ausdruck *_max;
};

class Ausdruck_Brauche : public Ausdruck
{
public:
    Ausdruck_Brauche(Spanne spanne, std::string dateiname);

    std::string dateiname() const;

private:
    std::string _dateiname;
};

class Ausdruck_Kompositum : public Ausdruck
{
public:
    class Eigenschaft
    {
    public:
        Eigenschaft(Spanne spanne, std::string name, Ausdruck *ausdruck);
        Eigenschaft(Spanne spanne, Ausdruck *ausdruck);

        std::string name() const;
        Ausdruck *ausdruck() const;
        bool ist_benamt() const;
        Spanne spanne() const;

    private:
        Spanne _spanne;
        std::string _name;
        Ausdruck *_ausdruck;
        bool _ist_benamt;
    };

    Ausdruck_Kompositum(Spanne spanne, std::vector<Eigenschaft *> eigenschaften,
                        bool ist_benamt, Spezifizierung *spezifizierung = nullptr);

    std::vector<Eigenschaft *> eigenschaften() const;
    Spezifizierung *spezifizierung() const;
    bool ist_benamt() const;

private:
    std::vector<Eigenschaft *> _eigenschaften;
    Spezifizierung *_spezifizierung;
    bool _ist_benamt;
};
/* }}} */
// spezifizierung {{{
class Spezifizierung : public Ast_Knoten
{
public:
    Spezifizierung(Ast_Knoten::Art art, Spanne spanne);

    int32_t größe() const;
    void größe_setzen(int32_t größe);

private:
    int32_t _größe;
};

class Spezifizierung_Bezeichner : public Spezifizierung
{
public:
    Spezifizierung_Bezeichner(Spanne spanne, std::string name);

    std::string name() const;

    void ausgeben(int32_t tiefe, std::ostream &ausgabe) override;

private:
    std::string _name;
};

class Spezifizierung_Feld : public Spezifizierung
{
public:
    Spezifizierung_Feld(Spanne spanne, Spezifizierung *basis, Ausdruck *index);

    Spezifizierung * basis() const;
    Ausdruck * index() const;

    void ausgeben(int32_t tiefe, std::ostream &ausgabe) override;

private:
    Spezifizierung *_basis;
    Ausdruck *_index;
};

class Spezifizierung_Zeiger : public Spezifizierung
{
public:
    Spezifizierung_Zeiger(Spanne spanne, Spezifizierung *basis);

    Spezifizierung *basis() const;

    void ausgeben(int32_t tiefe, std::ostream &ausgabe) override;

private:
    Spezifizierung *_basis;
};

class Spezifizierung_Funktion : public Spezifizierung
{
public:
    Spezifizierung_Funktion(Spanne spanne, std::vector<Spezifizierung *> parameter, Spezifizierung *rückgabe);

    std::vector<Spezifizierung *> parameter() const;
    Spezifizierung *rückgabe() const;

    void ausgeben(int32_t tiefe, std::ostream &ausgabe) override;

private:
    std::vector<Spezifizierung *> _parameter;
    Spezifizierung *_rückgabe;
};
// }}}
// deklaration {{{
class Deklaration : public Ast_Knoten
{
public:
    Deklaration(Ast_Knoten::Art art, Spanne spanne, std::vector<std::string> namen);
    Deklaration(Ast_Knoten::Art art, Spanne spanne, std::string name);

    std::vector<std::string> namen() const;
    Spezifizierung * spezifizierung() const;
    Symbol * symbol() const;
    void symbol_setzen(Symbol *symbol);

protected:
    std::vector<std::string> _namen;
    Spezifizierung *_spezifizierung;
    Symbol *_symbol;
};

class Deklaration_Variable : public Deklaration
{
public:
    Deklaration_Variable(Spanne spanne, std::vector<std::string> namen,
                         Spezifizierung *spezifizierung, Ausdruck *initialisierung);

    Ausdruck * initialisierung() const;

    void ausgeben(int32_t tiefe, std::ostream &ausgabe) override;

private:
    Ausdruck *_initialisierung;
};

class Deklaration_Funktion : public Deklaration
{
public:
    Deklaration_Funktion(Spanne spanne, std::string name, std::vector<Deklaration *> parameter,
                         Spezifizierung *rückgabe, Anweisung *rumpf);

    std::vector<Deklaration *> parameter() const;
    Spezifizierung * rückgabe() const;
    Anweisung * rumpf() const;

    void ausgeben(int32_t tiefe, std::ostream &ausgabe) override;

private:
    std::vector<Deklaration *> _parameter;
    Spezifizierung *_rückgabe;
    Anweisung *_rumpf;
};

class Deklaration_Schablone : public Deklaration
{
public:
    Deklaration_Schablone(Spanne spanne, std::string name, std::vector<Deklaration *> eigenschaften);

    std::vector<Deklaration *> eigenschaften() const;

    void ausgeben(int32_t tiefe, std::ostream &ausgabe) override;

private:
    std::vector<Deklaration *> _eigenschaften;
};

class Deklaration_Brauche : public Deklaration
{
public:
    Deklaration_Brauche(Spanne spanne, std::string name, Ausdruck_Brauche *ausdruck);

    Ausdruck_Brauche * ausdruck() const;
    std::string name() const;

private:
    Ausdruck_Brauche *_ausdruck;
};

class Deklaration_Option : public Deklaration
{
public:
    Deklaration_Option(Spanne spanne, std::string name, std::vector<Deklaration_Variable *> eigenschaften);

    std::vector<Deklaration_Variable *> eigenschaften() const;

private:
    std::vector<Deklaration_Variable *> _eigenschaften;
};
// }}}
/* anweisungen {{{ */
class Anweisung : public Ast_Knoten
{
public:
    Anweisung(Ast_Knoten::Art art, Spanne spanne);

    void marken_setzen(std::vector<Marke *> marken);

    void ausgeben(int32_t tiefe, std::ostream &ausgabe) override;

private:
    std::vector<Marke *> _marken;
};

class Anweisung_Zuweisung : public Anweisung
{
public:
    Anweisung_Zuweisung(Spanne spanne, Ausdruck *links, Glied *op, Ausdruck *rechts);

    Glied * op() const;
    Ausdruck * links() const;
    Ausdruck * rechts() const;

    void ausgeben(int32_t tiefe, std::ostream &ausgabe) override;

private:
    Glied *_op;
    Ausdruck *_links;
    Ausdruck *_rechts;
};

class Anweisung_Wenn : public Anweisung
{
public:
    Anweisung_Wenn(Spanne spanne, Ausdruck *bedingung, Anweisung *rumpf, Anweisung_Wenn *sonst);

    Ausdruck * bedingung() const;
    Anweisung * rumpf() const;
    Anweisung_Wenn * sonst() const;

    void ausgeben(int32_t tiefe, std::ostream &ausgabe) override;

private:
    Ausdruck *_bedingung;
    Anweisung *_rumpf;
    Anweisung_Wenn *_sonst;
};

class Anweisung_Für : public Anweisung
{
public:
    Anweisung_Für(Spanne spanne, Ausdruck *index, Ausdruck *bedingung, Anweisung *rumpf);

    Ausdruck * index() const;
    Ausdruck * bedingung() const;
    Anweisung * rumpf() const;

    void ausgeben(int32_t tiefe, std::ostream &ausgabe) override;

private:
    Ausdruck *_index;
    Ausdruck *_bedingung;
    Anweisung *_rumpf;
};

class Anweisung_Block : public Anweisung
{
public:
    Anweisung_Block(Spanne spanne, std::vector<Anweisung *> anweisungen);

    std::vector<Anweisung *> anweisungen() const;

    void ausgeben(int32_t tiefe, std::ostream &ausgabe) override;

private:
    std::vector<Anweisung *> _anweisungen;
};

class Anweisung_Deklaration : public Anweisung
{
public:
    Anweisung_Deklaration(Spanne spanne, Deklaration *deklaration);

    Deklaration *deklaration() const;

    void ausgeben(int32_t tiefe, std::ostream &ausgabe) override;

private:
    Deklaration *_deklaration;
};

class Anweisung_Brauche : public Anweisung
{
public:
    Anweisung_Brauche(Spanne spanne, std::string dateiname);

    std::string dateiname() const;

    void ausgeben(int32_t tiefe, std::ostream &ausgabe) override;

private:
    std::string _dateiname;
};

class Anweisung_Lade : public Anweisung
{
public:
    Anweisung_Lade(Spanne spanne, std::string dateiname);

    std::string dateiname() const;

private:
    std::string _dateiname;
};

class Anweisung_Ausdruck : public Anweisung
{
public:
    Anweisung_Ausdruck(Spanne spanne, Ausdruck *ausdruck);

    Ausdruck * ausdruck() const;

    void ausgeben(int32_t tiefe, std::ostream &ausgabe) override;

private:
    Ausdruck *_ausdruck;
};

class Anweisung_Danach : public Anweisung
{
public:
    Anweisung_Danach(Spanne spanne, Anweisung *anweisung);

    Anweisung *anweisung() const;

private:
    Anweisung *_anweisung;
};

class Anweisung_Weiche : public Anweisung
{
public:
    Anweisung_Weiche(Spanne spanne, Ausdruck *ausdruck, std::vector<Muster *> muster);

    Ausdruck *ausdruck() const;
    std::vector<Muster *> muster() const;

private:
    Ausdruck *_ausdruck;
    std::vector<Muster *> _muster;
};

class Anweisung_Res : public Anweisung
{
public:
    Anweisung_Res(Spanne spanne, Ausdruck *ausdruck);

    Ausdruck *ausdruck() const;

private:
    Ausdruck *_ausdruck;
};
/* }}} */

}

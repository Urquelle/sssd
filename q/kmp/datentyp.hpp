#pragma once

#include <vector>
#include <ostream>

namespace Sss::Kmp {

#define Datentyp_Status_Liste \
    X(UNBEARBEITET,       1, "Unbearbeitet") \
    X(IN_BEARBEITUNG,     2, "In Bearbeitung") \
    X(BEARBEITET,         3, "Bearbeitet")

#define Datentyp_Art_Liste \
    X(GANZE_ZAHL,         1, "Ganze Zahl") \
    X(DEZIMAL_ZAHL,       2, "Dezimal Zahl") \
    X(TEXT,               3, "Text") \
    X(ZEIGER,             4, "Zeiger") \
    X(SCHABLONE,          5, "Schablone") \
    X(FUNKTION,           6, "Funktion") \
    X(BOOL,               7, "Bool") \
    X(AUFZÄHLUNG,         8, "Aufzählung") \
    X(FELD,               9, "Feld") \
    X(NICHTS,            10, "Nichts") \
    X(OPT,               11, "Opt")
#if 0
    TYPE_VOID,
    TYPE_CHAR,
    TYPE_NAMESPACE,
    TYPE_COMPOUND,
    TYPE_VARIADIC,
    TYPE_UNION,
#endif

class Ausdruck;
class Deklaration_Schablone;
class Deklaration_Opt;

class Datentyp
{
public:
    enum Art
    {
    #define X(N, W, ...) N = W,
        Datentyp_Art_Liste
    #undef X
    };

    enum Status
    {
    #define X(N, W, ...) N = W,
        Datentyp_Status_Liste
    #undef X
    };

    Datentyp(Art art, size_t größe);

    Art art() const;
    Status status() const;
    bool abgeschlossen() const;
    void abschließen();

    size_t größe() const;
    void größe_setzen(size_t größe);

    template<typename T> T als();
    virtual void ausgeben(int32_t tiefe, std::ostream &ausgabe);

protected:
    Art _art;
    Status _status;
    uint32_t _merkmale;
    size_t _größe;
    bool _abgeschlossen { false };
};

class Datentyp_Feld : public Datentyp
{
public:
    Datentyp_Feld(Datentyp *basis, Datentyp *index);

    Datentyp *basis() const;
    Datentyp *index() const;

private:
    Datentyp *_basis;
    Datentyp *_index;
};

class Datentyp_Zeiger : public Datentyp
{
public:
    Datentyp_Zeiger(Datentyp *basis);

    Datentyp *basis() const;

private:
    Datentyp *_basis;
};

class Datentyp_Schablone : public Datentyp
{
public:
    class Eigenschaft
    {
    public:
        Eigenschaft(Datentyp *datentyp, size_t versatz, std::string name);

        Datentyp *datentyp() const;
        size_t versatz() const;
        std::string name() const;

    private:
        Datentyp *_datentyp;
        size_t _versatz;
        std::string _name;
    };

    Datentyp_Schablone(Deklaration_Schablone *deklaration);

    std::vector<Eigenschaft *> eigenschaften() const;
    void eigenschaft_hinzufügen(Datentyp *datentyp, size_t versatz, std::string name);
    Deklaration_Schablone *deklaration() const;

private:
    std::vector<Eigenschaft *> _eigenschaften;
    Deklaration_Schablone *_deklaration;
};

class Datentyp_Funktion : public Datentyp
{
public:
    Datentyp_Funktion();
    Datentyp_Funktion(std::vector<Datentyp *> parameter, Datentyp *rückgabe);

    std::vector<Datentyp *> parameter() const;
    void parameter_hinzufügen(Datentyp *datentyp);
    Datentyp *rückgabe() const;
    void rückgabe_setzen(Datentyp *datentyp);

private:
    std::vector<Datentyp *> _parameter;
    Datentyp *_rückgabe;
};

class Datentyp_Opt : public Datentyp
{
public:
    class Eigenschaft
    {
    public:
        Eigenschaft(std::string name, Ausdruck *wert = nullptr);
    };

    Datentyp_Opt(Datentyp *basis = nullptr);

private:
    Datentyp *_basis { nullptr };
    std::vector<Eigenschaft *> _eigenschaften;
};

}

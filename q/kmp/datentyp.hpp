#pragma once

#include <vector>
#include <ostream>

namespace Sss::Kmp {

#define Datentyp_Status_Liste \
    X(UNBEHANDELT,       1, "Unbehandelt") \
    X(IN_BEHANDLUNG,     2, "In Behandlung") \
    X(BEHANDELT,         3, "Behandelt")

#define Datentyp_Art_Liste \
    X(GANZE_ZAHL,         1, "Ganze Zahl") \
    X(DEZIMAL_ZAHL,       2, "Dezimal Zahl") \
    X(TEXT,               3, "Text") \
    X(ZEIGER,             4, "Zeiger") \
    X(SCHABLONE,          5, "Schablone") \
    X(FUNKTION,           6, "Funktion") \
    X(BOOL,               7, "Wahrheitswert") \
    X(AUFZÄHLUNG,         8, "Aufzählung") \
    X(FELD,               9, "Feld") \
    X(NIHIL,             10, "Nihil") \
    X(OPTION,            11, "Option") \

#define Kompatibilität_Liste \
    X(INKOMPATIBEL,         0, "Inkompatibel") \
    X(KOMPATIBEL,           1, "Kompatibel") \
    X(KOMPATIBEL_IMPLIZIT,  2, "Implizit") \
    X(KOMPATIBEL_MIT_DATENVERLUST, 3, "Kompatibel mit Datenverlust") \

class Ausdruck;
class Deklaration;
class Deklaration_Schablone;
class Deklaration_Option;
class Operand;
class Symbol;

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

    enum Kompatibilität
    {
    #define X(N, W, ...) N = W,
        Kompatibilität_Liste
    #undef X
    };

    static Kompatibilität datentypen_kompatibel(Datentyp *ziel, Datentyp *quelle, bool implizit = true);

    Datentyp(Art art, size_t größe, bool abgeschlossen = false);

    Art art() const;
    bool abgeschlossen() const;
    void abschließen();

    Status status() const;
    void status_setzen(Status status);

    size_t größe() const;
    void größe_setzen(size_t größe);

    Symbol * symbol() const;
    void symbol_setzen(Symbol *symbol);

    Deklaration * deklaration() const;
    void deklaration_setzen(Deklaration *deklaration);

    template<typename T> T als();
    virtual void ausgeben(int32_t tiefe, std::ostream &ausgabe);

protected:
    Art _art;
    Status _status;
    uint32_t _merkmale;
    Symbol * _symbol;
    Deklaration * _deklaration;
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
    bool eigenschaft_hinzufügen(Datentyp *datentyp, size_t versatz, std::string name);
    Deklaration_Schablone *deklaration() const;

private:
    std::vector<Eigenschaft *> _eigenschaften;
    Deklaration_Schablone *_deklaration;
};

class Datentyp_Anon : public Datentyp
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

    Datentyp_Anon(std::vector<Eigenschaft *> eigenschaften);

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

class Datentyp_Option : public Datentyp
{
public:
    class Eigenschaft
    {
    public:
        Eigenschaft(Datentyp *datentyp, Operand *operand, std::string& name);

    private:
        Datentyp *_datentyp;
        Operand *_operand;
        std::string _name;
    };

    Datentyp_Option(Datentyp *basis = nullptr);

    void eigenschaft_hinzufügen(Datentyp *datentyp, Operand *operand, std::string& name);

private:
    Datentyp *_basis { nullptr };
    std::vector<Eigenschaft *> _eigenschaften;
};

}

#pragma once

#include "spanne.hpp"

#define Token_Art \
    X(ENDE,            0, "<EOF>") \
    X(GANZZAHL,        1, "Ganzzahl") \
    X(DEZIMALZAHL,     2, "Dezimalzahl") \
    X(BEZEICHNER,      3, "Bezeichner") \
    X(TEXT,            4, "Text") \
    X(RKLAMMER_AUF,    5, "Runde Klammer auf") \
    X(RKLAMMER_ZU,     6, "Runde Klammer zu") \
    X(EKLAMMER_AUF,    7, "Eckige Klammer auf") \
    X(EKLAMMER_ZU,     8, "Eckige Klammer zu") \
    X(GKLAMMER_AUF,    9, "Geschweifte Klammer auf") \
    X(GKLAMMER_ZU,    10, "Geschweifte Klammer zu") \
    X(KLAMMERAFFE,    11, "Klammeraffe") \
    X(PUNKT,          12, "Punkt") \
    X(KOMMA,          13, "Komma") \
    X(STRICHPUNKT,    14, "Strichpunkt") \
    X(DOPPELPUNKT,    15, "Doppelpunkt") \
    X(PLUS,           16, "Plus") \
    X(MINUS,          17, "Minus") \
    X(STERN,          18, "Stern") \
    X(PISA,           19, "Pisa") \
    X(RAUTE,          20, "Raute") \
    X(AUSRUFEZEICHEN, 21, "Ausrufezeichen") \
    X(PROZENT,        22, "Prozent") \
    X(KAUFMANNSUND,   23, "Kaufmannsund") \
    X(GRÖSSER,        24, "Größer") \
    X(GRÖSSER_GLEICH, 25, "Größergleich") \
    X(GLEICH,         26, "Gleich") \
    X(UNGLEICH,       27, "Ungleich") \
    X(KLEINER_GLEICH, 28, "Kleinergleich") \
    X(KLEINER,        29, "Kleiner") \
    X(ZUWEISUNG,      30, "Zuweisung")

namespace Sss {

class Token
{
public:
    enum Art
    {
        #define X(N, W, ...) N = W,
        Token_Art
        #undef X
    };

    Token(Art art, Spanne spanne);

    static Token * Bezeichner(Spanne spanne);
    static Token * Text(Spanne spanne);
    static Token * Ganzzahl(Spanne spanne, int32_t zahl, int32_t basis = 10);
    static Token * Dezimalzahl(Spanne spanne, float zahl);

    Art        art() const;
    Spanne     spanne() const;
    uint16_t   länge() const;
    char     * text() const;
    char     * text_kopieren(Spanne spanne);

    virtual void ausgeben(std::ostream &ausgabe);

    template<typename T> T als();

protected:
    Art _art;
    Spanne _spanne;
    char *_text;
};

class Token_Ganzzahl : public Token
{
public:
    Token_Ganzzahl(Spanne spanne, int32_t wert, int32_t basis = 10);

    int32_t wert() const;
    int32_t basis() const;

private:
    int32_t _wert;
    int32_t _basis;
};

class Token_Dezimalzahl : public Token
{
public:
    Token_Dezimalzahl(Spanne spanne, float wert);

    float wert() const;

private:
    float _wert;
};

}

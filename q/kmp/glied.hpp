#pragma once

#include "spanne.hpp"

#define Glied_Art \
    X(ENDE,               0, "<EOF>") \
    X(GANZZAHL,           1, "Ganzzahl") \
    X(DEZIMALZAHL,        2, "Dezimalzahl") \
    X(BEZEICHNER,         3, "Bezeichner") \
    X(TEXT,               4, "Text") \
    X(RKLAMMER_AUF,       5, "Runde Klammer auf") \
    X(RKLAMMER_ZU,        6, "Runde Klammer zu") \
    X(EKLAMMER_AUF,       7, "Eckige Klammer auf") \
    X(EKLAMMER_ZU,        8, "Eckige Klammer zu") \
    X(GKLAMMER_AUF,       9, "Geschweifte Klammer auf") \
    X(GKLAMMER_ZU,       10, "Geschweifte Klammer zu") \
    X(KLAMMERAFFE,       11, "Klammeraffe") \
    X(PUNKT,             12, "Punkt") \
    X(KOMMA,             13, "Komma") \
    X(STRICHPUNKT,       14, "Strichpunkt") \
    X(DOPPELPUNKT,       15, "Doppelpunkt") \
    X(PLUS,              16, "Plus") \
    X(MINUS,             17, "Minus") \
    X(STERN,             18, "Stern") \
    X(PISA,              19, "Pisa") \
    X(RAUTE,             20, "Raute") \
    X(AUSRUFEZEICHEN,    21, "Ausrufezeichen") \
    X(PROZENT,           22, "Prozent") \
    X(KAUFMANNSUND,      23, "Kaufmannsund") \
    X(PFEIL,             24, "Pfeil") \
    X(TILDE,             25, "Tilde") \
    X(BALKEN,            26, "Balken") \
    X(FOLGERUNG,         27, "Folgerung") \
    X(DREIECK_RECHTS,    28, "Dreieck Rechts") \
    X(DREIECK_LINKS,     29, "Dreieck Links") \
    \
    X(KLEINER,           30, "Kleiner") \
    X(KLEINER_GLEICH,    31, "Kleinergleich") \
    X(GLEICH,            32, "Gleich") \
    X(UNGLEICH,          33, "Ungleich") \
    X(GRÖẞER_GLEICH,     34, "Größergleich") \
    X(GRÖẞER,            35, "Größer") \
    \
    X(PUNKT_PUNKT,       40, "PunktPunkt") \
    \
    X(ZUWEISUNG,         50, "Zuweisung") \
    X(ZUWEISUNG_PLUS,    51, "Zuweisung Plus") \
    X(ZUWEISUNG_MINUS,   52, "Zuweisung Minus") \
    X(ZUWEISUNG_STERN,   53, "Zuweisung Stern") \
    X(ZUWEISUNG_PISA,    54, "Zuweisung Pisa") \
    X(ZUWEISUNG_PROZENT, 55, "Zuweisung Prozent") \
    X(ILLEGAL,           100, "Illegal")

namespace Sss::Kmp {

class Glied
{
public:
    enum Art
    {
    #define X(N, W, ...) N = W,
        Glied_Art
    #undef X
    };

    Glied(Art art, Spanne spanne);

    static Glied * Bezeichner(Spanne spanne);
    static Glied * Text(Spanne spanne);
    static Glied * Ganzzahl(Spanne spanne, int32_t zahl, int32_t basis = 10);
    static Glied * Dezimalzahl(Spanne spanne, float zahl);

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

class Glied_Ganzzahl : public Glied
{
public:
    Glied_Ganzzahl(Spanne spanne, int32_t wert, int32_t basis = 10);

    int32_t wert() const;
    int32_t basis() const;

private:
    int32_t _wert;
    int32_t _basis;
};

class Glied_Dezimalzahl : public Glied
{
public:
    Glied_Dezimalzahl(Spanne spanne, float wert);

    float wert() const;

private:
    float _wert;
};

}

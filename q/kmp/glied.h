#ifndef __KMP_GLIED_H__
#define __KMP_GLIED_H__

#include "basis/definitionen.h"
#include "basis/text.h"
#include "kmp/spanne.h"

typedef enum   KMP_glied_art_e KMP_glied_art_e;
typedef struct KMP_glied_t     KMP_glied_t;

#define Glied_Art \
    X(KMP_GLIED_ENDE,               0, "<EOF>") \
    X(KMP_GLIED_GANZZAHL,           1, "Ganzzahl") \
    X(KMP_GLIED_DEZIMALZAHL,        2, "Dezimalzahl") \
    X(KMP_GLIED_BEZEICHNER,         3, "Bezeichner") \
    X(KMP_GLIED_TEXT,               4, "Text") \
    X(KMP_GLIED_RKLAMMER_AUF,       5, "Runde Klammer auf") \
    X(KMP_GLIED_RKLAMMER_ZU,        6, "Runde Klammer zu") \
    X(KMP_GLIED_EKLAMMER_AUF,       7, "Eckige Klammer auf") \
    X(KMP_GLIED_EKLAMMER_ZU,        8, "Eckige Klammer zu") \
    X(KMP_GLIED_GKLAMMER_AUF,       9, "Geschweifte Klammer auf") \
    X(KMP_GLIED_GKLAMMER_ZU,       10, "Geschweifte Klammer zu") \
    X(KMP_GLIED_KLAMMERAFFE,       11, "Klammeraffe") \
    X(KMP_GLIED_PUNKT,             12, "Punkt") \
    X(KMP_GLIED_KOMMA,             13, "Komma") \
    X(KMP_GLIED_STRICHPUNKT,       14, "Strichpunkt") \
    X(KMP_GLIED_DOPPELPUNKT,       15, "Doppelpunkt") \
    X(KMP_GLIED_PLUS,              16, "Plus") \
    X(KMP_GLIED_MINUS,             17, "Minus") \
    X(KMP_GLIED_STERN,             18, "Stern") \
    X(KMP_GLIED_PISA,              19, "Pisa") \
    X(KMP_GLIED_RAUTE,             20, "Raute") \
    X(KMP_GLIED_AUSRUFEZEICHEN,    21, "Ausrufezeichen") \
    X(KMP_GLIED_PROZENT,           22, "Prozent") \
    X(KMP_GLIED_KAUFMANNSUND,      23, "Kaufmannsund") \
    X(KMP_GLIED_PFEIL,             24, "Pfeil") \
    X(KMP_GLIED_TILDE,             25, "Tilde") \
    X(KMP_GLIED_BALKEN,            26, "Balken") \
    X(KMP_GLIED_FOLGERUNG,         27, "Folgerung") \
    X(KMP_GLIED_DREIECK_RECHTS,    28, "Dreieck Rechts") \
    X(KMP_GLIED_DREIECK_LINKS,     29, "Dreieck Links") \
    \
    X(KMP_GLIED_KLEINER,           30, "Kleiner") \
    X(KMP_GLIED_KLEINER_GLEICH,    31, "Kleinergleich") \
    X(KMP_GLIED_GLEICH,            32, "Gleich") \
    X(KMP_GLIED_UNGLEICH,          33, "Ungleich") \
    X(KMP_GLIED_GRÖẞER_GLEICH,     34, "Größergleich") \
    X(KMP_GLIED_GRÖẞER,            35, "Größer") \
    \
    X(KMP_GLIED_PUNKT_PUNKT,       40, "PunktPunkt") \
    \
    X(KMP_GLIED_ZUWEISUNG,         50, "Zuweisung") \
    X(KMP_GLIED_ZUWEISUNG_PLUS,    51, "Zuweisung Plus") \
    X(KMP_GLIED_ZUWEISUNG_MINUS,   52, "Zuweisung Minus") \
    X(KMP_GLIED_ZUWEISUNG_STERN,   53, "Zuweisung Stern") \
    X(KMP_GLIED_ZUWEISUNG_PISA,    54, "Zuweisung Pisa") \
    X(KMP_GLIED_ZUWEISUNG_PROZENT, 55, "Zuweisung Prozent") \
    X(KMP_GLIED_ILLEGAL,           100, "Illegal")
enum KMP_glied_art_e
{
#define X(N, W, ...) N = W,
    Glied_Art
#undef X
};

struct KMP_glied_t
{
    KMP_glied_art_e art;
    KMP_spanne_t spanne;
    BSS_text_t text;

    union {
        struct {
            g32 basis;
            g32 wert;
        } gz;

        struct {
            d32 wert;
        } dz;
    };
};

KMP_glied_t* kmp_glied(BSS_speicher_t* speicher, g32 art, KMP_spanne_t spanne);
KMP_glied_t* kmp_glied_bezeichner(BSS_speicher_t* speicher, KMP_spanne_t spanne);
KMP_glied_t* kmp_glied_text(BSS_speicher_t* speicher, KMP_spanne_t spanne);
KMP_glied_t* kmp_glied_ganzzahl(BSS_speicher_t* speicher, KMP_spanne_t spanne, g32 zahl, g32 basis);
KMP_glied_t* kmp_glied_dezimalzahl(BSS_speicher_t* speicher, KMP_spanne_t spanne, d32 wert);
BSS_text_t* kmp_glied_text_kopieren(KMP_spanne_t spanne);

#endif


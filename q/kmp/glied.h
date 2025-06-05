#ifndef __KMP_GLIED_H__
#define __KMP_GLIED_H__

#include "basis/definitionen.h"
#include "basis/text.h"
#include "kmp/spanne.h"

typedef enum   KMP_glied_art_e KMP_glied_art_e;
typedef struct KMP_glied_t     KMP_glied_t;

#define Glied_Art \
    X(GLIED_ENDE,               0, "<EOF>") \
    X(GLIED_GANZZAHL,           1, "Ganzzahl") \
    X(GLIED_DEZIMALZAHL,        2, "Dezimalzahl") \
    X(GLIED_BEZEICHNER,         3, "Bezeichner") \
    X(GLIED_TEXT,               4, "Text") \
    X(GLIED_RKLAMMER_AUF,       5, "Runde Klammer auf") \
    X(GLIED_RKLAMMER_ZU,        6, "Runde Klammer zu") \
    X(GLIED_EKLAMMER_AUF,       7, "Eckige Klammer auf") \
    X(GLIED_EKLAMMER_ZU,        8, "Eckige Klammer zu") \
    X(GLIED_GKLAMMER_AUF,       9, "Geschweifte Klammer auf") \
    X(GLIED_GKLAMMER_ZU,       10, "Geschweifte Klammer zu") \
    X(GLIED_KLAMMERAFFE,       11, "Klammeraffe") \
    X(GLIED_PUNKT,             12, "Punkt") \
    X(GLIED_KOMMA,             13, "Komma") \
    X(GLIED_STRICHPUNKT,       14, "Strichpunkt") \
    X(GLIED_DOPPELPUNKT,       15, "Doppelpunkt") \
    X(GLIED_PLUS,              16, "Plus") \
    X(GLIED_MINUS,             17, "Minus") \
    X(GLIED_STERN,             18, "Stern") \
    X(GLIED_PISA,              19, "Pisa") \
    X(GLIED_RAUTE,             20, "Raute") \
    X(GLIED_AUSRUFEZEICHEN,    21, "Ausrufezeichen") \
    X(GLIED_PROZENT,           22, "Prozent") \
    X(GLIED_KAUFMANNSUND,      23, "Kaufmannsund") \
    X(GLIED_PFEIL,             24, "Pfeil") \
    X(GLIED_TILDE,             25, "Tilde") \
    X(GLIED_BALKEN,            26, "Balken") \
    X(GLIED_FOLGERUNG,         27, "Folgerung") \
    X(GLIED_DREIECK_RECHTS,    28, "Dreieck Rechts") \
    X(GLIED_DREIECK_LINKS,     29, "Dreieck Links") \
    \
    X(GLIED_KLEINER,           30, "Kleiner") \
    X(GLIED_KLEINER_GLEICH,    31, "Kleinergleich") \
    X(GLIED_GLEICH,            32, "Gleich") \
    X(GLIED_UNGLEICH,          33, "Ungleich") \
    X(GLIED_GRÖẞER_GLEICH,     34, "Größergleich") \
    X(GLIED_GRÖẞER,            35, "Größer") \
    \
    X(GLIED_PUNKT_PUNKT,       40, "PunktPunkt") \
    \
    X(GLIED_ZUWEISUNG,         50, "Zuweisung") \
    X(GLIED_ZUWEISUNG_PLUS,    51, "Zuweisung Plus") \
    X(GLIED_ZUWEISUNG_MINUS,   52, "Zuweisung Minus") \
    X(GLIED_ZUWEISUNG_STERN,   53, "Zuweisung Stern") \
    X(GLIED_ZUWEISUNG_PISA,    54, "Zuweisung Pisa") \
    X(GLIED_ZUWEISUNG_PROZENT, 55, "Zuweisung Prozent") \
    X(GLIED_ILLEGAL,           100, "Illegal")
enum KMP_glied_art_e
{
#define X(N, W, ...) N = W,
    Glied_Art
#undef X
};

struct KMP_glied_t
{
    KMP_glied_art_e art;
    KMP_spanne_t    spanne;
    BSS_text_t      text;

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

KMP_glied_t * kmp_glied(BSS_speicher_t *speicher, KMP_glied_art_e art, KMP_spanne_t spanne);
KMP_glied_t * kmp_glied_bezeichner(BSS_speicher_t *speicher, KMP_spanne_t spanne, d32 wert);
KMP_glied_t * kmp_glied_text(BSS_speicher_t *speicher, KMP_spanne_t spanne, d32 wert);
KMP_glied_t * kmp_glied_ganzzahl(BSS_speicher_t *speicher, KMP_spanne_t spanne, g32 zahl, g32 basis);
KMP_glied_t * kmp_glied_dezimalzahl(BSS_speicher_t *speicher, KMP_spanne_t spanne, d32 wert);

BSS_text_t  * kmp_glied_text_kopieren(KMP_spanne_t spanne);

#endif


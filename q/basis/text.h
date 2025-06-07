#ifndef __BSS_TEXT_H__
#define __BSS_TEXT_H__

#include "basis/definitionen.h"

typedef struct BSS_text_t BSS_text_t;
struct BSS_text_t
{
    char* daten;
    g64 größe;
    g64 länge;
};

BSS_text_t bss_text(char *text);
g64 bss_text_größe(BSS_text_t text);
g64 bss_text_länge(BSS_text_t text);
w32 bss_text_ist_gleich(BSS_text_t links, BSS_text_t rechts);
g32 bss_text_utf8_dekodieren(BSS_text_t text, g8 *daten);
g32 bss_text_utf8_zeichen_größe(char* text);

#endif


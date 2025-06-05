#ifndef __BSS_WERT_H__
#define __BSS_WERT_H__

#include "basis/definitionen.h"
#include "basis/text.h"
#include "basis/speicher.h"

typedef struct BSS_wert_t BSS_wert_t;
struct BSS_wert_t
{
    union
    {
        g32        wert_g32;
        d32        wert_d32;
        w32        wert_w32;
        BSS_text_t wert_txt;
    };
};

BSS_wert_t* bss_wert_g32(BSS_speicher_t* speicher, g32 wert);
BSS_wert_t* bss_wert_d32(BSS_speicher_t* speicher, d32 wert);
BSS_wert_t* bss_wert_w32(BSS_speicher_t* speicher, w32 wert);
BSS_wert_t* bss_wert_text(BSS_speicher_t* speicher, BSS_text_t wert);

#endif


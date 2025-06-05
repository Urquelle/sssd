#ifndef __BSS_STAPEL_H__
#define __BSS_STAPEL_H__

#include "basis/definitionen.h"
#include "basis/speicher.h"

#ifndef BSS_STAPEL_ANZAHL_ELEMENTE
#define BSS_STAPEL_ANZAHL_ELEMENTE 128
#endif

typedef struct BSS_stapel_t BSS_stapel_t;

struct BSS_stapel_t
{
    g64 element_größe;
    g32 anzahl_elemente;
    g32 max_kapazität;
    void* daten;
};

BSS_stapel_t bss_stapel(BSS_speicher_t* speicher, g64 element_größe);
void bss_stapel_rein(BSS_speicher_t* speicher, BSS_stapel_t* stapel, void* element);
void* bss_stapel_raus(BSS_stapel_t* stapel);
void* bss_stapel_schauen(BSS_stapel_t* stapel);
void bss_stapel_daten_kopieren(void* ziel, void* quelle, g64 größe);

#endif


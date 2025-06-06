#ifndef __BSS_FELD_H__
#define __BSS_FELD_H__

#include "basis/definitionen.h"
#include "basis/speicher.h"

#define BSS_Feld(T) BSS_feld_t

#define MAX_ANZAHL_ELEMENTE 10

typedef struct BSS_feld_t BSS_feld_t;
struct BSS_feld_t
{
    g32 element_größe;
    g32 anzahl_elemente;
    g32 max_anzahl_elemente;
    void* daten;
};

BSS_feld_t bss_feld(BSS_speicher_t* speicher, g32 element_größe);
void bss_feld_hinzufügen(BSS_speicher_t* speicher, BSS_feld_t* feld, void* element);
void* bss_feld_element(BSS_feld_t feld, g32 index);
void bss_feld_leeren(BSS_feld_t* feld);

#endif


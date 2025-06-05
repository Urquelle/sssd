#include "basis/feld.h"
#include "basis/speicher.h"

#include <memory.h>

BSS_feld_t
bss_feld(BSS_speicher_t* speicher, g32 element_größe)
{
    BSS_feld_t erg = {};

    erg.element_größe = element_größe;
    erg.anzahl_elemente = 0;
    erg.max_anzahl_elemente = MAX_ANZAHL_ELEMENTE;
    erg.daten = speicher->anfordern(speicher, element_größe*erg.max_anzahl_elemente);

    return erg;
}

void
bss_feld_hinzufügen(BSS_speicher_t* speicher, BSS_feld_t *feld, void *element)
{
    if (feld->anzahl_elemente >= feld->max_anzahl_elemente)
    {
        void* daten = speicher->anfordern(speicher, feld->element_größe*feld->max_anzahl_elemente*2);
        memcpy(daten, feld->daten, feld->anzahl_elemente*feld->element_größe);
        speicher->freigeben(feld->daten);
        feld->daten = daten;
    }

    void *daten = speicher->anfordern(speicher, feld->element_größe);
    memcpy(daten, element, feld->element_größe);

    feld->anzahl_elemente += 1;
}

void *
bss_feld_element(BSS_feld_t feld, g32 index)
{
    if (index >= feld.anzahl_elemente)
    {
        return NULL;
    }

    void *erg = (g8*) feld.daten + index*feld.element_größe;

    return erg;
}

void
bss_feld_leeren(BSS_feld_t *feld)
{
    feld->anzahl_elemente = 0;
}


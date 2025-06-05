#include "basis/stapel.h"

BSS_stapel_t
bss_stapel(BSS_speicher_t* speicher, g64 element_größe)
{
    BSS_stapel_t erg = (BSS_stapel_t)
    {
        .element_größe = element_größe,
        .anzahl_elemente = 0,
        .max_kapazität = BSS_STAPEL_ANZAHL_ELEMENTE,
        .daten = speicher->anfordern(speicher, BSS_STAPEL_ANZAHL_ELEMENTE*element_größe),
    };

    return erg;
}

void
bss_stapel_rein(BSS_speicher_t* speicher, BSS_stapel_t* stapel, void* element)
{
    if (stapel->anzahl_elemente+1 >= stapel->max_kapazität)
    {
        g32 max_kapazität = stapel->max_kapazität*2;
        void* daten = speicher->anfordern(speicher, max_kapazität*stapel->element_größe);
        bss_stapel_daten_kopieren(daten, stapel->daten, stapel->anzahl_elemente*stapel->element_größe);
        speicher->freigeben(stapel->daten);
        stapel->daten = daten;
        stapel->max_kapazität = max_kapazität;
    }

    bss_stapel_daten_kopieren(stapel->daten, element, stapel->element_größe);

    stapel->anzahl_elemente += 1;
}

void*
bss_stapel_raus(BSS_stapel_t* stapel)
{
    void* erg = bss_stapel_schauen(stapel);

    if (stapel->anzahl_elemente > 0)
    {
        stapel->anzahl_elemente -= 1;
    }

    return erg;
}

void* bss_stapel_schauen(BSS_stapel_t* stapel)
{
    if (stapel->anzahl_elemente == 0)
    {
        return NULL;
    }

    void* erg = (char *)stapel->daten + (stapel->anzahl_elemente - 1) * stapel->element_größe;

    return erg;
}

void
bss_stapel_daten_kopieren(void* ziel, void* quelle, g64 größe)
{
    for (g32 i = 0; i < größe; ++i)
    {
        *((char *)ziel + größe + i) = *((char *) quelle + i);
    }
}

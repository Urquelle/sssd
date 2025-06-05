#include "basis/speicher.h"

#include <stdlib.h>
#include <memory.h>

BSS_speicher_t
bss_speicher_arena()
{
    BSS_speicher_t erg = {};

    erg.anfordern = BSS_speicher_anfordern_arena;
    erg.freigeben = BSS_speicher_freigeben_arena;

    return erg;
}

void*
BSS_speicher_anfordern_arena(BSS_speicher_t* speicher, g64 größe)
{
    if ((speicher->verwendet + größe) >= speicher->verfügbar)
    {
        n64 verfügbar = speicher->verfügbar == 0 ? 1024 : speicher->verfügbar*2;
        void* daten = malloc(verfügbar);
        memcpy(daten, speicher->daten, speicher->verwendet);

        free(speicher->daten);
        speicher->daten = daten;
        speicher->verfügbar = verfügbar;
    }

    void *erg = (g8*) speicher->daten + speicher->verwendet;
    speicher->verwendet += größe;

    return erg;
}

void
BSS_speicher_freigeben_arena(BSS_speicher_t *speicher)
{
    speicher->verwendet = 0;
}

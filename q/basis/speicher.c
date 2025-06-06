#include "basis/speicher.h"

#include <memory.h>
#include <windows.h>

BSS_speicher_t
bss_speicher_arena(void)
{
    BSS_speicher_t erg = {};

    erg.daten = NULL;
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
        void* daten = VirtualAlloc(NULL, verfügbar, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
        bss_speicher_kopieren(daten, speicher->daten, speicher->verwendet);

        if (speicher->daten)
        {
            //VirtualFree(speicher->daten, 0, MEM_RELEASE);
        }

        speicher->daten = daten;
        speicher->verfügbar = verfügbar;
    }

    void *erg = (g8*) speicher->daten + speicher->verwendet;
    speicher->verwendet += größe;

    return erg;
}

void
BSS_speicher_freigeben_arena(BSS_speicher_t* speicher)
{
    speicher->verwendet = 0;
}

void*
bss_speicher_kopieren(void* ziel_ptr, void* quelle_ptr, g32 anzahl_bytes)
{
    g8* ziel = (g8*)ziel_ptr;
    g8* quelle = (g8*)quelle_ptr;

    if (ziel_ptr == 0 || quelle_ptr == 0)
    {
        return ziel_ptr;
    }

    // Prüfen ob Adressen word-aligned sind (4-Byte Grenze)
    if (((n32)ziel % sizeof(n32)) == 0 &&
        ((n32)quelle % sizeof(n32)) == 0 &&
        anzahl_bytes >= sizeof(n32))
    {
        // Word-weise kopieren für bessere Performance
        n32* ziel_words = (n32*)ziel;
        n32* quelle_words = (n32*)quelle;
        n32 anzahl_words = anzahl_bytes / sizeof(n32);

        for (n32 i = 0; i < anzahl_words; i++)
        {
            ziel_words[i] = quelle_words[i];
        }

        // Restliche Bytes kopieren
        n32 restliche_bytes = anzahl_bytes % sizeof(n32);
        n32 offset = anzahl_words * sizeof(n32);

        for (n32 i = 0; i < restliche_bytes; i++)
        {
            ziel[offset + i] = quelle[offset + i];
        }
    }
    else
    {
        // Fallback: byte-weise kopieren
        for (g32 i = 0; i < anzahl_bytes; i++)
        {
            ziel[i] = quelle[i];
        }
    }

    return ziel_ptr;
}

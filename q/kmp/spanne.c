#include "kmp/spanne.h"

#include <memory.h>

KMP_spanne_t
kmp_spanne()
{
    KMP_spanne_t erg = {};

    return erg;
}

KMP_spanne_t
kmp_spanne_kopie(const KMP_spanne_t original)
{
    KMP_spanne_t erg = kmp_spanne();

    erg.von = original.von;
    erg.bis = original.bis;

    kmp_spanne_länge_berechnen(&erg);

    return erg;
}

KMP_spanne_t
kmp_spanne_bereich(KMP_spanne_t von, KMP_spanne_t bis)
{
    KMP_spanne_t erg = kmp_spanne();

    erg.von = von.von;
    erg.bis = bis.bis;

    kmp_spanne_länge_berechnen(&erg);

    return erg;
}

KMP_spanne_t
kmp_spanne_zeichen(KMP_zeichen_t von, KMP_zeichen_t bis)
{
    KMP_spanne_t erg = kmp_spanne();

    erg.von = von;
    erg.bis = bis;

    kmp_spanne_länge_berechnen(&erg);

    return erg;
}

BSS_text_t
kmp_spanne_text_kopieren(BSS_speicher_t *speicher, KMP_spanne_t spanne)
{
    BSS_text_t utf8_text  = spanne.von.utf8;
    g32        text_länge = spanne.länge;

    char * erg = speicher->anfordern(speicher, text_länge + 1);
    memcpy(erg, utf8_text.daten, text_länge);
    erg[text_länge] = 0;

    return (BSS_text_t)
    {
        .daten = erg
    };
}

void
kmp_spanne_länge_berechnen(KMP_spanne_t *spanne)
{
    uint32_t   länge = 1;
    BSS_text_t anfang = spanne->von.utf8;

    while (anfang.daten != spanne->bis.utf8.daten)
    {
        länge        += 1;
        anfang.daten += 1;
    }

    spanne->länge = länge;
}

int
kmp_spanne_ist_leer(KMP_spanne_t spanne)
{
    int erg = spanne.länge == 0;

    return erg;
}

void
kmp_spanne_ausgeben(KMP_spanne_t spanne, FILE *ausgabe)
{
    if (kmp_spanne_ist_leer(spanne))
    {
        return;
    }

    if (spanne.länge > 0)
    {
        BSS_text_t utf8 = spanne.von.utf8;

        if (utf8.daten != NULL)
        {
            fprintf(ausgabe, "%d", spanne.länge);
        }
        else
        {
            /*ausgabe.setstate(std::ios_base::failbit);*/
        }
    }
}

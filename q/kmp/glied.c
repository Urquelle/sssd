#include "kmp/glied.h"

KMP_glied_t*
kmp_glied(BSS_speicher_t* speicher, g32 art, KMP_spanne_t spanne)
{
    KMP_glied_t *erg = (KMP_glied_t *) speicher->anfordern(speicher, sizeof(KMP_glied_t));

    erg->art = art;
    erg->spanne = spanne;
    erg->text = kmp_spanne_text_kopieren(speicher, spanne);

    return erg;
}


void
kmp_glied_ausgeben(KMP_glied_t glied, FILE* ausgabe)
{
    switch (glied.art)
    {
    #define X(N, W, B) case N: fprintf(ausgabe, "\033[1;34m" B "\033[0m"); break;
        Glied_Art
    #undef X
    }

    fprintf(ausgabe, ": %s", glied.text.daten);
}

KMP_glied_t*
kmp_glied_ganzzahl(BSS_speicher_t* speicher, KMP_spanne_t spanne, g32 wert, g32 basis)
{
    KMP_glied_t* erg = kmp_glied(speicher, KMP_GLIED_GANZZAHL, spanne);

    erg->gz.basis = basis;
    erg->gz.wert  = wert;

    return erg;
}

KMP_glied_t*
kmp_glied_dezimalzahl(BSS_speicher_t* speicher, KMP_spanne_t spanne, d32 wert)
{
    KMP_glied_t* erg = kmp_glied(speicher, KMP_GLIED_DEZIMALZAHL, spanne);

    erg->dz.wert = wert;

    return erg;
}

KMP_glied_t*
kmp_glied_bezeichner(BSS_speicher_t* speicher, KMP_spanne_t spanne)
{
    KMP_glied_t* erg = kmp_glied(speicher, KMP_GLIED_BEZEICHNER, spanne);

    return erg;
}

KMP_glied_t*
kmp_glied_text(BSS_speicher_t* speicher, KMP_spanne_t spanne)
{
    KMP_glied_t* erg = kmp_glied(speicher, KMP_GLIED_TEXT, spanne);

    return erg;
}


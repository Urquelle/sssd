#include "kmp/fehler.h"

KMP_fehler_t *
kmp_fehler(BSS_speicher_t *speicher, BSS_text_t text)
{
    KMP_fehler_t *erg = speicher->anfordern(speicher, sizeof(KMP_fehler_t));

    erg->text = text;
    erg->kennung = 0;

    return erg;
}


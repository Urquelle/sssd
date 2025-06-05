#include "kmp/ergebnis.h"

KMP_ergebnis_t
kmp_ergebnis_erfolg(void *wert)
{
    KMP_ergebnis_t erg = {};

    erg.wert     = wert;
    erg.fehler   = NULL;
    erg.gut      = true;
    erg.schlecht = false;

    return erg;
}

KMP_ergebnis_t
kmp_ergebnis_fehler(KMP_fehler_t *fehler)
{
    KMP_ergebnis_t erg = {};

    erg.wert     = NULL;
    erg.fehler   = fehler;
    erg.gut      = false;
    erg.schlecht = true;

    return erg;
}


#include "basis/wert.h"

BSS_wert_t*
bss_wert_g32(BSS_speicher_t* speicher, g32 wert)
{
    BSS_wert_t* erg = speicher->anfordern(speicher, sizeof(BSS_wert_t));

    erg->wert_g32 = wert;

    return erg;
}

BSS_wert_t*
bss_wert_d32(BSS_speicher_t* speicher, d32 wert)
{
    BSS_wert_t* erg = speicher->anfordern(speicher, sizeof(BSS_wert_t));

    erg->wert_d32 = wert;

    return erg;
}

BSS_wert_t*
bss_wert_w32(BSS_speicher_t* speicher, w32 wert)
{
    BSS_wert_t* erg = speicher->anfordern(speicher, sizeof(BSS_wert_t));

    erg->wert_w32 = wert;

    return erg;
}

BSS_wert_t*
bss_wert_text(BSS_speicher_t* speicher, BSS_text_t wert)
{
    BSS_wert_t* erg = speicher->anfordern(speicher, sizeof(BSS_wert_t));

    erg->wert_txt = wert;

    return erg;
}

#include "basis/paar.h"

BSS_paar_t
bss_paar(void* eins, void* zwei)
{
    BSS_paar_t erg = {
        .eins = eins,
        .zwei = zwei
    };

    return erg;
}

BSS_paar_text_t
bss_paar_text(BSS_text_t eins, BSS_text_t zwei)
{
    BSS_paar_text_t erg = {
        .eins = eins,
        .zwei = zwei
    };

    return erg;
}

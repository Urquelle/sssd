#ifndef __BSS_PAAR_H__
#define __BSS_PAAR_H__

#include "basis/text.h"

typedef struct BSS_paar_t BSS_paar_t;
struct BSS_paar_t
{
    void* eins;
    void* zwei;
};

typedef struct BSS_paar_text_t BSS_paar_text_t;
struct BSS_paar_text_t
{
    BSS_text_t eins;
    BSS_text_t zwei;
};

BSS_paar_t bss_paar(void* eins, void* zwei);
BSS_paar_text_t bss_paar_text(BSS_text_t eins, BSS_text_t zwei);

#endif


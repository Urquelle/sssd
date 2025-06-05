#ifndef __BSS_SPEICHER_H__
#define __BSS_SPEICHER_H__

#include "basis/definitionen.h"

typedef struct BSS_speicher_t BSS_speicher_t;
typedef void* (BSS_Speicher_Anfordern)(BSS_speicher_t* speicher, g64 größe);
typedef void (BSS_Speicher_Freigeben)(BSS_speicher_t* speicher);

struct BSS_speicher_t
{
    g32 verfügbar;
    g32 verwendet;
    void* daten;

    BSS_Speicher_Anfordern* anfordern;
    BSS_Speicher_Freigeben* freigeben;
};

void* BSS_speicher_anfordern_arena(BSS_speicher_t* speicher, g64 größe);
void BSS_speicher_freigeben_arena(BSS_speicher_t* speicher);
BSS_speicher_t bss_speicher_arena();

#endif


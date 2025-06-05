#include "kmp/symbol.h"
#include "kmp/datentyp.h"
#include "kmp/zone.h"

KMP_sem_symbol_t*
kmp_sem_symbol_datentyp(BSS_speicher_t* speicher, KMP_spanne_t spanne, g32 art, g32 status, BSS_text_t name, KMP_sem_datentyp_t* datentyp)
{
    KMP_sem_symbol_t *erg = speicher->anfordern(speicher, sizeof(KMP_sem_symbol_t));

    erg->art      = art;
    erg->status   = status;
    erg->name     = name;
    erg->datentyp = datentyp;
    erg->zone     = kmp_sem_zone(speicher, name, NULL);
    erg->spanne   = spanne;

    return erg;
}

KMP_sem_symbol_t *
kmp_sem_symbol_zone(BSS_speicher_t* speicher, KMP_spanne_t spanne, g32 art, g32 status, BSS_text_t name, KMP_sem_zone_t* zone)
{
    KMP_sem_symbol_t *erg = speicher->anfordern(speicher, sizeof(KMP_sem_symbol_t));

    erg->art      = art;
    erg->status   = status;
    erg->name     = name;
    erg->datentyp = NULL;
    erg->zone     = zone;
    erg->spanne   = spanne;

    return erg;
}


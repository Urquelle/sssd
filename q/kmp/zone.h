#ifndef __KMP_ZONE_H__
#define __KMP_ZONE_H__

#include "basis/definitionen.h"
#include "basis/text.h"
#include "basis/feld.h"

struct KMP_sem_symbol_t;

typedef struct KMP_sem_zone_t KMP_sem_zone_t;
struct KMP_sem_zone_t
{
    BSS_Feld(KMP_sem_symbol_t) symbole;
    BSS_text_t name;
    KMP_sem_zone_t* über;
};

KMP_sem_zone_t* kmp_sem_zone(BSS_speicher_t* speicher, BSS_text_t name, KMP_sem_zone_t* über);
w32 kmp_sem_zone_symbol_registrieren(BSS_speicher_t* speicher, KMP_sem_zone_t* zone, BSS_text_t name, struct KMP_sem_symbol_t* symbol);
w32 kmp_sem_zone_symbol_ist_registriert(KMP_sem_zone_t* zone, BSS_text_t name);
struct KMP_sem_symbol_t* kmp_sem_zone_symbol_suchen(KMP_sem_zone_t* zone, BSS_text_t name, w32 über_suche);
struct KMP_sem_symbol_t* kmp_sem_zone_symbol(KMP_sem_zone_t* zone, g32 index);

#endif


#ifndef __KMP_SPANNE_H__
#define __KMP_SPANNE_H__

#include <stdio.h>

#include "basis/definitionen.h"
#include "basis/speicher.h"
#include "kmp/zeichen.h"

typedef struct KMP_spanne_t KMP_spanne_t;
struct KMP_spanne_t
{
    KMP_zeichen_t von;
    KMP_zeichen_t bis;
    g32           länge;
};

KMP_spanne_t kmp_spanne(void);
KMP_spanne_t kmp_spanne_zeichen(KMP_zeichen_t von, KMP_zeichen_t bis);
KMP_spanne_t kmp_spanne_kopie(KMP_spanne_t original);
KMP_spanne_t kmp_spanne_bereich(KMP_spanne_t von, KMP_spanne_t bis);
void         kmp_spanne_länge_berechnen(KMP_spanne_t *spanne);
int          kmp_spanne_ist_leer(KMP_spanne_t spanne);
void         kmp_spanne_ausgeben(KMP_spanne_t spanne, FILE *ausgabe);
BSS_text_t   kmp_spanne_text_kopieren(BSS_speicher_t *speicher, KMP_spanne_t spanne);

#endif


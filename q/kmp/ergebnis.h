#ifndef __KMP_ERGEBNIS_H__
#define __KMP_ERGEBNIS_H__

#include "kmp/fehler.h"

typedef struct KMP_ergebnis_t KMP_ergebnis_t;
struct KMP_ergebnis_t
{
    w32 gut;
    w32 schlecht;
    void* wert;
    KMP_fehler_t* fehler;
};

KMP_ergebnis_t kmp_ergebnis_erfolg(void *wert);
KMP_ergebnis_t kmp_ergebnis_fehler(KMP_fehler_t *fehler);

#endif


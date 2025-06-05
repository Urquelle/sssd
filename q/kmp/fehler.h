#ifndef __KMP_FEHLER_H__
#define __KMP_FEHLER_H__

#include "basis/speicher.h"
#include "basis/definitionen.h"
#include "basis/text.h"

typedef struct KMP_fehler_t KMP_fehler_t;
struct KMP_fehler_t
{
    BSS_text_t text;
    g32        kennung;
};

KMP_fehler_t * kmp_fehler(BSS_speicher_t *speicher, BSS_text_t text);

#endif


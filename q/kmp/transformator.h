#ifndef __KMP_TRANSFORMATOR_H__
#define __KMP_TRANSFORMATOR_H__

#include "kmp/sem_asb.h"

typedef struct KMP_transformator_t KMP_transformator_t;
typedef struct KMP_bytecode_t      KMP_bytecode_t;

enum KMP_bytecode_art_e
{
    KMP_BYTECODE_ART_0 = 0,
    KMP_BYTECODE_ART_ANWEISUNG = 1,
};

struct KMP_transformator_t
{
    BSS_Feld(KMP_sem_anweisung_t*) sem_anweisungen;
};

struct KMP_bytecode_t
{
    g32 art;
};

KMP_transformator_t* kmp_transformator(BSS_speicher_t* speicher, BSS_Feld(KMP_sem_anweisung_t*) anweisungen);

#endif


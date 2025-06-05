#ifndef __KMP_OPERAND_H__
#define __KMP_OPERAND_H__

#include "basis/wert.h"
#include "kmp/symbol.h"

typedef enum   KMP_sem_operand_merkmal_e  KMP_sem_operand_merkmal_e;
typedef struct KMP_sem_operand_t          KMP_sem_operand_t;

struct KMP_sem_datentyp_t;

#define Operand_Merkmal_Liste \
    X(KMP_SEM_OPERAND_MERKMAL_0,            0, "Keins") \
    X(KMP_SEM_OPERAND_MERKMAL_LITERAL,      1, "Literal") \
    X(KMP_SEM_OPERAND_MERKMAL_ARITHMETISCH, 2, "Arithmetisch")
enum KMP_sem_operand_merkmal_e
{
#define X(N, W, ...) N = W,
    Operand_Merkmal_Liste
#undef X
};

struct KMP_sem_operand_t
{
    struct KMP_sem_datentyp_t* datentyp;
    struct KMP_sem_symbol_t* symbol;
    g32 merkmale;
    BSS_wert_t* wert;
};

KMP_sem_operand_t* kmp_sem_operand_mit_datentyp(BSS_speicher_t *speicher, struct KMP_sem_datentyp_t *datentyp, g32 merkmale);
KMP_sem_operand_t* kmp_sem_operand_mit_symbol(BSS_speicher_t *speicher, KMP_sem_symbol_t *symbol, g32 merkmale);
w32 kmp_sem_operand_mirkmal_gesetzt(KMP_sem_operand_t* operand, g32 merkmal);

#endif


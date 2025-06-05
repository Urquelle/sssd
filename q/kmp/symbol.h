#ifndef __KMP_SYMBOL_H__
#define __KMP_SYMBOL_H__

#include "basis/text.h"
#include "kmp/spanne.h"
#include "kmp/zone.h"

typedef enum KMP_sem_symbol_art_e    KMP_sem_symbol_art_e;
typedef enum KMP_sem_symbol_status_e KMP_sem_symbol_status_e;
typedef struct KMP_sem_symbol_t      KMP_sem_symbol_t;

struct KMP_sem_datentyp_t;
struct KMP_sem_deklaration_t;

#define Symbol_Art_Liste \
    X(KMP_SYM_VARIABLE,  1, "Variable") \
    X(KMP_SYM_ROUTINE,   2, "Routine") \
    X(KMP_SYM_DATENTYP,  3, "Datentyp") \
    X(KMP_SYM_MODUL,     4, "Modul")

enum KMP_sem_symbol_art_e
{
#define X(N, W, ...) N = W,
    Symbol_Art_Liste
#undef X
};

#define Symbol_Status_Liste \
    X(KMP_SYM_UNBEHANDELT,   0, "Unbehandelt") \
    X(KMP_SYM_IN_BEHANDLUNG, 1, "In Behandlung") \
    X(KMP_SYM_BEHANDELT,     2, "Behandelt")

enum KMP_sem_symbol_status_e
{
#define X(N, W, ...) N = W,
    Symbol_Status_Liste
#undef X
};

struct KMP_sem_symbol_t
{
    KMP_sem_symbol_art_e art;
    KMP_sem_symbol_status_e status;
    BSS_text_t name;
    struct KMP_sem_datentyp_t* datentyp;
    KMP_sem_zone_t* zone;
    KMP_spanne_t spanne;
    struct KMP_sem_deklaration_t* deklaration;
};

KMP_sem_symbol_t* kmp_sem_symbol_datentyp(BSS_speicher_t* speicher, KMP_spanne_t spanne, g32 art, g32 status, BSS_text_t name, struct KMP_sem_datentyp_t* datentyp);
KMP_sem_symbol_t * kmp_sem_symbol_zone(BSS_speicher_t* speicher, KMP_spanne_t spanne, g32 art, g32 status, BSS_text_t name, KMP_sem_zone_t* zone);

#endif


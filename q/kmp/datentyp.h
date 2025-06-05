#ifndef __KMP_DATENTYP_H__
#define __KMP_DATENTYP_H__

#include "basis/definitionen.h"
#include "basis/feld.h"
#include "basis/text.h"
#include "kmp/asb.h"
#include "kmp/operand.h"

typedef enum   KMP_sem_datentyp_status_e         KMP_sem_datentyp_status_e;
typedef enum   KMP_sem_datentyp_art_e            KMP_sem_datentyp_art_e;
typedef enum   KMP_sem_datentyp_kompatibilität_e KMP_sem_datentyp_kompatibilität_e;
typedef enum   KMP_sem_datentyp_merkmale_e       KMP_sem_datentyp_merkmale_e;
typedef struct KMP_sem_datentyp_t                KMP_sem_datentyp_t;
typedef struct KMP_sem_datentyp_schablone_eigenschaft_t KMP_sem_datentyp_schablone_eigenschaft_t;
typedef struct KMP_sem_datentyp_option_eigenschaft_t KMP_sem_datentyp_option_eigenschaft_t;

struct KMP_semantik_t;

#define Datentyp_Status_Liste \
    X(KMP_DATENTYP_STATUS_UNBEHANDELT,       1, "Unbehandelt") \
    X(KMP_DATENTYP_STATUS_IN_BEHANDLUNG,     2, "In Behandlung") \
    X(KMP_DATENTYP_STATUS_BEHANDELT,         3, "Behandelt")
enum KMP_sem_datentyp_status_e
{
#define X(N, W, ...) N = W,
    Datentyp_Status_Liste
#undef X
};

#define Datentyp_Art_Liste \
    X(KMP_DATENTYP_ART_GANZE_ZAHL,         1, "Ganze Zahl") \
    X(KMP_DATENTYP_ART_DEZIMAL_ZAHL,       2, "Dezimal Zahl") \
    X(KMP_DATENTYP_ART_TEXT,               3, "Text") \
    X(KMP_DATENTYP_ART_ZEIGER,             4, "Zeiger") \
    X(KMP_DATENTYP_ART_SCHABLONE,          5, "Schablone") \
    X(KMP_DATENTYP_ART_ROUTINE,            6, "Routine") \
    X(KMP_DATENTYP_ART_BOOL,               7, "Wahrheitswert") \
    X(KMP_DATENTYP_ART_AUFZÄHLUNG,         8, "Aufzählung") \
    X(KMP_DATENTYP_ART_FELD,               9, "Feld") \
    X(KMP_DATENTYP_ART_NIHIL,             10, "Nihil") \
    X(KMP_DATENTYP_ART_OPTION,            11, "Option")
enum KMP_sem_datentyp_art_e
{
#define X(N, W, ...) N = W,
    Datentyp_Art_Liste
#undef X
};

#define Kompatibilität_Liste \
    X(KMP_DATENTYP_KOMPATIBILITÄT_INKOMPATIBEL,         0, "Inkompatibel") \
    X(KMP_DATENTYP_KOMPATIBILITÄT_KOMPATIBEL,           1, "Kompatibel") \
    X(KMP_DATENTYP_KOMPATIBILITÄT_KOMPATIBEL_IMPLIZIT,  2, "Implizit") \
    X(KMP_DATENTYP_KOMPATIBILITÄT_KOMPATIBEL_MIT_DATENVERLUST, 3, "Kompatibel mit Datenverlust")
enum KMP_sem_datentyp_kompatibilität_e
{
#define X(N, W, ...) N = W,
    Kompatibilität_Liste
#undef X
};

enum KMP_sem_datentyp_merkmale_e
{
    KMP_DATENTYP_MERKMAL_0            = 0,
    KMP_DATENTYP_MERKMAL_ARITHMETISCH = 1,
};

struct KMP_sem_datentyp_t
{
    KMP_sem_datentyp_art_e art;
    KMP_sem_datentyp_status_e status;
    g32 merkmale;
    struct KMP_sem_symbol_t* symbol;
    struct KMP_syn_deklaration_t* deklaration;
    g64 größe;
    w32 ist_abgeschlossen;

    union
    {
        struct
        {
            KMP_sem_datentyp_t* basis;
            KMP_sem_datentyp_t* index;
        } feld;

        struct
        {
            KMP_sem_datentyp_t* basis;
        } zeiger;

        struct
        {
            BSS_Feld(KMP_sem_datentyp_schablone_eigenschaft_t*) eigenschaften;
        } schablone;

        struct
        {
            BSS_Feld(KMP_sem_datentyp_schablone_eigenschaft_t*) eigenschaften;
        } anon;

        struct
        {
            BSS_Feld(KMP_sem_datentyp_t*) parameter;
            KMP_sem_datentyp_t*           rückgabe;
        } routine;

        struct
        {
            KMP_sem_datentyp_t* basis;
            BSS_Feld(KMP_sem_datentyp_option_eigenschaft_t*) eigenschaften;
        } option;
    };
};

struct KMP_sem_datentyp_schablone_eigenschaft_t
{
    KMP_sem_datentyp_t* datentyp;
    g64 versatz;
    BSS_text_t name;
};

struct KMP_sem_datentyp_option_eigenschaft_t
{
    KMP_sem_datentyp_t* datentyp;
    KMP_sem_operand_t* operand;
    BSS_text_t name;
};

KMP_sem_datentyp_t* kmp_sem_datentyp(BSS_speicher_t *speicher, g32 art, g64 größe, w32 ist_abgeschlossen, g32 merkmale);
KMP_sem_datentyp_t* kmp_sem_datentyp_routine(BSS_speicher_t* speicher, BSS_Feld(KMP_sem_datentyp_t*) parameter, KMP_sem_datentyp_t* rückgabe, KMP_syn_deklaration_t* deklaration);
KMP_sem_datentyp_t* kmp_sem_datentyp_option(BSS_speicher_t* speicher, KMP_sem_datentyp_t *basis, KMP_syn_deklaration_t* deklaration);
KMP_sem_datentyp_t* kmp_sem_datentyp_feld(BSS_speicher_t *speicher, KMP_sem_datentyp_t *basis, KMP_sem_datentyp_t *index);
KMP_sem_datentyp_t* kmp_sem_datentyp_zeiger(BSS_speicher_t *speicher, KMP_sem_datentyp_t *basis);
KMP_sem_datentyp_t* kmp_sem_datentyp_schablone(BSS_speicher_t *speicher, KMP_syn_deklaration_t *deklaration);
w32 kmp_sem_datentyp_schablone_eigenschaft_hinzufügen(BSS_speicher_t* speicher, KMP_sem_datentyp_t* datentyp, g64 versatz, BSS_text_t name);
w32 kmp_sem_datentyp_ist_arithmetisch(KMP_sem_datentyp_t *datentyp);
g32 kmp_sem_datentypen_kompatibel(KMP_sem_datentyp_t *ziel, KMP_sem_datentyp_t *quelle, w32 implizit);
void kmp_sem_datentyp_abschließen(BSS_speicher_t* speicher, struct KMP_semantik_t* semantik, KMP_sem_datentyp_t* datentyp, w32 basis_eines_zeigers);
KMP_sem_datentyp_schablone_eigenschaft_t* kmp_sem_datentyp_schablone_eigenschaft(BSS_speicher_t* speicher, KMP_sem_datentyp_t* datentyp, g64 versatz, BSS_text_t name);
KMP_sem_datentyp_option_eigenschaft_t* kmp_sem_datentyp_option_eigenschaft(BSS_speicher_t* speicher, KMP_sem_datentyp_t* datentyp, KMP_sem_operand_t* operand, BSS_text_t name);
w32 kmp_sem_datentyp_option_eigenschaft_hinzufügen(BSS_speicher_t* speicher, KMP_sem_datentyp_t* option, KMP_sem_datentyp_t* datentyp, KMP_sem_operand_t* operand, BSS_text_t name);

KMP_sem_datentyp_t* Datentyp_N8;
KMP_sem_datentyp_t* Datentyp_N16;
KMP_sem_datentyp_t* Datentyp_N32;
KMP_sem_datentyp_t* Datentyp_N64;
KMP_sem_datentyp_t* Datentyp_N128;

KMP_sem_datentyp_t* Datentyp_G8;
KMP_sem_datentyp_t* Datentyp_G16;
KMP_sem_datentyp_t* Datentyp_G32;
KMP_sem_datentyp_t* Datentyp_G64;
KMP_sem_datentyp_t* Datentyp_G128;

KMP_sem_datentyp_t* Datentyp_D32;
KMP_sem_datentyp_t* Datentyp_D64;

KMP_sem_datentyp_t* Datentyp_Text;
KMP_sem_datentyp_t* Datentyp_Bool;
KMP_sem_datentyp_t* Datentyp_Nihil;

#endif


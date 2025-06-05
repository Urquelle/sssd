#ifndef __KMP_sem_H__
#define __KMP_sem_H__

#include "kmp/asb.h"
#include "kmp/operand.h"
#include "kmp/zone.h"

typedef struct KMP_sem_knoten_t            KMP_sem_knoten_t;
typedef struct KMP_sem_muster_t            KMP_sem_muster_t;
typedef struct KMP_sem_eigenschaft_t       KMP_sem_eigenschaft_t;
typedef struct KMP_sem_ausdruck_t          KMP_sem_ausdruck_t;
typedef struct KMP_sem_anweisung_t         KMP_sem_anweisung_t;
typedef struct KMP_sem_deklaration_t       KMP_sem_deklaration_t;
typedef struct KMP_sem_asb_t               KMP_sem_asb_t;

struct KMP_sem_asb_t
{
    KMP_sem_zone_t* system;
    KMP_sem_zone_t* global;
    BSS_Feld(KMP_sem_knoten_t*) anweisungen;
};

struct KMP_sem_muster_t
{
    KMP_sem_ausdruck_t* ausdruck;
    KMP_sem_anweisung_t* rumpf;
};

struct KMP_sem_knoten_t
{
    KMP_spanne_t spanne;
    KMP_syn_knoten_t* syn_knoten;
    g32 art;
};

struct KMP_sem_eigenschaft_t
{
    KMP_sem_operand_t* operand;
};

struct KMP_sem_ausdruck_t
{
    KMP_sem_knoten_t* basis;
    KMP_sem_operand_t* operand;

    union
    {
        struct
        {
            g32 op;
            KMP_sem_ausdruck_t* links;
            KMP_sem_ausdruck_t* rechts;
        } binär;

        struct
        {
            g32 op;
            KMP_sem_ausdruck_t* ausdruck;
        } unär;

        struct
        {
            KMP_sem_ausdruck_t* basis;
            KMP_sem_ausdruck_t* index;
        } index;

        struct
        {
            KMP_sem_ausdruck_t* basis;
            KMP_sem_symbol_t* eigenschaft;
        } eigenschaft;

        struct
        {
            KMP_sem_ausdruck_t* basis;
            BSS_Feld(KMP_knoten_ausdruck_t*) argumente;
        } aufruf;

        struct
        {
            KMP_sem_ausdruck_t* ausdruck;
        } ausführen;

        struct
        {
            KMP_sem_ausdruck_t* min;
            KMP_sem_ausdruck_t* max;
        } reihe;

        struct
        {
            BSS_Feld(KMP_sem_eigenschaft_t*) eigenschaften;
        } kompositum;
    };
};

struct KMP_sem_deklaration_t
{
    KMP_sem_knoten_t* basis;
    BSS_Feld(KMP_sem_symbol_t*) symbole;
    KMP_sem_datentyp_t* datentyp;
    KMP_sem_ausdruck_t* initialisierung;
    KMP_syn_deklaration_t* deklaration;

    union
    {
        struct
        {
            KMP_sem_anweisung_t* rumpf;
        } routine;
    };
};

struct KMP_sem_anweisung_t
{
    KMP_sem_knoten_t* basis;

    union
    {
        struct
        {
            KMP_sem_deklaration_t* deklaration;
        } deklaration;

        struct
        {
            KMP_sem_ausdruck_t* ausdruck;
        } ausdruck;

        struct
        {
            BSS_Feld(KMP_sem_knoten_t*) anweisungen;
        } block;

        struct
        {
            KMP_sem_symbol_t* index;
            KMP_sem_ausdruck_t* bedingung;
            KMP_sem_anweisung_t* rumpf;
            BSS_text_t weiter_markierung;
            BSS_text_t raus_markierung;
        } für;

        struct
        {
            KMP_sem_ausdruck_t* ausdruck;
        } res;

        struct
        {
            KMP_sem_ausdruck_t* bedingung;
            KMP_sem_anweisung_t* rumpf;
            KMP_sem_anweisung_t* alternative;
        } wenn;

        struct
        {
            KMP_sem_ausdruck_t* bedingung;
            KMP_sem_anweisung_t* rumpf;
            BSS_text_t weiter_markierung;
            BSS_text_t raus_markierung;
        } solange;

        struct
        {
            BSS_text_t markierung;
        } sprung;

        struct
        {
            BSS_text_t markierung;
        } markierung;

        struct
        {
            KMP_sem_ausdruck_t* ausdruck;
            BSS_Feld(KMP_sem_muster_t*) muster;
        } weiche;

        struct
        {
            KMP_glied_t* op;
            KMP_sem_ausdruck_t* links;
            KMP_sem_ausdruck_t* rechts;
        } zuweisung;

        struct
        {
            KMP_sem_anweisung_t* anweisung;
        } final;
    };
};

KMP_sem_muster_t* kmp_sem_muster(BSS_speicher_t* speicher, KMP_syn_muster_t* syn_knoten, KMP_sem_ausdruck_t* ausdruck, KMP_sem_anweisung_t* rumpf);
KMP_sem_knoten_t* kmp_sem_knoten(BSS_speicher_t* speicher, KMP_syn_knoten_t* syn_knoten, g32 art);
KMP_sem_eigenschaft_t* kmp_sem_eigenschaft(BSS_speicher_t* speicher, KMP_sem_operand_t* operand);

KMP_sem_ausdruck_t* kmp_sem_ausdruck(BSS_speicher_t* speicher, KMP_syn_ausdruck_t* syn_knoten, g32 art, KMP_sem_operand_t* operand);
KMP_sem_ausdruck_t* kmp_sem_ausdruck_binär(BSS_speicher_t* speicher, KMP_syn_ausdruck_t* syn_knoten, KMP_sem_operand_t* operand, g32 op, KMP_sem_ausdruck_t* links, KMP_sem_ausdruck_t* rechts);
KMP_sem_ausdruck_t* kmp_sem_ausdruck_bezeichner(BSS_speicher_t* speicher, KMP_syn_ausdruck_t* syn_knoten, KMP_sem_operand_t* operand);
KMP_sem_ausdruck_t* kmp_sem_ausdruck_kompositum(BSS_speicher_t* speicher, KMP_syn_ausdruck_t* syn_knoten, KMP_sem_operand_t* operand, BSS_Feld(KMP_sem_eigenschaft_t*) eigenschaften);
KMP_sem_ausdruck_t* kmp_sem_ausdruck_index(BSS_speicher_t* speicher, KMP_syn_ausdruck_t* syn_knoten, KMP_sem_operand_t* operand, KMP_sem_ausdruck_t* basis, KMP_sem_ausdruck_t* index);
KMP_sem_ausdruck_t* kmp_sem_ausdruck_unär(BSS_speicher_t* speicher, KMP_syn_ausdruck_t* syn_knoten, KMP_sem_operand_t* operand, g32 op, KMP_sem_ausdruck_t* ausdruck);
KMP_sem_ausdruck_t* kmp_sem_ausdruck_aufruf(BSS_speicher_t* speicher, KMP_syn_ausdruck_t* syn_knoten, KMP_sem_operand_t* operand, KMP_sem_ausdruck_t* basis, BSS_Feld(KMP_sem_ausdruck_t*) argumente);
KMP_sem_ausdruck_t* kmp_sem_ausdruck_eigenschaft(BSS_speicher_t* speicher, KMP_syn_ausdruck_t* syn_knoten, KMP_sem_operand_t* operand, KMP_sem_ausdruck_t* basis, KMP_sem_symbol_t* eigenschaft);

KMP_sem_anweisung_t* kmp_sem_anweisung(BSS_speicher_t* speicher, KMP_syn_anweisung_t* syn_knoten, g32 art);
KMP_sem_anweisung_t* kmp_sem_anweisung_ausdruck(BSS_speicher_t* speicher, KMP_syn_anweisung_t* syn_knoten, KMP_sem_ausdruck_t* ausdruck);
KMP_sem_anweisung_t* kmp_sem_anweisung_deklaration(BSS_speicher_t* speicher, KMP_syn_anweisung_t* syn_knoten, KMP_sem_deklaration_t* deklaration);
KMP_sem_anweisung_t* kmp_sem_anweisung_block(BSS_speicher_t* speicher, KMP_syn_anweisung_t* syn_knoten, BSS_Feld(KMP_sem_anweisung_t*) anweisungen);
KMP_sem_anweisung_t* kmp_sem_anweisung_solange(BSS_speicher_t* speicher, KMP_syn_anweisung_t* syn_knoten, KMP_sem_ausdruck_t* bedingung, KMP_sem_anweisung_t* rumpf, BSS_text_t weiter_markierung, BSS_text_t raus_markierung);
KMP_sem_anweisung_t* kmp_sem_anweisung_für(BSS_speicher_t* speicher, KMP_syn_anweisung_t* syn_knoten, KMP_sem_symbol_t* index, KMP_sem_ausdruck_t* bedingung, KMP_sem_anweisung_t* rumpf, BSS_text_t weiter_markierung, BSS_text_t raus_markierung);
KMP_sem_anweisung_t* kmp_sem_anweisung_sprung(BSS_speicher_t* speicher, KMP_syn_anweisung_t* syn_knoten, BSS_text_t markierung);
KMP_sem_anweisung_t* kmp_sem_anweisung_res(BSS_speicher_t* speicher, KMP_syn_anweisung_t* syn_knoten, KMP_sem_ausdruck_t* ausdruck);
KMP_sem_anweisung_t* kmp_sem_anweisung_wenn(BSS_speicher_t* speicher, KMP_syn_anweisung_t* syn_knoten, KMP_sem_ausdruck_t* bedingung, KMP_sem_anweisung_t* rumpf, KMP_sem_anweisung_t* alternative);
KMP_sem_anweisung_t* kmp_sem_anweisung_weiche(BSS_speicher_t* speicher, KMP_syn_anweisung_t* syn_knoten, KMP_sem_ausdruck_t* ausdruck, BSS_Feld(KMP_bestimmtes_muster_t*) muster);
KMP_sem_anweisung_t* kmp_sem_anweisung_zuweisung(BSS_speicher_t* speicher, KMP_syn_anweisung_t* syn_knoten, KMP_glied_t* op, KMP_sem_ausdruck_t* links, KMP_sem_ausdruck_t* rechts);
KMP_sem_anweisung_t* kmp_sem_anweisung_final(BSS_speicher_t* speicher, KMP_syn_anweisung_t* syn_knoten, KMP_sem_anweisung_t* anweisung);

KMP_sem_deklaration_t* kmp_sem_deklaration(BSS_speicher_t* speicher, KMP_syn_deklaration_t* syn_knoten, g32 art, BSS_Feld(KMP_sem_symbol_t*) symbole, KMP_sem_datentyp_t* datentyp, KMP_sem_ausdruck_t* initialisierung /* = nullptr */);
KMP_sem_deklaration_t* kmp_sem_deklaration_routine(BSS_speicher_t* speicher, KMP_syn_deklaration_t* syn_knoten, BSS_Feld(KMP_sem_symbol_t*) symbole, KMP_sem_datentyp_t* datentyp, KMP_sem_anweisung_t* rumpf);
KMP_sem_deklaration_t* kmp_sem_deklaration_option(BSS_speicher_t* speicher, KMP_syn_deklaration_t* syn_knoten, BSS_Feld(KMP_sem_symbol_t*) symbole, KMP_sem_datentyp_t* datentyp);
KMP_sem_deklaration_t* kmp_sem_deklaration_schablone(BSS_speicher_t* speicher, KMP_syn_deklaration_t* syn_knoten, BSS_Feld(KMP_sem_symbol_t*) symbole, KMP_sem_datentyp_t* datentyp);
KMP_sem_asb_t* kmp_sem_asb(BSS_speicher_t* speicher, KMP_sem_zone_t* system, KMP_sem_zone_t* global);

#endif


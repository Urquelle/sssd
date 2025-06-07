#ifndef __KMP_SYN_H__
#define __KMP_SYN_H__

#include "basis/feld.h"
#include "kmp/spanne.h"
#include "kmp/glied.h"
#include "kmp/symbol.h"

typedef enum   KMP_syn_knoten_art_e        KMP_syn_knoten_art_e;
typedef enum   KMP_syn_ausdruck_binop_e    KMP_syn_ausdruck_binop_e;
typedef struct KMP_syn_asb_t               KMP_syn_asb_t;
typedef struct KMP_syn_marke_t             KMP_syn_marke_t;
typedef struct KMP_syn_muster_t            KMP_syn_muster_t;
typedef struct KMP_syn_knoten_t            KMP_syn_knoten_t;
typedef struct KMP_syn_ausdruck_t          KMP_syn_ausdruck_t;
typedef struct KMP_syn_anweisung_t         KMP_syn_anweisung_t;
typedef struct KMP_syn_spezifizierung_t    KMP_syn_spezifizierung_t;
typedef struct KMP_syn_deklaration_t       KMP_syn_deklaration_t;

typedef struct KMP_syn_ausdruck_kompositum_eigenschaft_t KMP_syn_ausdruck_kompositum_eigenschaft_t;

#define Syn_Knoten_Art \
    X(KMP_SYN_KNOTEN_AUSDRUCK_UNGÜLTIG,             0, "Ungültiger Ausdruck") \
    X(KMP_SYN_KNOTEN_ANWEISUNG_UNGÜLTIG,            1, "Ungültige Anweisung") \
    X(KMP_SYN_KNOTEN_DEKLARATION_UNGÜLTIG,          2, "Ungültige Deklaration") \
    X(KMP_SYN_KNOTEN_SPEZIFIZIERUNG_UNGÜLTIG,       3, "Ungültige Spezifizierung") \
    \
    X(KMP_SYN_KNOTEN_AUSDRUCK_GANZZAHL,            11, "Ausdruck Ganzzahl") \
    X(KMP_SYN_KNOTEN_AUSDRUCK_DEZIMALZAHL,         12, "Ausdruck Dezimalzahl") \
    X(KMP_SYN_KNOTEN_AUSDRUCK_TEXT,                13, "Ausdruck Text") \
    X(KMP_SYN_KNOTEN_AUSDRUCK_BEZEICHNER,          14, "Ausdruck Bezeichner") \
    X(KMP_SYN_KNOTEN_AUSDRUCK_KLAMMER,             15, "Ausdruck Klammer") \
    X(KMP_SYN_KNOTEN_AUSDRUCK_BINÄR,               16, "Ausdruck Binär") \
    X(KMP_SYN_KNOTEN_AUSDRUCK_UNÄR,                17, "Ausdruck Unär") \
    X(KMP_SYN_KNOTEN_AUSDRUCK_EIGENSCHAFT,         18, "Ausdruck Eigenschaft") \
    X(KMP_SYN_KNOTEN_AUSDRUCK_INDEX,               19, "Ausdruck Index") \
    X(KMP_SYN_KNOTEN_AUSDRUCK_AUFRUF,              20, "Ausdruck Aufruf") \
    X(KMP_SYN_KNOTEN_AUSDRUCK_REIHE,               21, "Ausdruck Reihe") \
    X(KMP_SYN_KNOTEN_AUSDRUCK_IMPORT,             22, "Ausdruck Import") \
    X(KMP_SYN_KNOTEN_AUSDRUCK_KOMPOSITUM,          23, "Ausdruck Kompositum") \
    X(KMP_SYN_KNOTEN_AUSDRUCK_AUSFÜHREN,           24, "Ausdruck Ausführen") \
    X(KMP_SYN_KNOTEN_AUSDRUCK_ROUTINE,             25, "Ausdruck Routine") \
    X(KMP_SYN_KNOTEN_AUSDRUCK_OPTION,              26, "Ausdruck Option") \
    X(KMP_SYN_KNOTEN_AUSDRUCK_SCHABLONE,           27, "Ausdruck Schablone") \
    X(KMP_SYN_KNOTEN_AUSDRUCK_ZERLEGEN,            28, "Ausdruck Zerlegen") \
    \
    X(KMP_SYN_KNOTEN_SPEZIFIZIERUNG_BEZEICHNER,    30, "Spezifizierung Name") \
    X(KMP_SYN_KNOTEN_SPEZIFIZIERUNG_FELD,          31, "Spezifizierung Feld") \
    X(KMP_SYN_KNOTEN_SPEZIFIZIERUNG_ZEIGER,        32, "Spezifizierung Zeiger") \
    X(KMP_SYN_KNOTEN_SPEZIFIZIERUNG_ROUTINE,       33, "Spezifizierung Routine") \
    \
    X(KMP_SYN_KNOTEN_DEKLARATION_VARIABLE,         40, "Deklaration Variable") \
    X(KMP_SYN_KNOTEN_DEKLARATION_SCHABLONE,        41, "Deklaration Schablone") \
    X(KMP_SYN_KNOTEN_DEKLARATION_ROUTINE,          42, "Deklaration Routine") \
    X(KMP_SYN_KNOTEN_DEKLARATION_IMPORT,          43, "Deklaration Import") \
    X(KMP_SYN_KNOTEN_DEKLARATION_OPTION,           44, "Deklaration Option") \
    \
    X(KMP_SYN_KNOTEN_ANWEISUNG_ZUWEISUNG,          50, "Anweisung Zuweisung") \
    X(KMP_SYN_KNOTEN_ANWEISUNG_WENN,               51, "Anweisung Wenn") \
    X(KMP_SYN_KNOTEN_ANWEISUNG_FÜR,                52, "Anweisung Für") \
    X(KMP_SYN_KNOTEN_ANWEISUNG_BLOCK,              53, "Anweisung Block") \
    X(KMP_SYN_KNOTEN_ANWEISUNG_DEKLARATION,        54, "Anweisung Deklaration") \
    X(KMP_SYN_KNOTEN_ANWEISUNG_IMPORT,            55, "Anweisung Import") \
    X(KMP_SYN_KNOTEN_ANWEISUNG_LADE,               56, "Anweisung Lade") \
    X(KMP_SYN_KNOTEN_ANWEISUNG_AUSDRUCK,           57, "Anweisung Ausdruck") \
    X(KMP_SYN_KNOTEN_ANWEISUNG_FINAL,              58, "Anweisung Final") \
    X(KMP_SYN_KNOTEN_ANWEISUNG_WEICHE,             59, "Anweisung Weiche") \
    X(KMP_SYN_KNOTEN_ANWEISUNG_RES,                60, "Anweisung Res") \
    X(KMP_SYN_KNOTEN_ANWEISUNG_SOLANGE,            61, "Anweisung Solange") \
    X(KMP_SYN_KNOTEN_ANWEISUNG_WEITER,             62, "Anweisung Weiter") \
    X(KMP_SYN_KNOTEN_ANWEISUNG_SPRUNG,             63, "Anweisung Sprung") \
    X(KMP_SYN_KNOTEN_ANWEISUNG_RAUS,               64, "Anweisung Raus") \
    X(KMP_SYN_KNOTEN_ANWEISUNG_MARKIERUNG,         65, "Anweisung Markierung") \
    X(KMP_SYN_KNOTEN_ANWEISUNG_BEDINGTER_SPRUNG,   66, "Anweisung Bedingter Sprung")
enum KMP_syn_knoten_art_e
{
#define X(N, W, ...) N = W,
    Syn_Knoten_Art
#undef X
};

#define Bin_Op_Liste \
    X(KMP_SYN_AUSDRUCK_BINÄR_UNBEKANNT,            0, "Unbekannt") \
    X(KMP_SYN_AUSDRUCK_BINÄR_ADDITION,             1, "Addition") \
    X(KMP_SYN_AUSDRUCK_BINÄR_SUBTRAKTION,          2, "Subtraktion") \
    X(KMP_SYN_AUSDRUCK_BINÄR_MULTIPLIKATION,       3, "Multiplikation") \
    X(KMP_SYN_AUSDRUCK_BINÄR_DIVISION,             4, "Division") \
    X(KMP_SYN_AUSDRUCK_BINÄR_MODULO,               5, "Modulo") \
    X(KMP_SYN_AUSDRUCK_BINÄR_GLEICH,               6, "Gleich") \
    X(KMP_SYN_AUSDRUCK_BINÄR_UNGLEICH,             7, "Ungleich") \
    X(KMP_SYN_AUSDRUCK_BINÄR_KLEINER,              8, "Kleiner") \
    X(KMP_SYN_AUSDRUCK_BINÄR_KLEINER_GLEICH,       9, "Kleinergleich") \
    X(KMP_SYN_AUSDRUCK_BINÄR_GRÖẞER,              10, "Größer") \
    X(KMP_SYN_AUSDRUCK_BINÄR_GRÖẞER_GLEICH,       11, "Größergleich") \
    X(KMP_SYN_AUSDRUCK_BINÄR_BIT_VERSATZ_LINKS,   12, "Bitversatz links") \
    X(KMP_SYN_AUSDRUCK_BINÄR_BIT_VERSATZ_RECHTS,  13, "Bitversatz rechts")
enum KMP_syn_ausdruck_binop_e
{
#define X(N, W, ...) N = W,
    Bin_Op_Liste
#undef X
};

#define Unär_Op_Liste \
    X(KMP_SYN_AUSDRUCK_UNÄR_UNBEKANNT,    0, "Unbekannt") \
    X(KMP_SYN_AUSDRUCK_UNÄR_MINUS,        1, "Minus") \
    X(KMP_SYN_AUSDRUCK_UNÄR_NEGIERUNG,    2, "Negierung") \
    X(KMP_SYN_AUSDRUCK_UNÄR_INVERTIERUNG, 3, "Invertierung")
enum KMP_syn_ausdruck_unop_e
{
#define X(N, W, ...) N = W,
    Unär_Op_Liste
#undef X
};

struct KMP_syn_marke_t
{
    KMP_spanne_t spanne;
    BSS_text_t wert;
};

struct KMP_syn_muster_t
{
    KMP_spanne_t spanne;
    KMP_syn_ausdruck_t* muster;
    KMP_syn_anweisung_t* anweisung;
};

struct KMP_syn_asb_t
{
    BSS_Feld(KMP_knoten_t*) anweisungen;
};

struct KMP_syn_ausdruck_kompositum_eigenschaft_t
{
    KMP_spanne_t spanne;
    BSS_text_t name;
    KMP_syn_ausdruck_t* ausdruck;
    w32 ist_benamt;
};

struct KMP_syn_knoten_t
{
    g64 id;
    KMP_syn_knoten_art_e art;
    KMP_spanne_t spanne;
};

struct KMP_syn_ausdruck_t
{
    KMP_syn_knoten_t* basis;

    union
    {
        struct
        {
            g32 wert;
        } ganzzahl;

        struct
        {
            d32 wert;
        } dezimalzahl;

        struct
        {
            BSS_text_t wert;
        } bezeichner;

        struct
        {
            BSS_text_t wert;
        } text;

        struct
        {
            g32 op;
            KMP_syn_ausdruck_t* links;
            KMP_syn_ausdruck_t* rechts;
        } binär;

        struct
        {
            g32 op;
            KMP_syn_ausdruck_t* ausdruck;
        } unär;

        struct
        {
            KMP_syn_ausdruck_t* ausdruck;
        } klammer;

        struct
        {
            KMP_syn_ausdruck_t* basis;
            KMP_syn_ausdruck_t* index;
        } index;

        struct
        {
            KMP_syn_ausdruck_t* basis;
            KMP_syn_ausdruck_t* eigenschaft;
        } eigenschaft;

        struct
        {
            KMP_syn_ausdruck_t* basis;
            BSS_Feld(KMP_syn_ausdruck_t*) argumente;
        } aufruf;

        struct
        {
            KMP_syn_ausdruck_t* ausdruck;
        } ausführen;

        struct
        {
            KMP_syn_ausdruck_t* min;
            KMP_syn_ausdruck_t* max;
        } reihe;

        struct
        {
            BSS_text_t dateiname;
        } import;

        struct
        {
            w32 ist_benamt;
            KMP_syn_spezifizierung_t* spezifizierung;
            BSS_Feld(KMP_syn_kompositum_eigentschaft_t) eigenschaften;
        } kompositum;

        struct
        {
            BSS_Feld(KMP_syn_deklaration_t*) parameter;
            KMP_syn_spezifizierung_t* rückgabe;
            KMP_syn_anweisung_t* rumpf;
        } routine;

        struct
        {
            BSS_Feld(KMP_syn_deklaration_t*) eigenschaften;
        } schablone;

        struct
        {
            BSS_Feld(KMP_syn_deklaration_t*) eigenschaften;
        } option;
#if 0
        struct
        {
            // AUFGABE: überlegen was hier hin soll und umsetzung!
        } zerlegen;
#endif
    };
};

struct KMP_syn_spezifizierung_t
{
    KMP_syn_knoten_t* basis;
    g64 größe;

    union
    {
        struct
        {
            BSS_text_t name;
        } bezeichner;

        struct
        {
            KMP_syn_spezifizierung_t* basis;
            KMP_syn_ausdruck_t* index;
        } feld;

        struct
        {
            KMP_syn_spezifizierung_t* basis;
        } zeiger;

        struct
        {
            BSS_Feld(KMP_syn_spezifizierung_t*) parameter;
            KMP_syn_spezifizierung_t *rückgabe;
        } routine;
    };
};

struct KMP_syn_deklaration_t
{
    KMP_syn_knoten_t* basis;
    BSS_Feld(BSS_text_t) namen;
    KMP_syn_spezifizierung_t* spezifizierung;
    KMP_sem_symbol_t* symbol;

    union
    {
        struct
        {
            KMP_syn_ausdruck_t* initialisierung;
        } variable;

        struct
        {
            KMP_syn_ausdruck_t* ausdruck;
        } routine;

        struct
        {
            BSS_Feld(KMP_syn_deklaration_t*) eigenschaften;
        } schablone;

        struct
        {
            KMP_syn_ausdruck_t* ausdruck;
        } import;

        struct
        {
            KMP_syn_ausdruck_t* ausdruck;
        } option;
    };
};

struct KMP_syn_anweisung_t
{
    KMP_syn_knoten_t* basis;
    BSS_Feld(KMP_syn_marke_t*) marken;

    union
    {
        struct
        {
            KMP_glied_t* op;
            KMP_syn_ausdruck_t* links;
            KMP_syn_ausdruck_t* rechts;
        } zuweisung;

        struct
        {
            KMP_syn_ausdruck_t* bedingung;
            KMP_syn_anweisung_t* rumpf;
            KMP_syn_anweisung_t* alternative;
        } wenn;

        struct
        {
            KMP_syn_ausdruck_t* index;
            KMP_syn_ausdruck_t* bedingung;
            KMP_syn_anweisung_t* rumpf;
        } für;

        struct
        {
            KMP_syn_ausdruck_t* bedingung;
            KMP_syn_anweisung_t* rumpf;
        } solange;

        struct
        {
            BSS_Feld(KMP_syn_anweisung_t*) anweisungen;
        } block;

        struct
        {
            KMP_syn_deklaration_t* deklaration;
        } deklaration;

        struct
        {
            BSS_text_t dateiname;
        } import;

        // FRAGE: wird das noch genutzt, oder ist das durch import ersetzt worden?
        struct
        {
            BSS_text_t dateiname;
        } lade;

        struct
        {
            KMP_syn_ausdruck_t* ausdruck;
        } ausdruck;

        struct
        {
            KMP_syn_anweisung_t* anweisung;
        } final;

        struct
        {
            KMP_syn_ausdruck_t* ausdruck;
            BSS_Feld(KMP_syn_muster_t*) muster;
        } weiche;

        struct
        {
            KMP_syn_ausdruck_t* ausdruck;
        } res;

        struct
        {
            KMP_syn_ausdruck_t* markierung;
        } sprung;

        struct
        {
            BSS_text_t markierung;
        } markierung;
    };
};

KMP_syn_muster_t* kmp_syn_muster(BSS_speicher_t* speicher, KMP_spanne_t spanne, KMP_syn_ausdruck_t *muster, KMP_syn_anweisung_t *anweisung);
KMP_syn_marke_t* kmp_syn_marke(BSS_speicher_t* speicher, KMP_spanne_t spanne, BSS_text_t wert);
KMP_syn_knoten_t* kmp_syn_knoten(BSS_speicher_t *speicher, g32 art, KMP_spanne_t spanne);

KMP_syn_ausdruck_t* kmp_syn_ausdruck(BSS_speicher_t* speicher, g32 art, KMP_spanne_t spanne);
KMP_syn_ausdruck_t* kmp_syn_ausdruck_ungültig(BSS_speicher_t* speicher, KMP_spanne_t spanne);
KMP_syn_ausdruck_t* kmp_syn_ausdruck_ganzzahl(BSS_speicher_t* speicher, KMP_spanne_t spanne, g32 wert);
KMP_syn_ausdruck_t* kmp_syn_ausdruck_dezimalzahl(BSS_speicher_t* speicher, KMP_spanne_t spanne, d32 wert);
KMP_syn_ausdruck_t* kmp_syn_ausdruck_bezeichner(BSS_speicher_t* speicher, KMP_spanne_t spanne, BSS_text_t wert);
KMP_syn_ausdruck_t* kmp_syn_ausdruck_text(BSS_speicher_t* speicher, KMP_spanne_t spanne, BSS_text_t wert);
KMP_syn_ausdruck_t* kmp_syn_ausdruck_binär(BSS_speicher_t* speicher, KMP_spanne_t spanne, g32 op, KMP_syn_ausdruck_t* links, KMP_syn_ausdruck_t* rechts);
KMP_syn_ausdruck_t* kmp_syn_ausdruck_unär(BSS_speicher_t* speicher, KMP_spanne_t spanne, g32 op, KMP_syn_ausdruck_t* ausdruck);
KMP_syn_ausdruck_t* kmp_syn_ausdruck_klammer(BSS_speicher_t* speicher, KMP_spanne_t spanne, KMP_syn_ausdruck_t* ausdruck);
KMP_syn_ausdruck_t* kmp_syn_ausdruck_index(BSS_speicher_t* speicher, KMP_spanne_t spanne, KMP_syn_ausdruck_t* basis, KMP_syn_ausdruck_t* index);
KMP_syn_ausdruck_t* kmp_syn_ausdruck_eigenschaft(BSS_speicher_t* speicher, KMP_spanne_t spanne, KMP_syn_ausdruck_t* basis, KMP_syn_ausdruck_t* eigenschaft);
KMP_syn_ausdruck_t* kmp_syn_ausdruck_aufruf(BSS_speicher_t* speicher, KMP_spanne_t spanne, KMP_syn_ausdruck_t* basis, BSS_Feld(KMP_syn_ausdruck_t*) argumente);
KMP_syn_ausdruck_t* kmp_syn_ausdruck_ausführen(BSS_speicher_t* speicher, KMP_spanne_t spanne, KMP_syn_ausdruck_t* ausdruck);
KMP_syn_ausdruck_t* kmp_syn_ausdruck_reihe(BSS_speicher_t* speicher, KMP_spanne_t spanne, KMP_syn_ausdruck_t* min, KMP_syn_ausdruck_t* max);
KMP_syn_ausdruck_t* kmp_syn_ausdruck_import(BSS_speicher_t* speicher, KMP_spanne_t spanne, BSS_text_t dateiname);
KMP_syn_ausdruck_t* kmp_syn_ausdruck_kompositum(BSS_speicher_t* speicher, KMP_spanne_t spanne, BSS_Feld(KMP_syn_ausdruck_kompositum_eigenschaft_t*) eigenschaften, w32 ist_benamt, KMP_syn_spezifizierung_t* spezifizierung);
KMP_syn_ausdruck_t* kmp_syn_ausdruck_routine(BSS_speicher_t* speicher, KMP_spanne_t spanne, BSS_Feld(KMP_syn_deklaration_t*) parameter, KMP_syn_spezifizierung_t* rückgabe, KMP_syn_anweisung_t* rumpf);
KMP_syn_ausdruck_t* kmp_syn_ausdruck_schablone(BSS_speicher_t* speicher, KMP_spanne_t spanne, BSS_Feld(KMP_syn_deklaration_t*) eigenschaften);
KMP_syn_ausdruck_t* kmp_syn_ausdruck_option(BSS_speicher_t* speicher, KMP_spanne_t spanne, BSS_Feld(KMP_syn_deklaration_t*) eigenschaften);

KMP_syn_spezifizierung_t* kmp_syn_spezifizierung(BSS_speicher_t* speicher, g32 art, KMP_spanne_t spanne);
KMP_syn_spezifizierung_t* kmp_syn_spezifizierung_bezeichner(BSS_speicher_t* speicher, KMP_spanne_t spanne, BSS_text_t name);
KMP_syn_spezifizierung_t* kmp_syn_spezifizierung_feld(BSS_speicher_t* speicher, KMP_spanne_t spanne, KMP_syn_spezifizierung_t* basis, KMP_syn_ausdruck_t* index);
KMP_syn_spezifizierung_t* kmp_syn_spezifizierung_zeiger(BSS_speicher_t* speicher, KMP_spanne_t spanne, KMP_syn_spezifizierung_t* basis);
KMP_syn_spezifizierung_t* kmp_syn_spezifizierung_routine(BSS_speicher_t* speicher, KMP_spanne_t spanne, BSS_Feld(KMP_syn_spezifizierung_t*) parameter, KMP_syn_spezifizierung_t* rückgabe);

KMP_syn_deklaration_t* kmp_syn_deklaration(BSS_speicher_t* speicher, g32 art, KMP_spanne_t spanne, BSS_Feld(BSS_text_t) namen, KMP_syn_spezifizierung_t* spezifizierung);
KMP_syn_deklaration_t* kmp_syn_deklaration_ungültig(BSS_speicher_t* speicher, KMP_spanne_t spanne);
KMP_syn_deklaration_t* kmp_syn_deklaration_variable(BSS_speicher_t* speicher, KMP_spanne_t spanne, BSS_Feld(BSS_text_t) namen, KMP_syn_spezifizierung_t* spezifizierung, KMP_syn_ausdruck_t* initialisierung);
KMP_syn_deklaration_t* kmp_syn_deklaration_routine(BSS_speicher_t* speicher, KMP_spanne_t spanne, BSS_Feld(BSS_text_t) namen, KMP_syn_spezifizierung_t* spezifizierung, KMP_syn_ausdruck_t* ausdruck);
KMP_syn_deklaration_t* kmp_syn_deklaration_schablone(BSS_speicher_t* speicher, KMP_spanne_t spanne, BSS_Feld(BSS_text_t) namen, BSS_Feld(KMP_syn_deklaration_t*) eigenschaften);
KMP_syn_deklaration_t* kmp_syn_deklaration_import(BSS_speicher_t* speicher, KMP_spanne_t spanne, BSS_Feld(BSS_text_t) namen, KMP_syn_ausdruck_t* ausdruck);
KMP_syn_deklaration_t* kmp_syn_deklaration_option(BSS_speicher_t* speicher, KMP_spanne_t spanne, BSS_Feld(BSS_text_t) namen, KMP_syn_ausdruck_t* ausdruck);

KMP_syn_anweisung_t* kmp_syn_anweisung(BSS_speicher_t* speicher, g32 art, KMP_spanne_t spanne);
KMP_syn_anweisung_t* kmp_syn_anweisung_ungültig(BSS_speicher_t* speicher, KMP_spanne_t spanne);
KMP_syn_anweisung_t* kmp_syn_anweisung_zuweisung(BSS_speicher_t* speicher, KMP_spanne_t spanne, KMP_glied_t* op, KMP_syn_ausdruck_t* links, KMP_syn_ausdruck_t* rechts);
KMP_syn_anweisung_t* kmp_syn_anweisung_wenn(BSS_speicher_t* speicher, KMP_spanne_t spanne, KMP_syn_ausdruck_t* bedingung, KMP_syn_anweisung_t* rumpf, KMP_syn_anweisung_t* alternative);
KMP_syn_anweisung_t* kmp_syn_anweisung_für(BSS_speicher_t* speicher, KMP_spanne_t spanne, KMP_syn_ausdruck_t* index, KMP_syn_ausdruck_t* bedingung, KMP_syn_anweisung_t* rumpf);
KMP_syn_anweisung_t* kmp_syn_anweisung_solange(BSS_speicher_t* speicher, KMP_spanne_t spanne, KMP_syn_ausdruck_t* bedingung, KMP_syn_anweisung_t* rumpf);
KMP_syn_anweisung_t* kmp_syn_anweisung_block(BSS_speicher_t* speicher, KMP_spanne_t spanne, BSS_Feld(KMP_syn_anweisung_t*) anweisungen);
KMP_syn_anweisung_t* kmp_syn_anweisung_deklaration(BSS_speicher_t* speicher, KMP_spanne_t spanne, KMP_syn_deklaration_t* deklaration);
KMP_syn_anweisung_t* kmp_syn_anweisung_import(BSS_speicher_t* speicher, KMP_spanne_t spanne, BSS_text_t dateiname);
KMP_syn_anweisung_t* kmp_syn_anweisung_lade(BSS_speicher_t* speicher, KMP_spanne_t spanne, BSS_text_t dateiname);
KMP_syn_anweisung_t* kmp_syn_anweisung_ausdruck(BSS_speicher_t* speicher, KMP_spanne_t spanne, KMP_syn_ausdruck_t* ausdruck);
KMP_syn_anweisung_t* kmp_syn_anweisung_final(BSS_speicher_t* speicher, KMP_spanne_t spanne, KMP_syn_anweisung_t* anweisung);
KMP_syn_anweisung_t* kmp_syn_anweisung_weiche(BSS_speicher_t* speicher, KMP_spanne_t spanne, KMP_syn_ausdruck_t* ausdruck, BSS_Feld(KMP_syn_muster_t*) muster);
KMP_syn_anweisung_t* kmp_syn_anweisung_res(BSS_speicher_t* speicher, KMP_spanne_t spanne, KMP_syn_ausdruck_t* ausdruck);
KMP_syn_anweisung_t* kmp_syn_anweisung_weiter(BSS_speicher_t* speicher, KMP_spanne_t spanne);
KMP_syn_anweisung_t* kmp_syn_anweisung_sprung(BSS_speicher_t* speicher, KMP_spanne_t spanne, KMP_syn_ausdruck_t* markierung);
KMP_syn_anweisung_t* kmp_syn_anweisung_raus(BSS_speicher_t* speicher, KMP_spanne_t spanne);
KMP_syn_anweisung_t* kmp_syn_anweisung_markierung(BSS_speicher_t* speicher, KMP_spanne_t spanne, BSS_text_t markierung);
KMP_syn_ausdruck_kompositum_eigenschaft_t* kmp_syn_ausdruck_kompositum_eigenschaft(BSS_speicher_t* speicher, KMP_spanne_t spanne, BSS_text_t name, KMP_syn_ausdruck_t* ausdruck, w32 ist_benamt);

void kmp_syn_knoten_ausgeben(BSS_speicher_t* speicher, KMP_syn_knoten_t* knoten, g32 tiefe, FILE* ausgabe);
void kmp_syn_ausdruck_ausgeben(BSS_speicher_t* speicher, KMP_syn_ausdruck_t* ausdruck, g32 tiefe, FILE* ausgabe);
void kmp_syn_anweisung_ausgeben(BSS_speicher_t* speicher, KMP_syn_anweisung_t* anweisung, g32 tiefe, FILE* ausgabe);
void kmp_syn_deklaration_ausgeben(BSS_speicher_t* speicher, KMP_syn_deklaration_t* deklaration, g32 tiefe, FILE* ausgabe);
void kmp_syn_spezifizierung_ausgeben(BSS_speicher_t* speicher, KMP_syn_spezifizierung_t* spezifizierung, g32 tiefe, FILE* ausgabe);

#endif


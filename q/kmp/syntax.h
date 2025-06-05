#ifndef __KMP_syn_H__
#define __KMP_syn_H__

#include "basis/feld.h"
#include "kmp/fehler.h"
#include "kmp/ergebnis.h"
#include "kmp/diagnostik.h"
#include "kmp/glied.h"
#include "kmp/asb.h"

typedef struct KMP_syntax_t KMP_syntax_t;

struct KMP_syntax_t
{
    KMP_diagnostik_t diagnostik;
    BSS_Feld(KMP_glied_t*) glieder;
    g32 glied_index;
};

KMP_syntax_t* kmp_syntax(BSS_speicher_t* speicher, BSS_Feld(KMP_glied_t*) glieder);
KMP_syn_asb_t kmp_syn_starten(BSS_speicher_t* speicher, KMP_syntax_t* syn, g32 index);
KMP_ergebnis_t kmp_syn_ausdruck_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syn);
KMP_ergebnis_t kmp_syn_reihe_ausdruck_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syn);
KMP_ergebnis_t kmp_syn_bitschieben_ausdruck_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syn);
KMP_ergebnis_t kmp_syn_vergleich_ausdruck_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syn);
KMP_ergebnis_t kmp_syn_add_ausdruck_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syn);
KMP_ergebnis_t kmp_syn_mult_ausdruck_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syn);
KMP_ergebnis_t kmp_syn_aufruf_ausdruck_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syn);
KMP_ergebnis_t kmp_syn_eigenschaft_ausdruck_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syn);
KMP_ergebnis_t kmp_syn_index_ausdruck_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syn);
KMP_ergebnis_t kmp_syn_basis_ausdruck_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syn);
KMP_syn_spezifizierung_t* kmp_syn_spezifizierung_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syn);
KMP_syn_deklaration_t* kmp_syn_deklaration_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syn, w32 mit_abschluss /* = true */);
KMP_ergebnis_t kmp_syn_anweisung_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syn);
KMP_syn_anweisung_t* kmp_syn_block_anweisung_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syn);
KMP_syn_anweisung_t* kmp_syn_wenn_anweisung_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syn);
KMP_syn_anweisung_t* kmp_syn_für_anweisung_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syn);
KMP_syn_anweisung_t* kmp_syn_solange_anweisung_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syn);
KMP_syn_anweisung_t* kmp_syn_weiche_anweisung_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syn);
KMP_syn_anweisung_t* kmp_syn_deklaration_anweisung_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syn);
KMP_syn_anweisung_t* kmp_syn_final_anweisung_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syn);
KMP_syn_anweisung_t* kmp_syn_res_anweisung_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syn);
KMP_syn_anweisung_t* kmp_syn_weiter_anweisung_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syn);
KMP_syn_anweisung_t* kmp_syn_sprung_anweisung_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syn);
KMP_syn_anweisung_t* kmp_syn_raus_anweisung_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syn);
KMP_syn_anweisung_t* kmp_syn_markierung_anweisung_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syn);
BSS_Feld(KMP_syn_marke_t*) kmp_syn_marken_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syn);
KMP_glied_t * kmp_syn_glied(KMP_syntax_t* syn, g32 versatz /* = 0 */);
KMP_glied_t * kmp_syn_weiter(KMP_syntax_t* syn, g32 anzahl /* = 1 */);
KMP_glied_t * kmp_syn_erwarte(KMP_syntax_t* syn, g32 art);
w32 kmp_syn_passt(KMP_syntax_t* syn, g32 art);
w32 kmp_syn_ist(KMP_syntax_t* syn, g32 art, g32 versatz /* = 0 */);
w32 kmp_syn_dateiende(KMP_syntax_t* syn);

void kmp_syn_melden_spanne(BSS_speicher_t* speicher, KMP_syntax_t* syn, KMP_spanne_t spanne, KMP_fehler_t *fehler);
void kmp_syn_melden_knoten(BSS_speicher_t* speicher, KMP_syntax_t* syn, KMP_syn_knoten_t* knoten, KMP_fehler_t* fehler);
void kmp_syn_melden_glied(BSS_speicher_t* speicher, KMP_syntax_t* syn, KMP_glied_t* glied, KMP_fehler_t* fehler);

g32 kmp_syn_glied_zu_binär_op(KMP_glied_t* glied);
g32 kmp_syn_glied_zu_unär_op(KMP_glied_t* glied);

#endif


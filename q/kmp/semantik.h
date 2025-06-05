#ifndef __KMP_SEMANTIK_H__
#define __KMP_SEMANTIK_H__

#include "kmp/asb.h"
#include "kmp/diagnostik.h"
#include "kmp/operand.h"
#include "kmp/symbol.h"
#include "kmp/zone.h"
#include "kmp/sem_asb.h"

typedef struct KMP_semantik_t KMP_semantik_t;

struct KMP_semantik_t
{
    KMP_sem_zone_t* zone;
    KMP_sem_zone_t* system;
    KMP_sem_zone_t* global;
    KMP_diagnostik_t* diagnostik;
    KMP_syn_asb_t* syn_asb;
    KMP_sem_asb_t* sem_asb;

    KMP_sem_symbol_t* hauptmethode;
    BSS_Feld(KMP_syn_knoten_t*) schablonen;
};

KMP_semantik_t* kmp_semantik(BSS_speicher_t* speicher, KMP_syn_asb_t* syn_asb, KMP_sem_zone_t* system, KMP_sem_zone_t* global);
BSS_text_t kmp_sem_markierung_erstellen(BSS_text_t markierung);
KMP_sem_asb_t* kmp_sem_starten(BSS_speicher_t* speicher, KMP_semantik_t* semantik, w32 mit_hauptmethode);
void kmp_sem_system_zone_initialisieren(BSS_speicher_t* speicher, KMP_sem_zone_t *zone);

w32 kmp_sem_registrieren_symbol(BSS_speicher_t* speicher, KMP_semantik_t* semantik, KMP_sem_zone_t* zone, BSS_text_t name, KMP_sem_symbol_t* symbol);
w32 kmp_sem_ist_registriert(KMP_semantik_t* semantik, BSS_text_t name);

void kmp_sem_importe_registrieren(BSS_speicher_t* speicher, KMP_semantik_t* semantik);
void kmp_sem_deklarationen_registrieren(BSS_speicher_t* speicher, KMP_semantik_t* semantik);
void kmp_sem_symbole_analysieren(BSS_speicher_t* speicher, KMP_semantik_t* semantik, w32 mit_hauptmethode);
// void kmp_sem_datentyp_abschließen(BSS_speicher_t* speicher, KMP_semantik_t* semantik, KMP_sem_datentyp_t* datentyp, w32 basis_eines_zeigers /* = false */);
void kmp_sem_globale_anweisungen_der_hauptmethode_zuordnen(BSS_speicher_t* speicher, KMP_semantik_t* semantik);
KMP_sem_deklaration_t* kmp_sem_symbol_analysieren(BSS_speicher_t* speicher, KMP_semantik_t* semantik, KMP_sem_symbol_t* symbol, KMP_syn_deklaration_t* deklaration);
KMP_sem_deklaration_t* kmp_sem_variable_analysieren(BSS_speicher_t* speicher, KMP_semantik_t* semantik, KMP_sem_symbol_t* symbol, KMP_syn_deklaration_t* deklaration);
KMP_sem_deklaration_t* kmp_sem_routine_analysieren(BSS_speicher_t* speicher, KMP_semantik_t* semantik, KMP_sem_symbol_t* symbol, KMP_syn_deklaration_t* deklaration);
KMP_sem_deklaration_t* kmp_sem_option_analysieren(BSS_speicher_t* speicher, KMP_semantik_t* semantik, KMP_sem_symbol_t* symbol, KMP_syn_deklaration_t* deklaration);
KMP_sem_deklaration_t* kmp_sem_schablone_analysieren(BSS_speicher_t* speicher, KMP_semantik_t* semantik, KMP_sem_symbol_t* symbol, KMP_syn_deklaration_t* deklaration, w32 zirkularität_ignorieren /* = false */);

w32 kmp_sem_anweisung_analysieren(BSS_speicher_t* speicher, KMP_semantik_t* semantik, KMP_syn_anweisung_t* anweisung, KMP_sem_datentyp_t* über /* = NULL */, KMP_sem_anweisung_t** bestimmte_anweisung /* = NULL */);
KMP_sem_deklaration_t* kmp_sem_deklaration_analysieren(BSS_speicher_t* speicher, KMP_semantik_t* semantik, KMP_syn_deklaration_t* deklaration);
KMP_sem_datentyp_t* kmp_sem_spezifizierung_analysieren(BSS_speicher_t* speicher, KMP_semantik_t* semantik, KMP_syn_spezifizierung_t* spezifizierung);
KMP_sem_ausdruck_t* kmp_sem_ausdruck_analysieren(BSS_speicher_t* speicher, KMP_semantik_t* semantik, KMP_syn_ausdruck_t* ausdruck, KMP_sem_datentyp_t* erwarteter_datentyp /* = NULL */);
KMP_sem_symbol_t* kmp_sem_bezeichner_analysieren(BSS_speicher_t* speicher, KMP_semantik_t* semantik, BSS_text_t bezeichner);
KMP_sem_ausdruck_t* kmp_sem_muster_analysieren(BSS_speicher_t* speicher, KMP_semantik_t* semantik, KMP_syn_ausdruck_t* muster, KMP_sem_datentyp_t* datentyp);

void kmp_sem_import_verarbeiten(BSS_speicher_t* speicher, KMP_semantik_t* semantik, BSS_text_t dateiname);
w32 kmp_sem_operanden_kompatibel(KMP_sem_operand_t* ziel, KMP_sem_operand_t* quelle);

void kmp_sem_zone_betreten(BSS_speicher_t* speicher, KMP_semantik_t* semantik, KMP_sem_zone_t *zone /* = NULL */);
void kmp_sem_zone_verlassen(KMP_semantik_t* semantik);

#endif


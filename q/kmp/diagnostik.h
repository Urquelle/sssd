#ifndef __KMP_DIAGNOSTIK_H__
#define __KMP_DIAGNOSTIK_H__

#include "basis/feld.h"
#include "kmp/fehler.h"
#include "kmp/spanne.h"

struct KMP_syn_deklaration_t;
struct KMP_syn_ausdruck_t;
struct KMP_syn_anweisung_t;
struct KMP_syn_spezifizierung_t;
struct KMP_sem_datentyp_t;

typedef struct KMP_meldung_t KMP_meldung_t;
struct KMP_meldung_t
{
    KMP_spanne_t spanne;
    KMP_fehler_t* fehler;
};

typedef struct KMP_diagnostik_t KMP_diagnostik_t;
struct KMP_diagnostik_t
{
    BSS_feld_t meldungen;
};


KMP_diagnostik_t kmp_diagnostik();
w32 kmp_diagnostik_hat_meldungen(KMP_diagnostik_t diagnostik);
void kmp_diagnostik_melden(BSS_speicher_t* speicher, KMP_diagnostik_t* diagnostik, KMP_meldung_t* meldung);
void kmp_diagnostik_melden_fehler(BSS_speicher_t* speicher, KMP_diagnostik_t* diagnostik, KMP_spanne_t spanne, KMP_fehler_t* fehler);
void kmp_diagnostik_meldungen_löschen(KMP_diagnostik_t* diagnostik);
void kmp_diagnostik_meldung_ausgeben(KMP_meldung_t m, FILE* ausgabe);
void kmp_diagnostik_datei_verknüpfung_ausgeben(char* dateiname, g32 versatz, FILE* ausgabe);

void kmp_diagnostik_melden_fehler_bezeichner(BSS_speicher_t* speicher, KMP_diagnostik_t* diagnostik, KMP_spanne_t spanne, BSS_text_t bezeichner);
void kmp_diagnostik_melden_fehler_datentypen_inkompatibel(BSS_speicher_t* speicher, KMP_diagnostik_t* diagnostik, KMP_spanne_t spanne, struct KMP_sem_datentyp_t* dt1, struct KMP_sem_datentyp_t* dt2);
void kmp_diagnostik_melden_fehler_unbekannte_deklaration(BSS_speicher_t* speicher, KMP_diagnostik_t* diagnostik, KMP_spanne_t spanne, struct KMP_syn_deklaration_t* deklaration);
void kmp_diagnostik_melden_fehler_unbekannter_ausdruck(BSS_speicher_t* speicher, KMP_diagnostik_t* diagnostik, KMP_spanne_t spanne, struct KMP_syn_ausdruck_t* ausdruck);
void kmp_diagnostik_melden_fehler_unbekannte_anweisung(BSS_speicher_t* speicher, KMP_diagnostik_t* diagnostik, KMP_spanne_t spanne, struct KMP_syn_anweisung_t* anweisung);
void kmp_diagnostik_melden_fehler_unbekannte_spezifizierung(BSS_speicher_t* speicher, KMP_diagnostik_t* diagnostik, KMP_spanne_t spanne, struct KMP_syn_spezifizierung_t* spezifizierung);

KMP_meldung_t* kmp_meldung(BSS_speicher_t* speicher, KMP_spanne_t spanne, KMP_fehler_t* fehler);

#endif


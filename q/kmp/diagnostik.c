#include "kmp/diagnostik.h"
#include "basis/feld.h"

#include <stdlib.h>

// Escape-Sequenzen für verschiedene Terminal-Typen
#define ANSI_HYPERLINK_START "\x1b]8;;"
#define ANSI_HYPERLINK_END "\x1b]8;;\a"
#define ANSI_HYPERLINK_MID "\a"

KMP_diagnostik_t kmp_diagnostik()
{
    KMP_diagnostik_t erg = {};

    return erg;
}

KMP_meldung_t*
kmp_meldung(BSS_speicher_t* speicher, KMP_spanne_t spanne, KMP_fehler_t* fehler)
{
    KMP_meldung_t* erg = speicher->anfordern(speicher, sizeof(KMP_meldung_t));

    erg->spanne = spanne;
    erg->fehler = fehler;

    return erg;
}

w32
kmp_diagnostik_hat_meldungen(KMP_diagnostik_t diagnostik)
{
    w32 erg = diagnostik.meldungen.anzahl_elemente > 0;

    return erg;
}

void
kmp_diagnostik_melden(BSS_speicher_t* speicher, KMP_diagnostik_t* diagnostik, KMP_meldung_t *meldung)
{
    bss_feld_hinzufügen(speicher, &diagnostik->meldungen, meldung);
}

void
kmp_diagnostik_melden_fehler(BSS_speicher_t* speicher, KMP_diagnostik_t* diagnostik, KMP_spanne_t spanne, KMP_fehler_t* fehler)
{
    KMP_meldung_t* meldung = kmp_meldung(speicher, spanne, fehler);
    bss_feld_hinzufügen(speicher, &diagnostik->meldungen, meldung);
}

void
kmp_diagnostik_meldungen_löschen(KMP_diagnostik_t *diagnostik)
{
    bss_feld_leeren(&diagnostik->meldungen);
}

void
kmp_diagnostik_meldung_ausgeben(KMP_meldung_t m, FILE *ausgabe)
{
    kmp_diagnostik_datei_verknüpfung_ausgeben(m.spanne.von.quelldatei.daten, m.spanne.von.versatz, ausgabe);
    fprintf(ausgabe, " fehler: %s\n\t", m.fehler->text.daten);
    kmp_spanne_ausgeben(m.spanne, ausgabe);
}

void
kmp_diagnostik_datei_verknüpfung_ausgeben(char* dateiname, g32 versatz, FILE *ausgabe)
{
    char pfad[1024];
    _fullpath(pfad, dateiname, sizeof(pfad));

    // Konvertiere Backslashes zu Forward Slashes
    for (char* p = pfad; *p; p++)
    {
        if (*p == '\\') *p = '/';
    }

    // Erstelle den file://-URL mit Byte-Offset
    // Format: file:///path/to/file#byte=N
    fprintf(ausgabe, "%sfile://%s#byte=%d%s%s%s",
           ANSI_HYPERLINK_START,
           pfad,
           versatz,
           ANSI_HYPERLINK_MID,
           dateiname,
           ANSI_HYPERLINK_END);
}

void
kmp_diagnostik_melden_fehler_bezeichner(BSS_speicher_t* speicher, KMP_diagnostik_t* diagnostik, KMP_spanne_t spanne, BSS_text_t bezeichner)
{
    KMP_fehler_t* fehler = kmp_fehler(speicher, bss_text("bezeichner bereits vorhanden."));
    kmp_diagnostik_melden_fehler(speicher, diagnostik, spanne, fehler);
}

void
kmp_diagnostik_melden_fehler_datentypen_inkompatibel(BSS_speicher_t* speicher, KMP_diagnostik_t* diagnostik, KMP_spanne_t spanne, struct KMP_sem_datentyp_t* dt1, struct KMP_sem_datentyp_t* dt2)
{
    KMP_fehler_t* fehler = kmp_fehler(speicher, bss_text("datentypen sind nicht kompatibel."));
    kmp_diagnostik_melden_fehler(speicher, diagnostik, spanne, fehler);
}

void
kmp_diagnostik_melden_fehler_unbekannte_deklaration(BSS_speicher_t* speicher, KMP_diagnostik_t* diagnostik, KMP_spanne_t spanne, struct KMP_syn_deklaration_t* deklaration)
{
    KMP_fehler_t* fehler = kmp_fehler(speicher, bss_text("unbekannte deklaration."));
    kmp_diagnostik_melden_fehler(speicher, diagnostik, spanne, fehler);
}

void
kmp_diagnostik_melden_fehler_unbekannter_ausdruck(BSS_speicher_t* speicher, KMP_diagnostik_t* diagnostik, KMP_spanne_t spanne, struct KMP_syn_ausdruck_t* ausdruck)
{
    KMP_fehler_t* fehler = kmp_fehler(speicher, bss_text("unbekannter ausdruck."));
    kmp_diagnostik_melden_fehler(speicher, diagnostik, spanne, fehler);
}

void
kmp_diagnostik_melden_fehler_unbekannte_spezifizierung(BSS_speicher_t* speicher, KMP_diagnostik_t* diagnostik, KMP_spanne_t spanne, struct KMP_syn_spezifizierung_t* spezifizierung)
{
    KMP_fehler_t* fehler = kmp_fehler(speicher, bss_text("unbekannte spezifizierung."));
    kmp_diagnostik_melden_fehler(speicher, diagnostik, spanne, fehler);
}

void
kmp_diagnostik_melden_fehler_unbekannte_anweisung(BSS_speicher_t* speicher, KMP_diagnostik_t* diagnostik, KMP_spanne_t spanne, struct KMP_syn_anweisung_t* anweisung)
{
    KMP_fehler_t* fehler = kmp_fehler(speicher, bss_text("unbekannte anweisung."));
    kmp_diagnostik_melden_fehler(speicher, diagnostik, spanne, fehler);
}


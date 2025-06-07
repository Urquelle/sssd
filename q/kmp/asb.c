#include "kmp/asb.h"
#include "basis/speicher.h"

g64 global_id = 0;

KMP_syn_muster_t*
kmp_syn_muster(BSS_speicher_t* speicher, KMP_spanne_t spanne, KMP_syn_ausdruck_t *muster, KMP_syn_anweisung_t *anweisung)
{
    KMP_syn_muster_t* erg = speicher->anfordern(speicher, sizeof(KMP_syn_muster_t));

    erg->spanne = spanne;
    erg->muster = muster;
    erg->anweisung = anweisung;

    return erg;
}

KMP_syn_marke_t*
kmp_syn_marke(BSS_speicher_t* speicher, KMP_spanne_t spanne, BSS_text_t wert)
{
    KMP_syn_marke_t* erg = speicher->anfordern(speicher, sizeof(KMP_syn_marke_t));

    erg->spanne = spanne;
    erg->wert = wert;

    return erg;
}

KMP_syn_knoten_t*
kmp_syn_knoten(BSS_speicher_t* speicher, g32 art, KMP_spanne_t spanne)
{
    KMP_syn_knoten_t* erg = speicher->anfordern(speicher, sizeof(KMP_syn_knoten_t));

    erg->id = ++global_id;
    erg->art = art;
    erg->spanne = spanne;

    return erg;
}
// ausdruck {{{
KMP_syn_ausdruck_t*
kmp_syn_ausdruck(BSS_speicher_t* speicher, g32 art, KMP_spanne_t spanne)
{
    KMP_syn_ausdruck_t* erg  = speicher->anfordern(speicher, sizeof(KMP_syn_ausdruck_t));
    erg->basis = kmp_syn_knoten(speicher, art, spanne);

    return erg;
}

KMP_syn_ausdruck_t*
kmp_syn_ausdruck_ungültig(BSS_speicher_t* speicher, KMP_spanne_t spanne)
{
    KMP_syn_ausdruck_t* erg = kmp_syn_ausdruck(speicher, KMP_SYN_KNOTEN_AUSDRUCK_UNGÜLTIG, spanne);

    return erg;
}

KMP_syn_ausdruck_t*
kmp_syn_ausdruck_ganzzahl(BSS_speicher_t* speicher, KMP_spanne_t spanne, g32 wert)
{
    KMP_syn_ausdruck_t* erg = kmp_syn_ausdruck(speicher, KMP_SYN_KNOTEN_AUSDRUCK_GANZZAHL, spanne);

    erg->ganzzahl.wert = wert;

    return erg;
}

KMP_syn_ausdruck_t*
kmp_syn_ausdruck_dezimalzahl(BSS_speicher_t* speicher, KMP_spanne_t spanne, d32 wert)
{
    KMP_syn_ausdruck_t* erg = kmp_syn_ausdruck(speicher, KMP_SYN_KNOTEN_AUSDRUCK_DEZIMALZAHL, spanne);

    erg->dezimalzahl.wert = wert;

    return erg;
}

KMP_syn_ausdruck_t*
kmp_syn_ausdruck_bezeichner(BSS_speicher_t* speicher, KMP_spanne_t spanne, BSS_text_t wert)
{
    KMP_syn_ausdruck_t* erg = kmp_syn_ausdruck(speicher, KMP_SYN_KNOTEN_AUSDRUCK_BEZEICHNER, spanne);

    erg->bezeichner.wert = wert;

    return erg;
}

KMP_syn_ausdruck_t*
kmp_syn_ausdruck_text(BSS_speicher_t* speicher, KMP_spanne_t spanne, BSS_text_t wert)
{
    KMP_syn_ausdruck_t *erg = kmp_syn_ausdruck(speicher, KMP_SYN_KNOTEN_AUSDRUCK_TEXT, spanne);

    erg->text.wert = wert;

    return erg;
}

KMP_syn_ausdruck_t*
kmp_syn_ausdruck_binär(BSS_speicher_t* speicher, KMP_spanne_t spanne, g32 op, KMP_syn_ausdruck_t* links, KMP_syn_ausdruck_t* rechts)
{
    KMP_syn_ausdruck_t* erg = kmp_syn_ausdruck(speicher, KMP_SYN_KNOTEN_AUSDRUCK_BINÄR, spanne);

    erg->binär.op = op;
    erg->binär.links = links;
    erg->binär.rechts = rechts;

    return erg;
}

KMP_syn_ausdruck_t*
kmp_syn_ausdruck_unär(BSS_speicher_t* speicher, KMP_spanne_t spanne, g32 op, KMP_syn_ausdruck_t* ausdruck)
{
    KMP_syn_ausdruck_t* erg = kmp_syn_ausdruck(speicher, KMP_SYN_KNOTEN_AUSDRUCK_UNÄR, spanne);

    erg->unär.op = op;
    erg->unär.ausdruck = ausdruck;

    return erg;
}

KMP_syn_ausdruck_t*
kmp_syn_ausdruck_klammer(BSS_speicher_t* speicher, KMP_spanne_t spanne, KMP_syn_ausdruck_t* ausdruck)
{
    KMP_syn_ausdruck_t* erg = kmp_syn_ausdruck(speicher, KMP_SYN_KNOTEN_AUSDRUCK_KLAMMER, spanne);

    erg->klammer.ausdruck = ausdruck;

    return erg;
}

KMP_syn_ausdruck_t*
kmp_syn_ausdruck_index(BSS_speicher_t* speicher, KMP_spanne_t spanne, KMP_syn_ausdruck_t* basis, KMP_syn_ausdruck_t* index)
{
    KMP_syn_ausdruck_t* erg = kmp_syn_ausdruck(speicher, KMP_SYN_KNOTEN_AUSDRUCK_INDEX, spanne);

    erg->index.basis = basis;
    erg->index.index = index;

    return erg;
}

KMP_syn_ausdruck_t*
kmp_syn_ausdruck_eigenschaft(BSS_speicher_t* speicher, KMP_spanne_t spanne, KMP_syn_ausdruck_t* basis, KMP_syn_ausdruck_t* eigenschaft)
{
    KMP_syn_ausdruck_t* erg = kmp_syn_ausdruck(speicher, KMP_SYN_KNOTEN_AUSDRUCK_EIGENSCHAFT, spanne);

    erg->eigenschaft.basis = basis;
    erg->eigenschaft.eigenschaft = eigenschaft;

    return erg;
}

KMP_syn_ausdruck_t*
kmp_syn_ausdruck_aufruf(BSS_speicher_t* speicher, KMP_spanne_t spanne, KMP_syn_ausdruck_t* basis, BSS_Feld(KMP_syn_ausdruck_t*) argumente)
{
    KMP_syn_ausdruck_t* erg = kmp_syn_ausdruck(speicher, KMP_SYN_KNOTEN_AUSDRUCK_AUFRUF, spanne);

    erg->aufruf.basis = basis;
    erg->aufruf.argumente = argumente;

    return erg;
}

KMP_syn_ausdruck_t*
kmp_syn_ausdruck_ausführen(BSS_speicher_t* speicher, KMP_spanne_t spanne, KMP_syn_ausdruck_t* ausdruck)
{
    KMP_syn_ausdruck_t* erg = kmp_syn_ausdruck(speicher, KMP_SYN_KNOTEN_AUSDRUCK_AUSFÜHREN, spanne);

    erg->ausführen.ausdruck = ausdruck;

    return erg;
}

KMP_syn_ausdruck_t*
kmp_syn_ausdruck_reihe(BSS_speicher_t* speicher, KMP_spanne_t spanne, KMP_syn_ausdruck_t* min, KMP_syn_ausdruck_t* max)
{
    KMP_syn_ausdruck_t* erg = kmp_syn_ausdruck(speicher, KMP_SYN_KNOTEN_AUSDRUCK_REIHE, spanne);

    erg->reihe.min = min;
    erg->reihe.max = max;

    return erg;
}

KMP_syn_ausdruck_t*
kmp_syn_ausdruck_import(BSS_speicher_t* speicher, KMP_spanne_t spanne, BSS_text_t dateiname)
{
    KMP_syn_ausdruck_t* erg = kmp_syn_ausdruck(speicher, KMP_SYN_KNOTEN_AUSDRUCK_IMPORT, spanne);

    erg->import.dateiname = dateiname;

    return erg;
}

KMP_syn_ausdruck_t*
kmp_syn_ausdruck_kompositum(BSS_speicher_t* speicher, KMP_spanne_t spanne, BSS_Feld(KMP_syn_ausdruck_kompositum_eigenschaft_t*) eigenschaften, w32 ist_benamt, KMP_syn_spezifizierung_t* spezifizierung)
{
    KMP_syn_ausdruck_t* erg = kmp_syn_ausdruck(speicher, KMP_SYN_KNOTEN_AUSDRUCK_KOMPOSITUM, spanne);

    erg->kompositum.eigenschaften = eigenschaften;
    erg->kompositum.ist_benamt = ist_benamt;
    erg->kompositum.spezifizierung = spezifizierung;

    return erg;
}

KMP_syn_ausdruck_t*
kmp_syn_ausdruck_routine(BSS_speicher_t* speicher, KMP_spanne_t spanne, BSS_Feld(KMP_syn_deklaration_t*) parameter, KMP_syn_spezifizierung_t* rückgabe, KMP_syn_anweisung_t* rumpf)
{
    KMP_syn_ausdruck_t* erg = kmp_syn_ausdruck(speicher, KMP_SYN_KNOTEN_AUSDRUCK_ROUTINE, spanne);

    erg->routine.parameter = parameter;
    erg->routine.rückgabe = rückgabe;
    erg->routine.rumpf = rumpf;

    return erg;
}

KMP_syn_ausdruck_t*
kmp_syn_ausdruck_schablone(BSS_speicher_t* speicher, KMP_spanne_t spanne, BSS_Feld(KMP_syn_deklaration_t*) eigenschaften)
{
    KMP_syn_ausdruck_t* erg = kmp_syn_ausdruck(speicher, KMP_SYN_KNOTEN_AUSDRUCK_SCHABLONE, spanne);

    erg->schablone.eigenschaften = eigenschaften;

    return erg;
}

KMP_syn_ausdruck_t*
kmp_syn_ausdruck_option(BSS_speicher_t* speicher, KMP_spanne_t spanne, BSS_Feld(KMP_syn_deklaration_t*) eigenschaften)
{
    KMP_syn_ausdruck_t* erg = kmp_syn_ausdruck(speicher, KMP_SYN_KNOTEN_AUSDRUCK_OPTION, spanne);

    erg->option.eigenschaften = eigenschaften;

    return erg;
}
// }}}
// spezifizierung {{{
KMP_syn_spezifizierung_t*
kmp_syn_spezifizierung(BSS_speicher_t* speicher, g32 art, KMP_spanne_t spanne)
{
    KMP_syn_spezifizierung_t* erg = speicher->anfordern(speicher, sizeof(KMP_syn_spezifizierung_t));
    erg->basis = kmp_syn_knoten(speicher, art, spanne);
    erg->größe = 0;

    return erg;
}

KMP_syn_spezifizierung_t*
kmp_syn_spezifizierung_ungültig(BSS_speicher_t* speicher, KMP_spanne_t spanne)
{
    KMP_syn_spezifizierung_t* erg = kmp_syn_spezifizierung(speicher, KMP_SYN_KNOTEN_SPEZIFIZIERUNG_UNGÜLTIG, spanne);

    return erg;
}

KMP_syn_spezifizierung_t*
kmp_syn_spezifizierung_bezeichner(BSS_speicher_t* speicher, KMP_spanne_t spanne, BSS_text_t name)
{
    KMP_syn_spezifizierung_t* erg = kmp_syn_spezifizierung(speicher, KMP_SYN_KNOTEN_SPEZIFIZIERUNG_BEZEICHNER, spanne);

    erg->bezeichner.name = name;

    return erg;
}

KMP_syn_spezifizierung_t*
kmp_syn_spezifizierung_feld(BSS_speicher_t* speicher, KMP_spanne_t spanne, KMP_syn_spezifizierung_t* basis, KMP_syn_ausdruck_t* index)
{
    KMP_syn_spezifizierung_t* erg = kmp_syn_spezifizierung(speicher, KMP_SYN_KNOTEN_SPEZIFIZIERUNG_FELD, spanne);

    erg->feld.basis = basis;
    erg->feld.index = index;

    return erg;
}

KMP_syn_spezifizierung_t*
kmp_syn_spezifizierung_zeiger(BSS_speicher_t* speicher, KMP_spanne_t spanne, KMP_syn_spezifizierung_t* basis)
{
    KMP_syn_spezifizierung_t* erg = kmp_syn_spezifizierung(speicher, KMP_SYN_KNOTEN_SPEZIFIZIERUNG_ZEIGER, spanne);

    erg->zeiger.basis = basis;

    return erg;
}

KMP_syn_spezifizierung_t*
kmp_syn_spezifizierung_routine(BSS_speicher_t* speicher, KMP_spanne_t spanne, BSS_Feld(KMP_syn_spezifizierung_t*) parameter, KMP_syn_spezifizierung_t* rückgabe)
{
    KMP_syn_spezifizierung_t* erg = kmp_syn_spezifizierung(speicher, KMP_SYN_KNOTEN_SPEZIFIZIERUNG_ROUTINE, spanne);

    erg->routine.parameter = parameter;
    erg->routine.rückgabe = rückgabe;

    return erg;
}
// }}}
// deklaration {{{
KMP_syn_deklaration_t*
kmp_syn_deklaration(BSS_speicher_t* speicher, g32 art, KMP_spanne_t spanne, BSS_Feld(BSS_text_t) namen, KMP_syn_spezifizierung_t* spezifizierung)
{
    KMP_syn_deklaration_t* erg = speicher->anfordern(speicher, sizeof(KMP_syn_deklaration_t));
    erg->basis = kmp_syn_knoten(speicher, art, spanne);
    erg->namen = namen;
    erg->spezifizierung = spezifizierung;
    erg->symbol = NULL;

    return erg;
}

KMP_syn_deklaration_t*
kmp_syn_deklaration_ungültig(BSS_speicher_t* speicher, KMP_spanne_t spanne)
{
    KMP_syn_deklaration_t* erg = kmp_syn_deklaration(speicher, KMP_SYN_KNOTEN_DEKLARATION_UNGÜLTIG, spanne, bss_feld(speicher, 0), kmp_syn_spezifizierung_ungültig(speicher, spanne));

    return erg;
}

KMP_syn_deklaration_t*
kmp_syn_deklaration_variable(BSS_speicher_t* speicher, KMP_spanne_t spanne, BSS_Feld(BSS_text_t) namen, KMP_syn_spezifizierung_t* spezifizierung, KMP_syn_ausdruck_t* initialisierung)
{
    KMP_syn_deklaration_t* erg = kmp_syn_deklaration(speicher, KMP_SYN_KNOTEN_DEKLARATION_VARIABLE, spanne, namen, spezifizierung);

    erg->variable.initialisierung = initialisierung;

    return erg;
}

KMP_syn_deklaration_t*
kmp_syn_deklaration_routine(BSS_speicher_t* speicher, KMP_spanne_t spanne, BSS_Feld(BSS_text_t) namen, KMP_syn_spezifizierung_t* spezifizierung, KMP_syn_ausdruck_t* ausdruck)
{
    KMP_syn_deklaration_t* erg = kmp_syn_deklaration(speicher, KMP_SYN_KNOTEN_DEKLARATION_ROUTINE, spanne, namen, spezifizierung);

    erg->routine.ausdruck = ausdruck;

    return erg;
}

KMP_syn_deklaration_t*
kmp_syn_deklaration_schablone(BSS_speicher_t* speicher, KMP_spanne_t spanne, BSS_Feld(BSS_text_t) namen, BSS_Feld(KMP_syn_deklaration_t*) eigenschaften)
{
    KMP_syn_deklaration_t* erg = kmp_syn_deklaration(speicher, KMP_SYN_KNOTEN_DEKLARATION_SCHABLONE, spanne, namen, NULL);

    erg->schablone.eigenschaften = eigenschaften;

    return erg;
}

KMP_syn_deklaration_t*
kmp_syn_deklaration_import(BSS_speicher_t* speicher, KMP_spanne_t spanne, BSS_Feld(BSS_text_t) namen, KMP_syn_ausdruck_t* ausdruck)
{
    KMP_syn_deklaration_t* erg = kmp_syn_deklaration(speicher, KMP_SYN_KNOTEN_DEKLARATION_IMPORT, spanne, namen, NULL);

    erg->import.ausdruck = ausdruck;

    return erg;
}

KMP_syn_deklaration_t*
kmp_syn_deklaration_option(BSS_speicher_t* speicher, KMP_spanne_t spanne, BSS_Feld(BSS_text_t) namen, KMP_syn_ausdruck_t* ausdruck)
{
    KMP_syn_deklaration_t* erg = kmp_syn_deklaration(speicher, KMP_SYN_KNOTEN_DEKLARATION_OPTION, spanne, namen, NULL);

    erg->option.ausdruck = ausdruck;

    return erg;
}
// }}}
// anweisung {{{
KMP_syn_anweisung_t*
kmp_syn_anweisung(BSS_speicher_t* speicher, g32 art, KMP_spanne_t spanne)
{
    KMP_syn_anweisung_t* erg = speicher->anfordern(speicher, sizeof(KMP_syn_deklaration_t));
    erg->basis = kmp_syn_knoten(speicher, art, spanne);

    return erg;
}

KMP_syn_anweisung_t*
kmp_syn_anweisung_ungültig(BSS_speicher_t* speicher, KMP_spanne_t spanne)
{
    KMP_syn_anweisung_t* erg = kmp_syn_anweisung(speicher, KMP_SYN_KNOTEN_ANWEISUNG_UNGÜLTIG, spanne);

    return erg;
}

KMP_syn_anweisung_t*
kmp_syn_anweisung_zuweisung(BSS_speicher_t* speicher, KMP_spanne_t spanne, KMP_glied_t* op, KMP_syn_ausdruck_t* links, KMP_syn_ausdruck_t* rechts)
{
    KMP_syn_anweisung_t* erg = kmp_syn_anweisung(speicher, KMP_SYN_KNOTEN_ANWEISUNG_ZUWEISUNG, spanne);

    erg->zuweisung.op = op;
    erg->zuweisung.links = links;
    erg->zuweisung.rechts = rechts;

    return erg;
}

KMP_syn_anweisung_t*
kmp_syn_anweisung_wenn(BSS_speicher_t* speicher, KMP_spanne_t spanne, KMP_syn_ausdruck_t* bedingung, KMP_syn_anweisung_t* rumpf, KMP_syn_anweisung_t* alternative)
{
    KMP_syn_anweisung_t* erg = kmp_syn_anweisung(speicher, KMP_SYN_KNOTEN_ANWEISUNG_WENN, spanne);

    erg->wenn.bedingung = bedingung;
    erg->wenn.rumpf = rumpf;
    erg->wenn.alternative = alternative;

    return erg;
}

KMP_syn_anweisung_t*
kmp_syn_anweisung_für(BSS_speicher_t* speicher, KMP_spanne_t spanne, KMP_syn_ausdruck_t* index, KMP_syn_ausdruck_t* bedingung, KMP_syn_anweisung_t* rumpf)
{
    KMP_syn_anweisung_t* erg = kmp_syn_anweisung(speicher, KMP_SYN_KNOTEN_ANWEISUNG_FÜR, spanne);

    erg->für.index = index;
    erg->für.bedingung = bedingung;
    erg->für.rumpf = rumpf;

    return erg;
}

KMP_syn_anweisung_t*
kmp_syn_anweisung_solange(BSS_speicher_t* speicher, KMP_spanne_t spanne, KMP_syn_ausdruck_t* bedingung, KMP_syn_anweisung_t* rumpf)
{
    KMP_syn_anweisung_t* erg = kmp_syn_anweisung(speicher, KMP_SYN_KNOTEN_ANWEISUNG_SOLANGE, spanne);

    erg->solange.bedingung = bedingung;
    erg->solange.rumpf = rumpf;

    return erg;
}

KMP_syn_anweisung_t*
kmp_syn_anweisung_block(BSS_speicher_t* speicher, KMP_spanne_t spanne, BSS_Feld(KMP_syn_anweisung_t*) anweisungen)
{
    KMP_syn_anweisung_t* erg = kmp_syn_anweisung(speicher, KMP_SYN_KNOTEN_ANWEISUNG_BLOCK, spanne);

    erg->block.anweisungen = anweisungen;

    return erg;
}

KMP_syn_anweisung_t*
kmp_syn_anweisung_deklaration(BSS_speicher_t* speicher, KMP_spanne_t spanne, KMP_syn_deklaration_t* deklaration)
{
    KMP_syn_anweisung_t* erg = kmp_syn_anweisung(speicher, KMP_SYN_KNOTEN_ANWEISUNG_DEKLARATION, spanne);

    erg->deklaration.deklaration = deklaration;

    return erg;
}

KMP_syn_anweisung_t*
kmp_syn_anweisung_import(BSS_speicher_t* speicher, KMP_spanne_t spanne, BSS_text_t dateiname)
{
    KMP_syn_anweisung_t* erg = kmp_syn_anweisung(speicher, KMP_SYN_KNOTEN_ANWEISUNG_IMPORT, spanne);

    erg->import.dateiname = dateiname;

    return erg;
}

KMP_syn_anweisung_t*
kmp_syn_anweisung_lade(BSS_speicher_t* speicher, KMP_spanne_t spanne, BSS_text_t dateiname)
{
    KMP_syn_anweisung_t* erg = kmp_syn_anweisung(speicher, KMP_SYN_KNOTEN_ANWEISUNG_LADE, spanne);

    erg->lade.dateiname = dateiname;

    return erg;
}

KMP_syn_anweisung_t*
kmp_syn_anweisung_ausdruck(BSS_speicher_t* speicher, KMP_spanne_t spanne, KMP_syn_ausdruck_t* ausdruck)
{
    KMP_syn_anweisung_t* erg = kmp_syn_anweisung(speicher, KMP_SYN_KNOTEN_ANWEISUNG_AUSDRUCK, spanne);

    erg->ausdruck.ausdruck = ausdruck;

    return erg;
}

KMP_syn_anweisung_t*
kmp_syn_anweisung_final(BSS_speicher_t* speicher, KMP_spanne_t spanne, KMP_syn_anweisung_t* anweisung)
{
    KMP_syn_anweisung_t* erg = kmp_syn_anweisung(speicher, KMP_SYN_KNOTEN_ANWEISUNG_FINAL, spanne);

    erg->final.anweisung = anweisung;

    return erg;
}

KMP_syn_anweisung_t*
kmp_syn_anweisung_weiche(BSS_speicher_t* speicher, KMP_spanne_t spanne, KMP_syn_ausdruck_t* ausdruck, BSS_Feld(KMP_syn_muster_t*) muster)
{
    KMP_syn_anweisung_t* erg = kmp_syn_anweisung(speicher, KMP_SYN_KNOTEN_ANWEISUNG_WEICHE, spanne);

    erg->weiche.ausdruck = ausdruck;
    erg->weiche.muster = muster;

    return erg;
}

KMP_syn_anweisung_t*
kmp_syn_anweisung_res(BSS_speicher_t* speicher, KMP_spanne_t spanne, KMP_syn_ausdruck_t* ausdruck)
{
    KMP_syn_anweisung_t* erg = kmp_syn_anweisung(speicher, KMP_SYN_KNOTEN_ANWEISUNG_RES, spanne);

    erg->res.ausdruck = ausdruck;

    return erg;
}

KMP_syn_anweisung_t*
kmp_syn_anweisung_weiter(BSS_speicher_t* speicher, KMP_spanne_t spanne)
{
    KMP_syn_anweisung_t* erg = kmp_syn_anweisung(speicher, KMP_SYN_KNOTEN_ANWEISUNG_WEITER, spanne);

    return erg;
}

KMP_syn_anweisung_t*
kmp_syn_anweisung_sprung(BSS_speicher_t* speicher, KMP_spanne_t spanne, KMP_syn_ausdruck_t* markierung)
{
    KMP_syn_anweisung_t* erg = kmp_syn_anweisung(speicher, KMP_SYN_KNOTEN_ANWEISUNG_SPRUNG, spanne);

    erg->sprung.markierung = markierung;

    return erg;
}

KMP_syn_anweisung_t*
kmp_syn_anweisung_raus(BSS_speicher_t* speicher, KMP_spanne_t spanne)
{
    KMP_syn_anweisung_t* erg = kmp_syn_anweisung(speicher, KMP_SYN_KNOTEN_ANWEISUNG_RAUS, spanne);

    return erg;
}

KMP_syn_anweisung_t*
kmp_syn_anweisung_markierung(BSS_speicher_t* speicher, KMP_spanne_t spanne, BSS_text_t markierung)
{
    KMP_syn_anweisung_t* erg = kmp_syn_anweisung(speicher, KMP_SYN_KNOTEN_ANWEISUNG_MARKIERUNG, spanne);

    erg->markierung.markierung = markierung;

    return erg;
}
// }}}

KMP_syn_ausdruck_kompositum_eigenschaft_t*
kmp_syn_ausdruck_kompositum_eigenschaft(BSS_speicher_t* speicher, KMP_spanne_t spanne, BSS_text_t name, KMP_syn_ausdruck_t* ausdruck, w32 ist_benamt)
{
    KMP_syn_ausdruck_kompositum_eigenschaft_t* erg = speicher->anfordern(speicher, sizeof(KMP_syn_ausdruck_kompositum_eigenschaft_t));

    erg->spanne = spanne;
    erg->name = name;
    erg->ausdruck = ausdruck;
    erg->ist_benamt = ist_benamt;

    return erg;
}

void
kmp_syn_knoten_ausgeben(BSS_speicher_t* speicher, KMP_syn_knoten_t* knoten, g32 tiefe, FILE* ausgabe)
{
    fprintf(ausgabe, "SYNKNOTEN!!!\n");
}

void
kmp_syn_ausdruck_ausgeben(BSS_speicher_t* speicher, KMP_syn_ausdruck_t* ausdruck, g32 tiefe, FILE* ausgabe)
{
    kmp_syn_knoten_ausgeben(speicher, ausdruck->basis, tiefe, ausgabe);
    fprintf(ausgabe, "AUSDRUCK!!!\n");
}

void
kmp_syn_anweisung_ausgeben(BSS_speicher_t* speicher, KMP_syn_anweisung_t* anweisung, g32 tiefe, FILE* ausgabe)
{
    kmp_syn_knoten_ausgeben(speicher, anweisung->basis, tiefe, ausgabe);
    fprintf(ausgabe, "ANWEISUNG!!!\n");
}

void
kmp_syn_deklaration_ausgeben(BSS_speicher_t* speicher, KMP_syn_deklaration_t* deklaration, g32 tiefe, FILE* ausgabe)
{
    kmp_syn_knoten_ausgeben(speicher, deklaration->basis, tiefe, ausgabe);
    fprintf(ausgabe, "DEKLARATION!!!\n");
}

void
kmp_syn_spezifizierung_ausgeben(BSS_speicher_t* speicher, KMP_syn_spezifizierung_t* spezifizierung, g32 tiefe, FILE* ausgabe)
{
    kmp_syn_knoten_ausgeben(speicher, spezifizierung->basis, tiefe, ausgabe);
    fprintf(ausgabe, "SPEZIFIZIERUNG!!!\n");
}


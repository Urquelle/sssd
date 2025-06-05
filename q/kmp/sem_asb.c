#include "kmp/sem_asb.h"

#include "kmp/asb.h"

KMP_sem_knoten_t*
kmp_sem_knoten(BSS_speicher_t* speicher, KMP_syn_knoten_t* syn_knoten, g32 art)
{
    KMP_sem_knoten_t* erg = speicher->anfordern(speicher, sizeof(KMP_sem_knoten_t));

    erg->syn_knoten = syn_knoten;
    erg->art = art;

    return erg;
}

KMP_sem_muster_t*
kmp_sem_muster(BSS_speicher_t* speicher, KMP_syn_muster_t* syn_knoten, struct KMP_sem_ausdruck_t* ausdruck, struct KMP_sem_anweisung_t* rumpf)
{
    KMP_sem_muster_t* erg = speicher->anfordern(speicher, sizeof(KMP_sem_muster_t));

    erg->ausdruck = ausdruck;
    erg->rumpf = rumpf;

    return erg;
}

// ausdrücke {{{
KMP_sem_ausdruck_t*
kmp_sem_ausdruck(BSS_speicher_t* speicher, KMP_syn_ausdruck_t* syn_knoten, g32 art, KMP_sem_operand_t* operand)
{
    KMP_sem_ausdruck_t* erg = speicher->anfordern(speicher, sizeof(KMP_sem_ausdruck_t));
    erg->basis = kmp_sem_knoten(speicher, syn_knoten->basis, art);
    erg->operand = operand;

    return erg;
}

KMP_sem_ausdruck_t*
kmp_sem_ausdruck_bezeichner(BSS_speicher_t* speicher, KMP_syn_ausdruck_t* syn_knoten, KMP_sem_operand_t* operand)
{
    KMP_sem_ausdruck_t* erg = kmp_sem_ausdruck(speicher, syn_knoten, KMP_SYN_KNOTEN_AUSDRUCK_BEZEICHNER, operand);

    return erg;
}

KMP_sem_ausdruck_t*
kmp_sem_ausdruck_binär(BSS_speicher_t* speicher, KMP_syn_ausdruck_t* syn_knoten, KMP_sem_operand_t* operand, g32 op, KMP_sem_ausdruck_t* links, KMP_sem_ausdruck_t* rechts)
{
    KMP_sem_ausdruck_t* erg = kmp_sem_ausdruck(speicher, syn_knoten, KMP_SYN_KNOTEN_AUSDRUCK_BINÄR, operand);

    erg->binär.op = op;
    erg->binär.links = links;
    erg->binär.rechts = rechts;

    return erg;
}

KMP_sem_ausdruck_t*
kmp_sem_ausdruck_unär(BSS_speicher_t* speicher, KMP_syn_ausdruck_t* syn_knoten, KMP_sem_operand_t* operand, g32 op, KMP_sem_ausdruck_t* ausdruck)
{
    KMP_sem_ausdruck_t* erg = kmp_sem_ausdruck(speicher, syn_knoten, KMP_SYN_KNOTEN_AUSDRUCK_UNÄR, operand);

    erg->unär.op = op;
    erg->unär.ausdruck = ausdruck;

    return erg;
}

KMP_sem_ausdruck_t*
kmp_sem_ausdruck_index(BSS_speicher_t* speicher, KMP_syn_ausdruck_t* syn_knoten, KMP_sem_operand_t* operand, KMP_sem_ausdruck_t* basis, KMP_sem_ausdruck_t* index)
{
    KMP_sem_ausdruck_t* erg = kmp_sem_ausdruck(speicher, syn_knoten, KMP_SYN_KNOTEN_AUSDRUCK_INDEX, operand);

    erg->index.basis = basis;
    erg->index.index = index;

    return erg;
}

KMP_sem_ausdruck_t*
kmp_sem_ausdruck_eigenschaft(BSS_speicher_t* speicher, KMP_syn_ausdruck_t* syn_knoten, KMP_sem_operand_t* operand, KMP_sem_ausdruck_t* basis, KMP_sem_symbol_t* eigenschaft)
{
    KMP_sem_ausdruck_t* erg = kmp_sem_ausdruck(speicher, syn_knoten, KMP_SYN_KNOTEN_AUSDRUCK_INDEX, operand);

    erg->eigenschaft.basis = basis;
    erg->eigenschaft.eigenschaft = eigenschaft;

    return erg;
}

KMP_sem_ausdruck_t*
kmp_sem_ausdruck_aufruf(BSS_speicher_t* speicher, KMP_syn_ausdruck_t* syn_knoten, KMP_sem_operand_t* operand, KMP_sem_ausdruck_t* basis, BSS_Feld(KMP_sem_ausdruck_t*) argumente)
{
    KMP_sem_ausdruck_t* erg = kmp_sem_ausdruck(speicher, syn_knoten, KMP_SYN_KNOTEN_AUSDRUCK_AUFRUF, operand);

    erg->aufruf.basis = basis;
    erg->aufruf.argumente = argumente;

    return erg;
}

KMP_sem_ausdruck_t*
kmp_sem_ausdruck_ausführen(BSS_speicher_t* speicher, KMP_syn_ausdruck_t* syn_knoten, KMP_sem_operand_t* operand,
    KMP_sem_ausdruck_t* ausdruck)
{
    KMP_sem_ausdruck_t* erg = kmp_sem_ausdruck(speicher, syn_knoten, KMP_SYN_KNOTEN_AUSDRUCK_AUSFÜHREN, operand);

    erg->ausführen.ausdruck = ausdruck;

    return erg;
}

KMP_sem_ausdruck_t*
kmp_sem_ausdruck_reihe(BSS_speicher_t* speicher, KMP_syn_ausdruck_t* syn_knoten, KMP_sem_operand_t* operand,
    KMP_sem_ausdruck_t* min, KMP_sem_ausdruck_t* max)
{
    KMP_sem_ausdruck_t* erg = kmp_sem_ausdruck(speicher, syn_knoten, KMP_SYN_KNOTEN_AUSDRUCK_REIHE, operand);

    erg->reihe.min = min;
    erg->reihe.max = max;

    return erg;
}

KMP_sem_ausdruck_t*
kmp_sem_ausdruck_kompositum(BSS_speicher_t* speicher, KMP_syn_ausdruck_t* syn_knoten, KMP_sem_operand_t* operand,
    BSS_Feld(KMP_sem_eigenschaft_t*) eigenschaften)
{
    KMP_sem_ausdruck_t* erg = kmp_sem_ausdruck(speicher, syn_knoten, KMP_SYN_KNOTEN_AUSDRUCK_KOMPOSITUM, operand);

    erg->kompositum.eigenschaften = eigenschaften;

    return erg;
}

KMP_sem_eigenschaft_t*
kmp_sem_eigenschaft(BSS_speicher_t* speicher, KMP_sem_operand_t* operand)
{
    KMP_sem_eigenschaft_t* erg = speicher->anfordern(speicher, sizeof(KMP_sem_eigenschaft_t));

    erg->operand = operand;

    return erg;
}
// }}}
// deklarationen {{{
KMP_sem_deklaration_t*
kmp_sem_deklaration(BSS_speicher_t* speicher, KMP_syn_deklaration_t* syn_knoten, g32 art, BSS_Feld(KMP_sem_symbol_t*) symbole, struct KMP_sem_datentyp_t* datentyp, KMP_sem_ausdruck_t* initialisierung)
{
    KMP_sem_deklaration_t* erg = speicher->anfordern(speicher, sizeof(KMP_sem_deklaration_t));
    erg->basis = kmp_sem_knoten(speicher, syn_knoten->basis, art);
    erg->deklaration = NULL;
    erg->datentyp = datentyp;
    erg->symbole = symbole;
    erg->initialisierung = initialisierung;

    return erg;
}

KMP_sem_deklaration_t*
kmp_sem_deklaration_routine(BSS_speicher_t* speicher, KMP_syn_deklaration_t* syn_knoten, BSS_Feld(KMP_sem_symbol_t*) symbole, struct KMP_sem_datentyp_t* datentyp, KMP_sem_anweisung_t* rumpf)
{
    KMP_sem_deklaration_t* erg = kmp_sem_deklaration(speicher, syn_knoten, KMP_SYN_KNOTEN_DEKLARATION_ROUTINE, symbole, datentyp, NULL);

    erg->routine.rumpf = rumpf;

    return erg;
}

KMP_sem_deklaration_t*
kmp_sem_deklaration_option(BSS_speicher_t* speicher, KMP_syn_deklaration_t* syn_knoten, BSS_Feld(KMP_sem_symbol_t*) symbole, struct KMP_sem_datentyp_t* datentyp)
{
    KMP_sem_deklaration_t* erg = kmp_sem_deklaration(speicher, syn_knoten, KMP_SYN_KNOTEN_DEKLARATION_OPTION, symbole, datentyp, NULL);

    return erg;
}

KMP_sem_deklaration_t*
kmp_sem_deklaration_schablone(BSS_speicher_t* speicher, KMP_syn_deklaration_t* syn_knoten, BSS_Feld(KMP_sem_symbol_t*) symbole, struct KMP_sem_datentyp_t* datentyp)
{
    KMP_sem_deklaration_t* erg = kmp_sem_deklaration(speicher, syn_knoten, KMP_SYN_KNOTEN_DEKLARATION_SCHABLONE, symbole, datentyp, NULL);

    return erg;
}
// }}}
// anweisungen {{{
KMP_sem_anweisung_t*
kmp_sem_anweisung(BSS_speicher_t* speicher, KMP_syn_anweisung_t* syn_knoten, g32 art)
{
    KMP_sem_anweisung_t* erg = speicher->anfordern(speicher, sizeof(KMP_sem_anweisung_t));
    erg->basis = kmp_sem_knoten(speicher, syn_knoten->basis, art);;

    return erg;
}

KMP_sem_anweisung_t*
kmp_sem_anweisung_deklaration(BSS_speicher_t* speicher, KMP_syn_anweisung_t* syn_knoten, KMP_sem_deklaration_t* deklaration)
{
    KMP_sem_anweisung_t* erg = kmp_sem_anweisung(speicher, syn_knoten, KMP_SYN_KNOTEN_ANWEISUNG_DEKLARATION);

    erg->deklaration.deklaration = deklaration;

    return erg;
}

KMP_sem_anweisung_t*
kmp_sem_anweisung_ausdruck(BSS_speicher_t* speicher, KMP_syn_anweisung_t* syn_knoten, KMP_sem_ausdruck_t* ausdruck)
{
    KMP_sem_anweisung_t* erg = kmp_sem_anweisung(speicher, syn_knoten, KMP_SYN_KNOTEN_ANWEISUNG_AUSDRUCK);

    erg->ausdruck.ausdruck = ausdruck;

    return erg;
}

KMP_sem_anweisung_t*
kmp_sem_anweisung_block(BSS_speicher_t* speicher, KMP_syn_anweisung_t* syn_knoten, BSS_Feld(KMP_sem_anweisung_t*) anweisungen)
{
    KMP_sem_anweisung_t* erg = kmp_sem_anweisung(speicher, syn_knoten, KMP_SYN_KNOTEN_ANWEISUNG_BLOCK);

    erg->block.anweisungen = anweisungen;

    return erg;
}

KMP_sem_anweisung_t*
kmp_sem_anweisung_für(BSS_speicher_t* speicher, KMP_syn_anweisung_t* syn_knoten, KMP_sem_symbol_t* index, KMP_sem_ausdruck_t* bedingung, KMP_sem_anweisung_t* rumpf, BSS_text_t weiter_markierung, BSS_text_t raus_markierung)
{
    KMP_sem_anweisung_t* erg = kmp_sem_anweisung(speicher, syn_knoten, KMP_SYN_KNOTEN_ANWEISUNG_FÜR);

    erg->für.index = index;
    erg->für.bedingung = bedingung;
    erg->für.rumpf = rumpf;

    return erg;
}

KMP_sem_anweisung_t*
kmp_sem_anweisung_res(BSS_speicher_t* speicher, KMP_syn_anweisung_t* syn_knoten, KMP_sem_ausdruck_t* ausdruck)
{
    KMP_sem_anweisung_t* erg = kmp_sem_anweisung(speicher, syn_knoten, KMP_SYN_KNOTEN_ANWEISUNG_RES);

    erg->res.ausdruck = ausdruck;

    return erg;
}

KMP_sem_anweisung_t*
kmp_sem_anweisung_wenn(BSS_speicher_t* speicher, KMP_syn_anweisung_t* syn_knoten, KMP_sem_ausdruck_t* bedingung, KMP_sem_anweisung_t* rumpf, KMP_sem_anweisung_t* alternative)
{
    KMP_sem_anweisung_t* erg = kmp_sem_anweisung(speicher, syn_knoten, KMP_SYN_KNOTEN_ANWEISUNG_WENN);

    erg->wenn.bedingung = bedingung;
    erg->wenn.rumpf = rumpf;
    erg->wenn.alternative = alternative;

    return erg;
}

KMP_sem_anweisung_t*
kmp_sem_anweisung_sprung(BSS_speicher_t* speicher, KMP_syn_anweisung_t* syn_knoten, BSS_text_t markierung)
{
    KMP_sem_anweisung_t* erg = kmp_sem_anweisung(speicher, syn_knoten, KMP_SYN_KNOTEN_ANWEISUNG_SPRUNG);

    erg->sprung.markierung = markierung;

    return erg;
}

KMP_sem_anweisung_t*
kmp_sem_anweisung_markierung(BSS_speicher_t* speicher, KMP_syn_anweisung_t* syn_knoten, BSS_text_t markierung)
{
    KMP_sem_anweisung_t* erg = kmp_sem_anweisung(speicher, syn_knoten, KMP_SYN_KNOTEN_ANWEISUNG_MARKIERUNG);

    erg->markierung.markierung = markierung;

    return erg;
}

KMP_sem_anweisung_t*
kmp_sem_anweisung_solange(BSS_speicher_t* speicher, KMP_syn_anweisung_t* syn_knoten, KMP_sem_ausdruck_t* bedingung, KMP_sem_anweisung_t* rumpf, BSS_text_t weiter_markierung, BSS_text_t raus_markierung)
{
    KMP_sem_anweisung_t* erg = kmp_sem_anweisung(speicher, syn_knoten, KMP_SYN_KNOTEN_ANWEISUNG_SOLANGE);

    erg->solange.bedingung = bedingung;
    erg->solange.rumpf = rumpf;
    erg->solange.weiter_markierung = weiter_markierung;
    erg->solange.raus_markierung = raus_markierung;

    return erg;
}

KMP_sem_anweisung_t*
kmp_sem_anweisung_weiche(BSS_speicher_t* speicher, KMP_syn_anweisung_t* syn_knoten, KMP_sem_ausdruck_t* ausdruck, BSS_Feld(KMP_bestimmtes_muster_t*) muster)
{
    KMP_sem_anweisung_t* erg = kmp_sem_anweisung(speicher, syn_knoten, KMP_SYN_KNOTEN_ANWEISUNG_WEICHE);

    erg->weiche.ausdruck = ausdruck;
    erg->weiche.muster = muster;

    return erg;
}

KMP_sem_anweisung_t*
kmp_sem_anweisung_zuweisung(BSS_speicher_t* speicher, KMP_syn_anweisung_t* syn_knoten, KMP_glied_t* op, KMP_sem_ausdruck_t* links, KMP_sem_ausdruck_t* rechts)
{
    KMP_sem_anweisung_t* erg = kmp_sem_anweisung(speicher, syn_knoten, KMP_SYN_KNOTEN_ANWEISUNG_ZUWEISUNG);

    erg->zuweisung.op = op;
    erg->zuweisung.links = links;
    erg->zuweisung.rechts = rechts;

    return erg;
}

KMP_sem_anweisung_t*
kmp_sem_anweisung_final(BSS_speicher_t* speicher, KMP_syn_anweisung_t* syn_knoten, KMP_sem_anweisung_t* anweisung)
{
    KMP_sem_anweisung_t* erg = kmp_sem_anweisung(speicher, syn_knoten, KMP_SYN_KNOTEN_ANWEISUNG_FINAL);

    erg->final.anweisung = anweisung;

    return erg;
}
// }}}
// asb {{{
KMP_sem_asb_t*
kmp_sem_asb(BSS_speicher_t* speicher, KMP_sem_zone_t* system, KMP_sem_zone_t* global)
{
    KMP_sem_asb_t* erg = speicher->anfordern(speicher, sizeof(KMP_sem_asb_t));

    erg->system = system;
    erg->global = global;

    return erg;
}
// }}}

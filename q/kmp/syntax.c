#include "kmp/syntax.h"

#include "kmp/asb.h"
#include "kmp/diagnostik.h"
#include "kmp/lexer.h"

#include <assert.h>

char* SCHLÜSSELWORT_SCHABLONE = "schablone";  // INFO: struct
char* SCHLÜSSELWORT_MERKMAL   = "merkmal";    // INFO: trait
char* SCHLÜSSELWORT_OPTION    = "option";     // INFO: enum
char* SCHLÜSSELWORT_WEICHE    = "weiche";     // INFO: match
char* SCHLÜSSELWORT_WENN      = "wenn";       // INFO: if
char* SCHLÜSSELWORT_FÜR       = "für";        // INFO: for
char* SCHLÜSSELWORT_SOLANGE   = "solange";    // INFO: while
char* SCHLÜSSELWORT_SONST     = "sonst";      // INFO: else
char* SCHLÜSSELWORT_IMPORT    = "import";     // INFO: import
char* SCHLÜSSELWORT_LADE      = "lade";       // INFO: include
char* SCHLÜSSELWORT_FINAL     = "final";      // INFO: defer
char* SCHLÜSSELWORT_RES       = "res";        // INFO: resultat -> return
char* SCHLÜSSELWORT_MARKE     = "marke";
char* SCHLÜSSELWORT_WEITER    = "weiter";     // INFO: continue
char* SCHLÜSSELWORT_SPRUNG    = "sprung";     // INFO: goto
char* SCHLÜSSELWORT_RAUS      = "raus";       // INFO: break

KMP_syntax_t*
kmp_syntax(BSS_speicher_t* speicher, BSS_Feld(KMP_glied_t*) glieder)
{
    KMP_syntax_t* erg = speicher->anfordern(speicher, sizeof(KMP_syntax_t));

    erg->glieder = glieder;
    erg->glied_index = 0;

    return erg;
}

w32
kmp_syn_dateiende(KMP_syntax_t* syntax)
{
    w32 erg = kmp_syn_ist(syntax, KMP_GLIED_ENDE, 0);

    return erg;
}

KMP_syn_asb_t
kmp_syn_starten(BSS_speicher_t* speicher, KMP_syntax_t* syntax, g32 index)
{
    KMP_syn_asb_t erg = {0};

    for (;;)
    {
        if (kmp_syn_dateiende(syntax))
        {
            break;
        }

        KMP_syn_anweisung_t* anweisung = kmp_syn_anweisung_einlesen(speicher, syntax);
        if (anweisung->basis->art == KMP_SYN_KNOTEN_ANWEISUNG_LADE)
        {
            BSS_text_t dateiname = anweisung->lade.dateiname;

            FILE *datei;
            char *inhalt = NULL;
            g64 größe = 0;

            if (fopen_s(&datei, dateiname.daten, "r") == 0)
            {
                fseek(datei, 0, SEEK_END);
                größe = ftell(datei);
                fseek(datei, 0, SEEK_SET);

                inhalt = speicher->anfordern(speicher, größe + 1);
                if (inhalt)
                {
                    fread(inhalt, 1, größe, datei);
                    inhalt[größe] = '\0';
                }

                fclose(datei);
            }

            KMP_lexer_t* lexer = kmp_lexer(speicher, dateiname, bss_text(inhalt));
            BSS_Feld(KMP_glied_t*) glieder = kmp_lexer_starten(speicher, lexer, 0);
            KMP_syntax_t* syn_neu = kmp_syntax(speicher, glieder);
            KMP_syn_asb_t asb = kmp_syn_starten(speicher, syn_neu, 0);

            for (g32 i = 0; i < asb.anweisungen.anzahl_elemente; ++i)
            {
                KMP_syn_anweisung_t* a = bss_feld_element(asb.anweisungen, i);
                bss_feld_hinzufügen(speicher, &erg.anweisungen, a);
            }

            continue;
        }

        bss_feld_hinzufügen(speicher, &erg.anweisungen, anweisung);
    }

    return erg;
}

BSS_Feld(KMP_syn_marke_t *)
kmp_syn_marken_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syntax)
{
    BSS_Feld(KMP_syn_marke_t*) erg = bss_feld(speicher, sizeof(KMP_syn_marke_t*));
    KMP_glied_t* anfang = kmp_syn_glied(syntax, 0);
    KMP_glied_t* glied = anfang;

    if (glied->art != KMP_GLIED_RAUTE)
    {
        return erg;
    }

    if (!kmp_syn_ist(syntax, KMP_GLIED_BEZEICHNER, 1) ||
        !bss_text_ist_gleich(kmp_syn_glied(syntax, 0)->text, bss_text(SCHLÜSSELWORT_MARKE)))
    {
        return erg;
    }

    kmp_syn_weiter(syntax, 2);

    for (;;)
    {
        KMP_syn_ausdruck_t* ausdruck = kmp_syn_ausdruck_einlesen(speicher, syntax);
        if (ausdruck->basis->art != KMP_SYN_KNOTEN_AUSDRUCK_BEZEICHNER)
        {
            break;
        }

        bss_feld_hinzufügen(speicher, &erg, kmp_syn_marke(speicher, ausdruck->basis->spanne, ausdruck->bezeichner.wert));

        if (!kmp_syn_passt(syntax, KMP_GLIED_KOMMA))
        {
            break;
        }
    }

    return erg;
}

// anweisung {{{
KMP_syn_anweisung_t*
kmp_syn_anweisung_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syntax)
{
    BSS_Feld(KMP_syn_marke_t*) marken = kmp_syn_marken_einlesen(speicher, syntax);

    KMP_syn_anweisung_t* anweisung = NULL;

    if (kmp_syn_ist(syntax, KMP_GLIED_GKLAMMER_AUF, 0))
    {
        anweisung = kmp_syn_block_anweisung_einlesen(speicher, syntax);
    }

    else if (kmp_syn_ist(syntax, KMP_GLIED_KLEINER, 0))
    {
        anweisung = kmp_syn_markierung_anweisung_einlesen(speicher, syntax);
        kmp_syn_erwarte(syntax, KMP_GLIED_PUNKT);
    }

    else if (kmp_syn_ist(syntax, KMP_GLIED_BEZEICHNER, 0) &&
             bss_text_ist_gleich(kmp_syn_glied(syntax, 0)->text, bss_text(SCHLÜSSELWORT_WENN)))
    {
        anweisung = kmp_syn_wenn_anweisung_einlesen(speicher, syntax);
    }

    else if (kmp_syn_ist(syntax, KMP_GLIED_BEZEICHNER, 0) &&
             bss_text_ist_gleich(kmp_syn_glied(syntax, 0)->text, bss_text(SCHLÜSSELWORT_FÜR)))
    {
        anweisung = kmp_syn_für_anweisung_einlesen(speicher, syntax);
    }

    else if (kmp_syn_ist(syntax, KMP_GLIED_BEZEICHNER, 0) &&
             bss_text_ist_gleich(kmp_syn_glied(syntax, 0)->text, bss_text(SCHLÜSSELWORT_SOLANGE)))
    {
        anweisung = kmp_syn_solange_anweisung_einlesen(speicher, syntax);
    }

    else if (kmp_syn_ist(syntax, KMP_GLIED_BEZEICHNER, 0) &&
             bss_text_ist_gleich(kmp_syn_glied(syntax, 0)->text, bss_text(SCHLÜSSELWORT_WEICHE)))
    {
        anweisung = kmp_syn_weiche_anweisung_einlesen(speicher, syntax);
    }

    else if (kmp_syn_ist(syntax, KMP_GLIED_BEZEICHNER, 0) &&
             bss_text_ist_gleich(kmp_syn_glied(syntax, 0)->text, bss_text(SCHLÜSSELWORT_FINAL)))
    {
        anweisung = kmp_syn_final_anweisung_einlesen(speicher, syntax);
    }

    else if (kmp_syn_ist(syntax, KMP_GLIED_BEZEICHNER, 0) &&
             bss_text_ist_gleich(kmp_syn_glied(syntax, 0)->text, bss_text(SCHLÜSSELWORT_RES)))
    {
        anweisung = kmp_syn_res_anweisung_einlesen(speicher, syntax);
    }

    else if (kmp_syn_ist(syntax, KMP_GLIED_BEZEICHNER, 0) &&
             bss_text_ist_gleich(kmp_syn_glied(syntax, 0)->text, bss_text(SCHLÜSSELWORT_WEITER)))
    {
        anweisung = kmp_syn_weiter_anweisung_einlesen(speicher, syntax);
        kmp_syn_erwarte(syntax, KMP_GLIED_PUNKT);
    }

    else if (kmp_syn_ist(syntax, KMP_GLIED_BEZEICHNER, 0) &&
             bss_text_ist_gleich(kmp_syn_glied(syntax, 0)->text, bss_text(SCHLÜSSELWORT_SPRUNG)))
    {
        anweisung = kmp_syn_sprung_anweisung_einlesen(speicher, syntax);
        kmp_syn_erwarte(syntax, KMP_GLIED_PUNKT);
    }

    else if (kmp_syn_ist(syntax, KMP_GLIED_BEZEICHNER, 0) &&
             bss_text_ist_gleich(kmp_syn_glied(syntax, 0)->text, bss_text(SCHLÜSSELWORT_RAUS)))
    {
        anweisung = kmp_syn_raus_anweisung_einlesen(speicher, syntax);
        kmp_syn_erwarte(syntax, KMP_GLIED_PUNKT);
    }

    else if (kmp_syn_ist(syntax, KMP_GLIED_BEZEICHNER, 0) &&
             kmp_syn_ist(syntax, KMP_GLIED_BALKEN, 1))
    {
        anweisung = kmp_syn_deklaration_anweisung_einlesen(speicher, syntax);
    }

    else if (kmp_syn_ist(syntax, KMP_GLIED_RAUTE, 0))
    {
        if (kmp_syn_ist(syntax, KMP_GLIED_BEZEICHNER, 1) &&
            bss_text_ist_gleich(kmp_syn_glied(syntax, 1)->text, bss_text(SCHLÜSSELWORT_IMPORT)))
        {
            KMP_syn_ausdruck_t* ausdruck = kmp_syn_basis_ausdruck_einlesen(speicher, syntax);
            if (ausdruck->basis->art != KMP_SYN_KNOTEN_AUSDRUCK_IMPORT)
            {
                assert(!"meldung erstatten");
            }

            anweisung = kmp_syn_anweisung_import(speicher, ausdruck->basis->spanne, ausdruck->import.dateiname);
        }

        else if (kmp_syn_ist(syntax, KMP_GLIED_BEZEICHNER, 1) &&
                 bss_text_ist_gleich(kmp_syn_glied(syntax, 1)->text, bss_text(SCHLÜSSELWORT_LADE)))
        {
            kmp_syn_weiter(syntax, 2);

            KMP_syn_ausdruck_t* ausdruck = kmp_syn_basis_ausdruck_einlesen(speicher, syntax);
            if (ausdruck->basis->art != KMP_SYN_KNOTEN_AUSDRUCK_TEXT)
            {
                assert(!"meldung erstatten");
            }

            anweisung = kmp_syn_anweisung_lade(speicher, ausdruck->basis->spanne, ausdruck->text.wert);
        }

        else if (kmp_syn_ist(syntax, KMP_GLIED_AUSRUFEZEICHEN, 1))
        {
            KMP_syn_ausdruck_t* ausdruck = kmp_syn_basis_ausdruck_einlesen(speicher, syntax);
            kmp_syn_erwarte(syntax, KMP_GLIED_PUNKT);
            anweisung = kmp_syn_anweisung_ausdruck(speicher, ausdruck->basis->spanne, ausdruck);
        }
    }

    else
    {
        KMP_syn_ausdruck_t* links = kmp_syn_ausdruck_einlesen(speicher, syntax);

        if (kmp_syn_glied(syntax, 0)->art >= KMP_GLIED_ZUWEISUNG &&
            kmp_syn_glied(syntax, 0)->art <= KMP_GLIED_ZUWEISUNG_PROZENT)
        {
            KMP_glied_t* op = kmp_syn_weiter(syntax, 1);
            KMP_syn_ausdruck_t* rechts = kmp_syn_ausdruck_einlesen(speicher, syntax);

            anweisung = kmp_syn_anweisung_zuweisung(
                speicher,
                kmp_spanne_bereich(links->basis->spanne, rechts->basis->spanne),
                op, links, rechts);
        }
        else
        {
            anweisung = kmp_syn_anweisung_ausdruck(speicher, links->basis->spanne, links);
        }
    }

    if (anweisung == NULL)
    {
        anweisung = kmp_syn_anweisung_ungültig(speicher, kmp_syn_glied(syntax, 0)->spanne);
    }

    anweisung->marken = marken;

    return anweisung;
}

KMP_syn_anweisung_t*
kmp_syn_deklaration_anweisung_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syntax)
{
    KMP_syn_deklaration_t* deklaration = kmp_syn_deklaration_einlesen(speicher, syntax, true);
    KMP_syn_anweisung_t* erg = kmp_syn_anweisung_deklaration(speicher, deklaration->basis->spanne, deklaration);

    return erg;
}

KMP_syn_anweisung_t*
kmp_syn_block_anweisung_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syntax)
{
    KMP_glied_t* klammer_auf = kmp_syn_erwarte(syntax, KMP_GLIED_GKLAMMER_AUF);

    BSS_Feld(KMP_syn_anweisung_t*) anweisungen = bss_feld(speicher, sizeof(KMP_syn_anweisung_t*));
    if (!kmp_syn_ist(syntax, KMP_GLIED_GKLAMMER_ZU, 0))
    {
        for (;;)
        {
            KMP_syn_anweisung_t* anweisung = kmp_syn_anweisung_einlesen(speicher, syntax);
            kmp_syn_passt(syntax, KMP_GLIED_PUNKT);
            bss_feld_hinzufügen(speicher, &anweisungen, anweisung);
        }
    }

    KMP_glied_t* klammer_zu = kmp_syn_erwarte(syntax, KMP_GLIED_GKLAMMER_ZU);

    KMP_syn_anweisung_t* erg = kmp_syn_anweisung_block(
        speicher,
        kmp_spanne_bereich(klammer_auf->spanne, klammer_zu->spanne),
        anweisungen);

    return erg;
}

KMP_syn_anweisung_t*
kmp_syn_wenn_anweisung_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syntax)
{
    KMP_glied_t* schlüsselwort = kmp_syn_weiter(syntax, 1);

    KMP_syn_ausdruck_t* bedingung = kmp_syn_ausdruck_einlesen(speicher, syntax);
    KMP_syn_anweisung_t* rumpf = kmp_syn_anweisung_einlesen(speicher, syntax);

    KMP_syn_anweisung_t* alternative_anweisung = NULL;
    if (kmp_syn_ist(syntax, KMP_GLIED_BEZEICHNER, 0) &&
        bss_text_ist_gleich(kmp_syn_glied(syntax, 0)->text, bss_text(SCHLÜSSELWORT_SONST)))
    {
        KMP_glied_t* alternative = kmp_syn_weiter(syntax, 1);
        if (kmp_syn_ist(syntax, KMP_GLIED_BEZEICHNER, 0) &&
            bss_text_ist_gleich(kmp_syn_glied(syntax, 0)->text, bss_text(SCHLÜSSELWORT_WENN)))
        {
            alternative_anweisung = kmp_syn_wenn_anweisung_einlesen(speicher, syntax);
        }
        else
        {
            KMP_syn_anweisung_t* alternative_rumpf = kmp_syn_anweisung_einlesen(speicher, syntax);
            KMP_spanne_t spanne = kmp_spanne_bereich(alternative->spanne, alternative_rumpf->basis->spanne);
            KMP_syn_ausdruck_t* alternative_bedingung = kmp_syn_ausdruck_ganzzahl(speicher, alternative->spanne, 1);
            alternative_anweisung = kmp_syn_anweisung_wenn(
                speicher,
                spanne,
                alternative_bedingung,
                alternative_rumpf, NULL);
        }
    }

    KMP_spanne_t spanne = kmp_spanne_bereich(
        schlüsselwort->spanne,
        alternative_anweisung
           ? alternative_anweisung->basis->spanne
           : rumpf->basis->spanne);

    KMP_syn_anweisung_t* erg = kmp_syn_anweisung_wenn(
        speicher,
        spanne,
        bedingung,
        rumpf,
        alternative_anweisung);

    return erg;
}

KMP_syn_anweisung_t*
kmp_syn_markierung_anweisung_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syntax)
{
    KMP_glied_t* kleiner_als = kmp_syn_weiter(syntax, 1);

    KMP_syn_ausdruck_t* markierung = kmp_syn_basis_ausdruck_einlesen(speicher, syntax);
    if (markierung->basis->art != KMP_SYN_KNOTEN_AUSDRUCK_BEZEICHNER)
    {
        KMP_fehler_t* fehler = kmp_fehler(speicher, bss_text("gültigen Bezeichner erwartet."));
        kmp_diagnostik_melden_fehler(speicher, &syntax->diagnostik, markierung->basis->spanne, fehler);
        return NULL;
    }

    KMP_glied_t* größer_als = kmp_syn_erwarte(syntax, KMP_GLIED_GRÖẞER);

    KMP_syn_anweisung_t* erg = kmp_syn_anweisung_markierung(
        speicher,
        kmp_spanne_bereich(kleiner_als->spanne, größer_als->spanne),
        markierung->bezeichner.wert);

    return erg;
}

KMP_syn_anweisung_t*
kmp_syn_für_anweisung_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syntax)
{
    KMP_glied_t* schlüsselwort = kmp_syn_weiter(syntax, 1);

    KMP_syn_ausdruck_t* index = NULL;
    KMP_syn_ausdruck_t* bedingung = kmp_syn_basis_ausdruck_einlesen(speicher, syntax);
    if (kmp_syn_passt(syntax, KMP_GLIED_DOPPELPUNKT))
    {
        index = bedingung;
        bedingung = kmp_syn_ausdruck_einlesen(speicher, syntax);
    }

    KMP_syn_anweisung_t* rumpf = kmp_syn_anweisung_einlesen(speicher, syntax);
    KMP_syn_anweisung_t* erg = kmp_syn_anweisung_für(
        speicher,
        kmp_spanne_bereich(schlüsselwort->spanne, rumpf->basis->spanne),
        index, bedingung, rumpf);

    return erg;
}

KMP_syn_anweisung_t*
kmp_syn_solange_anweisung_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syntax)
{
    KMP_glied_t* schlüsselwort = kmp_syn_weiter(syntax, 1);

    KMP_syn_ausdruck_t* bedingung = kmp_syn_ausdruck_einlesen(speicher, syntax);
    KMP_syn_anweisung_t* rumpf = kmp_syn_anweisung_einlesen(speicher, syntax);

    KMP_syn_anweisung_t* erg = kmp_syn_anweisung_solange(
        speicher,
        schlüsselwort->spanne,
        bedingung,
        rumpf);

    return erg;
}

KMP_syn_anweisung_t*
kmp_syn_weiche_anweisung_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syntax)
{
    KMP_glied_t* schlüsselwort = kmp_syn_weiter(syntax, 1);
    KMP_syn_ausdruck_t* ausdruck = kmp_syn_ausdruck_einlesen(speicher, syntax);

    BSS_Feld(KMP_syn_muster_t*) muster = bss_feld(speicher, sizeof(KMP_syn_muster_t*));
    if (kmp_syn_passt(syntax, KMP_GLIED_GKLAMMER_AUF))
    {
        for (;;)
        {
            KMP_syn_ausdruck_t* muster_ausdruck = kmp_syn_ausdruck_einlesen(speicher, syntax);

            kmp_syn_erwarte(syntax, KMP_GLIED_FOLGERUNG);

            KMP_syn_anweisung_t* anweisung = kmp_syn_anweisung_einlesen(speicher, syntax);

            KMP_syn_muster_t* m = kmp_syn_muster(
                speicher,
                kmp_spanne_bereich(muster_ausdruck->basis->spanne, anweisung->basis->spanne),
                muster_ausdruck, anweisung);
            bss_feld_hinzufügen(speicher, &muster, m);

            if (!kmp_syn_passt(syntax, KMP_GLIED_PUNKT))
            {
                break;
            }
        }

        kmp_syn_erwarte(syntax, KMP_GLIED_GKLAMMER_ZU);
    }

    KMP_syn_anweisung_t* erg = kmp_syn_anweisung_weiche(
        speicher,
        kmp_spanne(), ausdruck, muster);

    return erg;
}

KMP_syn_anweisung_t*
kmp_syn_final_anweisung_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syntax)
{
    KMP_glied_t* schlüsselwort = kmp_syn_weiter(syntax, 1);

    KMP_syn_anweisung_t* anweisung = kmp_syn_anweisung_einlesen(speicher, syntax);

    KMP_syn_anweisung_t* erg = kmp_syn_anweisung_final(
        speicher,
        kmp_spanne_bereich(schlüsselwort->spanne, anweisung->basis->spanne),
        anweisung);

    return erg;
}

KMP_syn_anweisung_t*
kmp_syn_res_anweisung_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syntax)
{
    KMP_glied_t* schlüsselwort = kmp_syn_weiter(syntax, 1);

    KMP_syn_ausdruck_t* ausdruck = kmp_syn_ausdruck_einlesen(speicher, syntax);

    KMP_syn_anweisung_t* erg = kmp_syn_anweisung_res(
        speicher,
        kmp_spanne_bereich(schlüsselwort->spanne, ausdruck->basis->spanne),
        ausdruck);

    return erg;
}

KMP_syn_anweisung_t*
kmp_syn_weiter_anweisung_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syntax)
{
    KMP_glied_t* schlüsselwort = kmp_syn_weiter(syntax, 1);

    KMP_syn_anweisung_t* erg = kmp_syn_anweisung_weiter(speicher, schlüsselwort->spanne);

    return erg;
}

KMP_syn_anweisung_t*
kmp_syn_sprung_anweisung_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syntax)
{
    KMP_glied_t* schlüsselwort = kmp_syn_weiter(syntax, 1);

    KMP_syn_ausdruck_t* ausdruck = kmp_syn_ausdruck_einlesen(speicher, syntax);
    if (ausdruck->basis->art != KMP_SYN_KNOTEN_AUSDRUCK_BEZEICHNER)
    {
        KMP_fehler_t* fehler = kmp_fehler(speicher, bss_text("Bezeichner erwartet."));
        kmp_diagnostik_melden_fehler(speicher, &syntax->diagnostik, ausdruck->basis->spanne, fehler);
    }

    KMP_spanne_t spanne = kmp_spanne_bereich(schlüsselwort->spanne, ausdruck->basis->spanne);
    KMP_syn_anweisung_t* erg = kmp_syn_anweisung_sprung(
        speicher,
        spanne,
        ausdruck);

    return erg;
}

KMP_syn_anweisung_t*
kmp_syn_raus_anweisung_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syntax)
{
    KMP_glied_t* schlüsselwort = kmp_syn_weiter(syntax, 1);

    KMP_syn_anweisung_t* erg = kmp_syn_anweisung_raus(
        speicher,
        schlüsselwort->spanne);

    return erg;
}
// }}}
// deklaration {{{
KMP_syn_deklaration_t*
kmp_syn_deklaration_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syntax, w32 mit_abschluss)
{
    KMP_glied_t* anfang = kmp_syn_glied(syntax, 0);
    BSS_Feld(BSS_text_t) namen = bss_feld(speicher, sizeof(BSS_text_t));

    for (;;)
    {
        KMP_syn_ausdruck_t* ausdruck = kmp_syn_ausdruck_einlesen(speicher, syntax);
        if (ausdruck->basis->art != KMP_SYN_KNOTEN_AUSDRUCK_BEZEICHNER)
        {
            break;
        }

        bss_feld_hinzufügen(speicher, &namen, &ausdruck->bezeichner.wert);

        if (!kmp_syn_passt(syntax, KMP_GLIED_KOMMA))
        {
            break;
        }
    }

    if (namen.anzahl_elemente == 0)
    {
        return kmp_syn_deklaration_ungültig(speicher, anfang->spanne);
    }

    kmp_syn_erwarte(syntax, KMP_GLIED_BALKEN);
    KMP_syn_spezifizierung_t* spezifizierung = kmp_syn_spezifizierung_einlesen(speicher, syntax);

    if (kmp_syn_passt(syntax, KMP_GLIED_ZUWEISUNG))
    {
        KMP_syn_ausdruck_t* ausdruck = kmp_syn_ausdruck_einlesen(speicher, syntax);

        if (mit_abschluss)
        {
            kmp_syn_erwarte(syntax, KMP_GLIED_PUNKT);
        }

        KMP_spanne_t spanne = kmp_spanne_bereich(anfang->spanne, ausdruck->basis->spanne);
        KMP_syn_deklaration_t* erg = kmp_syn_deklaration_variable(
            speicher,
            spanne,
            namen,
            spezifizierung,
            ausdruck);
    }

    else if (kmp_syn_passt(syntax, KMP_GLIED_BALKEN))
    {
        if (kmp_syn_ist(syntax, KMP_GLIED_RKLAMMER_AUF, 0))
        {
            KMP_syn_ausdruck_t* ausdruck = kmp_syn_basis_ausdruck_einlesen(speicher, syntax);
            if (ausdruck->basis->art == KMP_SYN_KNOTEN_AUSDRUCK_ROUTINE)
            {
                if (ausdruck->routine.rumpf == NULL)
                {
                    kmp_syn_erwarte(syntax, KMP_GLIED_PUNKT);
                }

                KMP_spanne_t spanne = kmp_spanne_bereich(anfang->spanne, ausdruck->basis->spanne);
                KMP_syn_deklaration_t* erg = kmp_syn_deklaration_routine(
                    speicher,
                    spanne,
                    namen,
                    spezifizierung,
                    ausdruck);

                return erg;
            }
            else if (ausdruck->basis->art == KMP_SYN_KNOTEN_AUSDRUCK_OPTION)
            {
                KMP_spanne_t spanne = kmp_spanne_bereich(anfang->spanne, ausdruck->basis->spanne);
                KMP_syn_deklaration_t* erg = kmp_syn_deklaration_option(
                    speicher,
                    spanne,
                    namen,
                    ausdruck);

                return erg;
            }
            else
            {
                return kmp_syn_deklaration_ungültig(speicher, ausdruck->basis->spanne);
            }
        }
        else if (kmp_syn_ist(syntax, KMP_GLIED_BEZEICHNER, 0) &&
                 bss_text_ist_gleich(kmp_syn_glied(syntax, 0)->text, bss_text(SCHLÜSSELWORT_OPTION)))
        {
            KMP_syn_ausdruck_t* ausdruck = kmp_syn_basis_ausdruck_einlesen(speicher, syntax);
            KMP_spanne_t spanne = kmp_spanne_bereich(anfang->spanne, ausdruck->basis->spanne);
            KMP_syn_deklaration_t* erg = kmp_syn_deklaration_option(
                speicher,
                spanne,
                namen,
                ausdruck);

            return erg;
        }

        else if (kmp_syn_ist(syntax, KMP_GLIED_BEZEICHNER, 0) &&
                 bss_text_ist_gleich(kmp_syn_glied(syntax, 0)->text, bss_text(SCHLÜSSELWORT_SCHABLONE)))
        {
            KMP_glied_t* schlüsselwort = kmp_syn_weiter(syntax, 1);
            kmp_syn_erwarte(syntax, KMP_GLIED_GKLAMMER_AUF);
            BSS_Feld(KMP_syn_deklaration_t*) eigenschaften = bss_feld(speicher, sizeof(KMP_syn_deklaration_t*));

            for (;;)
            {
                KMP_syn_deklaration_t* eigenschaft = kmp_syn_deklaration_einlesen(speicher, syntax, true);

                if (eigenschaft == NULL)
                {
                    break;
                }

                bss_feld_hinzufügen(speicher, &eigenschaften, eigenschaft);
            }

            KMP_glied_t* klammer_zu = kmp_syn_erwarte(syntax, KMP_GLIED_GKLAMMER_ZU);

            KMP_spanne_t spanne = kmp_spanne_bereich(anfang->spanne, klammer_zu->spanne);
            KMP_syn_deklaration_t* erg = kmp_syn_deklaration_schablone(
                speicher,
                spanne,
                namen,
                eigenschaften);

            return erg;
        }

        else if (kmp_syn_ist(syntax, KMP_GLIED_RAUTE, 0))
        {
            KMP_syn_ausdruck_t* ausdruck = kmp_syn_basis_ausdruck_einlesen(speicher, syntax);
            if (ausdruck->basis->art != KMP_SYN_KNOTEN_AUSDRUCK_IMPORT)
            {
                assert(!"meldung erstatten");
                return NULL;
            }

            KMP_syn_deklaration_t* erg = kmp_syn_deklaration_import(
                speicher,
                kmp_spanne(),
                namen,
                ausdruck);

            return erg;
        }
    }

    assert(spezifizierung != NULL);

    if (mit_abschluss)
    {
        kmp_syn_erwarte(syntax, KMP_GLIED_PUNKT);
    }

    KMP_spanne_t spanne = kmp_spanne_bereich(anfang->spanne, spezifizierung->basis->spanne);
    KMP_syn_deklaration_t* erg = kmp_syn_deklaration_variable(
        speicher,
        spanne,
        namen,
        spezifizierung,
        NULL);

    return erg;
}
// }}}
// ausdruck {{{
KMP_syn_ausdruck_t*
kmp_syn_ausdruck_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syntax)
{
    KMP_syn_ausdruck_t* erg = kmp_syn_reihe_ausdruck_einlesen(speicher, syntax);

    return erg;
}

KMP_syn_ausdruck_t*
kmp_syn_reihe_ausdruck_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syntax)
{
    KMP_syn_ausdruck_t* links = kmp_syn_vergleich_ausdruck_einlesen(speicher, syntax);

    if (kmp_syn_passt(syntax, KMP_GLIED_PUNKT_PUNKT))
    {
        KMP_syn_ausdruck_t* max = kmp_syn_vergleich_ausdruck_einlesen(speicher, syntax);
        KMP_spanne_t spanne = kmp_spanne_bereich(links->basis->spanne, max->basis->spanne);
        links = kmp_syn_ausdruck_reihe(speicher, spanne, links, max);
    }

    return links;
}

KMP_syn_ausdruck_t*
kmp_syn_vergleich_ausdruck_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syntax)
{
    KMP_syn_ausdruck_t* links = kmp_syn_bitschieben_ausdruck_einlesen(speicher, syntax);

    if (kmp_syn_glied(syntax, 0)->art >= KMP_GLIED_KLEINER && kmp_syn_glied(syntax, 0)->art <= KMP_GLIED_GRÖẞER)
    {
        KMP_glied_t* op = kmp_syn_weiter(syntax, 1);
        KMP_syn_ausdruck_t* rechts = kmp_syn_bitschieben_ausdruck_einlesen(speicher, syntax);
        KMP_spanne_t spanne = kmp_spanne_bereich(links->basis->spanne, rechts->basis->spanne);
        links = kmp_syn_ausdruck_binär(
            speicher,
            spanne,
            kmp_syn_glied_zu_binär_op(op),
            links,
            rechts
        );
    }

    return links;
}

KMP_syn_ausdruck_t*
kmp_syn_bitschieben_ausdruck_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syntax)
{
    KMP_syn_ausdruck_t* links = kmp_syn_add_ausdruck_einlesen(speicher, syntax);

    if (kmp_syn_ist(syntax, KMP_GLIED_DREIECK_LINKS, 0) || kmp_syn_ist(syntax, KMP_GLIED_DREIECK_RECHTS, 0))
    {
        KMP_glied_t* op = kmp_syn_weiter(syntax, 1);

        KMP_syn_ausdruck_t* rechts = kmp_syn_add_ausdruck_einlesen(speicher, syntax);
        KMP_spanne_t spanne = kmp_spanne_bereich(links->basis->spanne, rechts->basis->spanne);
        links = kmp_syn_ausdruck_binär(
            speicher,
            spanne,
            kmp_syn_glied_zu_binär_op(op),
            links,
            rechts
        );
    }

    return links;
}

KMP_syn_ausdruck_t*
kmp_syn_add_ausdruck_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syntax)
{
    KMP_syn_ausdruck_t* links = kmp_syn_mult_ausdruck_einlesen(speicher, syntax);

    while (kmp_syn_glied(syntax, 0)->art == KMP_GLIED_PLUS || kmp_syn_glied(syntax, 0)->art == KMP_GLIED_MINUS)
    {
        KMP_glied_t* op = kmp_syn_weiter(syntax, 1);

        KMP_syn_ausdruck_t* rechts = kmp_syn_mult_ausdruck_einlesen(speicher, syntax);
        KMP_spanne_t spanne = kmp_spanne_bereich(links->basis->spanne, rechts->basis->spanne);
        links = kmp_syn_ausdruck_binär(
            speicher,
            spanne,
            kmp_syn_glied_zu_binär_op(op),
            links,
            rechts
        );
    }

    return links;
}

KMP_syn_ausdruck_t*
kmp_syn_mult_ausdruck_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syntax)
{
    KMP_syn_ausdruck_t* links = kmp_syn_aufruf_ausdruck_einlesen(speicher, syntax);

    while (kmp_syn_glied(syntax, 0)->art == KMP_GLIED_STERN       ||
           kmp_syn_glied(syntax, 0)->art == KMP_GLIED_DOPPELPUNKT ||
           kmp_syn_glied(syntax, 0)->art == KMP_GLIED_PROZENT)
    {
        KMP_glied_t* op = kmp_syn_weiter(syntax, 1);

        KMP_syn_ausdruck_t* rechts = kmp_syn_aufruf_ausdruck_einlesen(speicher, syntax);
        KMP_spanne_t spanne = kmp_spanne_bereich(links->basis->spanne, rechts->basis->spanne);

        links = kmp_syn_ausdruck_binär(
            speicher,
            spanne,
            kmp_syn_glied_zu_binär_op(op),
            links,
            rechts
        );
    }

    return links;
}

KMP_syn_ausdruck_t*
kmp_syn_aufruf_ausdruck_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syntax)
{
    KMP_syn_ausdruck_t* links = kmp_syn_index_ausdruck_einlesen(speicher, syntax);

    if (kmp_syn_glied(syntax, 0)->art == KMP_GLIED_RKLAMMER_AUF)
    {
        KMP_glied_t* klammer_auf = kmp_syn_weiter(syntax, 1);

        BSS_Feld(KMP_syn_ausdruck_t*) argumente = bss_feld(speicher, sizeof(KMP_syn_ausdruck_t*));
        if (kmp_syn_glied(syntax, 0)->art != KMP_GLIED_RKLAMMER_ZU)
        {
            KMP_syn_ausdruck_t* argument = kmp_syn_ausdruck_einlesen(speicher, syntax);
            while (argument->basis->art != KMP_SYN_KNOTEN_AUSDRUCK_UNGÜLTIG)
            {
                bss_feld_hinzufügen(speicher, &argumente, argument);

                if (!kmp_syn_passt(syntax, KMP_GLIED_STRICHPUNKT))
                {
                    break;
                }

                argument = kmp_syn_ausdruck_einlesen(speicher, syntax);
            }
        }

        KMP_glied_t* klammer_zu = kmp_syn_erwarte(syntax, KMP_GLIED_RKLAMMER_ZU);
        KMP_spanne_t spanne = kmp_spanne_bereich(links->basis->spanne, klammer_zu->spanne);

        links = kmp_syn_ausdruck_aufruf(
            speicher,
            spanne,
            links,
            argumente
        );
    }

    return links;
}

KMP_syn_ausdruck_t*
kmp_syn_index_ausdruck_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syntax)
{
    KMP_syn_ausdruck_t* links = kmp_syn_eigenschaft_ausdruck_einlesen(speicher, syntax);

    while (kmp_syn_glied(syntax, 0)->art == KMP_GLIED_EKLAMMER_AUF)
    {
        KMP_glied_t* klammer_auf = kmp_syn_weiter(syntax, 1);
        KMP_syn_ausdruck_t* index = kmp_syn_ausdruck_einlesen(speicher, syntax);
        KMP_glied_t* klammer_zu = kmp_syn_erwarte(syntax, KMP_GLIED_EKLAMMER_ZU);
        KMP_spanne_t spanne = kmp_spanne_bereich(links->basis->spanne, klammer_zu->spanne);

        links = kmp_syn_ausdruck_index(
            speicher,
            spanne,
            links,
            index
        );
    }

    return links;
}

KMP_syn_ausdruck_t*
kmp_syn_eigenschaft_ausdruck_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syntax)
{
    KMP_syn_ausdruck_t* links = kmp_syn_basis_ausdruck_einlesen(speicher, syntax);

    while (kmp_syn_glied(syntax, 0)->art == KMP_GLIED_PISA)
    {
        KMP_glied_t* punkt = kmp_syn_weiter(syntax, 1);

        KMP_syn_ausdruck_t* feld = kmp_syn_basis_ausdruck_einlesen(speicher, syntax);
        if (feld->basis->art != KMP_SYN_KNOTEN_AUSDRUCK_BEZEICHNER)
        {
            return kmp_syn_ausdruck_ungültig(speicher, feld->basis->spanne);
        }

        KMP_spanne_t spanne = kmp_spanne_bereich(links->basis->spanne, feld->basis->spanne);
        links = kmp_syn_ausdruck_eigenschaft(
            speicher,
            spanne,
            links,
            feld
        );
    }

    return links;
}

KMP_syn_ausdruck_t*
kmp_syn_basis_ausdruck_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syntax)
{
    KMP_glied_t* t = kmp_syn_glied(syntax, 0);

    switch (t->art)
    {
        case KMP_GLIED_MINUS:
        case KMP_GLIED_TILDE:
        case KMP_GLIED_AUSRUFEZEICHEN:
        {
            KMP_glied_t* op = kmp_syn_weiter(syntax, 1);
            KMP_syn_ausdruck_t* ausdruck = kmp_syn_basis_ausdruck_einlesen(speicher, syntax);
            KMP_spanne_t spanne = kmp_spanne_bereich(op->spanne, ausdruck->basis->spanne);
            KMP_syn_ausdruck_t* erg = kmp_syn_ausdruck_unär(
                speicher,
                spanne,
                kmp_syn_glied_zu_unär_op(op),
                ausdruck
            );

            return erg;
        } break;

        case KMP_GLIED_RAUTE:
        {
            KMP_glied_t* raute = kmp_syn_weiter(syntax, 1);
            if (kmp_syn_glied(syntax, 0)->art == KMP_GLIED_BEZEICHNER &&
                bss_text_ist_gleich(kmp_syn_glied(syntax, 0)->text, bss_text(SCHLÜSSELWORT_IMPORT)))
            {
                kmp_syn_weiter(syntax, 1);

                KMP_syn_ausdruck_t* dateiname = kmp_syn_basis_ausdruck_einlesen(speicher, syntax);
                if (dateiname->basis->art != KMP_SYN_KNOTEN_AUSDRUCK_TEXT)
                {
                    return kmp_syn_ausdruck_ungültig(speicher, dateiname->basis->spanne);
                }

                KMP_spanne_t spanne = kmp_spanne_bereich(raute->spanne, dateiname->basis->spanne);
                KMP_syn_ausdruck_t* erg = kmp_syn_ausdruck_import(
                    speicher,
                    spanne,
                    dateiname->text.wert
                );

                return erg;
            }

            else if (kmp_syn_glied(syntax, 0)->art == KMP_GLIED_AUSRUFEZEICHEN)
            {
                kmp_syn_weiter(syntax, 1);

                KMP_syn_ausdruck_t* ausdruck = kmp_syn_ausdruck_einlesen(speicher, syntax);
                KMP_spanne_t spanne = kmp_spanne_bereich(raute->spanne, ausdruck->basis->spanne);

                KMP_syn_ausdruck_t* erg = kmp_syn_ausdruck_ausführen(
                    speicher,
                    spanne,
                    ausdruck
                );

                return erg;
            }
        } break;

        case KMP_GLIED_GANZZAHL:
        {
            kmp_syn_weiter(syntax, 1);
            KMP_syn_ausdruck_t* erg = kmp_syn_ausdruck_ganzzahl(
                speicher,
                t->spanne,
                t->gz.wert);

            return erg;
        } break;

        case KMP_GLIED_DEZIMALZAHL:
        {
            kmp_syn_weiter(syntax, 1);
            KMP_syn_ausdruck_t* erg = kmp_syn_ausdruck_dezimalzahl(
                speicher,
                t->spanne,
                t->dz.wert);

            return erg;
        } break;

        case KMP_GLIED_BEZEICHNER:
        {
            if (bss_text_ist_gleich(t->text, bss_text(SCHLÜSSELWORT_SCHABLONE)) &&
                kmp_syn_ist(syntax, KMP_GLIED_GKLAMMER_AUF, 1))
            {
                KMP_glied_t* anfang = kmp_syn_weiter(syntax, 1);
                KMP_glied_t* klammer_auf = kmp_syn_erwarte(syntax, KMP_GLIED_GKLAMMER_AUF);

                BSS_Feld(KMP_syn_deklaration_t*) eigenschaften = bss_feld(speicher, sizeof(KMP_syn_deklaration_t*));
                for (;;)
                {
                    KMP_syn_deklaration_t* eigenschaft = kmp_syn_deklaration_einlesen(speicher, syntax, false);
                    bss_feld_hinzufügen(speicher, &eigenschaften, eigenschaft);

                    if (!kmp_syn_passt(syntax, KMP_GLIED_PUNKT))
                    {
                        break;
                    }
                }

                KMP_glied_t* klammer_zu = kmp_syn_erwarte(syntax, KMP_GLIED_GKLAMMER_ZU);

                KMP_spanne_t spanne = kmp_spanne_bereich(anfang->spanne, klammer_zu->spanne);
                KMP_syn_ausdruck_t* erg = kmp_syn_ausdruck_schablone(
                    speicher,
                    spanne,
                    eigenschaften
                );

                return erg;
            }

            else if (bss_text_ist_gleich(t->text, bss_text(SCHLÜSSELWORT_OPTION)) &&
                     kmp_syn_ist(syntax, KMP_GLIED_GKLAMMER_AUF, 1))
            {
                KMP_glied_t* anfang = kmp_syn_weiter(syntax, 1);
                KMP_glied_t* klammer_auf = kmp_syn_erwarte(syntax, KMP_GLIED_GKLAMMER_AUF);
                BSS_Feld(KMP_syn_deklaration_t*) eigenschaften = bss_feld(speicher, sizeof(KMP_syn_deklaration_t*));
                BSS_Feld(BSS_text_t) namen = bss_feld(speicher, sizeof(BSS_text_t));

                for (;;)
                {
                    KMP_syn_ausdruck_t* name = kmp_syn_basis_ausdruck_einlesen(speicher, syntax);
                    if (name->basis->art != KMP_SYN_KNOTEN_AUSDRUCK_BEZEICHNER)
                    {
                        break;
                    }

                    bss_feld_hinzufügen(speicher, &namen, &name->bezeichner.wert);

                    KMP_syn_spezifizierung_t* spezifizierung = NULL;
                    if (kmp_syn_passt(syntax, KMP_GLIED_BALKEN) && !kmp_syn_ist(syntax, KMP_GLIED_ZUWEISUNG, 0))
                    {
                        spezifizierung = kmp_syn_spezifizierung_einlesen(speicher, syntax);
                    }

                    KMP_syn_ausdruck_t* init = NULL;
                    if (kmp_syn_passt(syntax, KMP_GLIED_ZUWEISUNG))
                    {
                        init = kmp_syn_ausdruck_einlesen(speicher, syntax);
                    }

                    KMP_syn_deklaration_t* eigenschaft = kmp_syn_deklaration_variable(
                        speicher,
                        name->basis->spanne,
                        namen,
                        spezifizierung,
                        init
                    );

                    bss_feld_hinzufügen(speicher, &eigenschaften, eigenschaft);

                    if (!kmp_syn_passt(syntax, KMP_GLIED_PUNKT))
                    {
                        break;
                    }
                }

                KMP_glied_t* klammer_zu = kmp_syn_erwarte(syntax, KMP_GLIED_GKLAMMER_ZU);

                KMP_spanne_t spanne = kmp_spanne_bereich(anfang->spanne, klammer_zu->spanne);
                KMP_syn_ausdruck_t* erg = kmp_syn_ausdruck_option(
                    speicher,
                    spanne,
                    eigenschaften
                );

                return erg;
            }
            else
            {
                kmp_syn_weiter(syntax, 1);
                KMP_syn_ausdruck_t* erg = kmp_syn_ausdruck_bezeichner(speicher, t->spanne, t->text);

                return erg;
            }
        } break;

        case KMP_GLIED_TEXT:
        {
            kmp_syn_weiter(syntax, 1);
            KMP_syn_ausdruck_t* erg = kmp_syn_ausdruck_text(speicher, t->spanne, t->text);

            return erg;
        } break;

        case KMP_GLIED_RKLAMMER_AUF:
        {
            KMP_glied_t* klammer_auf = kmp_syn_weiter(syntax, 1);

            BSS_Feld(KMP_syn_deklaration_t*) parameter = bss_feld(speicher, sizeof(KMP_syn_deklaration_t*));
            bool könnte_funktion_sein = true;
            g32 vorheriger_index = syntax->glied_index;

            if (kmp_syn_glied(syntax, 0)->art != KMP_GLIED_RKLAMMER_ZU)
            {
                for (;;)
                {
                    KMP_syn_deklaration_t* param = kmp_syn_deklaration_einlesen(speicher, syntax, false);

                    if (!param)
                    {
                        könnte_funktion_sein = false;
                        break;
                    }

                    bss_feld_hinzufügen(speicher, &parameter, param);

                    if (!kmp_syn_passt(syntax, KMP_GLIED_STRICHPUNKT))
                    {
                        break;
                    }
                }
            }

            if (könnte_funktion_sein && kmp_syn_glied(syntax, 0)->art == KMP_GLIED_RKLAMMER_ZU)
            {
                KMP_glied_t* klammer_zu = kmp_syn_weiter(syntax, 1);

                KMP_syn_spezifizierung_t* rückgabe = NULL;
                if (kmp_syn_passt(syntax, KMP_GLIED_PFEIL))
                {
                    rückgabe = kmp_syn_spezifizierung_einlesen(speicher, syntax);
                    if (rückgabe == NULL)
                    {
                        assert(!"meldung erstatten");
                    }
                }

                if (kmp_syn_glied(syntax, 0)->art != KMP_GLIED_GKLAMMER_AUF)
                {
                    KMP_spanne_t spanne = kmp_spanne_bereich(klammer_auf->spanne, klammer_zu->spanne);
                    KMP_syn_ausdruck_t* erg = kmp_syn_ausdruck_routine(
                        speicher,
                        spanne,
                        parameter,
                        rückgabe,
                        NULL
                    );

                    return erg;
                }

                KMP_syn_anweisung_t* rumpf = NULL;
                if (kmp_syn_glied(syntax, 0)->art == KMP_GLIED_GKLAMMER_AUF)
                {
                    rumpf = kmp_syn_anweisung_einlesen(speicher, syntax);
                }

                if (rückgabe || rumpf)
                {
                    KMP_spanne_t spanne = kmp_spanne_bereich(klammer_auf->spanne, rumpf
                            ? rumpf->basis->spanne
                            : (rückgabe
                                   ? rückgabe->basis->spanne
                                   : klammer_zu->spanne));

                    KMP_syn_ausdruck_t* erg = kmp_syn_ausdruck_routine(
                        speicher,
                        spanne,
                        parameter,
                        rückgabe,
                        rumpf
                    );

                    return erg;
                }
            }

            syntax->glied_index = vorheriger_index;

            KMP_syn_ausdruck_t* ausdruck = kmp_syn_ausdruck_einlesen(speicher, syntax);

            KMP_glied_t* klammer_zu = kmp_syn_erwarte(syntax, KMP_GLIED_RKLAMMER_ZU);
            if (!klammer_zu)
            {
                kmp_diagnostik_melden_fehler(speicher, &syntax->diagnostik, kmp_syn_glied(syntax, 0)->spanne, kmp_fehler(speicher, bss_text("')' erwartet")));
            }

            KMP_spanne_t spanne = kmp_spanne_bereich(klammer_auf->spanne, klammer_zu->spanne);
            KMP_syn_ausdruck_t* erg = kmp_syn_ausdruck_klammer(
                speicher,
                spanne,
                ausdruck
            );

            return erg;
        } break;

        case KMP_GLIED_GKLAMMER_AUF:
        {
            KMP_glied_t* klammer_auf = kmp_syn_weiter(syntax, 1);
            w32 ist_benamt = false;

            BSS_Feld(KMP_syn_ausdruck_kompositum_eigenschaft_t*) eigenschaften = bss_feld(speicher, sizeof(KMP_syn_ausdruck_kompositum_eigenschaft_t*));
            if (!kmp_syn_ist(syntax, KMP_GLIED_GKLAMMER_ZU, 0))
            {
                for (;;)
                {
                    if (kmp_syn_passt(syntax, KMP_GLIED_PISA))
                    {
                        ist_benamt = true;

                        KMP_syn_ausdruck_t* a = kmp_syn_basis_ausdruck_einlesen(speicher, syntax);
                        kmp_syn_erwarte(syntax, KMP_GLIED_ZUWEISUNG);

                        KMP_syn_ausdruck_t* ausdruck = kmp_syn_ausdruck_einlesen(speicher, syntax);
                        KMP_spanne_t spanne = kmp_spanne_bereich(a->basis->spanne, ausdruck->basis->spanne);
                        KMP_syn_ausdruck_kompositum_eigenschaft_t* eigenschaft = kmp_syn_ausdruck_kompositum_eigenschaft(
                            speicher,
                            spanne,
                            a->bezeichner.wert,
                            ausdruck,
                            ist_benamt);

                        bss_feld_hinzufügen(speicher, &eigenschaften, eigenschaft);
                    }
                    else
                    {
                        ist_benamt = false;

                        KMP_syn_ausdruck_t* ausdruck = kmp_syn_ausdruck_einlesen(speicher, syntax);
                        KMP_syn_ausdruck_kompositum_eigenschaft_t* eigenschaft = kmp_syn_ausdruck_kompositum_eigenschaft(
                            speicher,
                            ausdruck->basis->spanne,
                            bss_text(""),
                            ausdruck,
                            ist_benamt
                        );
                        bss_feld_hinzufügen(speicher, &eigenschaften, eigenschaft);
                    }

                    if (!kmp_syn_passt(syntax, KMP_GLIED_STRICHPUNKT))
                    {
                        break;
                    }
                }
            }

            KMP_glied_t* klammer_zu = kmp_syn_erwarte(syntax, KMP_GLIED_GKLAMMER_ZU);

            KMP_syn_spezifizierung_t* spezifizierung = NULL;
            if (kmp_syn_passt(syntax, KMP_GLIED_KLEINER))
            {
                spezifizierung = kmp_syn_spezifizierung_einlesen(speicher, syntax);
                kmp_syn_erwarte(syntax, KMP_GLIED_GRÖẞER);
            }

            KMP_spanne_t spanne = kmp_spanne_bereich(klammer_auf->spanne, klammer_zu->spanne);
            KMP_syn_ausdruck_t* erg = kmp_syn_ausdruck_kompositum(
                speicher,
                spanne,
                eigenschaften,
                ist_benamt,
                spezifizierung);

            return erg;
        } break;
    }

    return kmp_syn_ausdruck_ungültig(speicher, kmp_syn_glied(syntax, 0)->spanne);
}
// }}}
// spezifizierung {{{
KMP_syn_spezifizierung_t*
kmp_syn_spezifizierung_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syntax)
{
    if (kmp_syn_glied(syntax, 0)->art == KMP_GLIED_STERN)
    {
        KMP_glied_t* stern = kmp_syn_weiter(syntax, 1);
        KMP_syn_spezifizierung_t* basis = kmp_syn_spezifizierung_einlesen(speicher, syntax);

        if (basis == NULL)
        {
            assert(!"meldung erstatten");
            return NULL;
        }

        KMP_spanne_t spanne = kmp_spanne_bereich(stern->spanne, basis->basis->spanne);
        KMP_syn_spezifizierung_t* erg = kmp_syn_spezifizierung_zeiger(
            speicher,
            spanne,
            basis);

        return erg;
    }

    else if (kmp_syn_glied(syntax, 0)->art == KMP_GLIED_EKLAMMER_AUF)
    {
        KMP_glied_t* klammer_auf = kmp_syn_weiter(syntax, 1);

        KMP_syn_ausdruck_t* ausdruck = kmp_syn_ausdruck_einlesen(speicher, syntax);
        KMP_glied_t* klammer_zu = kmp_syn_erwarte(syntax, KMP_GLIED_EKLAMMER_ZU);
        KMP_syn_spezifizierung_t* basis = kmp_syn_spezifizierung_einlesen(speicher, syntax);

        KMP_spanne_t spanne = kmp_spanne_bereich(klammer_auf->spanne, basis->basis->spanne);
        KMP_syn_spezifizierung_t* erg = kmp_syn_spezifizierung_feld(
            speicher,
            spanne,
            basis,
            ausdruck
        );

        return erg;
    }

    else if (kmp_syn_glied(syntax, 0)->art == KMP_GLIED_RKLAMMER_AUF)
    {
        KMP_glied_t* klammer_auf = kmp_syn_weiter(syntax, 1);

        BSS_Feld(KMP_syn_spezifizierung_t*) parameter = bss_feld(speicher, sizeof(KMP_syn_spezifizierung_t*));
        if (kmp_syn_glied(syntax, 0)->art != KMP_GLIED_RKLAMMER_AUF)
        {
            for (;;)
            {
                if (kmp_syn_glied(syntax, 0)->art == KMP_GLIED_BEZEICHNER && kmp_syn_glied(syntax, 1)->art == KMP_GLIED_BALKEN)
                {
                    KMP_syn_ausdruck_t* bezeichner = kmp_syn_ausdruck_einlesen(speicher, syntax);
                    kmp_syn_erwarte(syntax, KMP_GLIED_BALKEN);
                }

                KMP_syn_spezifizierung_t* spez = kmp_syn_spezifizierung_einlesen(speicher, syntax);
                if (spez != NULL)
                {
                    bss_feld_hinzufügen(speicher, &parameter, spez);
                }

                if (!kmp_syn_passt(syntax, KMP_GLIED_STRICHPUNKT))
                {
                    break;
                }
            }
        }

        KMP_glied_t* klammer_zu = kmp_syn_erwarte(syntax, KMP_GLIED_RKLAMMER_ZU);
        KMP_zeichen_t bis = klammer_zu->spanne.bis;

        KMP_syn_spezifizierung_t* rückgabe = NULL;
        if (kmp_syn_passt(syntax, KMP_GLIED_PFEIL))
        {
            rückgabe = kmp_syn_spezifizierung_einlesen(speicher, syntax);
            bis = rückgabe->basis->spanne.bis;
        }

        KMP_syn_spezifizierung_t* erg = kmp_syn_spezifizierung_routine(
            speicher,
            kmp_spanne_bereich(klammer_auf->spanne, (KMP_spanne_t){ .bis = bis }),
            parameter,
            rückgabe);

        return erg;
    }

    else if (kmp_syn_glied(syntax, 0)->art == KMP_GLIED_BEZEICHNER)
    {
        KMP_glied_t* name = kmp_syn_weiter(syntax, 1);

        KMP_syn_spezifizierung_t* erg = kmp_syn_spezifizierung_bezeichner(
            speicher,
            name->spanne,
            name->text);

        return erg;
    }

    return NULL;
}
// }}}
// hilfsfunktionen {{{
KMP_glied_t*
kmp_syn_erwarte(KMP_syntax_t* syntax, g32 art)
{
    if (kmp_syn_glied(syntax, 0)->art == art)
    {
        KMP_glied_t* erg = kmp_syn_glied(syntax, 0);
        kmp_syn_weiter(syntax, 1);

        return erg;
    }

    return NULL;
}

KMP_glied_t*
kmp_syn_glied(KMP_syntax_t* syntax, g32 versatz)
{
    g32 index = syntax->glied_index + versatz;

    if (syntax->glieder.anzahl_elemente <= (g64) index)
    {
        index = (g32) syntax->glieder.anzahl_elemente - 1;
    }

    KMP_glied_t* erg = bss_feld_element(syntax->glieder, index);

    return erg;
}

w32
kmp_syn_ist(KMP_syntax_t* syntax, g32 art, g32 versatz)
{
    w32 erg = kmp_syn_glied(syntax, versatz)->art == art;

    return erg;
}

KMP_glied_t*
kmp_syn_weiter(KMP_syntax_t* syntax, g32 anzahl)
{
    KMP_glied_t* erg = kmp_syn_glied(syntax, 0);
    syntax->glied_index += anzahl;

    return erg;
}

w32
kmp_syn_passt(KMP_syntax_t* syntax, g32 art)
{
    if (kmp_syn_glied(syntax, 0)->art == art)
    {
        kmp_syn_weiter(syntax, 1);
        return true;
    }

    return false;
}
// }}}

g32
kmp_syn_glied_zu_binär_op(KMP_glied_t* glied)
{
    switch (glied->art)
    {
        case KMP_GLIED_PLUS:           return KMP_SYN_AUSDRUCK_BINÄR_ADDITION;
        case KMP_GLIED_MINUS:          return KMP_SYN_AUSDRUCK_BINÄR_SUBTRAKTION;
        case KMP_GLIED_STERN:          return KMP_SYN_AUSDRUCK_BINÄR_MULTIPLIKATION;
        case KMP_GLIED_DOPPELPUNKT:    return KMP_SYN_AUSDRUCK_BINÄR_DIVISION;
        case KMP_GLIED_PROZENT:        return KMP_SYN_AUSDRUCK_BINÄR_MODULO;
        case KMP_GLIED_KLEINER:        return KMP_SYN_AUSDRUCK_BINÄR_KLEINER;
        case KMP_GLIED_KLEINER_GLEICH: return KMP_SYN_AUSDRUCK_BINÄR_KLEINER_GLEICH;
        case KMP_GLIED_GLEICH:         return KMP_SYN_AUSDRUCK_BINÄR_GLEICH;
        case KMP_GLIED_UNGLEICH:       return KMP_SYN_AUSDRUCK_BINÄR_UNGLEICH;
        case KMP_GLIED_GRÖẞER_GLEICH:  return KMP_SYN_AUSDRUCK_BINÄR_GRÖẞER_GLEICH;
        case KMP_GLIED_GRÖẞER:         return KMP_SYN_AUSDRUCK_BINÄR_GRÖẞER;
        case KMP_GLIED_DREIECK_RECHTS: return KMP_SYN_AUSDRUCK_BINÄR_BIT_VERSATZ_RECHTS;
        case KMP_GLIED_DREIECK_LINKS:  return KMP_SYN_AUSDRUCK_BINÄR_BIT_VERSATZ_LINKS;

        default:
            assert(!"unbekannter op");
            return KMP_SYN_AUSDRUCK_BINÄR_UNBEKANNT;
    }
}

g32
kmp_syn_glied_zu_unär_op(KMP_glied_t* glied)
{
    switch (glied->art)
    {
        case KMP_GLIED_MINUS:          return KMP_SYN_AUSDRUCK_UNÄR_MINUS;
        case KMP_GLIED_AUSRUFEZEICHEN: return KMP_SYN_AUSDRUCK_UNÄR_NEGIERUNG;
        case KMP_GLIED_TILDE:          return KMP_SYN_AUSDRUCK_UNÄR_INVERTIERUNG;

        default:
            assert(!"unbekannter op");
            return KMP_SYN_AUSDRUCK_UNÄR_UNBEKANNT;
    }
}

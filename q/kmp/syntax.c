#include "kmp/syntax.h"

#include "kmp/asb.h"
#include "kmp/lexer.h"

#include <assert.h>

char * SCHLÜSSELWORT_SCHABLONE = "schablone";  // INFO: struct
char * SCHLÜSSELWORT_MERKMAL   = "merkmal";    // INFO: trait
char * SCHLÜSSELWORT_OPTION    = "option";     // INFO: enum
char * SCHLÜSSELWORT_WEICHE    = "weiche";     // INFO: match
char * SCHLÜSSELWORT_WENN      = "wenn";       // INFO: if
char * SCHLÜSSELWORT_FÜR       = "für";        // INFO: for
char * SCHLÜSSELWORT_SOLANGE   = "solange";    // INFO: while
char * SCHLÜSSELWORT_SONST     = "sonst";      // INFO: else
char * SCHLÜSSELWORT_BRAUCHE   = "brauche";    // INFO: import
char * SCHLÜSSELWORT_LADE      = "lade";       // INFO: include
char * SCHLÜSSELWORT_FINAL     = "final";      // INFO: defer
char * SCHLÜSSELWORT_RES       = "res";        // INFO: resultat -> return
char * SCHLÜSSELWORT_MARKE     = "marke";
char * SCHLÜSSELWORT_WEITER    = "weiter";     // INFO: continue
char * SCHLÜSSELWORT_SPRUNG    = "sprung";     // INFO: goto
char * SCHLÜSSELWORT_RAUS      = "raus";       // INFO: break

void
kmp_syn_melden_spanne(BSS_speicher_t* speicher, KMP_syntax_t* syntax, KMP_spanne_t spanne, KMP_fehler_t* fehler)
{
    kmp_diagnostik_melden_fehler(speicher, &syntax->diagnostik, spanne, fehler);
    /*__debugbreak();*/
}

void
kmp_syn_melden_knoten(BSS_speicher_t* speicher, KMP_syntax_t* syntax, KMP_syn_knoten_t* knoten, KMP_fehler_t* fehler)
{
    kmp_syn_melden_spanne(speicher, syntax, knoten->spanne, fehler);
}

void
kmp_syn_melden_glied(BSS_speicher_t* speicher, KMP_syntax_t* syntax, KMP_glied_t* glied, KMP_fehler_t* fehler)
{
    kmp_syn_melden_spanne(speicher, syntax, glied->spanne, fehler);
}

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
    w32 erg = kmp_syn_ist(syntax, GLIED_ENDE, 0);

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

        KMP_ergebnis_t ergebnis = kmp_syn_anweisung_einlesen(speicher, syntax);

        if (ergebnis.schlecht)
        {
            kmp_syn_melden_glied(speicher, syntax, kmp_syn_glied(syntax, 0), ergebnis.fehler);
            break;
        }

        KMP_syn_anweisung_t* anweisung = ergebnis.wert;
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
kmp_syn_marken_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syn)
{
    BSS_Feld(KMP_syn_marke_t*) erg = bss_feld(speicher, sizeof(KMP_syn_marke_t*));
    KMP_glied_t* anfang = kmp_syn_glied(syn, 0);
    KMP_glied_t* glied = anfang;

    if (glied->art != GLIED_RAUTE)
    {
        return erg;
    }

    if (!kmp_syn_ist(syn, GLIED_BEZEICHNER, 1) ||
         bss_text_ist_gleich(kmp_syn_glied(syn, 0)->text, bss_text(SCHLÜSSELWORT_MARKE)) != 0)
    {
        return erg;
    }

    kmp_syn_weiter(syn, 2);

    for (;;)
    {
        KMP_ergebnis_t ausdruck_erg = kmp_syn_ausdruck_einlesen(speicher, syn);
        if (ausdruck_erg.schlecht)
        {
            break;
        }

        KMP_syn_ausdruck_t* ausdruck = ausdruck_erg.wert;
        if (ausdruck->basis->art != KMP_SYN_KNOTEN_AUSDRUCK_BEZEICHNER)
        {
            break;
        }

        bss_feld_hinzufügen(speicher, &erg, kmp_syn_marke(speicher, ausdruck->basis->spanne, ausdruck->bezeichner.wert));

        if (!kmp_syn_passt(syn, GLIED_KOMMA))
        {
            break;
        }
    }

    return erg;
}

// anweisung {{{
KMP_ergebnis_t
kmp_syn_anweisung_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syn)
{
    BSS_Feld(KMP_syn_marke_t*) marken = kmp_syn_marken_einlesen(speicher, syn);

    KMP_syn_anweisung_t* anweisung = NULL;

    if (kmp_syn_ist(syn, GLIED_GKLAMMER_AUF, 0))
    {
        anweisung = kmp_syn_block_anweisung_einlesen(speicher, syn);
    }

    else if (kmp_syn_ist(syn, GLIED_KLEINER, 0))
    {
        anweisung = kmp_syn_markierung_anweisung_einlesen(speicher, syn);
        kmp_syn_erwarte(syn, GLIED_PUNKT);
    }

    else if (kmp_syn_ist(syn, GLIED_BEZEICHNER, 0) &&
             bss_text_ist_gleich(kmp_syn_glied(syn, 0)->text, bss_text(SCHLÜSSELWORT_WENN)) == 0)
    {
        anweisung = kmp_syn_wenn_anweisung_einlesen(speicher, syn);
    }

    else if (kmp_syn_ist(syn, GLIED_BEZEICHNER, 0) &&
             bss_text_ist_gleich(kmp_syn_glied(syn, 0)->text, bss_text(SCHLÜSSELWORT_FÜR)) == 0)
    {
        anweisung = kmp_syn_für_anweisung_einlesen(speicher, syn);
    }

    else if (kmp_syn_ist(syn, GLIED_BEZEICHNER, 0) &&
             bss_text_ist_gleich(kmp_syn_glied(syn, 0)->text, bss_text(SCHLÜSSELWORT_SOLANGE)) == 0)
    {
        anweisung = kmp_syn_solange_anweisung_einlesen(speicher, syn);
    }

    else if (kmp_syn_ist(syn, GLIED_BEZEICHNER, 0) &&
             bss_text_ist_gleich(kmp_syn_glied(syn, 0)->text, bss_text(SCHLÜSSELWORT_WEICHE)) == 0)
    {
        anweisung = kmp_syn_weiche_anweisung_einlesen(speicher, syn);
    }

    else if (kmp_syn_ist(syn, GLIED_BEZEICHNER, 0) &&
             bss_text_ist_gleich(kmp_syn_glied(syn, 0)->text, bss_text(SCHLÜSSELWORT_FINAL)) == 0)
    {
        anweisung = kmp_syn_final_anweisung_einlesen(speicher, syn);
    }

    else if (kmp_syn_ist(syn, GLIED_BEZEICHNER, 0) &&
             bss_text_ist_gleich(kmp_syn_glied(syn, 0)->text, bss_text(SCHLÜSSELWORT_RES)) == 0)
    {
        anweisung = kmp_syn_res_anweisung_einlesen(speicher, syn);
    }

    else if (kmp_syn_ist(syn, GLIED_BEZEICHNER, 0) &&
             bss_text_ist_gleich(kmp_syn_glied(syn, 0)->text, bss_text(SCHLÜSSELWORT_WEITER)) == 0)
    {
        anweisung = kmp_syn_weiter_anweisung_einlesen(speicher, syn);
        kmp_syn_erwarte(syn, GLIED_PUNKT);
    }

    else if (kmp_syn_ist(syn, GLIED_BEZEICHNER, 0) &&
             bss_text_ist_gleich(kmp_syn_glied(syn, 0)->text, bss_text(SCHLÜSSELWORT_SPRUNG)) == 0)
    {
        anweisung = kmp_syn_sprung_anweisung_einlesen(speicher, syn);
        kmp_syn_erwarte(syn, GLIED_PUNKT);
    }

    else if (kmp_syn_ist(syn, GLIED_BEZEICHNER, 0) &&
             bss_text_ist_gleich(kmp_syn_glied(syn, 0)->text, bss_text(SCHLÜSSELWORT_RAUS)) == 0)
    {
        anweisung = kmp_syn_raus_anweisung_einlesen(speicher, syn);
        kmp_syn_erwarte(syn, GLIED_PUNKT);
    }

    else if (kmp_syn_ist(syn, GLIED_BEZEICHNER, 0) &&
             kmp_syn_ist(syn, GLIED_BALKEN, 1))
    {
        anweisung = kmp_syn_deklaration_anweisung_einlesen(speicher, syn);
    }

    else if (kmp_syn_ist(syn, GLIED_RAUTE, 0))
    {
        if (kmp_syn_ist(syn, GLIED_BEZEICHNER, 1) &&
            bss_text_ist_gleich(kmp_syn_glied(syn, 1)->text, bss_text(SCHLÜSSELWORT_BRAUCHE)) == 0)
        {
            KMP_ergebnis_t ausdruck_erg = kmp_syn_basis_ausdruck_einlesen(speicher, syn);
            if (ausdruck_erg.schlecht)
            {
                assert(!"meldung erstatten");
            }

            KMP_syn_ausdruck_t* ausdruck = ausdruck_erg.wert;
            if (ausdruck->basis->art != KMP_SYN_KNOTEN_AUSDRUCK_BRAUCHE)
            {
                assert(!"meldung erstatten");
            }

            anweisung = kmp_syn_anweisung_brauche(speicher, ausdruck->basis->spanne, ausdruck->brauche.dateiname);
        }

        else if (kmp_syn_ist(syn, GLIED_BEZEICHNER, 1) &&
                 bss_text_ist_gleich(kmp_syn_glied(syn, 1)->text, bss_text(SCHLÜSSELWORT_LADE)) == 0)
        {
            kmp_syn_weiter(syn, 2);

            KMP_ergebnis_t knoten_erg = kmp_syn_basis_ausdruck_einlesen(speicher, syn);
            if (knoten_erg.schlecht)
            {
                assert(!"meldung erstatten");
            }

            KMP_syn_ausdruck_t* ausdruck = knoten_erg.wert;
            if (ausdruck->basis->art != KMP_SYN_KNOTEN_AUSDRUCK_TEXT)
            {
                assert(!"meldung erstatten");
            }

            anweisung = kmp_syn_anweisung_lade(speicher, ausdruck->basis->spanne, ausdruck->text.wert);
        }

        else if (kmp_syn_ist(syn, GLIED_AUSRUFEZEICHEN, 1))
        {
            KMP_ergebnis_t erg = kmp_syn_basis_ausdruck_einlesen(speicher, syn);
            if (erg.schlecht)
            {
                assert(!"meldung erstatten");
            }

            kmp_syn_erwarte(syn, GLIED_PUNKT);

            KMP_syn_ausdruck_t* ausdruck = erg.wert;
            anweisung = kmp_syn_anweisung_ausdruck(speicher, ausdruck->basis->spanne, ausdruck);
        }
    }

    else
    {
        KMP_ergebnis_t erg = kmp_syn_ausdruck_einlesen(speicher, syn);
        if (erg.gut)
        {
            KMP_syn_ausdruck_t* links = erg.wert;

            if (kmp_syn_glied(syn, 0)->art >= GLIED_ZUWEISUNG &&
                kmp_syn_glied(syn, 0)->art <= GLIED_ZUWEISUNG_PROZENT)
            {
                KMP_glied_t* op = kmp_syn_weiter(syn, 1);

                KMP_ergebnis_t rechts_erg = kmp_syn_ausdruck_einlesen(speicher, syn);
                if (rechts_erg.schlecht)
                {
                    assert(!"gültigen ausdruck erwartet");
                }

                KMP_syn_ausdruck_t* rechts = rechts_erg.wert;
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
        else
        {
            return kmp_ergebnis_fehler(erg.fehler);
        }
    }

    if (anweisung == NULL)
    {
        return kmp_ergebnis_fehler(kmp_fehler(speicher, bss_text("Konnte keine gültige Anweisung einlesen")));
    }

    anweisung->marken = marken;

    return kmp_ergebnis_erfolg(anweisung);
}

KMP_syn_anweisung_t*
kmp_syn_deklaration_anweisung_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syn)
{
    KMP_syn_deklaration_t* deklaration = kmp_syn_deklaration_einlesen(speicher, syn, true);
    KMP_syn_anweisung_t* erg = kmp_syn_anweisung_deklaration(speicher, deklaration->basis->spanne, deklaration);

    return erg;
}

KMP_syn_anweisung_t*
kmp_syn_block_anweisung_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syn)
{
    KMP_glied_t* klammer_auf = kmp_syn_erwarte(syn, GLIED_GKLAMMER_AUF);

    BSS_Feld(KMP_syn_anweisung_t*) anweisungen = bss_feld(speicher, sizeof(KMP_syn_anweisung_t*));
    if (!kmp_syn_ist(syn, GLIED_GKLAMMER_ZU, 0))
    {
        for (;;)
        {
            KMP_ergebnis_t erg = kmp_syn_anweisung_einlesen(speicher, syn);
            if (erg.schlecht)
            {
                break;
            }

            kmp_syn_passt(syn, GLIED_PUNKT);
            bss_feld_hinzufügen(speicher, &anweisungen, erg.wert);
        }
    }

    KMP_glied_t* klammer_zu = kmp_syn_erwarte(syn, GLIED_GKLAMMER_ZU);

    KMP_syn_anweisung_t* erg = kmp_syn_anweisung_block(
        speicher,
        kmp_spanne_bereich(klammer_auf->spanne, klammer_zu->spanne),
        anweisungen);

    return erg;
}

KMP_syn_anweisung_t*
kmp_syn_wenn_anweisung_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syn)
{
    KMP_glied_t* schlüsselwort = kmp_syn_weiter(syn, 1);

    KMP_ergebnis_t bedingung = kmp_syn_ausdruck_einlesen(speicher, syn);
    if (bedingung.schlecht)
    {
        assert(!"meldung erstatten");
        return NULL;
    }

    KMP_ergebnis_t rumpf = kmp_syn_anweisung_einlesen(speicher, syn);

    KMP_syn_anweisung_t* alternative_anweisung = NULL;
    if (kmp_syn_ist(syn, GLIED_BEZEICHNER, 0) &&
        bss_text_ist_gleich(kmp_syn_glied(syn, 0)->text, bss_text(SCHLÜSSELWORT_SONST)) == 0)
    {
        KMP_glied_t* alternative = kmp_syn_weiter(syn, 1);
        if (kmp_syn_ist(syn, GLIED_BEZEICHNER, 0) &&
            bss_text_ist_gleich(kmp_syn_glied(syn, 0)->text, bss_text(SCHLÜSSELWORT_WENN)) == 0)
        {
            alternative_anweisung = kmp_syn_wenn_anweisung_einlesen(speicher, syn);
        }
        else
        {
            KMP_ergebnis_t alternative_rumpf_erg = kmp_syn_anweisung_einlesen(speicher, syn);
            if (alternative_rumpf_erg.schlecht)
            {
                assert(!"meldung erstatten");
            }

            KMP_syn_anweisung_t* alternative_rumpf = alternative_rumpf_erg.wert;
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
           : ((KMP_syn_knoten_t*) rumpf.wert)->spanne);

    KMP_syn_anweisung_t* erg = kmp_syn_anweisung_wenn(
        speicher, spanne,
        (KMP_syn_ausdruck_t*) bedingung.wert,
        (KMP_syn_anweisung_t*) rumpf.wert,
        alternative_anweisung);

    return erg;
}

KMP_syn_anweisung_t*
kmp_syn_markierung_anweisung_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syn)
{
    KMP_glied_t* kleiner_als = kmp_syn_weiter(syn, 1);
    KMP_ergebnis_t markierung_erg = kmp_syn_basis_ausdruck_einlesen(speicher, syn);

    if (markierung_erg.schlecht)
    {
        kmp_syn_melden_glied(speicher, syn, kmp_syn_glied(syn, 0), markierung_erg.fehler);
        return NULL;
    }

    KMP_syn_ausdruck_t* markierung = markierung_erg.wert;
    if (markierung->basis->art != KMP_SYN_KNOTEN_AUSDRUCK_BEZEICHNER)
    {
        KMP_fehler_t* fehler = kmp_fehler(speicher, bss_text("gültigen Bezeichner erwartet."));
        kmp_syn_melden_knoten(speicher, syn, markierung->basis, fehler);
        return NULL;
    }

    KMP_glied_t* größer_als = kmp_syn_erwarte(syn, GLIED_GRÖẞER);

    KMP_syn_anweisung_t* erg = kmp_syn_anweisung_markierung(
        speicher,
        kmp_spanne_bereich(kleiner_als->spanne, größer_als->spanne),
        markierung->bezeichner.wert);

    return erg;
}

KMP_syn_anweisung_t*
kmp_syn_für_anweisung_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syn)
{
    KMP_glied_t* schlüsselwort = kmp_syn_weiter(syn, 1);

    KMP_syn_ausdruck_t* index = NULL;
    KMP_syn_ausdruck_t* bedingung = NULL;

    KMP_ergebnis_t ausdruck_erg = kmp_syn_basis_ausdruck_einlesen(speicher, syn);
    if (ausdruck_erg.schlecht)
    {
        assert(!"meldung erstatten");
        return NULL;
    }

    bedingung = ausdruck_erg.wert;
    if (kmp_syn_passt(syn, GLIED_DOPPELPUNKT))
    {
        index = ausdruck_erg.wert;
        ausdruck_erg = kmp_syn_ausdruck_einlesen(speicher, syn);

        if (ausdruck_erg.schlecht)
        {
            assert(!"meldung erstatten");
            return NULL;
        }

        bedingung = ausdruck_erg.wert;
    }

    KMP_ergebnis_t rumpf_erg = kmp_syn_anweisung_einlesen(speicher, syn);
    if (rumpf_erg.schlecht)
    {
        assert(!"meldung erstatten");
        return NULL;
    }

    KMP_syn_anweisung_t* rumpf = rumpf_erg.wert;
    KMP_syn_anweisung_t* erg = kmp_syn_anweisung_für(
        speicher,
        kmp_spanne_bereich(schlüsselwort->spanne, rumpf->basis->spanne),
        index, bedingung, rumpf);

    return erg;
}

KMP_syn_anweisung_t*
kmp_syn_solange_anweisung_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syn)
{
    KMP_glied_t* schlüsselwort = kmp_syn_weiter(syn, 1);

    KMP_ergebnis_t bedingung_erg = kmp_syn_ausdruck_einlesen(speicher, syn);
    if (bedingung_erg.schlecht)
    {
        kmp_syn_melden_glied(speicher, syn, kmp_syn_glied(syn, 0), bedingung_erg.fehler);
    }

    KMP_syn_ausdruck_t* bedingung = bedingung_erg.wert;

    KMP_ergebnis_t rumpf_erg = kmp_syn_anweisung_einlesen(speicher, syn);
    if (rumpf_erg.schlecht)
    {
        kmp_syn_melden_glied(speicher, syn, kmp_syn_glied(syn, 0), rumpf_erg.fehler);
    }

    KMP_syn_anweisung_t* rumpf = rumpf_erg.wert;

    KMP_syn_anweisung_t* erg = kmp_syn_anweisung_solange(
        speicher,
        schlüsselwort->spanne,
        bedingung,
        rumpf);

    return erg;
}

KMP_syn_anweisung_t*
kmp_syn_weiche_anweisung_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syn)
{
    KMP_glied_t* schlüsselwort = kmp_syn_weiter(syn, 1);

    KMP_ergebnis_t ausdruck_erg = kmp_syn_ausdruck_einlesen(speicher, syn);
    if (ausdruck_erg.schlecht)
    {
        kmp_syn_melden_glied(speicher, syn, kmp_syn_glied(syn, 0), ausdruck_erg.fehler);
    }

    KMP_syn_ausdruck_t* ausdruck = ausdruck_erg.wert;

    BSS_Feld(KMP_syn_muster_t*) muster = bss_feld(speicher, sizeof(KMP_syn_muster_t*));
    if (kmp_syn_passt(syn, GLIED_GKLAMMER_AUF))
    {
        for (;;)
        {
            ausdruck_erg = kmp_syn_ausdruck_einlesen(speicher, syn);

            if (ausdruck_erg.schlecht)
            {
                break;
            }

            KMP_syn_ausdruck_t* muster_ausdruck = ausdruck_erg.wert;

            kmp_syn_erwarte(syn, GLIED_FOLGERUNG);

            KMP_ergebnis_t anweisung_erg = kmp_syn_anweisung_einlesen(speicher, syn);

            if (anweisung_erg.schlecht)
            {
                kmp_syn_melden_glied(speicher, syn, kmp_syn_glied(syn, 0), anweisung_erg.fehler);
            }

            KMP_syn_anweisung_t* anweisung = anweisung_erg.wert;
            KMP_syn_muster_t* m = kmp_syn_muster(
                speicher,
                kmp_spanne_bereich(muster_ausdruck->basis->spanne, anweisung->basis->spanne),
                muster_ausdruck, anweisung);
            bss_feld_hinzufügen(speicher, &muster, m);

            if (!kmp_syn_passt(syn, GLIED_PUNKT))
            {
                break;
            }
        }

        kmp_syn_erwarte(syn, GLIED_GKLAMMER_ZU);
    }

    KMP_syn_anweisung_t* erg = kmp_syn_anweisung_weiche(
        speicher,
        kmp_spanne(), ausdruck, muster);

    return erg;
}

KMP_syn_anweisung_t*
kmp_syn_final_anweisung_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syn)
{
    KMP_glied_t* schlüsselwort = kmp_syn_weiter(syn, 1);

    KMP_ergebnis_t anweisung_erg = kmp_syn_anweisung_einlesen(speicher, syn);
    if (anweisung_erg.schlecht)
    {
        assert(!"meldung erstatten");
        return NULL;
    }

    KMP_syn_anweisung_t* anweisung = anweisung_erg.wert;
    KMP_syn_anweisung_t* erg = kmp_syn_anweisung_final(
        speicher,
        kmp_spanne_bereich(schlüsselwort->spanne, anweisung->basis->spanne),
        anweisung);

    return erg;
}

KMP_syn_anweisung_t*
kmp_syn_res_anweisung_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syn)
{
    KMP_glied_t* schlüsselwort = kmp_syn_weiter(syn, 1);

    KMP_ergebnis_t ausdruck_erg = kmp_syn_ausdruck_einlesen(speicher, syn);
    if (ausdruck_erg.schlecht)
    {
        assert(!"meldung erstatten");
        return NULL;
    }

    KMP_syn_ausdruck_t* ausdruck = ausdruck_erg.wert;
    KMP_syn_anweisung_t* erg = kmp_syn_anweisung_res(
        speicher,
        kmp_spanne_bereich(schlüsselwort->spanne, ausdruck->basis->spanne),
        ausdruck);

    return erg;
}

KMP_syn_anweisung_t*
kmp_syn_weiter_anweisung_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syn)
{
    KMP_glied_t* schlüsselwort = kmp_syn_weiter(syn, 1);

    KMP_syn_anweisung_t* erg = kmp_syn_anweisung_weiter(speicher, schlüsselwort->spanne);

    return erg;
}

KMP_syn_anweisung_t*
kmp_syn_sprung_anweisung_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syn)
{
    KMP_glied_t* schlüsselwort = kmp_syn_weiter(syn, 1);

    KMP_ergebnis_t ausdruck_erg = kmp_syn_ausdruck_einlesen(speicher, syn);
    if (ausdruck_erg.schlecht)
    {
        kmp_syn_melden_glied(speicher, syn, kmp_syn_glied(syn, 0), ausdruck_erg.fehler);
    }

    KMP_syn_ausdruck_t* ausdruck = ausdruck_erg.wert;
    if (ausdruck->basis->art != KMP_SYN_KNOTEN_AUSDRUCK_BEZEICHNER)
    {
        kmp_syn_melden_knoten(speicher, syn, ausdruck->basis, kmp_fehler(speicher, bss_text("Bezeichner erwartet.")));
    }

    KMP_spanne_t spanne = kmp_spanne_bereich(schlüsselwort->spanne, ausdruck->basis->spanne);
    KMP_syn_anweisung_t* erg = kmp_syn_anweisung_sprung(
        speicher,
        spanne,
        ausdruck);

    return erg;
}

KMP_syn_anweisung_t*
kmp_syn_raus_anweisung_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syn)
{
    KMP_glied_t* schlüsselwort = kmp_syn_weiter(syn, 1);

    KMP_syn_anweisung_t* erg = kmp_syn_anweisung_raus(
        speicher,
        schlüsselwort->spanne);

    return erg;
}
// }}}
// deklaration {{{
KMP_syn_deklaration_t*
kmp_syn_deklaration_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syn, w32 mit_abschluss)
{
    KMP_glied_t* anfang = kmp_syn_glied(syn, 0);
    BSS_Feld(BSS_text_t) namen = bss_feld(speicher, sizeof(BSS_text_t));

    for (;;)
    {
        KMP_ergebnis_t ausdruck_erg = kmp_syn_ausdruck_einlesen(speicher, syn);
        if (ausdruck_erg.schlecht)
        {
            break;
        }

        KMP_syn_ausdruck_t* ausdruck = ausdruck_erg.wert;
        if (ausdruck->basis->art != KMP_SYN_KNOTEN_AUSDRUCK_BEZEICHNER)
        {
            break;
        }

        bss_feld_hinzufügen(speicher, &namen, &ausdruck->bezeichner.wert);

        if (!kmp_syn_passt(syn, GLIED_KOMMA))
        {
            break;
        }
    }

    if (namen.anzahl_elemente == 0)
    {
        return NULL;
    }

    kmp_syn_erwarte(syn, GLIED_BALKEN);
    KMP_syn_spezifizierung_t* spezifizierung = kmp_syn_spezifizierung_einlesen(speicher, syn);

    if (kmp_syn_passt(syn, GLIED_ZUWEISUNG))
    {
        KMP_ergebnis_t ausdruck_erg = kmp_syn_ausdruck_einlesen(speicher, syn);

        if (ausdruck_erg.schlecht)
        {
            assert(!"meldung erstatten");
            return NULL;
        }

        KMP_syn_ausdruck_t* ausdruck = ausdruck_erg.wert;

        if (mit_abschluss)
        {
            kmp_syn_erwarte(syn, GLIED_PUNKT);
        }

        KMP_spanne_t spanne = kmp_spanne_bereich(anfang->spanne, ausdruck->basis->spanne);
        KMP_syn_deklaration_t* erg = kmp_syn_deklaration_variable(
            speicher,
            spanne,
            namen,
            spezifizierung,
            ausdruck);
    }

    else if (kmp_syn_passt(syn, GLIED_BALKEN))
    {
        if (kmp_syn_ist(syn, GLIED_RKLAMMER_AUF, 0))
        {
            KMP_ergebnis_t ausdruck_erg = kmp_syn_basis_ausdruck_einlesen(speicher, syn);
            if (ausdruck_erg.schlecht)
            {
                assert(!"schlecht");
            }

            KMP_syn_ausdruck_t* ausdruck = ausdruck_erg.wert;
            if (ausdruck->basis->art == KMP_SYN_KNOTEN_AUSDRUCK_ROUTINE)
            {
                if (ausdruck->routine.rumpf == NULL)
                {
                    kmp_syn_erwarte(syn, GLIED_PUNKT);
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
                assert(0);
                return NULL;
            }
        }
        else if (kmp_syn_ist(syn, GLIED_BEZEICHNER, 0) &&
                 bss_text_ist_gleich(kmp_syn_glied(syn, 0)->text, bss_text(SCHLÜSSELWORT_OPTION)) == 0)
        {
            KMP_ergebnis_t ausdruck_erg = kmp_syn_basis_ausdruck_einlesen(speicher, syn);
            if (ausdruck_erg.schlecht)
            {
                assert(!"fehler");
                return NULL;
            }

            KMP_syn_ausdruck_t* ausdruck = ausdruck_erg.wert;
            KMP_spanne_t spanne = kmp_spanne_bereich(anfang->spanne, ausdruck->basis->spanne);
            KMP_syn_deklaration_t* erg = kmp_syn_deklaration_option(
                speicher,
                spanne,
                namen,
                ausdruck);

            return erg;
        }

        else if (kmp_syn_ist(syn, GLIED_BEZEICHNER, 0) &&
                 bss_text_ist_gleich(kmp_syn_glied(syn, 0)->text, bss_text(SCHLÜSSELWORT_SCHABLONE)) == 0)
        {
            KMP_glied_t* schlüsselwort = kmp_syn_weiter(syn, 1);
            kmp_syn_erwarte(syn, GLIED_GKLAMMER_AUF);
            BSS_Feld(KMP_syn_deklaration_t*) eigenschaften = bss_feld(speicher, sizeof(KMP_syn_deklaration_t*));

            for (;;)
            {
                KMP_syn_deklaration_t* eigenschaft = kmp_syn_deklaration_einlesen(speicher, syn, true);

                if (eigenschaft == NULL)
                {
                    break;
                }

                bss_feld_hinzufügen(speicher, &eigenschaften, eigenschaft);
            }

            KMP_glied_t* klammer_zu = kmp_syn_erwarte(syn, GLIED_GKLAMMER_ZU);

            KMP_spanne_t spanne = kmp_spanne_bereich(anfang->spanne, klammer_zu->spanne);
            KMP_syn_deklaration_t* erg = kmp_syn_deklaration_schablone(
                speicher,
                spanne,
                namen,
                eigenschaften);

            return erg;
        }

        else if (kmp_syn_ist(syn, GLIED_RAUTE, 0))
        {
            KMP_ergebnis_t ausdruck_erg = kmp_syn_basis_ausdruck_einlesen(speicher, syn);
            if (ausdruck_erg.schlecht)
            {
                assert(!"meldung erstatten");
                return NULL;
            }

            KMP_syn_ausdruck_t* ausdruck = ausdruck_erg.wert;
            if (ausdruck->basis->art != KMP_SYN_KNOTEN_AUSDRUCK_BRAUCHE)
            {
                assert(!"meldung erstatten");
                return NULL;
            }

            KMP_syn_deklaration_t* erg = kmp_syn_deklaration_brauche(
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
        kmp_syn_erwarte(syn, GLIED_PUNKT);
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
KMP_ergebnis_t
kmp_syn_ausdruck_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syn)
{
    KMP_ergebnis_t erg = kmp_syn_reihe_ausdruck_einlesen(speicher, syn);

    return erg;
}

KMP_ergebnis_t
kmp_syn_reihe_ausdruck_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syn)
{
    KMP_ergebnis_t links_erg = kmp_syn_vergleich_ausdruck_einlesen(speicher, syn);

    if (kmp_syn_passt(syn, GLIED_PUNKT_PUNKT))
    {
        KMP_ergebnis_t max_erg = kmp_syn_vergleich_ausdruck_einlesen(speicher, syn);

        if (max_erg.schlecht)
        {
            assert(!"meldung erstatten");
            return max_erg;
        }

        KMP_syn_ausdruck_t* max = max_erg.wert;
        KMP_syn_ausdruck_t* links = links_erg.wert;
        KMP_spanne_t spanne = kmp_spanne_bereich(links->basis->spanne, max->basis->spanne);
        links = kmp_syn_ausdruck_reihe(speicher, spanne, links, max);

        links_erg = kmp_ergebnis_erfolg(links);
    }

    return links_erg;
}

KMP_ergebnis_t
kmp_syn_vergleich_ausdruck_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syn)
{
    KMP_ergebnis_t links_erg = kmp_syn_bitschieben_ausdruck_einlesen(speicher, syn);

    if (kmp_syn_glied(syn, 0)->art >= GLIED_KLEINER && kmp_syn_glied(syn, 0)->art <= GLIED_GRÖẞER)
    {
        KMP_glied_t* op = kmp_syn_weiter(syn, 1);
        KMP_ergebnis_t rechts_erg = kmp_syn_bitschieben_ausdruck_einlesen(speicher, syn);

        if (rechts_erg.schlecht)
        {
            assert(!"meldung erstatten");
            return rechts_erg;
        }

        KMP_syn_ausdruck_t* links = links_erg.wert;
        KMP_syn_ausdruck_t* rechts = rechts_erg.wert;
        KMP_spanne_t spanne = kmp_spanne_bereich(links->basis->spanne, rechts->basis->spanne);
        links = kmp_syn_ausdruck_binär(
            speicher,
            spanne,
            kmp_syn_glied_zu_binär_op(op),
            links,
            rechts
        );

        links_erg = kmp_ergebnis_erfolg(links);
    }

    return links_erg;
}

KMP_ergebnis_t
kmp_syn_bitschieben_ausdruck_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syn)
{
    KMP_ergebnis_t links_erg = kmp_syn_add_ausdruck_einlesen(speicher, syn);

    if (kmp_syn_ist(syn, GLIED_DREIECK_LINKS, 0) || kmp_syn_ist(syn, GLIED_DREIECK_RECHTS, 0))
    {
        KMP_glied_t* op = kmp_syn_weiter(syn, 1);

        KMP_ergebnis_t rechts_erg = kmp_syn_add_ausdruck_einlesen(speicher, syn);
        if (rechts_erg.schlecht)
        {
            return rechts_erg;
        }

        KMP_syn_ausdruck_t* links = links_erg.wert;
        KMP_syn_ausdruck_t* rechts = rechts_erg.wert;
        KMP_spanne_t spanne = kmp_spanne_bereich(links->basis->spanne, rechts->basis->spanne);
        links = kmp_syn_ausdruck_binär(
            speicher,
            spanne,
            kmp_syn_glied_zu_binär_op(op),
            links,
            rechts
        );

        links_erg = kmp_ergebnis_erfolg(links);
    }

    return links_erg;
}

KMP_ergebnis_t
kmp_syn_add_ausdruck_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syn)
{
    KMP_ergebnis_t links_erg = kmp_syn_mult_ausdruck_einlesen(speicher, syn);

    while (kmp_syn_glied(syn, 0)->art == GLIED_PLUS || kmp_syn_glied(syn, 0)->art == GLIED_MINUS)
    {
        KMP_glied_t* op = kmp_syn_weiter(syn, 1);

        KMP_ergebnis_t rechts_erg = kmp_syn_mult_ausdruck_einlesen(speicher, syn);
        if (rechts_erg.schlecht)
        {
            return rechts_erg;
        }

        KMP_syn_ausdruck_t* links = links_erg.wert;
        KMP_syn_ausdruck_t* rechts = rechts_erg.wert;
        KMP_spanne_t spanne = kmp_spanne_bereich(links->basis->spanne, rechts->basis->spanne);
        links = kmp_syn_ausdruck_binär(
            speicher,
            spanne,
            kmp_syn_glied_zu_binär_op(op),
            links,
            rechts
        );

        links_erg = kmp_ergebnis_erfolg(links);
    }

    return links_erg;
}

KMP_ergebnis_t
kmp_syn_mult_ausdruck_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syn)
{
    KMP_ergebnis_t links_erg = kmp_syn_aufruf_ausdruck_einlesen(speicher, syn);

    while (kmp_syn_glied(syn, 0)->art == GLIED_STERN       ||
           kmp_syn_glied(syn, 0)->art == GLIED_DOPPELPUNKT ||
           kmp_syn_glied(syn, 0)->art == GLIED_PROZENT)
    {
        KMP_glied_t* op = kmp_syn_weiter(syn, 1);

        KMP_ergebnis_t rechts_erg = kmp_syn_aufruf_ausdruck_einlesen(speicher, syn);
        if (rechts_erg.schlecht)
        {
            return rechts_erg;
        }

        KMP_syn_ausdruck_t* links = links_erg.wert;
        KMP_syn_ausdruck_t* rechts = rechts_erg.wert;
        KMP_spanne_t spanne = kmp_spanne_bereich(links->basis->spanne, rechts->basis->spanne);
        links = kmp_syn_ausdruck_binär(
            speicher,
            spanne,
            kmp_syn_glied_zu_binär_op(op),
            links,
            rechts
        );

        links_erg = kmp_ergebnis_erfolg(links);
    }

    return links_erg;
}

KMP_ergebnis_t
kmp_syn_aufruf_ausdruck_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syn)
{
    KMP_ergebnis_t links_erg = kmp_syn_index_ausdruck_einlesen(speicher, syn);

    if (kmp_syn_glied(syn, 0)->art == GLIED_RKLAMMER_AUF)
    {
        KMP_glied_t* klammer_auf = kmp_syn_weiter(syn, 1);

        BSS_Feld(KMP_syn_ausdruck_t*) argumente = bss_feld(speicher, sizeof(KMP_syn_ausdruck_t*));
        if (kmp_syn_glied(syn, 0)->art != GLIED_RKLAMMER_ZU)
        {
            KMP_ergebnis_t argument_erg = kmp_syn_ausdruck_einlesen(speicher, syn);
            while (argument_erg.gut)
            {
                KMP_syn_ausdruck_t* argument = argument_erg.wert;
                bss_feld_hinzufügen(speicher, &argumente, argument);

                if (!kmp_syn_passt(syn, GLIED_STRICHPUNKT))
                {
                    break;
                }

                argument_erg = kmp_syn_ausdruck_einlesen(speicher, syn);
            }
        }

        KMP_glied_t* klammer_zu = kmp_syn_erwarte(syn, GLIED_RKLAMMER_ZU);

        KMP_syn_ausdruck_t* links = links_erg.wert;
        KMP_spanne_t spanne = kmp_spanne_bereich(links->basis->spanne, klammer_zu->spanne);
        links = kmp_syn_ausdruck_aufruf(
            speicher,
            spanne,
            links,
            argumente
        );

        links_erg = kmp_ergebnis_erfolg(links);
    }

    return links_erg;
}

KMP_ergebnis_t
kmp_syn_index_ausdruck_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syn)
{
    KMP_ergebnis_t links_erg = kmp_syn_eigenschaft_ausdruck_einlesen(speicher, syn);

    while (kmp_syn_glied(syn, 0)->art == GLIED_EKLAMMER_AUF)
    {
        KMP_glied_t* klammer_auf = kmp_syn_weiter(syn, 1);

        KMP_ergebnis_t index_erg = kmp_syn_ausdruck_einlesen(speicher, syn);
        if (index_erg.schlecht)
        {
            return index_erg;
        }

        KMP_syn_ausdruck_t* index = index_erg.wert;
        KMP_glied_t* klammer_zu = kmp_syn_erwarte(syn, GLIED_EKLAMMER_ZU);

        KMP_syn_ausdruck_t* links = links_erg.wert;
        KMP_spanne_t spanne = kmp_spanne_bereich(links->basis->spanne, klammer_zu->spanne);
        links = kmp_syn_ausdruck_index(
            speicher,
            spanne,
            links,
            index
        );

        links_erg = kmp_ergebnis_erfolg(links);
    }

    return links_erg;
}

KMP_ergebnis_t
kmp_syn_eigenschaft_ausdruck_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syn)
{
    KMP_ergebnis_t links_erg = kmp_syn_basis_ausdruck_einlesen(speicher, syn);

    while (kmp_syn_glied(syn, 0)->art == GLIED_PISA)
    {
        KMP_glied_t* punkt = kmp_syn_weiter(syn, 1);

        KMP_ergebnis_t feld_erg = kmp_syn_basis_ausdruck_einlesen(speicher, syn);
        if (feld_erg.schlecht)
        {
            return feld_erg;
        }

        KMP_syn_ausdruck_t* feld = feld_erg.wert;
        if (feld->basis->art != KMP_SYN_KNOTEN_AUSDRUCK_BEZEICHNER)
        {
            assert(!"meldung erstetten");
        }

        KMP_syn_ausdruck_t* links = links_erg.wert;
        KMP_spanne_t spanne = kmp_spanne_bereich(links->basis->spanne, feld->basis->spanne);
        links = kmp_syn_ausdruck_eigenschaft(
            speicher,
            spanne,
            links,
            feld
        );

        links_erg = kmp_ergebnis_erfolg(links);
    }

    return links_erg;
}

KMP_ergebnis_t
kmp_syn_basis_ausdruck_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syn)
{
    KMP_glied_t* t = kmp_syn_glied(syn, 0);

    switch (t->art)
    {
        case GLIED_MINUS:
        case GLIED_TILDE:
        case GLIED_AUSRUFEZEICHEN:
        {
            KMP_glied_t* op = kmp_syn_weiter(syn, 1);
            KMP_ergebnis_t ausdruck_erg = kmp_syn_basis_ausdruck_einlesen(speicher, syn);

            if (ausdruck_erg.schlecht)
            {
                return ausdruck_erg;
            }

            KMP_syn_ausdruck_t* ausdruck = ausdruck_erg.wert;
            KMP_spanne_t spanne = kmp_spanne_bereich(op->spanne, ausdruck->basis->spanne);
            KMP_syn_ausdruck_t* erg = kmp_syn_ausdruck_unär(
                speicher,
                spanne,
                kmp_syn_glied_zu_unär_op(op),
                ausdruck
            );

            return kmp_ergebnis_erfolg(erg);
        } break;

        case GLIED_RAUTE:
        {
            KMP_glied_t* raute = kmp_syn_weiter(syn, 1);
            if (kmp_syn_glied(syn, 0)->art == GLIED_BEZEICHNER &&
                bss_text_ist_gleich(kmp_syn_glied(syn, 0)->text, bss_text(SCHLÜSSELWORT_BRAUCHE)) == 0)
            {
                kmp_syn_weiter(syn, 1);

                KMP_ergebnis_t dateiname_erg = kmp_syn_basis_ausdruck_einlesen(speicher, syn);
                if (dateiname_erg.schlecht)
                {
                    assert(!"meldung erstatten");
                    return kmp_ergebnis_fehler(kmp_fehler(speicher, bss_text("FEHLER")));
                }

                KMP_syn_ausdruck_t* dateiname = dateiname_erg.wert;
                if (dateiname->basis->art != KMP_SYN_KNOTEN_AUSDRUCK_TEXT)
                {
                    assert(!"meldung erstatten");
                }

                KMP_spanne_t spanne = kmp_spanne_bereich(raute->spanne, dateiname->basis->spanne);
                KMP_syn_ausdruck_t* erg = kmp_syn_ausdruck_brauche(
                    speicher,
                    spanne,
                    dateiname->text.wert
                );

                return kmp_ergebnis_erfolg(erg);
            }

            else if (kmp_syn_glied(syn, 0)->art == GLIED_AUSRUFEZEICHEN)
            {
                kmp_syn_weiter(syn, 1);

                KMP_ergebnis_t ausdruck_erg = kmp_syn_ausdruck_einlesen(speicher, syn);
                if (ausdruck_erg.schlecht)
                {
                    assert(!"meldung erstatten");
                    return ausdruck_erg;
                }

                KMP_syn_ausdruck_t* ausdruck = ausdruck_erg.wert;
                KMP_spanne_t spanne = kmp_spanne_bereich(raute->spanne, ausdruck->basis->spanne);
                KMP_syn_ausdruck_t* erg = kmp_syn_ausdruck_ausführen(
                    speicher,
                    spanne,
                    ausdruck
                );

                return kmp_ergebnis_erfolg(erg);
            }
        } break;

        case GLIED_GANZZAHL:
        {
            kmp_syn_weiter(syn, 1);
            KMP_syn_ausdruck_t* erg = kmp_syn_ausdruck_ganzzahl(
                speicher,
                t->spanne,
                t->gz.wert);

            return kmp_ergebnis_erfolg(erg);
        } break;

        case GLIED_DEZIMALZAHL:
        {
            kmp_syn_weiter(syn, 1);
            KMP_syn_ausdruck_t* erg = kmp_syn_ausdruck_dezimalzahl(
                speicher,
                t->spanne,
                t->dz.wert);

            return kmp_ergebnis_erfolg(erg);
        } break;

        case GLIED_BEZEICHNER:
        {
            if (bss_text_ist_gleich(t->text, bss_text(SCHLÜSSELWORT_SCHABLONE)) == 0 &&
                kmp_syn_ist(syn, GLIED_GKLAMMER_AUF, 1))
            {
                KMP_glied_t* anfang = kmp_syn_weiter(syn, 1);
                KMP_glied_t* klammer_auf = kmp_syn_erwarte(syn, GLIED_GKLAMMER_AUF);

                BSS_Feld(KMP_syn_deklaration_t*) eigenschaften = bss_feld(speicher, sizeof(KMP_syn_deklaration_t*));
                for (;;)
                {
                    KMP_syn_deklaration_t* eigenschaft = kmp_syn_deklaration_einlesen(speicher, syn, false);
                    if (!eigenschaft)
                    {
                        break;
                    }

                    bss_feld_hinzufügen(speicher, &eigenschaften, eigenschaft);

                    if (!kmp_syn_passt(syn, GLIED_PUNKT))
                    {
                        break;
                    }
                }

                KMP_glied_t* klammer_zu = kmp_syn_erwarte(syn, GLIED_GKLAMMER_ZU);

                KMP_spanne_t spanne = kmp_spanne_bereich(anfang->spanne, klammer_zu->spanne);
                KMP_syn_ausdruck_t* erg = kmp_syn_ausdruck_schablone(
                    speicher,
                    spanne,
                    eigenschaften
                );

                return kmp_ergebnis_erfolg(erg);
            }

            else if (bss_text_ist_gleich(t->text, bss_text(SCHLÜSSELWORT_OPTION)) == 0 && kmp_syn_ist(syn, GLIED_GKLAMMER_AUF, 1))
            {
                KMP_glied_t* anfang = kmp_syn_weiter(syn, 1);
                KMP_glied_t* klammer_auf = kmp_syn_erwarte(syn, GLIED_GKLAMMER_AUF);
                BSS_Feld(KMP_syn_deklaration_t*) eigenschaften = bss_feld(speicher, sizeof(KMP_syn_deklaration_t*));
                BSS_Feld(BSS_text_t) namen = bss_feld(speicher, sizeof(BSS_text_t));

                for (;;)
                {
                    KMP_ergebnis_t name_erg = kmp_syn_basis_ausdruck_einlesen(speicher, syn);
                    if (name_erg.schlecht)
                    {
                        break;
                    }

                    KMP_syn_ausdruck_t* name = name_erg.wert;
                    if (name->basis->art != KMP_SYN_KNOTEN_AUSDRUCK_BEZEICHNER)
                    {
                        break;
                    }

                    bss_feld_hinzufügen(speicher, &namen, &name->bezeichner.wert);

                    KMP_syn_spezifizierung_t* spezifizierung = NULL;
                    if (kmp_syn_passt(syn, GLIED_BALKEN) && !kmp_syn_ist(syn, GLIED_ZUWEISUNG, 0))
                    {
                        spezifizierung = kmp_syn_spezifizierung_einlesen(speicher, syn);
                    }

                    KMP_syn_ausdruck_t* init = NULL;
                    if (kmp_syn_passt(syn, GLIED_ZUWEISUNG))
                    {
                        KMP_ergebnis_t erg = kmp_syn_ausdruck_einlesen(speicher, syn);
                        if (erg.schlecht)
                        {
                            return erg;
                        }

                        init = erg.wert;
                    }

                    KMP_syn_deklaration_t* eigenschaft = kmp_syn_deklaration_variable(
                        speicher,
                        name->basis->spanne,
                        namen,
                        spezifizierung,
                        init
                    );

                    bss_feld_hinzufügen(speicher, &eigenschaften, eigenschaft);

                    if (!kmp_syn_passt(syn, GLIED_PUNKT))
                    {
                        break;
                    }
                }

                KMP_glied_t* klammer_zu = kmp_syn_erwarte(syn, GLIED_GKLAMMER_ZU);

                KMP_spanne_t spanne = kmp_spanne_bereich(anfang->spanne, klammer_zu->spanne);
                KMP_syn_ausdruck_t* erg = kmp_syn_ausdruck_option(
                    speicher,
                    spanne,
                    eigenschaften
                );

                return kmp_ergebnis_erfolg(erg);
            }
            else
            {
                kmp_syn_weiter(syn, 1);
                KMP_syn_ausdruck_t* erg = kmp_syn_ausdruck_bezeichner(speicher, t->spanne, t->text);

                return kmp_ergebnis_erfolg(erg);
            }
        } break;

        case GLIED_TEXT:
        {
            kmp_syn_weiter(syn, 1);
            KMP_syn_ausdruck_t* erg = kmp_syn_ausdruck_text(speicher, t->spanne, t->text);

            return kmp_ergebnis_erfolg(erg);
        } break;

        case GLIED_RKLAMMER_AUF:
        {
            KMP_glied_t* klammer_auf = kmp_syn_weiter(syn, 1);

            BSS_Feld(KMP_syn_deklaration_t*) parameter = bss_feld(speicher, sizeof(KMP_syn_deklaration_t*));
            bool könnte_funktion_sein = true;
            g32 vorheriger_index = syn->glied_index;

            if (kmp_syn_glied(syn, 0)->art != GLIED_RKLAMMER_ZU)
            {
                for (;;)
                {
                    KMP_syn_deklaration_t* param = kmp_syn_deklaration_einlesen(speicher, syn, false);

                    if (!param)
                    {
                        könnte_funktion_sein = false;
                        break;
                    }

                    bss_feld_hinzufügen(speicher, &parameter, param);

                    if (!kmp_syn_passt(syn, GLIED_STRICHPUNKT))
                    {
                        break;
                    }
                }
            }

            if (könnte_funktion_sein && kmp_syn_glied(syn, 0)->art == GLIED_RKLAMMER_ZU)
            {
                KMP_glied_t* klammer_zu = kmp_syn_weiter(syn, 1);

                KMP_syn_spezifizierung_t* rückgabe = NULL;
                if (kmp_syn_passt(syn, GLIED_PFEIL))
                {
                    rückgabe = kmp_syn_spezifizierung_einlesen(speicher, syn);
                    if (rückgabe == NULL)
                    {
                        assert(!"meldung erstatten");
                    }
                }

                if (kmp_syn_glied(syn, 0)->art != GLIED_GKLAMMER_AUF)
                {
                    KMP_spanne_t spanne = kmp_spanne_bereich(klammer_auf->spanne, klammer_zu->spanne);
                    KMP_syn_ausdruck_t* erg = kmp_syn_ausdruck_routine(
                        speicher,
                        spanne,
                        parameter,
                        rückgabe,
                        NULL
                    );

                    return kmp_ergebnis_erfolg(erg);
                }

                KMP_syn_anweisung_t* rumpf = NULL;
                if (kmp_syn_glied(syn, 0)->art == GLIED_GKLAMMER_AUF)
                {
                    KMP_ergebnis_t rumpf_erg = kmp_syn_anweisung_einlesen(speicher, syn);
                    if (rumpf_erg.gut)
                    {
                        rumpf = rumpf_erg.wert;
                    }
                    else
                    {
                        return rumpf_erg;
                    }
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

                    return kmp_ergebnis_erfolg(erg);
                }
            }

            syn->glied_index = vorheriger_index;
            KMP_ergebnis_t ausdruck_erg = kmp_syn_ausdruck_einlesen(speicher, syn);
            if (ausdruck_erg.schlecht)
            {
                return ausdruck_erg;
            }

            KMP_syn_ausdruck_t* ausdruck = ausdruck_erg.wert;

            KMP_glied_t* klammer_zu = kmp_syn_erwarte(syn, GLIED_RKLAMMER_ZU);
            if (!klammer_zu)
            {
                return kmp_ergebnis_fehler(kmp_fehler(speicher, bss_text("Erwartete schließende Klammer ')'")));
            }

            KMP_spanne_t spanne = kmp_spanne_bereich(klammer_auf->spanne, klammer_zu->spanne);
            KMP_syn_ausdruck_t* erg = kmp_syn_ausdruck_klammer(
                speicher,
                spanne,
                ausdruck
            );

            return kmp_ergebnis_erfolg(erg);
        } break;

        case GLIED_GKLAMMER_AUF:
        {
            KMP_glied_t* klammer_auf = kmp_syn_weiter(syn, 1);
            w32 ist_benamt = false;

            BSS_Feld(KMP_syn_ausdruck_kompositum_eigenschaft_t*) eigenschaften = bss_feld(speicher, sizeof(KMP_syn_ausdruck_kompositum_eigenschaft_t*));
            if (!kmp_syn_ist(syn, GLIED_GKLAMMER_ZU, 0))
            {
                for (;;)
                {
                    if (kmp_syn_passt(syn, GLIED_PISA))
                    {
                        ist_benamt = true;

                        KMP_ergebnis_t a_erg = kmp_syn_basis_ausdruck_einlesen(speicher, syn);
                        if (a_erg.schlecht)
                        {
                            return a_erg;
                        }

                        KMP_syn_ausdruck_t* a = a_erg.wert;
                        kmp_syn_erwarte(syn, GLIED_ZUWEISUNG);

                        KMP_ergebnis_t ausdruck_erg = kmp_syn_ausdruck_einlesen(speicher, syn);
                        if (ausdruck_erg.schlecht)
                        {
                            return ausdruck_erg;
                        }

                        KMP_syn_ausdruck_t* ausdruck = ausdruck_erg.wert;
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

                        KMP_ergebnis_t ausdruck_erg = kmp_syn_ausdruck_einlesen(speicher, syn);
                        if (ausdruck_erg.schlecht)
                        {
                            break;
                        }

                        KMP_syn_ausdruck_t* ausdruck = ausdruck_erg.wert;
                        KMP_syn_ausdruck_kompositum_eigenschaft_t* eigenschaft = kmp_syn_ausdruck_kompositum_eigenschaft(
                            speicher,
                            ausdruck->basis->spanne,
                            bss_text(""),
                            ausdruck,
                            ist_benamt
                        );
                        bss_feld_hinzufügen(speicher, &eigenschaften, eigenschaft);
                    }

                    if (!kmp_syn_passt(syn, GLIED_STRICHPUNKT))
                    {
                        break;
                    }
                }
            }

            KMP_glied_t* klammer_zu = kmp_syn_erwarte(syn, GLIED_GKLAMMER_ZU);

            KMP_syn_spezifizierung_t* spezifizierung = NULL;
            if (kmp_syn_passt(syn, GLIED_KLEINER))
            {
                spezifizierung = kmp_syn_spezifizierung_einlesen(speicher, syn);
                kmp_syn_erwarte(syn, GLIED_GRÖẞER);
            }

            KMP_spanne_t spanne = kmp_spanne_bereich(klammer_auf->spanne, klammer_zu->spanne);
            KMP_syn_ausdruck_t* erg = kmp_syn_ausdruck_kompositum(
                speicher,
                spanne,
                eigenschaften,
                ist_benamt,
                spezifizierung);

            return kmp_ergebnis_erfolg(erg);
        } break;
    }

    return kmp_ergebnis_fehler(kmp_fehler(speicher, bss_text("Konnte keinen Ausdruck einlesen.")));
}
// }}}
// spezifizierung {{{
KMP_syn_spezifizierung_t*
kmp_syn_spezifizierung_einlesen(BSS_speicher_t* speicher, KMP_syntax_t* syn)
{
    if (kmp_syn_glied(syn, 0)->art == GLIED_STERN)
    {
        KMP_glied_t* stern = kmp_syn_weiter(syn, 1);
        KMP_syn_spezifizierung_t* basis = kmp_syn_spezifizierung_einlesen(speicher, syn);

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

    else if (kmp_syn_glied(syn, 0)->art == GLIED_EKLAMMER_AUF)
    {
        KMP_glied_t* klammer_auf = kmp_syn_weiter(syn, 1);

        KMP_ergebnis_t ausdruck_erg = kmp_syn_ausdruck_einlesen(speicher, syn);
        KMP_syn_ausdruck_t* ausdruck = ausdruck_erg.gut ? ausdruck_erg.wert : NULL;
        KMP_glied_t* klammer_zu = kmp_syn_erwarte(syn, GLIED_EKLAMMER_ZU);
        KMP_syn_spezifizierung_t* basis = kmp_syn_spezifizierung_einlesen(speicher, syn);

        KMP_spanne_t spanne = kmp_spanne_bereich(klammer_auf->spanne, basis->basis->spanne);
        KMP_syn_spezifizierung_t* erg = kmp_syn_spezifizierung_feld(
            speicher,
            spanne,
            basis,
            ausdruck
        );

        return erg;
    }

    else if (kmp_syn_glied(syn, 0)->art == GLIED_RKLAMMER_AUF)
    {
        KMP_glied_t* klammer_auf = kmp_syn_weiter(syn, 1);

        BSS_Feld(KMP_syn_spezifizierung_t*) parameter = bss_feld(speicher, sizeof(KMP_syn_spezifizierung_t*));
        if (kmp_syn_glied(syn, 0)->art != GLIED_RKLAMMER_AUF)
        {
            for (;;)
            {
                if (kmp_syn_glied(syn, 0)->art == GLIED_BEZEICHNER && kmp_syn_glied(syn, 1)->art == GLIED_BALKEN)
                {
                    KMP_ergebnis_t bezeichner = kmp_syn_ausdruck_einlesen(speicher, syn);
                    kmp_syn_erwarte(syn, GLIED_BALKEN);
                }

                KMP_syn_spezifizierung_t* spez = kmp_syn_spezifizierung_einlesen(speicher, syn);
                if (spez != NULL)
                {
                    bss_feld_hinzufügen(speicher, &parameter, spez);
                }

                if (!kmp_syn_passt(syn, GLIED_STRICHPUNKT))
                {
                    break;
                }
            }
        }

        KMP_glied_t* klammer_zu = kmp_syn_erwarte(syn, GLIED_RKLAMMER_ZU);
        KMP_zeichen_t bis = klammer_zu->spanne.bis;

        KMP_syn_spezifizierung_t* rückgabe = NULL;
        if (kmp_syn_passt(syn, GLIED_PFEIL))
        {
            rückgabe = kmp_syn_spezifizierung_einlesen(speicher, syn);
            bis = rückgabe->basis->spanne.bis;
        }

        KMP_syn_spezifizierung_t* erg = kmp_syn_spezifizierung_routine(
            speicher,
            kmp_spanne_bereich(klammer_auf->spanne, (KMP_spanne_t){ .bis = bis }),
            parameter,
            rückgabe);

        return erg;
    }

    else if (kmp_syn_glied(syn, 0)->art == GLIED_BEZEICHNER)
    {
        KMP_glied_t* name = kmp_syn_weiter(syn, 1);

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
kmp_syn_erwarte(KMP_syntax_t* syn, g32 art)
{
    if (kmp_syn_glied(syn, 0)->art == art)
    {
        KMP_glied_t* erg = kmp_syn_glied(syn, 0);
        kmp_syn_weiter(syn, 1);

        return erg;
    }

    return NULL;
}

KMP_glied_t*
kmp_syn_glied(KMP_syntax_t* syn, g32 versatz)
{
    g32 index = syn->glied_index + versatz;

    if (syn->glieder.anzahl_elemente <= (g64) index)
    {
        index = (g32) syn->glieder.anzahl_elemente - 1;
    }

    KMP_glied_t* erg = bss_feld_element(syn->glieder, index);

    return erg;
}

w32
kmp_syn_ist(KMP_syntax_t* syn, g32 art, g32 versatz)
{
    w32 erg = kmp_syn_glied(syn, versatz)->art == art;

    return erg;
}

KMP_glied_t*
kmp_syn_weiter(KMP_syntax_t* syn, g32 anzahl)
{
    KMP_glied_t* erg = kmp_syn_glied(syn, 0);
    syn->glied_index += anzahl;

    return erg;
}

w32
kmp_syn_passt(KMP_syntax_t* syn, g32 art)
{
    if (kmp_syn_glied(syn, 0)->art == art)
    {
        kmp_syn_weiter(syn, 1);
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
        case GLIED_PLUS:           return KMP_SYN_AUSDRUCK_BINÄR_ADDITION;
        case GLIED_MINUS:          return KMP_SYN_AUSDRUCK_BINÄR_SUBTRAKTION;
        case GLIED_STERN:          return KMP_SYN_AUSDRUCK_BINÄR_MULTIPLIKATION;
        case GLIED_DOPPELPUNKT:    return KMP_SYN_AUSDRUCK_BINÄR_DIVISION;
        case GLIED_PROZENT:        return KMP_SYN_AUSDRUCK_BINÄR_MODULO;
        case GLIED_KLEINER:        return KMP_SYN_AUSDRUCK_BINÄR_KLEINER;
        case GLIED_KLEINER_GLEICH: return KMP_SYN_AUSDRUCK_BINÄR_KLEINER_GLEICH;
        case GLIED_GLEICH:         return KMP_SYN_AUSDRUCK_BINÄR_GLEICH;
        case GLIED_UNGLEICH:       return KMP_SYN_AUSDRUCK_BINÄR_UNGLEICH;
        case GLIED_GRÖẞER_GLEICH:  return KMP_SYN_AUSDRUCK_BINÄR_GRÖẞER_GLEICH;
        case GLIED_GRÖẞER:         return KMP_SYN_AUSDRUCK_BINÄR_GRÖẞER;
        case GLIED_DREIECK_RECHTS: return KMP_SYN_AUSDRUCK_BINÄR_BIT_VERSATZ_RECHTS;
        case GLIED_DREIECK_LINKS:  return KMP_SYN_AUSDRUCK_BINÄR_BIT_VERSATZ_LINKS;

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
        case GLIED_MINUS:          return KMP_SYN_AUSDRUCK_UNÄR_MINUS;
        case GLIED_AUSRUFEZEICHEN: return KMP_SYN_AUSDRUCK_UNÄR_NEGIERUNG;
        case GLIED_TILDE:          return KMP_SYN_AUSDRUCK_UNÄR_INVERTIERUNG;

        default:
            assert(!"unbekannter op");
            return KMP_SYN_AUSDRUCK_UNÄR_UNBEKANNT;
    }
}

#include "kmp/semantik.h"

#include "basis/paar.h"
#include "basis/feld.h"
#include "basis/stapel.h"
#include "kmp/asb.h"
#include "kmp/datentyp.h"
#include "kmp/diagnostik.h"
#include "kmp/lexer.h"
#include "kmp/operand.h"
#include "kmp/syntax.h"

#include <assert.h>

g32 global_semantik_markierung_index = 0;
// INFO: pair<weiter_markierung, raus_markierung>
BSS_stapel_t global_markierungen;

#define SSS_HAUPTMETHODE "Ausführen"

KMP_semantik_t*
kmp_semantik(BSS_speicher_t* speicher, KMP_syn_asb_t* asb, KMP_sem_zone_t* system /* = NULL */, KMP_sem_zone_t* global /* = NULL */)
{
    KMP_semantik_t* erg = speicher->anfordern(speicher, sizeof(KMP_semantik_t));

    erg->syn_asb = asb;
    erg->system = system;
    erg->global = global;
    erg->zone = erg->global;
    erg->hauptmethode = NULL;

    return erg;
}


KMP_sem_asb_t*
kmp_sem_starten(BSS_speicher_t* speicher, KMP_semantik_t* semantik, w32 mit_hauptmethode)
{
    semantik->sem_asb = kmp_sem_asb(speicher, semantik->system, semantik->global);

    kmp_sem_importe_registrieren(speicher, semantik);
    kmp_sem_deklarationen_registrieren(speicher, semantik);
    kmp_sem_symbole_analysieren(speicher, semantik, mit_hauptmethode);

    if (mit_hauptmethode)
    {
        if (semantik->hauptmethode == NULL)
        {
            KMP_syn_knoten_t* knoten = bss_feld_element(semantik->syn_asb->anweisungen, 0);
            KMP_fehler_t* fehler = kmp_fehler(speicher, bss_text("Die Hauptmethode \"" SSS_HAUPTMETHODE "\" muss im Programm vorhanden sein."));
            kmp_diagnostik_melden_fehler(speicher, semantik->diagnostik, knoten->spanne, fehler);

            return NULL;
        }

        kmp_sem_globale_anweisungen_der_hauptmethode_zuordnen(speicher, semantik);
    }

    return semantik->sem_asb;
}

void
kmp_sem_importe_registrieren(BSS_speicher_t* speicher, KMP_semantik_t* semantik)
{
    // AUFGABE: zirkuläre importe verhindern
    for (g32 i = 0; i < semantik->syn_asb->anweisungen.anzahl_elemente; ++i)
    {
        KMP_syn_anweisung_t* anweisung = bss_feld_element(semantik->syn_asb->anweisungen, i);
        if (anweisung->basis->art == KMP_SYN_KNOTEN_ANWEISUNG_BRAUCHE)
        {
            kmp_sem_import_verarbeiten(speicher, semantik, anweisung->brauche.dateiname);
        }

        else if (anweisung->basis->art == KMP_SYN_KNOTEN_ANWEISUNG_DEKLARATION)
        {
            KMP_syn_deklaration_t* deklaration = anweisung->deklaration.deklaration;
            if (deklaration->basis->art != KMP_SYN_KNOTEN_DEKLARATION_BRAUCHE)
            {
                continue;
            }

            BSS_text_t dateiname = deklaration->brauche.ausdruck->text.wert;
            BSS_text_t name = *(BSS_text_t*) bss_feld_element(deklaration->namen, 0);

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
            KMP_syntax_t* syntax_neu = kmp_syntax(speicher, glieder);
            KMP_syn_asb_t asb = kmp_syn_starten(speicher, syntax_neu, 0);

            KMP_sem_zone_t* global_neu = kmp_sem_zone(speicher, name, semantik->system);
            KMP_semantik_t* semantik_neu = kmp_semantik(speicher, &asb, semantik->system, global_neu);
            KMP_sem_asb_t* brauche_sem_asb = kmp_sem_starten(speicher, semantik_neu, false);
            KMP_sem_symbol_t* symbol = kmp_sem_symbol_zone(speicher, deklaration->basis->spanne, KMP_SYM_MODUL, KMP_SYM_BEHANDELT, name, global_neu);
            deklaration->symbol = symbol;

            if (!kmp_sem_registrieren_symbol(speicher, semantik, semantik->zone, symbol->name, symbol))
            {
                kmp_diagnostik_melden_fehler_bezeichner(speicher, semantik->diagnostik, deklaration->basis->spanne, name);
            }
        }
    }
}

void
kmp_sem_deklarationen_registrieren(BSS_speicher_t* speicher, KMP_semantik_t* semantik)
{
    for (g32 anweisung_idx = 0; anweisung_idx < semantik->syn_asb->anweisungen.anzahl_elemente; ++anweisung_idx)
    {
        KMP_syn_anweisung_t* anweisung = bss_feld_element(semantik->syn_asb->anweisungen, anweisung_idx);
        if (anweisung->basis->art != KMP_SYN_KNOTEN_ANWEISUNG_DEKLARATION)
        {
            continue;
        }

        KMP_syn_deklaration_t* deklaration = anweisung->deklaration.deklaration;
        for (g32 namen_idx = 0; namen_idx < semantik->syn_asb->anweisungen.anzahl_elemente; ++namen_idx)
        {
            BSS_text_t name = *(BSS_text_t*) bss_feld_element(deklaration->namen, namen_idx);
            switch (deklaration->basis->art)
            {
                case KMP_SYN_KNOTEN_DEKLARATION_BRAUCHE:
                {
                    // INFO: deklaration "brauche" wird bereits in der methode importe_registrieren behandelt.
                } break;

                case KMP_SYN_KNOTEN_DEKLARATION_VARIABLE:
                {
                    KMP_sem_symbol_t* symbol = kmp_sem_symbol_datentyp(speicher, deklaration->basis->spanne, KMP_SYM_VARIABLE, KMP_SYM_UNBEHANDELT, name, NULL);
                    deklaration->symbol = symbol;

                    if (!kmp_sem_registrieren_symbol(speicher, semantik, semantik->zone, name, symbol))
                    {
                        kmp_diagnostik_melden_fehler_bezeichner(speicher, semantik->diagnostik, deklaration->basis->spanne, name);
                    }
                } break;

                case KMP_SYN_KNOTEN_DEKLARATION_ROUTINE:
                {
                    BSS_Feld(KMP_sem_datentyp_t*) parameter = bss_feld(speicher, sizeof(KMP_sem_datentyp_t*));
                    KMP_sem_datentyp_t* datentyp = kmp_sem_datentyp_routine(speicher, parameter, NULL, deklaration);
                    KMP_sem_symbol_t* symbol = kmp_sem_symbol_datentyp(
                        speicher,
                        deklaration->basis->spanne,
                        KMP_SYM_ROUTINE, KMP_SYM_UNBEHANDELT,
                        name,
                        datentyp);

                    datentyp->symbol = symbol;
                    datentyp->deklaration = deklaration;
                    deklaration->symbol = symbol;

                    if (!kmp_sem_registrieren_symbol(speicher, semantik, semantik->zone, name, symbol))
                    {
                        kmp_diagnostik_melden_fehler_bezeichner(speicher, semantik->diagnostik, deklaration->basis->spanne, name);
                    }
                } break;

                case KMP_SYN_KNOTEN_DEKLARATION_SCHABLONE:
                {
                    KMP_sem_datentyp_t* datentyp = kmp_sem_datentyp_schablone(speicher, deklaration);
                    KMP_sem_symbol_t* symbol = kmp_sem_symbol_datentyp(
                        speicher,
                        deklaration->basis->spanne,
                        KMP_SYM_DATENTYP, KMP_SYM_BEHANDELT,
                        name,
                        datentyp);

                    datentyp->symbol = symbol;
                    datentyp->deklaration = deklaration;
                    deklaration->symbol = symbol;

                    bss_feld_hinzufügen(speicher, &semantik->schablonen, deklaration);

                    if (!kmp_sem_registrieren_symbol(speicher, semantik, semantik->zone, name, symbol))
                    {
                        kmp_diagnostik_melden_fehler_bezeichner(speicher, semantik->diagnostik, deklaration->basis->spanne, name);
                    }
                } break;

                case KMP_SYN_KNOTEN_DEKLARATION_OPTION:
                {
                    KMP_sem_datentyp_t* datentyp = kmp_sem_datentyp_option(speicher, NULL, deklaration);
                    KMP_sem_symbol_t* symbol = kmp_sem_symbol_datentyp(
                        speicher,
                        deklaration->basis->spanne,
                        KMP_SYM_DATENTYP, KMP_SYM_UNBEHANDELT,
                        name, datentyp);

                    datentyp->symbol = symbol;
                    datentyp->deklaration = deklaration;
                    deklaration->symbol = symbol;

                    if (!kmp_sem_registrieren_symbol(speicher, semantik, semantik->zone, name, symbol))
                    {
                        kmp_diagnostik_melden_fehler_bezeichner(speicher, semantik->diagnostik, deklaration->basis->spanne, name);
                    }
                } break;

                default:
                {
                    kmp_diagnostik_melden_fehler_unbekannte_deklaration(speicher, semantik->diagnostik, deklaration->basis->spanne, deklaration);
                    assert(!"unbekannte deklaration");
                } break;
            }
        }
    }
}

void
kmp_sem_symbole_analysieren(BSS_speicher_t* speicher, KMP_semantik_t* semantik, bool mit_hauptmethode)
{
    for (g32 i = 0; i < semantik->syn_asb->anweisungen.anzahl_elemente; ++i)
    {
        KMP_syn_anweisung_t* anweisung = bss_feld_element(semantik->syn_asb->anweisungen, i);
        if (anweisung->basis->art != KMP_SYN_KNOTEN_ANWEISUNG_DEKLARATION)
        {
            continue;
        }

        KMP_syn_deklaration_t* deklaration = anweisung->deklaration.deklaration;
        KMP_sem_symbol_t* symbol = deklaration->symbol;

        if (mit_hauptmethode)
        {
            if (bss_text_ist_gleich(symbol->name, bss_text(SSS_HAUPTMETHODE)) == 0 &&
                symbol->art == KMP_SYM_ROUTINE)
            {
                semantik->hauptmethode = deklaration->symbol;
            }
        }

        KMP_sem_deklaration_t* sem_deklaration = kmp_sem_symbol_analysieren(speicher, semantik, symbol, deklaration);
        if (sem_deklaration != NULL)
        {
            KMP_sem_anweisung_t* sem_anweisung = kmp_sem_anweisung_deklaration(speicher, anweisung, sem_deklaration);
            bss_feld_hinzufügen(speicher, &semantik->sem_asb->anweisungen, sem_anweisung);
        }
    }
}

KMP_sem_deklaration_t*
kmp_sem_symbol_analysieren(BSS_speicher_t* speicher, KMP_semantik_t* semantik, KMP_sem_symbol_t* symbol, KMP_syn_deklaration_t* deklaration)
{
    if (symbol->status == KMP_SYM_BEHANDELT)
    {
        return NULL;
    }

    if (symbol->status == KMP_SYM_IN_BEHANDLUNG)
    {
        kmp_diagnostik_melden_fehler(speicher, semantik->diagnostik, deklaration->basis->spanne, kmp_fehler(speicher, bss_text("Zirkuläre Abhängigkeit")));
        return NULL;
    }

    symbol->status = KMP_SYM_IN_BEHANDLUNG;

    KMP_sem_deklaration_t* erg = NULL;

    switch (deklaration->basis->art)
    {
        case KMP_SYN_KNOTEN_DEKLARATION_VARIABLE:
        {
            erg = kmp_sem_variable_analysieren(speicher, semantik, symbol, deklaration);
        } break;

        case KMP_SYN_KNOTEN_DEKLARATION_ROUTINE:
        {
            erg = kmp_sem_routine_analysieren(speicher, semantik, symbol, deklaration);
        } break;

        case KMP_SYN_KNOTEN_DEKLARATION_OPTION:
        {
            erg = kmp_sem_option_analysieren(speicher, semantik, symbol, deklaration);
        } break;

        case KMP_SYN_KNOTEN_DEKLARATION_SCHABLONE:
        {
            erg = kmp_sem_schablone_analysieren(speicher, semantik, symbol, deklaration, false);
        } break;

        default:
        {
            kmp_diagnostik_melden_fehler_unbekannte_deklaration(speicher, semantik->diagnostik, deklaration->basis->spanne, deklaration);
        } break;
    }

    symbol->deklaration = erg;
    symbol->status = KMP_SYM_BEHANDELT;

    return erg;
}

void
kmp_sem_globale_anweisungen_der_hauptmethode_zuordnen(BSS_speicher_t*speicher, KMP_semantik_t* semantik)
{
    assert(semantik->hauptmethode);
    KMP_sem_deklaration_t* dekl = semantik->hauptmethode->deklaration;

    BSS_Feld(KMP_sem_anweisung_t*) globale_anweisungen = bss_feld(speicher, sizeof(KMP_sem_anweisung_t*));
    for (g32 i = 0; i < semantik->syn_asb->anweisungen.anzahl_elemente; ++i)
    {
        KMP_syn_anweisung_t* anweisung = bss_feld_element(semantik->syn_asb->anweisungen, i);
        if (anweisung->basis->art == KMP_SYN_KNOTEN_ANWEISUNG_DEKLARATION ||
            anweisung->basis->art == KMP_SYN_KNOTEN_ANWEISUNG_LADE        ||
            anweisung->basis->art == KMP_SYN_KNOTEN_ANWEISUNG_BRAUCHE)
        {
            continue;
        }

        KMP_sem_anweisung_t* sem_anweisung = NULL;
        kmp_sem_anweisung_analysieren(speicher, semantik, anweisung, NULL, &sem_anweisung);
        assert(sem_anweisung);

        bss_feld_hinzufügen(speicher, &globale_anweisungen, sem_anweisung);
    }

    KMP_sem_anweisung_t* globaler_block = kmp_sem_anweisung_block(speicher, NULL, globale_anweisungen);
    // BSS_Feld(KMP_sem_anweisung_t*) block_anweisungen = bss_feld(sizeof(KMP_sem_anweisung_t*));
    // bss_feld_hinzufügen(&block_anweisungen, globaler_block);
    // KMP_sem_anweisung_t* neuer_rumpf = kmp_sem_anweisung_block(speicher, NULL, block_anweisungen);

    dekl->routine.rumpf = globaler_block;
}

KMP_sem_deklaration_t*
kmp_sem_variable_analysieren(BSS_speicher_t* speicher, KMP_semantik_t* semantik, KMP_sem_symbol_t* symbol, KMP_syn_deklaration_t* deklaration)
{
    KMP_sem_datentyp_t* datentyp = NULL;
    KMP_sem_deklaration_t* erg = NULL;
    KMP_sem_ausdruck_t* initialisierung = NULL;

    if (deklaration->spezifizierung != NULL)
    {
        datentyp = kmp_sem_spezifizierung_analysieren(speicher, semantik, deklaration->spezifizierung);
        kmp_sem_datentyp_abschließen(speicher, semantik, datentyp, false);
    }

    if (deklaration->variable.initialisierung != NULL)
    {
        initialisierung = kmp_sem_ausdruck_analysieren(speicher, semantik, deklaration->variable.initialisierung, datentyp);

        if (datentyp == NULL)
        {
            datentyp = initialisierung->operand->datentyp;
        }
        else
        {
            if (!kmp_sem_datentypen_kompatibel(datentyp, initialisierung->operand->datentyp, false))
            {
                KMP_spanne_t spanne = deklaration->variable.initialisierung->basis->spanne;
                kmp_diagnostik_melden_fehler_datentypen_inkompatibel(speicher, semantik->diagnostik, spanne, datentyp, initialisierung->operand->datentyp);
            }
        }
    }

    if (datentyp == NULL)
    {
        KMP_fehler_t* fehler = kmp_fehler(speicher, bss_text("datentyp konnte nicht ermittelt oder abgeleitet werden."));
        kmp_diagnostik_melden_fehler(speicher, semantik->diagnostik, deklaration->basis->spanne, fehler);

        return NULL;
    }

    BSS_Feld(KMP_sem_symbol_t*) symbole = bss_feld(speicher, sizeof(KMP_sem_symbol_t*));
    bss_feld_hinzufügen(speicher, &symbole, symbol);
    erg = kmp_sem_deklaration(speicher, deklaration, deklaration->basis->art, symbole, datentyp, initialisierung);

    return erg;
}

KMP_sem_deklaration_t*
kmp_sem_routine_analysieren(BSS_speicher_t* speicher, KMP_semantik_t* semantik, KMP_sem_symbol_t* symbol, KMP_syn_deklaration_t* deklaration)
{
    KMP_sem_datentyp_t* datentyp = symbol->datentyp;
    KMP_syn_ausdruck_t* routine = deklaration->routine.ausdruck;

    for (g32 i = 0; i < routine->routine.parameter.anzahl_elemente; ++i)
    {
        KMP_syn_deklaration_t* parameter = bss_feld_element(routine->routine.parameter, i);
        KMP_sem_datentyp_t* parameter_datentyp = kmp_sem_spezifizierung_analysieren(speicher, semantik, parameter->spezifizierung);
        kmp_sem_datentyp_abschließen(speicher, semantik, parameter_datentyp, false);

        if (parameter_datentyp == NULL)
        {
            assert(!"meldung erstatten");
            return NULL;
        }

        if (parameter->variable.initialisierung)
        {
            KMP_sem_ausdruck_t* parameter_op = kmp_sem_ausdruck_analysieren(speicher, semantik, parameter->variable.initialisierung, parameter_datentyp);
        }

        KMP_sem_symbol_t* parameter_symbol = kmp_sem_symbol_datentyp(
            speicher,
            parameter->basis->spanne,
            KMP_SYM_VARIABLE,
            KMP_SYM_BEHANDELT,
            *(BSS_text_t*)bss_feld_element(parameter->namen, 0),
            parameter_datentyp);

        if (!kmp_sem_registrieren_symbol(speicher, semantik, symbol->zone, parameter_symbol->name, parameter_symbol))
        {
            kmp_diagnostik_melden_fehler_bezeichner(speicher, semantik->diagnostik, parameter->basis->spanne, parameter_symbol->name);
            return NULL;
        }

        bss_feld_hinzufügen(speicher, &datentyp->routine.parameter, parameter_datentyp);
    }

    KMP_sem_datentyp_t* rückgabe = Datentyp_Nihil;
    if (routine->routine.rückgabe)
    {
        rückgabe = kmp_sem_spezifizierung_analysieren(speicher, semantik, routine->routine.rückgabe);
    }

    datentyp->routine.rückgabe = rückgabe;

    // INFO: eine funktionsdeklaration kann ohne rumpf auskommen.
    KMP_sem_anweisung_t* rumpf = NULL;
    if (routine->routine.rumpf != NULL)
    {
        KMP_sem_zone_t* z = symbol->zone;
        z->über = semantik->zone;

        kmp_sem_zone_betreten(speicher, semantik, symbol->zone);
        {
            w32 erg = kmp_sem_anweisung_analysieren(speicher, semantik, routine->routine.rumpf, datentyp->routine.rückgabe, &rumpf);
#if 0
            auto reduzierer = Reduzierer();
            rumpf = reduzierer.anweisung_transformieren(rumpf);
#endif

            if (!erg && rückgabe != Datentyp_Nihil)
            {
                KMP_fehler_t* fehler = kmp_fehler(speicher, bss_text("nicht alle codepfade geben einen wert zurück."));
                kmp_diagnostik_melden_fehler(speicher, semantik->diagnostik, routine->routine.rückgabe->basis->spanne, fehler);
            }
        }
        kmp_sem_zone_verlassen(semantik);
    }

    BSS_Feld(KMP_sem_symbol_t*) symbole = bss_feld(speicher, sizeof(KMP_sem_symbol_t*));
    bss_feld_hinzufügen(speicher, &symbole, symbol);
    KMP_sem_deklaration_t* erg = kmp_sem_deklaration_routine(speicher, deklaration, symbole, datentyp, rumpf);

    return erg;
}

KMP_sem_deklaration_t*
kmp_sem_option_analysieren(BSS_speicher_t* speicher, KMP_semantik_t* semantik, KMP_sem_symbol_t* symbol, KMP_syn_deklaration_t* deklaration)
{
    KMP_sem_datentyp_t* datentyp = symbol->datentyp;
    KMP_sem_zone_t* z = symbol->zone;

    z->über= semantik->zone;

    kmp_sem_zone_betreten(speicher, semantik, symbol->zone);
    {
        KMP_syn_ausdruck_t* option = deklaration->option.ausdruck;
        for (g32 i = 0; i < option->option.eigenschaften.anzahl_elemente; ++i)
        {
            KMP_syn_deklaration_t* eigenschaft = bss_feld_element(option->option.eigenschaften, i);
            KMP_sem_datentyp_t* dt = NULL;
            if (eigenschaft->spezifizierung)
            {
                dt = kmp_sem_spezifizierung_analysieren(speicher, semantik, eigenschaft->spezifizierung);
            }

            KMP_sem_operand_t* op = NULL;
            if (eigenschaft->variable.initialisierung)
            {
                KMP_sem_ausdruck_t* ausdruck = kmp_sem_ausdruck_analysieren(speicher, semantik, eigenschaft->variable.initialisierung, dt);
                op = ausdruck->operand;
            }

            for (g32 name_idx = 0; name_idx < eigenschaft->namen.anzahl_elemente; ++name_idx)
            {
                BSS_text_t name = *(BSS_text_t*) bss_feld_element(eigenschaft->namen, name_idx);
                kmp_sem_datentyp_option_eigenschaft_hinzufügen(speicher, datentyp, dt, op, name);
                // datentyp->eigenschaft_hinzufügen(dt, op, name);
            }
        }
    }
    kmp_sem_zone_verlassen(semantik);

    kmp_sem_datentyp_abschließen(speicher, semantik, datentyp, false);

    BSS_Feld(KMP_sem_symbol_t*) symbole = bss_feld(speicher, sizeof(KMP_sem_symbol_t*));
    bss_feld_hinzufügen(speicher, &symbole, symbol);
    KMP_sem_deklaration_t* erg = kmp_sem_deklaration_option(speicher, deklaration, symbole, datentyp);

    return erg;
}

KMP_sem_deklaration_t*
kmp_sem_schablone_analysieren(BSS_speicher_t* speicher, KMP_semantik_t* semantik, KMP_sem_symbol_t* symbol, KMP_syn_deklaration_t* deklaration, w32 zirkularität_ignorieren)
{
    KMP_sem_datentyp_t* datentyp = symbol->datentyp;

    if (datentyp->status == KMP_DATENTYP_STATUS_BEHANDELT ||
        datentyp->status == KMP_DATENTYP_STATUS_IN_BEHANDLUNG && zirkularität_ignorieren)
    {
        return NULL;
    }

    if (datentyp->status == KMP_DATENTYP_STATUS_IN_BEHANDLUNG)
    {
        KMP_fehler_t* fehler = kmp_fehler(speicher, bss_text("datentyp mit zirkulärer abhängigkeit."));
        kmp_diagnostik_melden_fehler(speicher, semantik->diagnostik, deklaration->basis->spanne, fehler);
        return NULL;
    }

    datentyp->status = KMP_DATENTYP_STATUS_IN_BEHANDLUNG;

    assert(symbol != NULL);
    assert(datentyp != NULL);

    g64 größe = 0;
    g64 versatz = 0;

    KMP_sem_zone_t* z = symbol->zone;
    z->über = semantik->zone;

    kmp_sem_zone_betreten(speicher, semantik, z);
    {
        for (g32 i = 0; i < deklaration->schablone.eigenschaften.anzahl_elemente; ++i)
        {
            KMP_syn_deklaration_t* eigenschaft = bss_feld_element(deklaration->schablone.eigenschaften, i);
            KMP_sem_deklaration_t* dt = kmp_sem_deklaration_analysieren(speicher, semantik, eigenschaft);

            for (g32 name_idx = 0; name_idx < eigenschaft->namen.anzahl_elemente; ++name_idx)
            {
                BSS_text_t name = *(BSS_text_t*) bss_feld_element(eigenschaft->namen, name_idx);
                größe += dt->datentyp->größe;

                if (!kmp_sem_datentyp_schablone_eigenschaft_hinzufügen(speicher, dt->datentyp, versatz, name))
                {
                    KMP_fehler_t* fehler = kmp_fehler(speicher, bss_text("eigenschaft bereits bekannt."));
                    kmp_diagnostik_melden_fehler(speicher, semantik->diagnostik, deklaration->basis->spanne, fehler);
                    return NULL;
                }
                versatz += dt->datentyp->größe;
            }
        }
    }
    kmp_sem_zone_verlassen(semantik);

    datentyp->größe = größe;
    datentyp->status = KMP_DATENTYP_STATUS_BEHANDELT;

    BSS_Feld(KMP_sem_symbol_t*) symbole = bss_feld(speicher, sizeof(KMP_sem_symbol_t*));
    bss_feld_hinzufügen(speicher, &symbole, symbol);
    KMP_sem_deklaration_t* erg = kmp_sem_deklaration_schablone(speicher, deklaration, symbole, datentyp);

    return erg;
}

KMP_sem_ausdruck_t*
kmp_sem_muster_analysieren(BSS_speicher_t* speicher, KMP_semantik_t* semantik, KMP_syn_ausdruck_t* muster, KMP_sem_datentyp_t* datentyp)
{
    switch (muster->basis->art)
    {
        case KMP_SYN_KNOTEN_AUSDRUCK_BEZEICHNER:
        {
            BSS_text_t name = muster->bezeichner.wert;
            KMP_sem_symbol_t* symbol = kmp_sem_symbol_datentyp(speicher, muster->basis->spanne, KMP_SYM_VARIABLE, KMP_SYM_BEHANDELT, name, datentyp);
            if (!kmp_sem_registrieren_symbol(speicher, semantik, semantik->zone, symbol->name, symbol))
            {
                kmp_diagnostik_melden_fehler_bezeichner(speicher, semantik->diagnostik, muster->basis->spanne, name);
            }

            KMP_sem_operand_t* operand = kmp_sem_operand_mit_symbol(speicher, symbol, KMP_SEM_OPERAND_MERKMAL_0);
            KMP_sem_ausdruck_t* erg = kmp_sem_ausdruck_bezeichner(speicher, muster, operand);

            return erg;
        } break;

        case KMP_SYN_KNOTEN_AUSDRUCK_KOMPOSITUM:
        {
            if (datentyp->art != KMP_DATENTYP_ART_SCHABLONE)
            {
                KMP_fehler_t* fehler = kmp_fehler(speicher, bss_text("kompositum ausdruck kann nur bei einer schablone angewendet werden"));
                kmp_diagnostik_melden_fehler(speicher, semantik->diagnostik, muster->basis->spanne, fehler);
            }

            KMP_sem_datentyp_t* schablone = datentyp;
            KMP_syn_ausdruck_t* kompositum = muster;

            BSS_Feld(KMP_sem_eigenschaft_t*) eigenschaften = bss_feld(speicher, sizeof(KMP_sem_eigenschaft_t*));
            for (g32 i = 0; i < kompositum->kompositum.eigenschaften.anzahl_elemente; ++i)
            {
                KMP_syn_ausdruck_kompositum_eigenschaft_t* kompositum_eigenschaft = bss_feld_element(kompositum->kompositum.eigenschaften, i);
                w32 eigenschaft_gefunden = false;

                for (g32 j = 0; j < schablone->schablone.eigenschaften.anzahl_elemente; ++j)
                {
                    KMP_sem_datentyp_schablone_eigenschaft_t* schablone_eigenschaft = bss_feld_element(schablone->schablone.eigenschaften, j);
                    if (bss_text_ist_gleich(kompositum_eigenschaft->name, schablone_eigenschaft->name) == 0)
                    {
                        KMP_sem_symbol_t* symbol = kmp_sem_symbol_datentyp(
                            speicher,
                            kompositum_eigenschaft->spanne,
                            KMP_SYM_VARIABLE,
                            KMP_SYM_BEHANDELT,
                            kompositum_eigenschaft->name,
                            schablone_eigenschaft->datentyp);

                        if (!kmp_sem_registrieren_symbol(speicher, semantik, semantik->zone, symbol->name, symbol))
                        {
                            kmp_diagnostik_melden_fehler_bezeichner(speicher, semantik->diagnostik, kompositum_eigenschaft->spanne, kompositum_eigenschaft->name);
                        }

                        KMP_sem_ausdruck_t* kompositum_eigenschaft_ausdruck = kmp_sem_ausdruck_analysieren(speicher, semantik, kompositum_eigenschaft->ausdruck, NULL);

                        if (kmp_sem_datentypen_kompatibel(
                            schablone_eigenschaft->datentyp,
                            kompositum_eigenschaft_ausdruck->operand->datentyp, false) == KMP_DATENTYP_KOMPATIBILITÄT_INKOMPATIBEL)
                        {
                            KMP_fehler_t* fehler = kmp_fehler(speicher, bss_text("datentyp des ausdrucks ist nicht kompatibel mit dem datentyp der eigenschaft"));
                            KMP_meldung_t* meldung = kmp_meldung(speicher, kompositum_eigenschaft->spanne, fehler);
                            kmp_diagnostik_melden(speicher, semantik->diagnostik, meldung);

                            // "datentyp des ausdrucks {} ist nicht kompatibel mit dem datentyp der eigenschaft {}",
                            //     kompositum_eigenschaft_ausdruck->operand()->datentyp()->symbol()->name(),
                            //     schablone_eigenschaft->datentyp()->symbol()->name()));
                        }

                        eigenschaft_gefunden = true;

                        kompositum_eigenschaft_ausdruck->operand->symbol = symbol;

                        bss_feld_hinzufügen(speicher, &eigenschaften, kmp_sem_eigenschaft(speicher, kompositum_eigenschaft_ausdruck->operand));

                        break;
                    }
                }

                if (!eigenschaft_gefunden)
                {
                    KMP_fehler_t* fehler = kmp_fehler(speicher, bss_text("eigenschaft in der schablone nicht vorhanden"));
                    kmp_diagnostik_melden_fehler(
                        speicher,
                        semantik->diagnostik,
                        kompositum_eigenschaft->spanne,
                        fehler);
                }
            }

            KMP_sem_ausdruck_t* erg = kmp_sem_ausdruck_kompositum(
                speicher,
                muster,
                kmp_sem_operand_mit_datentyp(speicher, datentyp, KMP_SEM_OPERAND_MERKMAL_0),
                eigenschaften);

            return erg;
        } break;

        default:
        {
            assert(!"unbekannter ausdruck");
            kmp_diagnostik_melden_fehler_unbekannter_ausdruck(speicher, semantik->diagnostik, muster->basis->spanne, muster);
            return NULL;
        } break;
    }
}

KMP_sem_deklaration_t*
kmp_sem_deklaration_analysieren(BSS_speicher_t* speicher, KMP_semantik_t* semantik, KMP_syn_deklaration_t* deklaration)
{
    switch (deklaration->basis->art)
    {
        case KMP_SYN_KNOTEN_DEKLARATION_VARIABLE:
        {
            KMP_syn_spezifizierung_t* spezifizierung  = deklaration->spezifizierung;
            KMP_syn_ausdruck_t* syn_init = deklaration->variable.initialisierung;

            KMP_sem_datentyp_t* dt_spez = NULL;
            if (spezifizierung)
            {
                dt_spez = kmp_sem_spezifizierung_analysieren(speicher, semantik, spezifizierung);
                kmp_sem_datentyp_abschließen(speicher, semantik, dt_spez, false);
            }

            KMP_sem_datentyp_t* dt_init = dt_spez;
            KMP_sem_ausdruck_t* sem_init = NULL;
            if (syn_init)
            {
                sem_init = kmp_sem_ausdruck_analysieren(speicher, semantik, syn_init, dt_spez);
                dt_init = sem_init->operand->datentyp;
                kmp_sem_datentyp_abschließen(speicher, semantik, dt_init, false);
            }

            if (dt_spez == NULL && dt_init != NULL)
            {
                dt_spez = dt_init;
            }

            if (!kmp_sem_datentypen_kompatibel(dt_spez, dt_init, false))
            {
                KMP_spanne_t spanne = deklaration->variable.initialisierung->basis->spanne;
                KMP_fehler_t* fehler = kmp_fehler(speicher, bss_text("datentyp der spezifizierung und der initialisierung sind nicht kompatibel."));
                kmp_diagnostik_melden_fehler(speicher, semantik->diagnostik, spanne, fehler);
            }

            KMP_sem_datentyp_t* datentyp = dt_spez
                    ? dt_spez
                    : dt_init;

            BSS_Feld(KMP_sem_symbol_t*) symbole = bss_feld(speicher, sizeof(KMP_sem_symbol_t*));
            for (g32 i = 0; i < deklaration->namen.anzahl_elemente; ++i)
            {
                BSS_text_t name = *(BSS_text_t*) bss_feld_element(deklaration->namen, i);
                KMP_sem_symbol_t* sym = kmp_sem_symbol_datentyp(
                    speicher,
                    deklaration->basis->spanne,
                    KMP_SYM_VARIABLE,
                    KMP_SYM_BEHANDELT,
                    name,
                    datentyp);

                if (!kmp_sem_registrieren_symbol(speicher, semantik, semantik->zone, sym->name, sym))
                {
                    kmp_diagnostik_melden_fehler_bezeichner(speicher, semantik->diagnostik, deklaration->basis->spanne, name);
                }

                bss_feld_hinzufügen(speicher, &symbole, sym);
            }

            KMP_sem_deklaration_t* erg = kmp_sem_deklaration(speicher, deklaration, deklaration->basis->art, symbole, datentyp, NULL);

            return erg;
        } break;

        case KMP_SYN_KNOTEN_DEKLARATION_SCHABLONE:
        {
            bss_feld_hinzufügen(speicher, &semantik->schablonen, deklaration);
        } break;

        default:
        {
            kmp_diagnostik_melden_fehler_unbekannte_deklaration(speicher, semantik->diagnostik, deklaration->basis->spanne, deklaration);
            assert(!"unbekannte deklaration");
        } break;
    }

    return NULL;
}

KMP_sem_datentyp_t*
kmp_sem_spezifizierung_analysieren(BSS_speicher_t* speicher, KMP_semantik_t* semantik, KMP_syn_spezifizierung_t* spezifizierung)
{
    switch (spezifizierung->basis->art)
    {
        case KMP_SYN_KNOTEN_SPEZIFIZIERUNG_BEZEICHNER:
        {
            BSS_text_t name = spezifizierung->bezeichner.name;
            KMP_sem_symbol_t* symbol = kmp_sem_bezeichner_analysieren(speicher, semantik, name);

            if (symbol == NULL)
            {
                KMP_fehler_t* fehler = kmp_fehler(speicher, bss_text("Kein bekannter Datentyp."));
                kmp_diagnostik_melden_fehler(speicher, semantik->diagnostik, spezifizierung->basis->spanne, fehler);
                return NULL;
            }

            return symbol->datentyp;
        } break;

        case KMP_SYN_KNOTEN_SPEZIFIZIERUNG_FELD:
        {
            KMP_syn_spezifizierung_t* feld = spezifizierung;

            KMP_sem_datentyp_t* basis_datentyp = kmp_sem_spezifizierung_analysieren(speicher, semantik, feld->feld.basis);
            KMP_sem_ausdruck_t* index = NULL;
            KMP_sem_datentyp_t* index_datentyp = NULL;

            if (feld->feld.index)
            {
                index = kmp_sem_ausdruck_analysieren(speicher, semantik, feld->feld.index, NULL);

                if (index != NULL && index->operand != NULL && index->operand->datentyp != NULL)
                {
                    index_datentyp = index->operand->datentyp;
                }
            }

            KMP_sem_datentyp_t* erg = kmp_sem_datentyp_feld(speicher, basis_datentyp, index_datentyp);

            return erg;
        } break;

        case KMP_SYN_KNOTEN_SPEZIFIZIERUNG_ZEIGER:
        {
            KMP_sem_datentyp_t* basis_datentyp = kmp_sem_spezifizierung_analysieren(speicher, semantik, spezifizierung->zeiger.basis);

            KMP_sem_datentyp_t* erg = kmp_sem_datentyp_zeiger(speicher, basis_datentyp);
        } break;

        case KMP_SYN_KNOTEN_SPEZIFIZIERUNG_ROUTINE:
        {
            KMP_sem_datentyp_t* rückgabe = NULL;

            BSS_Feld(KMP_sem_datentyp_t*) parameter = bss_feld(speicher, sizeof(KMP_sem_datentyp_t*));
            for (g32 i = 0; i < spezifizierung->routine.parameter.anzahl_elemente; ++i)
            {
                KMP_syn_spezifizierung_t* param = bss_feld_element(spezifizierung->routine.parameter, i);
                KMP_sem_datentyp_t* param_datentyp = kmp_sem_spezifizierung_analysieren(speicher, semantik, param);

                if (param_datentyp == NULL)
                {
                    KMP_fehler_t* fehler = kmp_fehler(speicher, bss_text("datentyp konnte nicht ermittelt werden."));
                    kmp_diagnostik_melden_fehler(speicher, semantik->diagnostik, spezifizierung->basis->spanne, fehler);
                    return NULL;
                }

                bss_feld_hinzufügen(speicher, &parameter, param_datentyp);
            }

            if (spezifizierung->routine.rückgabe)
            {
                rückgabe = kmp_sem_spezifizierung_analysieren(speicher, semantik, spezifizierung->routine.rückgabe);
            }

            KMP_sem_datentyp_t* erg = kmp_sem_datentyp_routine(speicher, parameter, rückgabe, NULL);

            return erg;
        } break;

        default:
        {
            kmp_diagnostik_melden_fehler_unbekannte_spezifizierung(speicher, semantik->diagnostik, spezifizierung->basis->spanne, spezifizierung);
            assert(!"unbekannte spezifizierung");
        } break;
    }

    return NULL;
}

KMP_sem_symbol_t*
kmp_sem_bezeichner_analysieren(BSS_speicher_t* speicher, KMP_semantik_t* semantik, BSS_text_t bezeichner)
{
    KMP_sem_symbol_t* erg = kmp_sem_zone_symbol_suchen(semantik->zone, bezeichner, true);

    return erg;
}

void
kmp_sem_datentyp_abschließen(BSS_speicher_t* speicher, KMP_semantik_t* semantik, KMP_sem_datentyp_t* datentyp, w32 basis_eines_zeigers)
{
    if (datentyp->ist_abgeschlossen)
    {
        return;
    }

    switch (datentyp->art)
    {
        case KMP_DATENTYP_ART_SCHABLONE:
        {
            kmp_sem_schablone_analysieren(speicher, semantik, datentyp->symbol, datentyp->deklaration, basis_eines_zeigers);
        } break;

        case KMP_DATENTYP_ART_FELD:
        {
            /*assert(!"Feld Datentyp abschließen");*/
        } break;

        case KMP_DATENTYP_ART_ZEIGER:
        {
            kmp_sem_datentyp_abschließen(speicher, semantik, datentyp->zeiger.basis, true);
        } break;

        /*default: assert(!"implementieren"); break;*/
    }
}

KMP_sem_ausdruck_t*
kmp_sem_ausdruck_analysieren(BSS_speicher_t* speicher, KMP_semantik_t* semantik, KMP_syn_ausdruck_t* ausdruck, KMP_sem_datentyp_t* erwarteter_datentyp)
{
    switch (ausdruck->basis->art)
    {
        case KMP_SYN_KNOTEN_AUSDRUCK_DEZIMALZAHL:
        {
            // auto *dez = ausdruck->als<Ausdruck_Dezimalzahl *>();
            KMP_sem_operand_t* op = kmp_sem_operand_mit_datentyp(speicher, Datentyp_D32, KMP_SEM_OPERAND_MERKMAL_LITERAL | KMP_SEM_OPERAND_MERKMAL_ARITHMETISCH);
            op->wert = bss_wert_d32(speicher, ausdruck->dezimalzahl.wert);

            KMP_sem_ausdruck_t* erg = kmp_sem_ausdruck(
                speicher,
                ausdruck,
                ausdruck->basis->art,
                op
            );

            return erg;
        } break;

        case KMP_SYN_KNOTEN_AUSDRUCK_GANZZAHL:
        {
            KMP_sem_operand_t* op = kmp_sem_operand_mit_datentyp(speicher, Datentyp_G32, KMP_SEM_OPERAND_MERKMAL_LITERAL | KMP_SEM_OPERAND_MERKMAL_ARITHMETISCH);
            op->wert = bss_wert_g32(speicher, ausdruck->ganzzahl.wert);

            KMP_sem_ausdruck_t* erg = kmp_sem_ausdruck(
                speicher,
                ausdruck,
                ausdruck->basis->art,
                op
            );

            return erg;
        } break;

        case KMP_SYN_KNOTEN_AUSDRUCK_TEXT:
        {
            KMP_sem_operand_t* op = kmp_sem_operand_mit_datentyp(speicher, Datentyp_Text, KMP_SEM_OPERAND_MERKMAL_LITERAL);
            op->wert = bss_wert_text(speicher, ausdruck->text.wert);

            KMP_sem_ausdruck_t* erg = kmp_sem_ausdruck(
                speicher,
                ausdruck,
                ausdruck->basis->art,
                op
            );

            return erg;
        } break;

        case KMP_SYN_KNOTEN_AUSDRUCK_BEZEICHNER:
        {
            KMP_sem_symbol_t* symbol = kmp_sem_bezeichner_analysieren(speicher, semantik, ausdruck->bezeichner.wert);

            if (symbol == NULL)
            {
                KMP_fehler_t* fehler = kmp_fehler(speicher, bss_text("{} konnte in der aktuellen zone nicht augelöst werden."));
                kmp_diagnostik_melden_fehler(speicher, semantik->diagnostik, ausdruck->basis->spanne, fehler);
                return NULL;
            }

            g32 merkmale = 0;
            if (symbol->datentyp && kmp_sem_datentyp_ist_arithmetisch(symbol->datentyp))
            {
                merkmale = KMP_SEM_OPERAND_MERKMAL_ARITHMETISCH;
            }

            KMP_sem_ausdruck_t* erg = kmp_sem_ausdruck(
                speicher,
                ausdruck,
                ausdruck->basis->art,
                kmp_sem_operand_mit_symbol(speicher, symbol, merkmale)
            );

            return erg;
        } break;

        case KMP_SYN_KNOTEN_AUSDRUCK_INDEX:
        {
            KMP_sem_ausdruck_t* basis = kmp_sem_ausdruck_analysieren(speicher, semantik, ausdruck->index.basis, NULL);
            if (basis == NULL)
            {
                return NULL;
            }

            KMP_sem_ausdruck_t* index = kmp_sem_ausdruck_analysieren(speicher, semantik, ausdruck->index.index, NULL);
            if (index == NULL)
            {
                return NULL;
            }

            KMP_sem_ausdruck_t* erg = kmp_sem_ausdruck_index(
                speicher,
                ausdruck,
                index->operand,
                basis,
                index
            );

            return erg;
        } break;

        case KMP_SYN_KNOTEN_AUSDRUCK_UNÄR:
        {
            KMP_sem_ausdruck_t* sem_ausdruck = kmp_sem_ausdruck_analysieren(speicher, semantik, ausdruck->unär.ausdruck, NULL);

            KMP_sem_ausdruck_t* erg = kmp_sem_ausdruck_unär(
                speicher,
                ausdruck,
                sem_ausdruck->operand,
                ausdruck->unär.op,
                sem_ausdruck
            );

            return erg;
        } break;

        case KMP_SYN_KNOTEN_AUSDRUCK_BINÄR:
        {
            KMP_sem_ausdruck_t* links  = kmp_sem_ausdruck_analysieren(speicher, semantik, ausdruck->binär.links, NULL);
            KMP_sem_ausdruck_t* rechts = kmp_sem_ausdruck_analysieren(speicher, semantik, ausdruck->binär.rechts, NULL);

            if (ausdruck->binär.op >= KMP_SYN_AUSDRUCK_BINÄR_ADDITION &&
                ausdruck->binär.op <= KMP_SYN_AUSDRUCK_BINÄR_MODULO ||
                ausdruck->binär.op >= KMP_SYN_AUSDRUCK_BINÄR_BIT_VERSATZ_LINKS &&
                ausdruck->binär.op <= KMP_SYN_AUSDRUCK_BINÄR_BIT_VERSATZ_RECHTS)
            {
                if (!kmp_sem_datentyp_ist_arithmetisch(links->operand->datentyp))
                {
                    KMP_fehler_t* fehler = kmp_fehler(speicher, bss_text("arithmetischen operanden erwartet"));
                    kmp_diagnostik_melden_fehler(speicher, semantik->diagnostik, ausdruck->binär.links->basis->spanne, fehler);
                    return NULL;
                }

                if (!kmp_sem_datentyp_ist_arithmetisch(rechts->operand->datentyp))
                {
                    KMP_fehler_t* fehler = kmp_fehler(speicher, bss_text("arithmetischen operanden erwartet"));
                    kmp_diagnostik_melden_fehler(speicher, semantik->diagnostik, ausdruck->binär.rechts->basis->spanne, fehler);
                    return NULL;
                }

                if (!kmp_sem_datentypen_kompatibel(links->operand->datentyp, rechts->operand->datentyp, false))
                {
                    KMP_fehler_t* fehler = kmp_fehler(speicher, bss_text("datentypen nicht kompatibel."));
                    kmp_diagnostik_melden_fehler(speicher, semantik->diagnostik, ausdruck->basis->spanne, fehler);
                    return NULL;
                }

                KMP_sem_ausdruck_t* erg = kmp_sem_ausdruck_binär(
                    speicher,
                    ausdruck,
                    kmp_sem_operand_mit_datentyp(speicher, links->operand->datentyp, KMP_SEM_OPERAND_MERKMAL_0),
                    ausdruck->binär.op,
                    links,
                    rechts
                );

                return erg;
            }
            else if (ausdruck->binär.op >= KMP_SYN_AUSDRUCK_BINÄR_GLEICH &&
                     ausdruck->binär.op <= KMP_SYN_AUSDRUCK_BINÄR_GRÖẞER_GLEICH)
            {
                if (!kmp_sem_datentypen_kompatibel(
                    links->operand->datentyp,
                    rechts->operand->datentyp,
                    false))
                {
                    KMP_fehler_t* fehler = kmp_fehler(speicher, bss_text("datentypen nicht kompatibel."));
                    kmp_diagnostik_melden_fehler(speicher, semantik->diagnostik, ausdruck->basis->spanne, fehler);
                    return NULL;
                }

                KMP_sem_ausdruck_t* erg = kmp_sem_ausdruck_binär(
                    speicher,
                    ausdruck,
                    kmp_sem_operand_mit_datentyp(speicher, Datentyp_Bool, KMP_SEM_OPERAND_MERKMAL_0),
                    ausdruck->binär.op,
                    links,
                    rechts
                );

                return erg;
            }
            else
            {
                assert(!"kein passender operand");
            }
        } break;

        case KMP_SYN_KNOTEN_AUSDRUCK_AUFRUF:
        {
            KMP_sem_ausdruck_t* basis = kmp_sem_ausdruck_analysieren(speicher, semantik, ausdruck->aufruf.basis, NULL);
            if (basis == NULL)
            {
                assert(!"meldung erstatten");
                return NULL;
            }

            if (basis->operand->symbol->art != KMP_SYM_ROUTINE)
            {
                assert(!"meldung erstatten");
                return NULL;
            }

            KMP_sem_datentyp_t* routine_dt = basis->operand->datentyp;
            if (routine_dt->routine.parameter.anzahl_elemente != ausdruck->aufruf.argumente.anzahl_elemente)
            {
                assert(!"meldung erstatten");
                return NULL;
            }

            BSS_Feld(KMP_sem_ausdruck_t*) argumente = bss_feld(speicher, sizeof(KMP_sem_ausdruck_t*));
            for (g32 i = 0; i < routine_dt->routine.parameter.anzahl_elemente; ++i)
            {
                KMP_sem_datentyp_t* parameter = bss_feld_element(routine_dt->routine.parameter, i);
                KMP_syn_ausdruck_t* argument = bss_feld_element(ausdruck->aufruf.argumente, i);
                KMP_sem_ausdruck_t* sem_arg = kmp_sem_ausdruck_analysieren(speicher, semantik, argument, parameter);

                if (!kmp_sem_datentypen_kompatibel(
                    parameter,
                    sem_arg->operand->datentyp,
                    false))
                {
                    assert(!"meldung erstatten");
                    return NULL;
                }

                bss_feld_hinzufügen(speicher, &argumente, sem_arg);
            }

            KMP_sem_ausdruck_t* erg = kmp_sem_ausdruck_aufruf(
                speicher,
                ausdruck,
                kmp_sem_operand_mit_datentyp(speicher, routine_dt->routine.rückgabe, KMP_SEM_OPERAND_MERKMAL_0),
                basis,
                argumente
            );

            return erg;
        } break;

        case KMP_SYN_KNOTEN_AUSDRUCK_EIGENSCHAFT:
        {
            KMP_sem_ausdruck_t* basis = kmp_sem_ausdruck_analysieren(speicher, semantik, ausdruck->eigenschaft.basis, NULL);
            if (basis == NULL)
            {
                assert(!"meldung erstatten");
                return NULL;
            }

            if (basis->operand->symbol != NULL &&
                basis->operand->symbol->art == KMP_SYM_MODUL)
            {
                assert(ausdruck->eigenschaft.eigenschaft->basis->art == KMP_SYN_KNOTEN_AUSDRUCK_BEZEICHNER);
                KMP_sem_symbol_t* symbol = kmp_sem_zone_symbol_suchen(
                    basis->operand->symbol->zone,
                    ausdruck->eigenschaft.eigenschaft->bezeichner.wert,
                    true);

                KMP_sem_ausdruck_t* erg = kmp_sem_ausdruck_eigenschaft(
                    speicher,
                    ausdruck,
                    kmp_sem_operand_mit_symbol(speicher, symbol, KMP_SEM_OPERAND_MERKMAL_0),
                    basis,
                    symbol
                );

                return erg;
            }
            else
            {
                if (basis->operand->datentyp == NULL)
                {
                    assert(!"meldung erstatten");
                    return NULL;
                }

                kmp_sem_datentyp_abschließen(speicher, semantik, basis->operand->datentyp, false);

                assert(ausdruck->eigenschaft.eigenschaft->basis->art == KMP_SYN_KNOTEN_AUSDRUCK_BEZEICHNER);
                BSS_text_t name = ausdruck->eigenschaft.eigenschaft->bezeichner.wert;
                KMP_sem_symbol_t* symbol = kmp_sem_zone_symbol_suchen(
                    basis->operand->datentyp->symbol->zone,
                    name,
                    true);

                if (symbol == NULL)
                {
                    KMP_fehler_t* fehler = kmp_fehler(
                        speicher,
                        bss_text("eigenschaft ist nicht teil der datenstruktur"));

                    kmp_diagnostik_melden_fehler(
                        speicher,
                        semantik->diagnostik,
                        ausdruck->eigenschaft.eigenschaft->basis->spanne,
                        fehler);
                }

                KMP_sem_ausdruck_t* erg = kmp_sem_ausdruck_eigenschaft(
                    speicher,
                    ausdruck,
                    kmp_sem_operand_mit_symbol(speicher, symbol, KMP_SEM_OPERAND_MERKMAL_0),
                    basis,
                    symbol);

                return erg;
            }
        } break;

        case KMP_SYN_KNOTEN_AUSDRUCK_KOMPOSITUM:
        {
            KMP_sem_datentyp_t* datentyp = NULL;
            if (ausdruck->kompositum.spezifizierung == NULL && erwarteter_datentyp == NULL)
            {
                KMP_sem_symbol_t* symbol = kmp_sem_symbol_datentyp(
                    speicher,
                    ausdruck->basis->spanne,
                    KMP_SYM_DATENTYP,
                    KMP_SYM_BEHANDELT,
                    bss_text("<anonymer datentyp>"),
                    NULL);

                datentyp = kmp_sem_datentyp_schablone(speicher, NULL);
                datentyp->symbol = symbol;
                symbol->datentyp = datentyp;

                KMP_sem_zone_t* z = symbol->zone;
                z->über = semantik->zone;

                g64 versatz = 0;
                g64 größe = 0;

                kmp_sem_zone_betreten(speicher, semantik, z);
                {
                    for (g32 i = 0; i < ausdruck->kompositum.eigenschaften.anzahl_elemente; ++i)
                    {
                        KMP_syn_ausdruck_kompositum_eigenschaft_t* eigenschaft = bss_feld_element(ausdruck->kompositum.eigenschaften, i);

                        KMP_sem_ausdruck_t* op = kmp_sem_ausdruck_analysieren(speicher, semantik, eigenschaft->ausdruck, NULL);
                        kmp_sem_datentyp_schablone_eigenschaft_hinzufügen(speicher, datentyp, versatz, eigenschaft->name);
                        KMP_sem_symbol_t* eigenschaft_symbol = kmp_sem_symbol_datentyp(
                            speicher,
                            eigenschaft->spanne,
                            KMP_SYM_VARIABLE,
                            KMP_SYM_BEHANDELT,
                            eigenschaft->name,
                            op->operand->datentyp);

                        if (!kmp_sem_registrieren_symbol(speicher, semantik, z, symbol->name, symbol))
                        {
                            kmp_diagnostik_melden_fehler_bezeichner(speicher, semantik->diagnostik, eigenschaft->spanne, symbol->name);
                        }

                        versatz += op->operand->datentyp->größe;
                        größe += op->operand->datentyp->größe;
                    }
                }
                kmp_sem_zone_verlassen(semantik);

                datentyp->größe = größe;
                kmp_sem_datentyp_abschließen(speicher, semantik, datentyp, false);
            }

            if (erwarteter_datentyp)
            {
                if (ausdruck->kompositum.spezifizierung)
                {
                    KMP_sem_datentyp_t* deklarierter_datentyp = kmp_sem_spezifizierung_analysieren(speicher, semantik, ausdruck->kompositum.spezifizierung);

                    if (deklarierter_datentyp == NULL)
                    {
                        assert(!"meldung erstatten");
                        return NULL;
                    }

                    kmp_sem_datentyp_abschließen(speicher, semantik, deklarierter_datentyp, false);

                    if (!kmp_sem_datentypen_kompatibel(deklarierter_datentyp, erwarteter_datentyp, false))
                    {
                        kmp_diagnostik_melden_fehler_datentypen_inkompatibel(speicher, semantik->diagnostik, ausdruck->basis->spanne, deklarierter_datentyp, erwarteter_datentyp);
                        return NULL;
                    }
                }

                datentyp = erwarteter_datentyp;
            }
            else
            {
                KMP_sem_datentyp_t* deklarierter_datentyp = NULL;

                if (ausdruck->kompositum.spezifizierung)
                {
                    deklarierter_datentyp = kmp_sem_spezifizierung_analysieren(speicher, semantik, ausdruck->kompositum.spezifizierung);
                    kmp_sem_datentyp_abschließen(speicher, semantik, deklarierter_datentyp, false);
                }

                if (deklarierter_datentyp == NULL)
                {
#if 0
                    // INFO: anonymen datentyp erstellen
                    std::vector<Datentyp_Anon::Eigenschaft *> eigenschaften;
                    size_t versatz = 0;

                    for (auto eigenschaft : kompositum->eigenschaften())
                    {
                        if (!eigenschaft->ist_benamt())
                        {
                            assert(!"anonymer datentyp darf nur benamte eigenschaften enthalten.");
                        }

                        auto *eigenschaft_op = ausdruck_analysieren(eigenschaft->ausdruck());

                        if (eigenschaft_op->datentyp() == nullptr)
                        {
                            assert(!"datentyp konnte nicht hergeleitet werden.");
                        }

                        eigenschaften.push_back(new Datentyp_Anon::Eigenschaft(eigenschaft_op->datentyp(), versatz, eigenschaft->name()));
                        versatz += eigenschaft_op->datentyp()->größe();
                    }

                    datentyp = new Datentyp_Anon(eigenschaften);
#endif
                }
                else
                {
                    datentyp = deklarierter_datentyp;
                }
            }

            if (ausdruck->kompositum.ist_benamt && datentyp->art != KMP_DATENTYP_ART_SCHABLONE)
            {
                KMP_fehler_t* fehler = kmp_fehler(speicher, bss_text("benamte eigenschaften können nur mit schablonen verwendet werden"));
                kmp_diagnostik_melden_fehler(speicher, semantik->diagnostik, ausdruck->basis->spanne, fehler);
                return NULL;
            }

            BSS_Feld(KMP_sem_eigenschaft_t*) eigenschaften = bss_feld(speicher, sizeof(KMP_sem_eigenschaft_t*));
            for (g32 i = 0; i < ausdruck->kompositum.eigenschaften.anzahl_elemente; ++i)
            {
                KMP_syn_ausdruck_kompositum_eigenschaft_t* eigenschaft = bss_feld_element(ausdruck->kompositum.eigenschaften, i);
                if (ausdruck->kompositum.ist_benamt && !eigenschaft->ist_benamt)
                {
                    KMP_fehler_t* fehler = kmp_fehler(speicher, bss_text("wenn eine eigenschaft benamt wird, müssen alle eigenschaften benamt werden."));
                    kmp_diagnostik_melden_fehler(speicher, semantik->diagnostik, eigenschaft->ausdruck->basis->spanne, fehler);
                    return NULL;
                }

                kmp_sem_datentyp_abschließen(speicher, semantik, datentyp, false);

                if (eigenschaft->ist_benamt)
                {
                    w32 gefunden = false;

                    for (g32 j = 0; j < datentyp->schablone.eigenschaften.anzahl_elemente; ++j)
                    {
                        KMP_sem_datentyp_schablone_eigenschaft_t* schablone_eigenschaft = bss_feld_element(datentyp->schablone.eigenschaften, j);
                        if (bss_text_ist_gleich(schablone_eigenschaft->name, eigenschaft->name) == 0)
                        {
                            gefunden = true;
                            break;
                        }
                    }

                    // AUFGABE: mehrfache verwendung der selben eigenschaft prüfen

                    if (!gefunden)
                    {
                        KMP_fehler_t* fehler = kmp_fehler(speicher, bss_text("eigenschaft konnte in der schablone nicht gefunden werden."));
                        kmp_diagnostik_melden_fehler(speicher, semantik->diagnostik, eigenschaft->ausdruck->basis->spanne, fehler);
                        return NULL;
                    }

                    KMP_sem_symbol_t* symbol = kmp_sem_zone_symbol_suchen(datentyp->symbol->zone, eigenschaft->name, false);
                    if (symbol == NULL)
                    {
                        KMP_fehler_t* fehler = kmp_fehler(speicher, bss_text("eigenschaft konnte in der schablone nicht gefunden werden."));
                        kmp_diagnostik_melden_fehler(speicher, semantik->diagnostik, eigenschaft->ausdruck->basis->spanne, fehler);
                        return NULL;
                    }

                    KMP_sem_eigenschaft_t* sem_eigenschaft = kmp_sem_eigenschaft(
                        speicher,
                        kmp_sem_operand_mit_symbol(speicher, symbol, KMP_SEM_OPERAND_MERKMAL_0));
                    bss_feld_hinzufügen(speicher, &eigenschaften, sem_eigenschaft);
                }
            }

            KMP_sem_ausdruck_t* erg = kmp_sem_ausdruck_kompositum(
                speicher,
                ausdruck,
                kmp_sem_operand_mit_datentyp(speicher, datentyp, KMP_SEM_OPERAND_MERKMAL_0),
                eigenschaften
            );

            return erg;
        } break;

        default:
        {
            kmp_diagnostik_melden_fehler_unbekannter_ausdruck(speicher, semantik->diagnostik, ausdruck->basis->spanne, ausdruck);
            assert(!"unbekannter ausdruck");
        } break;
    }

    return NULL;
}

w32
kmp_sem_anweisung_analysieren(BSS_speicher_t* speicher, KMP_semantik_t* semantik, KMP_syn_anweisung_t* anweisung, KMP_sem_datentyp_t* ziel_typ, KMP_sem_anweisung_t** erg_param)
{
    assert(erg_param);
    w32 erg = false;

    switch (anweisung->basis->art)
    {
        case KMP_SYN_KNOTEN_ANWEISUNG_AUSDRUCK:
        {
            KMP_sem_ausdruck_t* ausdruck = kmp_sem_ausdruck_analysieren(speicher, semantik, anweisung->ausdruck.ausdruck, NULL);
            if (ausdruck != NULL)
            {
                *erg_param = kmp_sem_anweisung_ausdruck(speicher, anweisung, ausdruck);
            }
        } break;

        case KMP_SYN_KNOTEN_ANWEISUNG_BLOCK:
        {
            BSS_Feld(KMP_sem_anweisung_t*) block_anweisungen = bss_feld(speicher, sizeof(KMP_sem_anweisung_t*));
            for (g32 i = 0; i < anweisung->block.anweisungen.anzahl_elemente; ++i)
            {
                KMP_syn_anweisung_t* syn_anweisung = bss_feld_element(anweisung->block.anweisungen, i);
                KMP_sem_anweisung_t *sem_anweisung = NULL;
                erg = kmp_sem_anweisung_analysieren(speicher, semantik, syn_anweisung, ziel_typ, &sem_anweisung) || erg;
                assert(sem_anweisung);
                bss_feld_hinzufügen(speicher, &block_anweisungen, sem_anweisung);
            }

            *erg_param = kmp_sem_anweisung_block(speicher, anweisung, block_anweisungen);
        } break;

        case KMP_SYN_KNOTEN_ANWEISUNG_DEKLARATION:
        {
            KMP_sem_deklaration_t* sem_deklaration = kmp_sem_deklaration_analysieren(speicher, semantik, anweisung->deklaration.deklaration);

            *erg_param = kmp_sem_anweisung_deklaration(speicher, anweisung, sem_deklaration);
        } break;

        case KMP_SYN_KNOTEN_ANWEISUNG_SOLANGE:
        {
            BSS_text_t weiter_markierung = kmp_sem_markierung_erstellen(bss_text("weiter"));
            BSS_text_t raus_markierung = kmp_sem_markierung_erstellen(bss_text("raus"));

            KMP_sem_ausdruck_t* bedingung = kmp_sem_ausdruck_analysieren(speicher, semantik, anweisung->solange.bedingung, Datentyp_Bool);

            KMP_sem_zone_t* solange_zone = kmp_sem_zone(speicher, bss_text("solange"), semantik->zone);
            KMP_sem_anweisung_t* rumpf = NULL;
            kmp_sem_zone_betreten(speicher, semantik, solange_zone);
            {
                BSS_paar_text_t paar = bss_paar_text(weiter_markierung, raus_markierung);
                bss_stapel_rein(speicher, &global_markierungen, &paar);
                erg = kmp_sem_anweisung_analysieren(speicher, semantik, anweisung->solange.rumpf, ziel_typ, &rumpf) && erg;
                bss_stapel_raus(&global_markierungen);
            }
            kmp_sem_zone_verlassen(semantik);

            *erg_param = kmp_sem_anweisung_solange(
                speicher,
                anweisung,
                bedingung,
                rumpf,
                weiter_markierung,
                raus_markierung);
        } break;

        case KMP_SYN_KNOTEN_ANWEISUNG_FÜR:
        {
            KMP_sem_symbol_t* für_index_symbol = NULL;

            BSS_text_t weiter_markierung = kmp_sem_markierung_erstellen(bss_text("weiter"));
            BSS_text_t raus_markierung = kmp_sem_markierung_erstellen(bss_text("raus"));
            KMP_sem_ausdruck_t* bedingung = NULL;
            KMP_sem_anweisung_t* für_rumpf = NULL;

            kmp_sem_zone_betreten(speicher, semantik, kmp_sem_zone(speicher, bss_text("für"), semantik->zone));
            {
                if (anweisung->für.index != NULL)
                {
                    assert(anweisung->für.index->basis->art == KMP_SYN_KNOTEN_AUSDRUCK_BEZEICHNER);
                    BSS_text_t für_index_symbol_name = anweisung->für.index->bezeichner.wert;
                    für_index_symbol = kmp_sem_symbol_datentyp(
                        speicher,
                        anweisung->für.index->basis->spanne,
                        KMP_SYM_VARIABLE,
                        KMP_SYM_BEHANDELT,
                        für_index_symbol_name,
                        NULL
                    );

                    if (!kmp_sem_registrieren_symbol(speicher, semantik, semantik->zone, für_index_symbol->name, für_index_symbol))
                    {
                        kmp_diagnostik_melden_fehler_bezeichner(speicher, semantik->diagnostik, anweisung->basis->spanne, für_index_symbol_name);
                    }
                }
                else
                {
                    für_index_symbol = kmp_sem_symbol_datentyp(
                        speicher,
                        kmp_spanne(),
                        KMP_SYM_VARIABLE,
                        KMP_SYM_BEHANDELT,
                        bss_text("it"),
                        NULL
                    );

                    if (!kmp_sem_registrieren_symbol(speicher, semantik, semantik->zone, für_index_symbol->name, für_index_symbol))
                    {
                        kmp_diagnostik_melden_fehler_bezeichner(speicher, semantik->diagnostik, anweisung->basis->spanne, bss_text("it"));
                    }
                }

                bedingung = kmp_sem_ausdruck_analysieren(speicher, semantik, anweisung->für.bedingung, Datentyp_Bool);
                BSS_paar_text_t paar = bss_paar_text(weiter_markierung, raus_markierung);
                bss_stapel_rein(speicher, &global_markierungen, &paar);
                erg = kmp_sem_anweisung_analysieren(speicher, semantik, anweisung->für.rumpf, ziel_typ, &für_rumpf) && erg;
                bss_stapel_raus(&global_markierungen);
            }
            kmp_sem_zone_verlassen(semantik);

            *erg_param = kmp_sem_anweisung_für(
                speicher,
                anweisung,
                für_index_symbol,
                bedingung,
                für_rumpf,
                weiter_markierung,
                raus_markierung);
        } break;

        case KMP_SYN_KNOTEN_ANWEISUNG_WEITER:
        {
            if (global_markierungen.anzahl_elemente == 0)
            {
                kmp_diagnostik_melden_fehler(
                    speicher,
                    semantik->diagnostik,
                    anweisung->basis->spanne,
                    kmp_fehler(speicher, bss_text("Kein gültiger Bereich für diese Anweisung.")));
                return false;
            }

            BSS_paar_text_t markierungen = *(BSS_paar_text_t*) bss_stapel_raus(&global_markierungen);
            BSS_text_t weiter_markierung = markierungen.eins;

            *erg_param = kmp_sem_anweisung_sprung(
                speicher,
                anweisung,
                weiter_markierung);
        } break;

        case KMP_SYN_KNOTEN_ANWEISUNG_RAUS:
        {
            if (global_markierungen.anzahl_elemente == 0)
            {
                kmp_diagnostik_melden_fehler(
                    speicher,
                    semantik->diagnostik,
                    anweisung->basis->spanne,
                    kmp_fehler(speicher, bss_text("Kein gültiger Bereich für diese Anweisung.")));
                return false;
            }

            BSS_paar_text_t markierungen = *(BSS_paar_text_t*) bss_stapel_schauen(&global_markierungen);
            BSS_text_t raus_markierung = markierungen.zwei;

            *erg_param = kmp_sem_anweisung_sprung(
                speicher,
                anweisung,
                raus_markierung);
        } break;

        case KMP_SYN_KNOTEN_ANWEISUNG_RES:
        {
            if (ziel_typ == NULL)
            {
                kmp_diagnostik_melden_fehler(
                    speicher,
                    semantik->diagnostik,
                    anweisung->basis->spanne,
                    kmp_fehler(speicher, bss_text("nur eine funktion kann ein ergebnis zurückgeben.")));
                return false;
            }

            KMP_sem_ausdruck_t* ausdruck = kmp_sem_ausdruck_analysieren(speicher, semantik, anweisung->res.ausdruck, ziel_typ);

            if (!kmp_sem_datentypen_kompatibel(ziel_typ, ausdruck->operand->datentyp, false))
            {
                KMP_spanne_t spanne = ausdruck->basis->spanne;
                kmp_diagnostik_melden_fehler_datentypen_inkompatibel(speicher, semantik->diagnostik, spanne, ziel_typ, ausdruck->operand->datentyp);
            }

            *erg_param = kmp_sem_anweisung_res(
                speicher,
                anweisung,
                ausdruck);

            erg = true;
        } break;

        case KMP_SYN_KNOTEN_ANWEISUNG_WENN:
        {
            KMP_sem_ausdruck_t* bedingung = kmp_sem_ausdruck_analysieren(speicher, semantik, anweisung->wenn.bedingung, ziel_typ);
            if (bedingung->operand->datentyp->art != KMP_DATENTYP_ART_BOOL)
            {
                assert(!"meldung erstatten");
            }

            KMP_sem_anweisung_t* rumpf = NULL;
            kmp_sem_zone_betreten(speicher, semantik, kmp_sem_zone(speicher, bss_text(""), semantik->zone));
            {
                erg = kmp_sem_anweisung_analysieren(speicher, semantik, anweisung->wenn.rumpf, ziel_typ, &rumpf);
            }
            kmp_sem_zone_verlassen(semantik);

            KMP_sem_anweisung_t* sonst = NULL;
            if (anweisung->wenn.alternative)
            {
                erg = kmp_sem_anweisung_analysieren(speicher, semantik, anweisung->wenn.alternative, ziel_typ, &sonst) && erg;
            }

            *erg_param = kmp_sem_anweisung_wenn(
                speicher,
                anweisung,
                bedingung,
                rumpf,
                sonst);
        } break;

        case KMP_SYN_KNOTEN_ANWEISUNG_WEICHE:
        {
            KMP_sem_ausdruck_t* ausdruck = kmp_sem_ausdruck_analysieren(speicher, semantik, anweisung->weiche.ausdruck, NULL);

            BSS_Feld(KMP_sem_muster_t*) sem_muster = bss_feld(speicher, sizeof(KMP_sem_muster_t*));
            for (g32 i = 0; i < anweisung->weiche.muster.anzahl_elemente; ++i)
            {
                KMP_syn_muster_t* syn_muster = bss_feld_element(anweisung->weiche.muster, i);
                KMP_sem_ausdruck_t* sem_ausdruck = NULL;
                KMP_sem_anweisung_t* rumpf = NULL;

                kmp_sem_zone_betreten(speicher, semantik, kmp_sem_zone(speicher, bss_text("muster"), semantik->zone));
                {
                    sem_ausdruck = kmp_sem_muster_analysieren(speicher, semantik, syn_muster->muster, ausdruck->operand->datentyp);
                    erg = kmp_sem_anweisung_analysieren(speicher, semantik, syn_muster->anweisung, ziel_typ, &rumpf) && erg;
                }
                kmp_sem_zone_verlassen(semantik);

                bss_feld_hinzufügen(speicher, &sem_muster, kmp_sem_muster(speicher, syn_muster, sem_ausdruck, rumpf));
            }

            *erg_param = kmp_sem_anweisung_weiche(
                speicher,
                anweisung,
                ausdruck,
                sem_muster);
        } break;

        case KMP_SYN_KNOTEN_ANWEISUNG_ZUWEISUNG:
        {
            KMP_sem_ausdruck_t* links  = kmp_sem_ausdruck_analysieren(speicher, semantik, anweisung->zuweisung.links, NULL);
            kmp_sem_datentyp_abschließen(speicher, semantik, links->operand->datentyp, false);

            KMP_sem_ausdruck_t* rechts = kmp_sem_ausdruck_analysieren(speicher, semantik, anweisung->zuweisung.rechts, links->operand->datentyp);
            kmp_sem_datentyp_abschließen(speicher, semantik, rechts->operand->datentyp, false);

            if (!kmp_sem_datentypen_kompatibel(links->operand->datentyp, rechts->operand->datentyp, false))
            {
                KMP_spanne_t spanne = rechts->basis->spanne;
                kmp_diagnostik_melden_fehler_datentypen_inkompatibel(
                    speicher,
                    semantik->diagnostik,
                    spanne,
                    links->operand->datentyp,
                    rechts->operand->datentyp);
            }

            *erg_param = kmp_sem_anweisung_zuweisung(
                speicher,
                anweisung,
                anweisung->zuweisung.op,
                links,
                rechts);
        } break;

        case KMP_SYN_KNOTEN_ANWEISUNG_FINAL:
        {
            KMP_sem_anweisung_t* final_anweisung = NULL;
            kmp_sem_anweisung_analysieren(speicher, semantik, anweisung->final.anweisung, ziel_typ, &final_anweisung);

            *erg_param = kmp_sem_anweisung_final(
                speicher,
                anweisung,
                final_anweisung);
        } break;

        default:
        {
            kmp_diagnostik_melden_fehler_unbekannte_anweisung(speicher, semantik->diagnostik, anweisung->basis->spanne, anweisung);
            assert(!"unbekannte anweisung");
        } break;
    }

    return erg;
}

void
kmp_sem_import_verarbeiten(BSS_speicher_t* speicher, KMP_semantik_t* semantik, BSS_text_t dateiname)
{

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

    KMP_syntax_t* syntax_neu = kmp_syntax(speicher, glieder);
    KMP_syn_asb_t asb = kmp_syn_starten(speicher, syntax_neu, 0);

    KMP_sem_zone_t* global_neu = kmp_sem_zone(speicher, bss_text("import"), semantik->system);
    KMP_semantik_t* semantik_neu = kmp_semantik(speicher, &asb, semantik->system, global_neu);
    KMP_sem_asb_t* sem_asb = kmp_sem_starten(speicher, semantik_neu, false);

    for (g32 i = 0; i < semantik_neu->global->symbole.anzahl_elemente; ++i)
    {
        KMP_sem_symbol_t* symbol = bss_feld_element(semantik_neu->global->symbole, i);
        if (!kmp_sem_registrieren_symbol(speicher, semantik, semantik->zone, symbol->name, symbol))
        {
            kmp_diagnostik_melden_fehler_bezeichner(speicher, semantik->diagnostik, symbol->spanne, symbol->name);
        }
    }
}

w32
kmp_sem_registrieren_symbol(BSS_speicher_t* speicher, KMP_semantik_t* semantik, KMP_sem_zone_t* zone, BSS_text_t name, KMP_sem_symbol_t* symbol)
{
    if (zone == NULL)
    {
        return false;
    }

    w32 erg = kmp_sem_zone_symbol_registrieren(speicher, zone, name, symbol);

    return erg;
}

void
kmp_sem_zone_betreten(BSS_speicher_t* speicher, KMP_semantik_t* semantik, KMP_sem_zone_t* zone)
{
    // ACHTUNG: an dieser stelle geht uns die vorher aktive zone verloren,
    // wenn die übergebene zone ihren über nicht gesetzt hat.
    if (zone == NULL)
    {
        zone = kmp_sem_zone(speicher, bss_text(""), NULL);
    }

    semantik->zone = zone;
}

void
kmp_sem_zone_verlassen(KMP_semantik_t* semantik)
{
    semantik->zone = semantik->zone->über;
}

w32
kmp_sem_operanden_kompatibel(KMP_sem_operand_t* ziel, KMP_sem_operand_t* quelle)
{
    return false;
}

void
kmp_sem_system_zone_initialisieren(BSS_speicher_t* speicher, KMP_sem_zone_t* zone)
{
#define I(N, D, A, M, G) \
    Datentyp_##D = speicher->anfordern(speicher, sizeof(KMP_sem_datentyp_t)); \
    *Datentyp_##D = (KMP_sem_datentyp_t){ .art = A, .merkmale = M, .größe = G, .ist_abgeschlossen = true }; \
    KMP_sem_symbol_t* sym_##N = kmp_sem_symbol_datentyp(speicher, kmp_spanne(), KMP_SYM_DATENTYP, KMP_SYM_BEHANDELT, bss_text(#N), Datentyp_##D); \
    Datentyp_##D->symbol = sym_##N; \
    kmp_sem_zone_symbol_registrieren(speicher, zone, bss_text(#N), sym_##N)

    I(n8,    N8,    KMP_DATENTYP_ART_GANZE_ZAHL, KMP_DATENTYP_MERKMAL_ARITHMETISCH, 1);
    I(n16,   N16,   KMP_DATENTYP_ART_GANZE_ZAHL, KMP_DATENTYP_MERKMAL_ARITHMETISCH, 2);
    I(n32,   N32,   KMP_DATENTYP_ART_GANZE_ZAHL, KMP_DATENTYP_MERKMAL_ARITHMETISCH, 4);
    I(n64,   N64,   KMP_DATENTYP_ART_GANZE_ZAHL, KMP_DATENTYP_MERKMAL_ARITHMETISCH, 8);
    I(n128,  N128,  KMP_DATENTYP_ART_GANZE_ZAHL, KMP_DATENTYP_MERKMAL_ARITHMETISCH, 16);

    I(g8,    G8,    KMP_DATENTYP_ART_GANZE_ZAHL, KMP_DATENTYP_MERKMAL_ARITHMETISCH, 1);
    I(g16,   G16,   KMP_DATENTYP_ART_GANZE_ZAHL, KMP_DATENTYP_MERKMAL_ARITHMETISCH, 2);
    I(g32,   G32,   KMP_DATENTYP_ART_GANZE_ZAHL, KMP_DATENTYP_MERKMAL_ARITHMETISCH, 4);
    I(g64,   G64,   KMP_DATENTYP_ART_GANZE_ZAHL, KMP_DATENTYP_MERKMAL_ARITHMETISCH, 8);
    I(g128,  G128,  KMP_DATENTYP_ART_GANZE_ZAHL, KMP_DATENTYP_MERKMAL_ARITHMETISCH, 16);

    I(d32,   D32,   KMP_DATENTYP_ART_DEZIMAL_ZAHL, KMP_DATENTYP_MERKMAL_ARITHMETISCH, 4);
    I(d64,   D64,   KMP_DATENTYP_ART_DEZIMAL_ZAHL, KMP_DATENTYP_MERKMAL_ARITHMETISCH, 8);

    I(text,  Text,  KMP_DATENTYP_ART_TEXT, KMP_DATENTYP_MERKMAL_0, 8);
    I(bool,  Bool,  KMP_DATENTYP_ART_GANZE_ZAHL, KMP_DATENTYP_MERKMAL_0, 4);
    I(nihil, Nihil, KMP_DATENTYP_ART_NIHIL, KMP_DATENTYP_MERKMAL_0, 4);
#undef I
}

BSS_text_t
kmp_sem_markierung_erstellen(BSS_text_t markierung)
{
    char erg[1024];
    sprintf(erg, "%s_%d", markierung.daten, global_semantik_markierung_index++);

    return bss_text(erg);
}

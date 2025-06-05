#include "basis/speicher.c"
#include "basis/feld.c"
#include "basis/paar.c"
#include "basis/stapel.c"
#include "basis/text.c"
#include "basis/wert.c"
#include "dienste/kmd.c"
#include "kmp/spanne.c"
#include "kmp/fehler.c"
#include "kmp/ergebnis.c"
#include "kmp/diagnostik.c"
#include "kmp/zone.c"
#include "kmp/operand.c"
#include "kmp/symbol.c"
#include "kmp/datentyp.c"
#include "kmp/zeichen.c"
#include "kmp/glied.c"
#include "kmp/lexer.c"
#include "kmp/asb.c"
#include "kmp/syntax.c"
#include "kmp/sem_asb.c"
#include "kmp/semantik.c"
#include "kmp/transformator.c"

#include <windows.h>

#define INHALT_GRÖSSE 1024*1024*10
char repl_inhalt[INHALT_GRÖSSE];

int main (int argc, char *argv[])
{
    SetConsoleOutputCP(CP_UTF8);

    size_t inhalt_versatz = 0;

    BSS_speicher_t speicher = bss_speicher_arena();
    KMP_sem_zone_t* system = kmp_sem_zone(&speicher, bss_text("system"), NULL);
    KMP_sem_zone_t* global = kmp_sem_zone(&speicher, bss_text("global"), system);
    kmp_sem_system_zone_initialisieren(&speicher, system);

    w32 beenden = false;

    DNST_kmd_t kmd = {};

    dnst_kmd_marke(&kmd, "r");
    dnst_kmd_marke(&kmd, "repl");
    dnst_kmd_param(&kmd, "s", "ausführen");
    dnst_kmd_param(&kmd, "start", "ausführen");

    w32 repl = false;
    if (!dnst_kmd_einlesen(&kmd, argc, argv))
    {
        //
    }

    if (dnst_kmd_gesetzt(&kmd, "r") || dnst_kmd_gesetzt(&kmd, "repl"))
    {
        while (!beenden)
        {
            fprintf(stdout, "\n>> ");
            gets_s(repl_inhalt + inhalt_versatz, INHALT_GRÖSSE);
            inhalt_versatz = strlen(repl_inhalt) - 1;

            KMP_lexer_t* lexer = kmp_lexer(&speicher, bss_text("<repl>"), bss_text(repl_inhalt));
            BSS_Feld(KMP_glied_t*) glieder = kmp_lexer_starten(&speicher, lexer, 0);

            KMP_syntax_t* syntax = kmp_syntax(&speicher, glieder);
            KMP_syn_asb_t asb = kmp_syn_starten(&speicher, syntax, 0);

            for (g32 i = 0; i < asb.anweisungen.anzahl_elemente; ++i)
            {
                KMP_syn_anweisung_t* anweisung = bss_feld_element(asb.anweisungen, i);
                kmp_syn_anweisung_ausgeben(&speicher, anweisung, 0, stdout);
            }

            if (syntax->diagnostik.meldungen.anzahl_elemente > 0)
            {
                for (g32 i = 0; i < syntax->diagnostik.meldungen.anzahl_elemente; ++i)
                {
                    KMP_meldung_t* meldung = bss_feld_element(syntax->diagnostik.meldungen, i);
                    kmp_diagnostik_meldung_ausgeben(*meldung, stdout);
                }

                kmp_diagnostik_meldungen_löschen(&syntax->diagnostik);
                continue;
            }

            KMP_semantik_t* semantik = kmp_semantik(&speicher, &asb, system, global);
            KMP_sem_asb_t* sem_asb = kmp_sem_starten(&speicher, semantik, false);
        }
    }
    else
    {
        if (dnst_kmd_freie_werte_anzahl(&kmd) == 0)
        {
            fprintf(stdout, "Fehler: es wurde keine Datei übergeben.\n");
            exit(1);
        }

        BSS_text_t dateiname = bss_text(dnst_kmd_freier_wert(&kmd, 0));
        FILE* datei;
        char* inhalt = NULL;
        g64 größe = 0;

        if (fopen_s(&datei, dateiname.daten, "r") == 0)
        {
            fseek(datei, 0, SEEK_END);
            größe = ftell(datei);
            fseek(datei, 0, SEEK_SET);

            inhalt = speicher.anfordern(&speicher, größe + 1);
            if (inhalt)
            {
                fread(inhalt, 1, größe, datei);
                inhalt[größe] = '\0';
            }

            fclose(datei);
        }

        KMP_lexer_t* lexer = kmp_lexer(&speicher, dateiname, bss_text(inhalt));
        BSS_Feld(KMP_glied_t*) glieder = kmp_lexer_starten(&speicher, lexer, 0);

        KMP_syntax_t* syntax = kmp_syntax(&speicher, glieder);
        KMP_syn_asb_t asb = kmp_syn_starten(&speicher, syntax, 0);

        if (syntax->diagnostik.meldungen.anzahl_elemente > 0)
        {
            for (g32 i = 0; i < syntax->diagnostik.meldungen.anzahl_elemente; ++i)
            {
                KMP_meldung_t* meldung = bss_feld_element(syntax->diagnostik.meldungen, i);
                kmp_diagnostik_meldung_ausgeben(*meldung, stdout);
            }

            kmp_diagnostik_meldungen_löschen(&syntax->diagnostik);
            exit(1);
        }

        KMP_semantik_t* semantik = kmp_semantik(&speicher, &asb, system, global);
        kmp_sem_system_zone_initialisieren(&speicher, semantik->system);
        KMP_sem_asb_t* sem_asb = kmp_sem_starten(&speicher, semantik, true);

        if (syntax->diagnostik.meldungen.anzahl_elemente > 0)
        {
            for (g32 i = 0; i < syntax->diagnostik.meldungen.anzahl_elemente; ++i)
            {
                KMP_meldung_t* meldung = bss_feld_element(syntax->diagnostik.meldungen, i);
                kmp_diagnostik_meldung_ausgeben(*meldung, stdout);
            }

            kmp_diagnostik_meldungen_löschen(&syntax->diagnostik);
        }
    }

    return 0;
}

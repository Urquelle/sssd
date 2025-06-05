#include "kmp/datentyp.h"

#include "kmp/symbol.h"
#include "kmp/zone.h"

#include <assert.h>

KMP_sem_datentyp_t*
kmp_sem_datentyp(BSS_speicher_t *speicher, g32 art, g64 größe, w32 ist_abgeschlossen, g32 merkmale)
{
    KMP_sem_datentyp_t* erg = speicher->anfordern(speicher, sizeof(KMP_sem_datentyp_t));

    erg->art = art;
    erg->größe = größe;
    erg->ist_abgeschlossen = ist_abgeschlossen;
    erg->merkmale = merkmale;

    erg->symbol = NULL;
    erg->deklaration = NULL;

    return erg;
}

KMP_sem_datentyp_t*
kmp_sem_datentyp_feld(BSS_speicher_t *speicher, KMP_sem_datentyp_t *basis, KMP_sem_datentyp_t *index)
{
    KMP_sem_datentyp_t* erg = kmp_sem_datentyp(speicher, KMP_DATENTYP_ART_FELD, 8, false, KMP_DATENTYP_MERKMAL_0);

    erg->feld.basis = basis;
    erg->feld.index = index;

    return erg;
}

KMP_sem_datentyp_t*
kmp_sem_datentyp_zeiger(BSS_speicher_t *speicher, KMP_sem_datentyp_t *basis)
{
    KMP_sem_datentyp_t* erg = kmp_sem_datentyp(speicher, KMP_DATENTYP_ART_ZEIGER, 8, false, KMP_DATENTYP_MERKMAL_0);

    erg->zeiger.basis = basis;

    return erg;
}

KMP_sem_datentyp_t*
kmp_sem_datentyp_schablone(BSS_speicher_t *speicher, KMP_syn_deklaration_t *deklaration)
{
    KMP_sem_datentyp_t* erg = kmp_sem_datentyp(speicher, KMP_DATENTYP_ART_SCHABLONE, 0, false, KMP_DATENTYP_MERKMAL_0);

    erg->deklaration = deklaration;

    return erg;
}

KMP_sem_datentyp_t*
kmp_sem_datentyp_routine(BSS_speicher_t* speicher, BSS_Feld(KMP_sem_datentyp_t*) parameter, KMP_sem_datentyp_t* rückgabe, KMP_syn_deklaration_t* deklaration)
{
    KMP_sem_datentyp_t* erg = kmp_sem_datentyp(speicher, KMP_DATENTYP_ART_ROUTINE, 8, false, KMP_DATENTYP_MERKMAL_0);

    erg->deklaration = deklaration;
    erg->routine.parameter = parameter;
    erg->routine.rückgabe = rückgabe;

    return erg;
}

KMP_sem_datentyp_t*
kmp_sem_datentyp_option(BSS_speicher_t* speicher, KMP_sem_datentyp_t *basis, KMP_syn_deklaration_t* deklaration)
{
    KMP_sem_datentyp_t* erg = kmp_sem_datentyp(speicher, KMP_DATENTYP_ART_OPTION, 8, false, KMP_DATENTYP_MERKMAL_0);

    erg->deklaration = deklaration;
    erg->option.basis = basis;

    return erg;
}

w32
kmp_sem_datentyp_ist_arithmetisch(KMP_sem_datentyp_t *datentyp)
{
    w32 erg = (datentyp->merkmale & KMP_DATENTYP_MERKMAL_ARITHMETISCH) == KMP_DATENTYP_MERKMAL_ARITHMETISCH;

    return erg;
}

void
kmp_sem_datentyp_ausgeben(KMP_sem_datentyp_t *datentyp, int32_t tiefe, FILE *ausgabe)
{
    switch (datentyp->art)
    {
    #define X(N, W, B) case (N): fprintf(ausgabe, "%s", B); break;
        Datentyp_Art_Liste
    #undef X
    }
}

w32
kmp_sem_datentyp_schablone_eigenschaft_hinzufügen(BSS_speicher_t* speicher, KMP_sem_datentyp_t* datentyp, g64 versatz, BSS_text_t name)
{
    KMP_sem_datentyp_schablone_eigenschaft_t* eigenschaft = kmp_sem_datentyp_schablone_eigenschaft(speicher, datentyp, versatz, name);

    for (g32 i = 0; i < datentyp->schablone.eigenschaften.anzahl_elemente; ++i)
    {
        KMP_sem_datentyp_schablone_eigenschaft_t* e = bss_feld_element(datentyp->schablone.eigenschaften, i);
        if (bss_text_ist_gleich(e->name, name))
        {
            return false;
        }
    }

    bss_feld_hinzufügen(speicher, &datentyp->schablone.eigenschaften, eigenschaft);

    return true;
}

w32
kmp_sem_datentyp_option_eigenschaft_hinzufügen(BSS_speicher_t* speicher, KMP_sem_datentyp_t* option, KMP_sem_datentyp_t* datentyp, KMP_sem_operand_t* operand, BSS_text_t name)
{
    KMP_sem_datentyp_option_eigenschaft_t* eigenschaft = kmp_sem_datentyp_option_eigenschaft(speicher, datentyp, operand, name);

    for (g32 i = 0; i < datentyp->option.eigenschaften.anzahl_elemente; ++i)
    {
        KMP_sem_datentyp_option_eigenschaft_t* e = bss_feld_element(datentyp->option.eigenschaften, i);
        if (bss_text_ist_gleich(e->name, name))
        {
            return false;
        }
    }

    bss_feld_hinzufügen(speicher, &datentyp->option.eigenschaften, eigenschaft);

    return true;
}

KMP_sem_datentyp_schablone_eigenschaft_t*
kmp_sem_datentyp_schablone_eigenschaft(BSS_speicher_t* speicher, KMP_sem_datentyp_t *datentyp, g64 versatz, BSS_text_t name)
{
    KMP_sem_datentyp_schablone_eigenschaft_t* erg = speicher->anfordern(speicher, sizeof(KMP_sem_datentyp_schablone_eigenschaft_t));

    erg->datentyp = datentyp;
    erg->versatz = versatz;
    erg->name = name;

    return erg;
}

KMP_sem_datentyp_option_eigenschaft_t*
kmp_sem_datentyp_option_eigenschaft(BSS_speicher_t* speicher, KMP_sem_datentyp_t *datentyp, KMP_sem_operand_t* operand, BSS_text_t name)
{
    KMP_sem_datentyp_option_eigenschaft_t* erg = speicher->anfordern(speicher, sizeof(KMP_sem_datentyp_option_eigenschaft_t));

    erg->datentyp = datentyp;
    erg->operand = operand;
    erg->name = name;

    return erg;
}

KMP_sem_datentyp_kompatibilität_e
kmp_sem_datentypen_kompatibel(KMP_sem_datentyp_t *ziel, KMP_sem_datentyp_t *quelle, w32 implizit)
{
    // INFO: wenn einer davon null ist => ungültig
    if (ziel == NULL || quelle == NULL)
    {
        return KMP_DATENTYP_KOMPATIBILITÄT_INKOMPATIBEL;
    }

    // INFO: wenn beide vom gleichen typ sind => dann kompatibel
    if (ziel == quelle)
    {
        return KMP_DATENTYP_KOMPATIBILITÄT_KOMPATIBEL;
    }

    // INFO: wenn die art gleich ist und der rechte datentyp in den linken
    //       reinpasst => dann kompatibel
    if (ziel->art == quelle->art && ziel->größe >= quelle->größe)
    {
        return KMP_DATENTYP_KOMPATIBILITÄT_KOMPATIBEL;
    }

    switch (ziel->art)
    {
        case KMP_DATENTYP_ART_DEZIMAL_ZAHL:
        {
            // Implizite Konvertierung von ganzen Zahlen zu Dezimalzahlen erlauben
            if (quelle->art == KMP_DATENTYP_ART_GANZE_ZAHL)
            {
                return KMP_DATENTYP_KOMPATIBILITÄT_KOMPATIBEL;
            }
        } break;

        case KMP_DATENTYP_ART_GANZE_ZAHL:
        {
            if (quelle->art == KMP_DATENTYP_ART_GANZE_ZAHL)
            {
                // INFO: Kleinere ganze Zahlen in größere konvertieren
                return (ziel->größe >= quelle->größe)
                    ? KMP_DATENTYP_KOMPATIBILITÄT_KOMPATIBEL
                    : KMP_DATENTYP_KOMPATIBILITÄT_INKOMPATIBEL;
            }

            // INFO: Von bool nach ganzer Zahl nur wenn implizit erlaubt
            if (quelle->art == KMP_DATENTYP_ART_BOOL && implizit)
            {
                return KMP_DATENTYP_KOMPATIBILITÄT_KOMPATIBEL;
            }

            if (quelle->art == KMP_DATENTYP_ART_DEZIMAL_ZAHL && implizit)
            {
                printf("Bei einer Umwandlung einer Dezimalzahl in eine Ganzzahl gehen Daten verloren!\n");

                return KMP_DATENTYP_KOMPATIBILITÄT_KOMPATIBEL_MIT_DATENVERLUST;
            }
        } break;

        case KMP_DATENTYP_ART_BOOL:
        {
            // INFO: Ganze Zahlen nach bool nur wenn implizit erlaubt
            if (quelle->art == KMP_DATENTYP_ART_GANZE_ZAHL && implizit)
            {
                return KMP_DATENTYP_KOMPATIBILITÄT_KOMPATIBEL;
            }

        } break;

        case KMP_DATENTYP_ART_ZEIGER:
        {
            if (quelle->art == KMP_DATENTYP_ART_ZEIGER)
            {
                KMP_sem_datentyp_t* ziel_basis   = ziel->zeiger.basis;
                KMP_sem_datentyp_t* quelle_basis = quelle->zeiger.basis;

                // Nullzeiger erlauben
                if (quelle_basis->art == KMP_DATENTYP_ART_NIHIL)
                {
                    return KMP_DATENTYP_KOMPATIBILITÄT_KOMPATIBEL;
                }

                // Zeiger-Hierarchie prüfen
                return kmp_sem_datentypen_kompatibel(ziel_basis, quelle_basis, false);
            }
        } break;

        case KMP_DATENTYP_ART_SCHABLONE:
        {
            if (quelle->art != KMP_DATENTYP_ART_SCHABLONE)
            {
                return KMP_DATENTYP_KOMPATIBILITÄT_INKOMPATIBEL;
            }

            if (quelle->symbol == ziel->symbol)
            {
                return KMP_DATENTYP_KOMPATIBILITÄT_KOMPATIBEL;
            }

            for (g32 i = 0; i < quelle->schablone.eigenschaften.anzahl_elemente; ++i)
            {
                KMP_sem_datentyp_schablone_eigenschaft_t *eigenschaft = bss_feld_element(quelle->schablone.eigenschaften, i);
                if (!kmp_sem_zone_symbol_suchen(ziel->symbol->zone, eigenschaft->name, false))
                {
                    return KMP_DATENTYP_KOMPATIBILITÄT_INKOMPATIBEL;
                }
            }
        } break;
    }

    return KMP_DATENTYP_KOMPATIBILITÄT_INKOMPATIBEL;
}


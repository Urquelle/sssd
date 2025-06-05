#include "kmp/operand.h"

KMP_sem_operand_t*
kmp_sem_operand_mit_datentyp(BSS_speicher_t* speicher, struct KMP_sem_datentyp_t* datentyp, g32 merkmale)
{
    KMP_sem_operand_t* erg = speicher->anfordern(speicher, sizeof(KMP_sem_operand_t));

    erg->datentyp = datentyp;
    erg->merkmale = merkmale;
    erg->symbol   = NULL;
    erg->wert     = NULL;

    return erg;
}

KMP_sem_operand_t*
kmp_sem_operand_mit_symbol(BSS_speicher_t* speicher, KMP_sem_symbol_t* symbol, g32 merkmale)
{
    KMP_sem_operand_t* erg = speicher->anfordern(speicher, sizeof(KMP_sem_operand_t));

    erg->datentyp = NULL;
    erg->merkmale = merkmale;
    erg->symbol   = symbol;
    erg->wert     = NULL;

    if (symbol != NULL)
    {
        erg->datentyp = symbol->datentyp;
    }

    return erg;
}


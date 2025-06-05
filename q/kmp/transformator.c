#include "kmp/transformator.h"

KMP_transformator_t*
kmp_transformator(BSS_speicher_t* speicher, BSS_Feld(KMP_sem_anweisung_t*) anweisungen)
{
    KMP_transformator_t* erg = speicher->anfordern(speicher, sizeof(KMP_transformator_t*));

    erg->sem_anweisungen = anweisungen;

    return erg;
}

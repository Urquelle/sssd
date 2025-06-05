#include "kmp/zone.h"

#include "basis/feld.h"
#include "kmp/datentyp.h"

KMP_sem_zone_t*
kmp_sem_zone(BSS_speicher_t* speicher, BSS_text_t name, KMP_sem_zone_t* über)
{
    KMP_sem_zone_t* erg = speicher->anfordern(speicher, sizeof(KMP_sem_zone_t));

    if (erg)
    {
        erg->name = name;
        erg->über = über;
        erg->symbole = bss_feld(speicher, sizeof(KMP_sem_symbol_t));
    }

    return erg;
}

KMP_sem_symbol_t *
kmp_sem_zone_symbol(KMP_sem_zone_t* zone, g32 index)
{
    KMP_sem_symbol_t* sym = bss_feld_element(zone->symbole, index);

    return sym;
}

w32
kmp_sem_zone_symbol_registrieren(BSS_speicher_t* speicher, KMP_sem_zone_t* zone, BSS_text_t name, KMP_sem_symbol_t* symbol)
{
    if (kmp_sem_zone_symbol_ist_registriert(zone, name))
    {
        return false;
    }

    bss_feld_hinzufügen(speicher, &zone->symbole, symbol);

    return true;
}

w32
kmp_sem_zone_symbol_ist_registriert(KMP_sem_zone_t* zone, BSS_text_t name)
{
    for (g32 i = 0; i < zone->symbole.anzahl_elemente; ++i)
    {
        KMP_sem_symbol_t* sym = kmp_sem_zone_symbol(zone, i);
        if (bss_text_ist_gleich(sym->name, name))
        {
            return true;
        }
    }

    return false;
}

KMP_sem_symbol_t*
kmp_sem_zone_symbol_suchen(KMP_sem_zone_t* zone, BSS_text_t name, w32 über_suche)
{
    KMP_sem_zone_t* z = zone;

    while (z != NULL)
    {
        for (g32 i = 0; i < z->symbole.anzahl_elemente; ++i)
        {
            KMP_sem_symbol_t* sym = kmp_sem_zone_symbol(z, i);
            if (bss_text_ist_gleich(sym->name, name))
            {
                return sym;
            }
        }

        z = z->über;
    }

    return NULL;
}


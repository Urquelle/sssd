#include "dienste/kmd.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

void
dnst_kmd_init(DNST_kmd_t *kmd)
{
    if (kmd == NULL)
    {
        return;
    }

    kmd->param_anzahl = 0;
    kmd->freie_werte_anzahl = 0;
}

w32
dnst_kmd_param(DNST_kmd_t* kmd, char* param_name, char* standard_wert)
{
    if (kmd == NULL || param_name == NULL || kmd->param_anzahl >= MAX_PARAMS)
    {
        return false;
    }

    DNST_kmd_param_t* param = &kmd->params[kmd->param_anzahl];

    strncpy(param->name, param_name, MAX_PARAM_LAENGE - 1);
    param->name[MAX_PARAM_LAENGE - 1] = '\0';

    param->wert[0] = '\0';
    param->ist_gesetzt = false;
    param->ist_flag = false;

    if (standard_wert != NULL)
    {
        strncpy(param->standard_wert, standard_wert, MAX_PARAM_LAENGE - 1);
        param->standard_wert[MAX_PARAM_LAENGE - 1] = '\0';
        strncpy(param->wert, standard_wert, MAX_PARAM_LAENGE - 1);
        param->wert[MAX_PARAM_LAENGE - 1] = '\0';
    }
    else
    {
        param->standard_wert[0] = '\0';
    }

    kmd->param_anzahl++;
    return true;
}

w32
dnst_kmd_marke(DNST_kmd_t* kmd, char* param_name)
{
    if (kmd == NULL || param_name == NULL || kmd->param_anzahl >= MAX_PARAMS)
    {
        return false;
    }

    DNST_kmd_param_t* param = &kmd->params[kmd->param_anzahl];

    strncpy(param->name, param_name, MAX_PARAM_LAENGE - 1);
    param->name[MAX_PARAM_LAENGE - 1] = '\0';

    param->wert[0] = '\0';
    param->standard_wert[0] = '\0';
    param->ist_gesetzt = false;
    param->ist_flag = true;

    kmd->param_anzahl++;
    return true;
}

w32
dnst_kmd_einlesen(DNST_kmd_t* kmd, int argc, char* argv[])
{
    if (kmd == NULL || argv == NULL)
    {
        return false;
    }

    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-' && strlen(argv[i]) > 1)
        {
            char* param_name = argv[i] + 1;
            if (param_name[0] == '-')
            {
                param_name++;  // Für '--' Parameter
            }

            for (int j = 0; j < kmd->param_anzahl; j++)
            {
                if (strcmp(kmd->params[j].name, param_name) == 0)
                {
                    if (kmd->params[j].ist_flag)
                    {
                        strncpy(kmd->params[j].wert, "true", MAX_PARAM_LAENGE - 1);
                        kmd->params[j].wert[MAX_PARAM_LAENGE - 1] = '\0';
                        kmd->params[j].ist_gesetzt = true;
                    }
                    else if (i + 1 < argc && argv[i + 1][0] != '-')
                    {
                        strncpy(kmd->params[j].wert, argv[i + 1], MAX_PARAM_LAENGE - 1);
                        kmd->params[j].wert[MAX_PARAM_LAENGE - 1] = '\0';
                        kmd->params[j].ist_gesetzt = true;
                        i++;  // Überspringe den Wert
                    }
                    break;
                }
            }
        }
        else
        {
            // Freier Wert gefunden
            if (kmd->freie_werte_anzahl < MAX_FREIE_WERTE)
            {
                strncpy(kmd->freie_werte[kmd->freie_werte_anzahl], argv[i], MAX_PARAM_LAENGE - 1);
                kmd->freie_werte[kmd->freie_werte_anzahl][MAX_PARAM_LAENGE - 1] = '\0';
                kmd->freie_werte_anzahl++;
            }
        }
    }

    return true;
}

char *
dnst_kmd_wert(DNST_kmd_t *kmd, char* param_name)
{
    if (kmd == NULL || param_name == NULL)
    {
        return NULL;
    }

    for (int i = 0; i < kmd->param_anzahl; i++)
    {
        if (strcmp(kmd->params[i].name, param_name) == 0)
        {
            return kmd->params[i].wert;
        }
    }

    return NULL;
}

w32
dnst_kmd_gesetzt(DNST_kmd_t *kmd, char* param_name)
{
    if (kmd == NULL || param_name == NULL)
    {
        return false;
    }

    for (int i = 0; i < kmd->param_anzahl; i++)
    {
        if (strcmp(kmd->params[i].name, param_name) == 0)
        {
            if (!kmd->params[i].ist_flag)
            {
                return false;  // Nur Flags können abgefragt werden
            }
            return kmd->params[i].ist_gesetzt;
        }
    }

    return false;  // Parameter nicht gefunden
}

char*
dnst_kmd_freier_wert(DNST_kmd_t* kmd, g32 index)
{
    if (kmd == NULL || index < 0 || index >= kmd->freie_werte_anzahl)
    {
        return NULL;
    }

    return kmd->freie_werte[index];
}

g32
dnst_kmd_freie_werte_anzahl(DNST_kmd_t* kmd)
{
    if (kmd == NULL)
    {
        return 0;
    }

    return kmd->freie_werte_anzahl;
}

// Freigabe des Parsers
void
dnst_kmd_freigeben(DNST_kmd_t* kmd)
{
    if (kmd != NULL)
    {
        free(kmd);
    }
}


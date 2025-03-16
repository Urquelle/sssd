#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Maximale Länge für Parameter-Namen und Werte
#define MAX_PARAM_LAENGE 50
#define MAX_PARAMS 20
#define MAX_FREIE_WERTE 20

// Datenstruktur für einen einzelnen Parameter
typedef struct
{
    char name[MAX_PARAM_LAENGE];
    char wert[MAX_PARAM_LAENGE];
    char standard_wert[MAX_PARAM_LAENGE];
    bool ist_gesetzt;     // Flag, ob Parameter gesetzt wurde
    bool ist_flag;        // Kennzeichnet ob Parameter ein Flag ist
} Kmd_Param;

// Datenstruktur für die Parameter-Komponente
typedef struct
{
    Kmd_Param params[MAX_PARAMS];
    int param_anzahl;
    char freie_werte[MAX_FREIE_WERTE][MAX_PARAM_LAENGE];
    int freie_werte_anzahl;
} Kmd;

// Initialisierung der Parser-Komponente
void
kmd_init(Kmd *kmd)
{
    if (kmd == NULL)
    {
        return;
    }

    kmd->param_anzahl = 0;
    kmd->freie_werte_anzahl = 0;
}

// Hinzufügen eines Parameters mit Standardwert
bool
kmd_param(Kmd* kmd, const char* param_name, const char* standard_wert)
{
    if (kmd == NULL || param_name == NULL || kmd->param_anzahl >= MAX_PARAMS)
    {
        return false;
    }

    Kmd_Param* param = &kmd->params[kmd->param_anzahl];

    strncpy(param->name, param_name, MAX_PARAM_LAENGE - 1);
    param->name[MAX_PARAM_LAENGE - 1] = '\0';

    param->wert[0] = '\0';
    param->ist_gesetzt = false;
    param->ist_flag = false;

    if (standard_wert != NULL) {
        strncpy(param->standard_wert, standard_wert, MAX_PARAM_LAENGE - 1);
        param->standard_wert[MAX_PARAM_LAENGE - 1] = '\0';
        strncpy(param->wert, standard_wert, MAX_PARAM_LAENGE - 1);
        param->wert[MAX_PARAM_LAENGE - 1] = '\0';
    } else {
        param->standard_wert[0] = '\0';
    }

    kmd->param_anzahl++;
    return true;
}

// Hinzufügen eines Flags
bool
kmd_marke(Kmd* kmd, const char* param_name)
{
    if (kmd == NULL || param_name == NULL || kmd->param_anzahl >= MAX_PARAMS)
    {
        return false;
    }

    Kmd_Param* param = &kmd->params[kmd->param_anzahl];

    strncpy(param->name, param_name, MAX_PARAM_LAENGE - 1);
    param->name[MAX_PARAM_LAENGE - 1] = '\0';

    param->wert[0] = '\0';
    param->standard_wert[0] = '\0';
    param->ist_gesetzt = false;
    param->ist_flag = true;

    kmd->param_anzahl++;
    return true;
}

bool
kmd_einlesen(Kmd* kmd, int argc, char* argv[])
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

// Getter für Parameter-Wert
const char *
kmd_wert(Kmd *kmd, const char* param_name)
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

// Prüfen, ob eine Marke gesetzt ist
bool
kmd_gesetzt(Kmd *kmd, const char* param_name)
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

// Getter für freie Werte
const char *
kmd_freier_wert(Kmd *kmd, int index)
{
    if (kmd == NULL || index < 0 || index >= kmd->freie_werte_anzahl)
    {
        return NULL;
    }

    return kmd->freie_werte[index];
}

// Anzahl der freien Werte
int
kmd_freie_werte_anzahl(Kmd *kmd)
{
    if (kmd == NULL)
    {
        return 0;
    }

    return kmd->freie_werte_anzahl;
}

// Freigabe des Parsers
void
kmd_freigeben(Kmd* kmd)
{
    if (kmd != NULL)
    {
        free(kmd);
    }
}

#if 0
// Beispiel zur Verwendung:
int main(int argc, char* argv[])
{
    Kmd kmd = {};

    // Parameter mit Standardwert
    kmd_param(&kmd, "einstieg", "main");
    // Flag-Parameter
    kmd_marke(&kmd, "verbose");

    kmd_einlesen(&kmd, argc, argv);

    printf("einstieg: %s\n", kmd_wert(&kmd, "einstieg"));
    printf("verbose ist gesetzt: %s\n", kmd_marke_gesetzt(&kmd, "verbose") ? "ja" : "nein");

    printf("Freie Werte (%d):\n", kmd_freie_werte_anzahl(&kmd));
    for (int i = 0; i < kmd_freie_werte_anzahl(&kmd); i++)
    {
        printf("  [%d]: %s\n", i, kmd_freier_wert(&kmd, i));
    }

    return 0;
}
#endif


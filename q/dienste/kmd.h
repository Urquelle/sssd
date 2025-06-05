#ifndef __DIENSTE_KMD_H__
#define __DIENSTE_KMD_H__

#include "basis/definitionen.h"

#define MAX_PARAM_LAENGE 50
#define MAX_PARAMS 20
#define MAX_FREIE_WERTE 20

typedef struct DNST_kmd_param_t DNST_kmd_param_t;
typedef struct DNST_kmd_t       DNST_kmd_t;

struct DNST_kmd_param_t
{
    char name[MAX_PARAM_LAENGE];
    char wert[MAX_PARAM_LAENGE];
    char standard_wert[MAX_PARAM_LAENGE];
    w32 ist_gesetzt;
    w32 ist_flag;
};

struct DNST_kmd_t
{
    DNST_kmd_param_t params[MAX_PARAMS];
    g32 param_anzahl;
    char freie_werte[MAX_FREIE_WERTE][MAX_PARAM_LAENGE];
    g32 freie_werte_anzahl;
};

void dnst_kmd_init(DNST_kmd_t* kmd);
w32 dnst_kmd_param(DNST_kmd_t* kmd, char* param_name, char* standard_wert);
w32 dnst_kmd_marke(DNST_kmd_t* kmd, char* param_name);
w32 dnst_kmd_einlesen(DNST_kmd_t* kmd, int argc, char* argv[]);
char* dnst_kmd_wert(DNST_kmd_t* kmd, char* param_name);
w32 dnst_kmd_gesetzt(DNST_kmd_t* kmd, char* param_name);
char* dnst_kmd_freier_wert(DNST_kmd_t* kmd, int index);
int dnst_kmd_freie_werte_anzahl(DNST_kmd_t *kmd);
void dnst_kmd_freigeben(DNST_kmd_t* kmd);

#endif


#ifndef __KMP_LEXER_H__
#define __KMP_LEXER_H__

#include "kmp/spanne.h"
#include "kmp/diagnostik.h"
#include "kmp/glied.h"

typedef struct KMP_lexer_t KMP_lexer_t;

struct KMP_lexer_t
{
    n32 index;
    BSS_text_t quelle;
    BSS_text_t inhalt;
    KMP_diagnostik_t diagnostik;
};

KMP_lexer_t* kmp_lexer(BSS_speicher_t* speicher, BSS_text_t quelle, BSS_text_t inhalt);
BSS_Feld(KMP_glied_t*) kmp_lexer_starten(BSS_speicher_t* speicher, KMP_lexer_t* lexer, g32 position);

KMP_zeichen_t kmp_lexer_nächstes_zeichen(KMP_lexer_t* lexer, g16 versatz /* = 0*/);
KMP_zeichen_t kmp_lexer_weiter(KMP_lexer_t* lexer, g32 anzahl /* = 1*/);
void kmp_lexer_leerzeichen_einlesen(KMP_lexer_t* lexer);

#endif


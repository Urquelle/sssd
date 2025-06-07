#ifndef __KMP_ZEICHEN_H__
#define __KMP_ZEICHEN_H__

#include <stdint.h>

#include "basis/definitionen.h"
#include "basis/text.h"

typedef struct KMP_zeichen_t KMP_zeichen_t;
struct KMP_zeichen_t
{
    BSS_text_t utf8;
    BSS_text_t quelldatei;
    n32        codepoint;
    BSS_text_t text;
    g32        versatz;
    g8         byte_länge;
};

KMP_zeichen_t kmp_zeichen_mit_daten(BSS_text_t utf8, g32 versatz, g8 byte_länge, BSS_text_t quelldatei, BSS_text_t text);
w32 kmp_zeichen_ist_ascii(KMP_zeichen_t zeichen);
w32 kmp_zeichen_ist_letter(KMP_zeichen_t zeichen);
w32 kmp_zeichen_ist_emoji(KMP_zeichen_t zeichen);
w32 kmp_zeichen_ist_ziffer(KMP_zeichen_t zeichen);
w32 kmp_zeichen_ist_zwj(KMP_zeichen_t zeichen);
w32 kmp_zeichen_ist_kombo(KMP_zeichen_t zeichen);

#endif


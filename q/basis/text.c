#include "basis/text.h"

BSS_text_t
bss_text(char *text)
{
    BSS_text_t erg = {};

    erg.daten = text;
    erg.größe = bss_text_größe(erg);
    erg.länge = bss_text_länge(erg);

    return erg;
}

g64
bss_text_größe(BSS_text_t text)
{
    char* z = text.daten;
    g64 erg = 0;

    while (*z)
    {
        erg += 1;
        z += 1;
    }

    return erg;
}

g64
bss_text_länge(BSS_text_t text)
{
    char* z = text.daten;
    g64 erg = 0;

    while (*z)
    {
        g32 größe = bss_text_utf8_zeichen_größe(z);
        erg += größe;
        z += größe;
    }

    return erg;
}

g32
bss_text_utf8_zeichen_größe(char* text)
{
    g8* bytes = (g8*)text;

    if ((bytes[0] & 0x80) == 0)
    {
        return 1;
    }
    else if ((bytes[0] & 0xE0) == 0xC0)
    {
        return 2;
    }
    else if ((bytes[0] & 0xF0) == 0xE0)
    {
        return 3;
    }
    else if ((bytes[0] & 0xF8) == 0xF0)
    {
        return 4;
    }

    return 0;
}

g32
bss_text_utf8_dekodieren(BSS_text_t text, g8* daten)
{
    g8* bytes = (g8*)text.daten;
    g32 codepoint = 0;

    if ((bytes[0] & 0x80) == 0)
    {
        *daten = 1;
        return bytes[0];
    }
    else if ((bytes[0] & 0xE0) == 0xC0 && text.größe >= 2)
    {
        *daten = 2;
        codepoint = ((bytes[0] & 0x1F) << 6) | (bytes[1] & 0x3F);
    }
    else if ((bytes[0] & 0xF0) == 0xE0 && text.größe >= 3)
    {
        *daten = 3;
        codepoint = ((bytes[0] & 0x0F) << 12) | ((bytes[1] & 0x3F) << 6) | (bytes[2] & 0x3F);
    }
    else if ((bytes[0] & 0xF8) == 0xF0 && text.größe >= 4)
    {
        *daten = 4;
        codepoint = ((bytes[0] & 0x07) << 18) | ((bytes[1] & 0x3F) << 12) |
                    ((bytes[2] & 0x3F) << 6)  |  (bytes[3] & 0x3F);
    }
    else
    {
        *daten = 1;
        return bytes[0];
    }

    return codepoint;
}

w32
bss_text_ist_gleich(BSS_text_t links, BSS_text_t rechts)
{
    if (links.länge != rechts.länge)
    {
        return false;
    }

    if (links.daten == NULL && rechts.daten == NULL)
    {
        return true;
    }

    if (links.daten == NULL || rechts.daten == NULL)
    {
        return false;
    }

    for (g64 position = 0; position < links.länge; position++)
    {
        if (links.daten[position] != rechts.daten[position])
        {
            return false;
        }
    }

    return true;
}

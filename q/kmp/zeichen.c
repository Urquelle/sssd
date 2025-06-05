#include "kmp/zeichen.h"

KMP_zeichen_t
kmp_zeichen()
{
    KMP_zeichen_t erg = {};

    return erg;
}

KMP_zeichen_t
kmp_zeichen_kopie(KMP_zeichen_t original)
{
    KMP_zeichen_t erg = {};

    erg.utf8       = original.utf8;
    erg.codepoint  = original.codepoint;
    erg.text       = original.text;
    erg.quelldatei = original.quelldatei;
    erg.versatz    = original.versatz;
    erg.byte_länge = original.byte_länge;

    return erg;
}

KMP_zeichen_t
kmp_zeichen_mit_daten(BSS_text_t utf8, g32 versatz, g8 byte_länge, BSS_text_t quelldatei, BSS_text_t text)
{
    KMP_zeichen_t erg = {};

    erg.utf8       = utf8;
    erg.text       = text;
    erg.quelldatei = quelldatei;
    erg.versatz    = versatz;
    erg.codepoint  = bss_text_utf8_dekodieren(utf8, &byte_länge);
    erg.byte_länge = byte_länge;

    return erg;
}

w32
kmp_zeichen_ist_ziffer(KMP_zeichen_t zeichen)
{
    w32 erg = zeichen.codepoint >= '0' && zeichen.codepoint <= '9';

    return erg;
}

w32
kmp_zeichen_ist_ascii(KMP_zeichen_t zeichen)
{
    w32 erg = zeichen.codepoint <= 0x7F;

    return erg;
}

w32
kmp_zeichen_ist_letter(KMP_zeichen_t zeichen)
{
    // INFO: https://www.utf8-chartable.de

    // ASCII Buchstaben
    if (zeichen.codepoint <= 0x7F)
    {
        w32 erg = (zeichen.codepoint >= 'A' && zeichen.codepoint <= 'Z') ||
                  (zeichen.codepoint >= 'a' && zeichen.codepoint <= 'z');

        return erg;
    }

    // Unicode Bereiche für Buchstaben

    // Lateinische Zeichen mit Akzenten/Umlaute (Latin-1 Supplement + Latin Extended-A)
    if ((zeichen.codepoint >= 0xC0  && zeichen.codepoint <= 0xFF) ||  // Latin-1 Supplement
        (zeichen.codepoint >= 0x100 && zeichen.codepoint <= 0x17F))  // Latin Extended-A
    {
        // Ausschließen von Sonderzeichen im Latin-1 Supplement
        if (zeichen.codepoint == 0xD7 || zeichen.codepoint == 0xF7)  // × ÷
        {
            return false;
        }

        return true;
    }

    // Weitere lateinische Erweiterungen (Latin Extended-B)
    if (zeichen.codepoint >= 0x180 && zeichen.codepoint <= 0x24F)
    {
        return true;
    }

    // IPA Erweiterungen
    if (zeichen.codepoint >= 0x250 && zeichen.codepoint <= 0x2AF)
    {
        return true;
    }

    // Griechisch
    if (zeichen.codepoint >= 0x370 && zeichen.codepoint <= 0x3FF)
    {
        // Ausschließen von griechischen Zahlen und Symbolen
        if ((zeichen.codepoint >= 0x374 && zeichen.codepoint <= 0x375) ||
            (zeichen.codepoint >= 0x37E && zeichen.codepoint <= 0x37F) ||
            (zeichen.codepoint >= 0x384 && zeichen.codepoint <= 0x385) ||
             zeichen.codepoint == 0x387)
        {
            return false;
        }

        return true;
    }

    // Runen
    if (zeichen.codepoint >= 0x16A0 && zeichen.codepoint <= 0x16F8)
    {
        return true;
    }

    // Hiragana/Katakana
    if ((zeichen.codepoint >= 0x3040 && zeichen.codepoint <= 0x3096) ||
        (zeichen.codepoint >= 0x3099 && zeichen.codepoint <= 0x30FF))
    {
        return true;
    }

    // Kyrillisch
    if (zeichen.codepoint >= 0x400 && zeichen.codepoint <= 0x4FF)
    {
        return true;
    }

    // Armenisch
    if (zeichen.codepoint >= 0x530 && zeichen.codepoint <= 0x58F)
    {
        // Ausschließen von armenischen Zahlen und Symbolen
        if (zeichen.codepoint < 0x531)
        {
            return false;
        }

        return true;
    }

    return false;
}

w32
kmp_zeichen_ist_emoji(KMP_zeichen_t zeichen)
{
    if ((zeichen.codepoint >= 0x1F600 && zeichen.codepoint <= 0x1F64F) || // Emoticons
        (zeichen.codepoint >= 0x1F300 && zeichen.codepoint <= 0x1F5FF) || // Verschiedene Symbole
        (zeichen.codepoint >= 0x1F900 && zeichen.codepoint <= 0x1F9FF) || // Zusätzliche Symbole
        (zeichen.codepoint >= 0x1F680 && zeichen.codepoint <= 0x1F6C5) ||
        (zeichen.codepoint >= 0x1FA70 && zeichen.codepoint <= 0x1FAFF) || // Erweitertes Piktogramm
        (zeichen.codepoint >= 0x2600  && zeichen.codepoint <= 0x26FF)  || // Verschiedene Symbole
        (zeichen.codepoint >= 0x2700  && zeichen.codepoint <= 0x27BF))    // Dingbats
    {
        return true;
    }

    return false;
}

w32
kmp_zeichen_ist_zwj(KMP_zeichen_t zeichen)
{
    w32 erg = zeichen.codepoint == 0x200D;

    return erg;
}

w32
kmp_zeichen_ist_kombo(KMP_zeichen_t zeichen)
{
    w32 erg =  (zeichen.codepoint >= 0x0300 && zeichen.codepoint <= 0x036F) ||
               (zeichen.codepoint == 0x200D) ||
               (zeichen.codepoint >= 0xFE00 && zeichen.codepoint <= 0xFE0F);

    return erg;
}

w32
kmp_zeichen_gleich(KMP_zeichen_t links, KMP_zeichen_t rechts)
{
    w32 erg = links.codepoint == rechts.codepoint;

    return erg;
}


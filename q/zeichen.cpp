#include "zeichen.hpp"

uint32_t utf8_dekodieren(std::string_view sv, uint8_t& daten);

Zeichen::Zeichen()
    : _utf8_char("")
    , _codepoint(0)
    , _byte_länge(0)
{
}

Zeichen::Zeichen(const Zeichen& z)
    : _utf8_char(z.utf8())
    , _codepoint(z.codepoint())
    , _text(z.text())
    , _byte_länge(z.byte_länge())
{
}

Zeichen::Zeichen(std::string_view utf8_char, uint32_t byte_index, std::string& q, std::string& text)
    : _utf8_char(utf8_char)
    , _text(text)
{
    _codepoint = utf8_dekodieren(_utf8_char, _byte_länge);
}

std::string_view
Zeichen::utf8() const
{
    return _utf8_char;
}

std::string
Zeichen::text() const
{
    return _text;
}

uint32_t
Zeichen::codepoint() const
{
    return _codepoint;
}

bool
Zeichen::ist_ziffer() const
{
    auto erg = _codepoint >= '0' && _codepoint <= '9';

    return erg;
}

bool
Zeichen::ist_ascii() const
{
    return _codepoint <= 0x7F;
}

bool
Zeichen::ist_letter() const
{
    // INFO: https://www.utf8-chartable.de

    // ASCII Buchstaben
    if (_codepoint <= 0x7F)
    {
        return (_codepoint >= 'A' && _codepoint <= 'Z') ||
               (_codepoint >= 'a' && _codepoint <= 'z');
    }

    // Unicode Bereiche für Buchstaben

    // Lateinische Zeichen mit Akzenten/Umlaute (Latin-1 Supplement + Latin Extended-A)
    if ((_codepoint >= 0xC0 && _codepoint <= 0xFF) ||  // Latin-1 Supplement
        (_codepoint >= 0x100 && _codepoint <= 0x17F))  // Latin Extended-A
    {
        // Ausschließen von Sonderzeichen im Latin-1 Supplement
        if (_codepoint == 0xD7 || _codepoint == 0xF7)  // × ÷
        {
            return false;
        }
        return true;
    }

    // Weitere lateinische Erweiterungen (Latin Extended-B)
    if (_codepoint >= 0x180 && _codepoint <= 0x24F)
    {
        return true;
    }

    // IPA Erweiterungen
    if (_codepoint >= 0x250 && _codepoint <= 0x2AF)
    {
        return true;
    }

    // Griechisch
    if (_codepoint >= 0x370 && _codepoint <= 0x3FF)
    {
        // Ausschließen von griechischen Zahlen und Symbolen
        if ((_codepoint >= 0x374 && _codepoint <= 0x375) ||
            (_codepoint >= 0x37E && _codepoint <= 0x37F) ||
            (_codepoint >= 0x384 && _codepoint <= 0x385) ||
            _codepoint == 0x387)
        {
            return false;
        }
        return true;
    }

    // Runen
    if (_codepoint >= 0x16A0 && _codepoint <= 0x16F8)
    {
        return true;
    }

    // Hiragana/Katakana
    if ((_codepoint >= 0x3040 && _codepoint <= 0x3096) ||
        (_codepoint >= 0x3099 && _codepoint <= 0x30FF))
    {
        return true;
    }

    // Kyrillisch
    if (_codepoint >= 0x400 && _codepoint <= 0x4FF)
    {
        return true;
    }

    // Armenisch
    if (_codepoint >= 0x530 && _codepoint <= 0x58F)
    {
        // Ausschließen von armenischen Zahlen und Symbolen
        if (_codepoint < 0x531)
        {
            return false;
        }
        return true;
    }

    return false;
}

bool
Zeichen::ist_emoji() const
{
    if ((_codepoint >= 0x1F600 && _codepoint <= 0x1F64F) || // Emoticons
        (_codepoint >= 0x1F300 && _codepoint <= 0x1F5FF) || // Verschiedene Symbole
        (_codepoint >= 0x1F900 && _codepoint <= 0x1F9FF) || // Zusätzliche Symbole und Piktogramme
        (_codepoint >= 0x1F680 && _codepoint <= 0x1F6C5) ||
        (_codepoint >= 0x1FA70 && _codepoint <= 0x1FAFF))   // Erweitertes Piktogramm-Bereich-Symbole
    {
        return true;
    }

    return false;
}

uint8_t
Zeichen::byte_länge() const
{
    return _byte_länge;
}

inline bool operator==(const Zeichen& lhs, const Zeichen& rhs)
{
    return lhs.codepoint() == rhs.codepoint();
}

inline bool operator!=(const Zeichen& lhs, const Zeichen& rhs)
{
    return !(lhs == rhs);
}

uint32_t utf8_dekodieren(std::string_view sv, uint8_t& daten)
{
    const uint8_t* bytes = reinterpret_cast<const uint8_t*>(sv.data());
    uint32_t codepoint = 0;

    if ((bytes[0] & 0x80) == 0)
    {
        daten = 1;
        return bytes[0];
    }
    else if ((bytes[0] & 0xE0) == 0xC0 && sv.length() >= 2)
    {
        daten = 2;
        codepoint = ((bytes[0] & 0x1F) << 6) | (bytes[1] & 0x3F);
    }
    else if ((bytes[0] & 0xF0) == 0xE0 && sv.length() >= 3)
    {
        daten = 3;
        codepoint = ((bytes[0] & 0x0F) << 12) | ((bytes[1] & 0x3F) << 6) | (bytes[2] & 0x3F);
    }
    else if ((bytes[0] & 0xF8) == 0xF0 && sv.length() >= 4)
    {
        daten = 4;
        codepoint = ((bytes[0] & 0x07) << 18) | ((bytes[1] & 0x3F) << 12) |
                   ((bytes[2] & 0x3F) << 6) | (bytes[3] & 0x3F);
    }
    else
    {
        daten = 1;  // Invalid UTF-8 sequence, treat as single byte
        return bytes[0];
    }

    return codepoint;
}


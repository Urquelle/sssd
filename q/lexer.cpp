#include "lexer.hpp"

#include "token.hpp"

namespace Sss {

Lexer::Lexer(std::string quelle, std::string inhalt)
    : _quelle(quelle)
    , _inhalt(inhalt)
{
    //
}

std::vector<Token *>
Lexer::starten(int32_t position)
{
    uint32_t ziffern['g'];

    ziffern['0'] = 0;
    ziffern['1'] = 1;
    ziffern['2'] = 2;
    ziffern['3'] = 3;
    ziffern['4'] = 4;
    ziffern['5'] = 5;
    ziffern['6'] = 6;
    ziffern['7'] = 7;
    ziffern['8'] = 8;
    ziffern['9'] = 9;
    ziffern['A'] = 10;
    ziffern['B'] = 11;
    ziffern['C'] = 12;
    ziffern['D'] = 13;
    ziffern['E'] = 14;
    ziffern['F'] = 15;
    ziffern['a'] = 10;
    ziffern['b'] = 11;
    ziffern['c'] = 12;
    ziffern['d'] = 13;
    ziffern['e'] = 14;
    ziffern['f'] = 15;

    std::vector<Token *> erg;

    _index = position;

    for (;;)
    {
        leerzeichen_einlesen();
        auto z = zeichen();
        auto anfang = z;

        if (z.codepoint() == '\0')
        {
            erg.push_back(new Token(Token::ENDE, Spanne(anfang, weiter())));

            break;
        }

        else if (z.codepoint() == '@')
        {
            weiter();
            erg.push_back(new Token(Token::KLAMMERAFFE, Spanne(anfang, z)));
        }

        else if (z.codepoint() == '.')
        {
            weiter();
            erg.push_back(new Token(Token::PUNKT, Spanne(anfang, z)));
        }

        else if (z.codepoint() == ',')
        {
            weiter();
            erg.push_back(new Token(Token::KOMMA, Spanne(anfang, z)));
        }

        else if (z.codepoint() == ':')
        {
            weiter();
            erg.push_back(new Token(Token::DOPPELPUNKT, Spanne(anfang, z)));
        }

        else if (z.codepoint() == ';')
        {
            weiter();
            erg.push_back(new Token(Token::STRICHPUNKT, Spanne(anfang, z)));
        }

        else if (z.codepoint() == '(')
        {
            weiter();
            erg.push_back(new Token(Token::RKLAMMER_AUF, Spanne(anfang, z)));
        }

        else if (z.codepoint() == ')')
        {
            weiter();
            erg.push_back(new Token(Token::RKLAMMER_ZU, Spanne(anfang, z)));
        }

        else if (z.codepoint() == '[')
        {
            weiter();
            erg.push_back(new Token(Token::EKLAMMER_AUF, Spanne(anfang, z)));
        }

        else if (z.codepoint() == ']')
        {
            weiter();
            erg.push_back(new Token(Token::EKLAMMER_ZU, Spanne(anfang, z)));
        }

        else if (z.codepoint() == '{')
        {
            weiter();
            erg.push_back(new Token(Token::GKLAMMER_AUF, Spanne(anfang, z)));
        }

        else if (z.codepoint() == '}')
        {
            weiter();
            erg.push_back(new Token(Token::GKLAMMER_ZU, Spanne(anfang, z)));
        }

        else if (z.codepoint() == '+')
        {
            weiter();
            erg.push_back(new Token(Token::PLUS, Spanne(anfang, z)));
        }

        else if (z.codepoint() == '-')
        {
            weiter();
            erg.push_back(new Token(Token::MINUS, Spanne(anfang, z)));
        }

        else if (z.codepoint() == '*')
        {
            weiter();
            erg.push_back(new Token(Token::STERN, Spanne(anfang, z)));
        }

        else if (z.codepoint() == '/')
        {
            weiter();
            erg.push_back(new Token(Token::PISA, Spanne(anfang, z)));
        }

        else if (z.codepoint() == '#')
        {
            z = weiter();

            // INFO: kommentare
            if (z.codepoint() == '#')
            {
                z = weiter();

                int rekursion = 1;
wiederholung:

                if ( z.codepoint() == '(' )
                {
                    z = weiter();

                    while (rekursion)
                    {
                        while (z.codepoint() != '#' || zeichen(1).codepoint() != '#' || zeichen(2).codepoint() != ')')
                        {
                            z = weiter();

                            if (z.codepoint() == '#' && zeichen(1).codepoint() == '#' && zeichen(2).codepoint() == '(')
                            {
                                z = weiter(2);

                                rekursion++;
                                goto wiederholung;
                            }
                        }

                        rekursion--;

                        z = weiter(3);
                    }
                }
                else
                {
                    while (z.codepoint() != '\n' && z.codepoint() != '\r')
                    {
                        z = weiter();
                    }
                }
            }
            else
            {
                erg.push_back(new Token(Token::RAUTE, Spanne(anfang, zeichen(-1))));
            }
        }

        else if (z.codepoint() == '!')
        {
            z = weiter();

            if (z.codepoint() == '=')
            {
                weiter();

                erg.push_back(new Token(Token::UNGLEICH, Spanne(anfang, z)));
            }
            else
            {
                erg.push_back(new Token(Token::AUSRUFEZEICHEN, Spanne(anfang, zeichen(-1))));
            }
        }

        else if (z.codepoint() == '%')
        {
            weiter();
            erg.push_back(new Token(Token::PROZENT, Spanne(anfang, z)));
        }

        else if (z.codepoint() == '&')
        {
            weiter();
            erg.push_back(new Token(Token::KAUFMANNSUND, Spanne(anfang, z)));
        }

        else if (z.codepoint() == '=')
        {
            z = weiter();

            if (z.codepoint() == '=')
            {
                weiter();
                erg.push_back(new Token(Token::GLEICH, Spanne(anfang, z)));
            }
            else
            {
                erg.push_back(new Token(Token::ZUWEISUNG, Spanne(anfang, zeichen(-1))));
            }
        }

        else if (z.codepoint() == '<')
        {
            z = weiter();

            if (z.codepoint() == '=')
            {
                weiter();
                erg.push_back(new Token(Token::KLEINER_GLEICH, Spanne(anfang, z)));
            }
            else
            {
                erg.push_back(new Token(Token::KLEINER, Spanne(anfang, zeichen(-1))));
            }
        }

        else if (z.codepoint() == '>')
        {
            z = weiter();

            if (z.codepoint() == '=')
            {
                weiter();
                erg.push_back(new Token(Token::GRÖSSER_GLEICH, Spanne(anfang, z)));
            }
            else
            {
                erg.push_back(new Token(Token::GRÖSSER, Spanne(anfang, zeichen(-1))));
            }
        }

        // INFO: text
        else if (z.codepoint() == L'»')
        {
            z = weiter();
            anfang = z; // um das initiale " nicht in der länge mitzuzählen
            auto ende = z;

            while (z.codepoint() != L'«')
            {
                if (z.codepoint() == '\\')
                {
                    z = weiter();
                }

                ende = z;
                z = weiter();
            }

            if (z.codepoint() == L'«')
            {
                weiter();
            }

            // AUFGABE: hartkodierte -3 weil die anführungszeichen aus mehreren bytes bestehen.
            //          aber eigentlich sollte die zeichen() funktion so implementiert werden,
            //          dass sie mit negativem versatz umgehen kann und die angabe die anzahl
            //          der zeichen ist, und nicht die anzahl der bytes.
            erg.push_back(Token::Text(Spanne(anfang, zeichen(-3))));
        }

        // INFO: name
        else if (z.ist_letter() || z.ist_emoji() || z.codepoint() == '_')
        {
            while (z.ist_letter() || z.ist_emoji() || z.ist_ziffer() || z.codepoint() == '_')
            {
                z = weiter();
            }

            erg.push_back(Token::Bezeichner(Spanne(anfang, zeichen(-1))));
        }

        // INFO: zahlen
        else if (z.ist_ziffer())
        {
            uint32_t wert = 0;

            while (z.ist_ziffer())
            {
                if (z.codepoint() == '_')
                {
                    z = weiter();
                    continue;
                }

                wert *= 10;
                wert += ziffern[z.codepoint()];

                z = weiter();
            }

            if (z.codepoint() == 'b')
            {
                z = weiter();
                uint32_t basis = wert;
                wert = 0;

                while (std::isalnum(z.codepoint()) || z.codepoint() == '_')
                {
                    if (z.codepoint() == '_')
                    {
                        z = weiter();
                        continue;
                    }

                    if (z.codepoint() >= '0' && z.codepoint() <= '9' ||
                        z.codepoint() >= 'a' && z.codepoint() <= 'f' ||
                        z.codepoint() >= 'A' && z.codepoint() <= 'F')
                    {
                        wert *= basis;
                        wert += ziffern[z.codepoint()];
                        z = weiter();
                    }
                    else
                    {
                        break;
                    }
                }

                erg.push_back(Token::Ganzzahl(Spanne(anfang, z), wert, basis));
            }
            else if (z.codepoint() == ',')
            {
                weiter();
                z = zeichen();

                float dezimalwert = wert;
                int32_t faktor = 10;
                while (z.ist_ziffer() || z.codepoint() == '_')
                {
                    if (z.codepoint() == '_')
                    {
                        continue;
                    }

                    dezimalwert += ((float) ziffern[z.codepoint()] / faktor);
                    faktor *= 10;

                    z = weiter();
                }

                erg.push_back(Token::Dezimalzahl(Spanne(anfang, z), dezimalwert));
            }
            else
            {
                erg.push_back(Token::Ganzzahl(Spanne(anfang, z), wert));
            }
        }
    }

    return erg;
}

void
Lexer::leerzeichen_einlesen()
{
    auto z = zeichen();

    while (z.codepoint() == ' '  ||
           z.codepoint() == '\n' ||
           z.codepoint() == '\r' ||
           z.codepoint() == '\t')
    {
        z = weiter();
    }
}

Zeichen
Lexer::zeichen(int16_t versatz)
{
    int32_t byte_index = _index + versatz;

    if (byte_index >= static_cast<int32_t>(_inhalt.size()) || byte_index < 0)
    {
        return Zeichen(std::string_view("\0", 1), byte_index, _quelle, _inhalt);
    }

    const uint8_t* bytes = reinterpret_cast<const uint8_t*>(_inhalt.data() + byte_index);
    uint8_t zeichen_länge = 1;

    if ((bytes[0] & 0x80) == 0)
    {
        zeichen_länge = 1;
    }
    else if ((bytes[0] & 0xE0) == 0xC0)
    {
        zeichen_länge = 2;
    }
    else if ((bytes[0] & 0xF0) == 0xE0)
    {
        zeichen_länge = 3;
    }
    else if ((bytes[0] & 0xF8) == 0xF0)
    {
        zeichen_länge = 4;
    }

    if (byte_index + zeichen_länge > static_cast<int32_t>(_inhalt.size()))
    {
        zeichen_länge = 1;
    }

    return Zeichen(std::string_view(_inhalt.data() + byte_index, zeichen_länge), byte_index, _quelle, _inhalt);
}

Zeichen
Lexer::weiter(int32_t anzahl)
{
    for (int32_t i = 0; i < anzahl; ++i)
    {
        _index += zeichen().byte_länge();
    }

    return zeichen();
}

}

#include "kmp/lexer.h"

KMP_lexer_t*
kmp_lexer(BSS_speicher_t* speicher, BSS_text_t quelle, BSS_text_t inhalt)
{
    KMP_lexer_t* erg = speicher->anfordern(speicher, sizeof(KMP_lexer_t));

    erg->index = 0;
    erg->quelle = quelle;
    erg->inhalt = inhalt;

    return erg;
}

BSS_Feld(KMP_glied_t*)
kmp_lexer_starten(BSS_speicher_t* speicher, KMP_lexer_t* lexer, g32 position)
{
    g32 ziffern['g'];

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

    BSS_Feld(KMP_glied_t*) erg;

    lexer->index = position;

    for (;;)
    {
        kmp_lexer_leerzeichen_einlesen(lexer);
        KMP_zeichen_t z = kmp_lexer_nächstes_zeichen(lexer, 0);
        KMP_zeichen_t anfang = z;

        if (z.codepoint == '\0')
        {
            KMP_spanne_t spanne = kmp_spanne_zeichen(anfang, kmp_lexer_weiter(lexer, 1));
            bss_feld_hinzufügen(speicher, &erg, kmp_glied(speicher, GLIED_ENDE, spanne));
            break;
        }

        else if (z.codepoint == '@')
        {
            kmp_lexer_weiter(lexer, 1);
            KMP_spanne_t spanne = kmp_spanne_zeichen(anfang, z);
            bss_feld_hinzufügen(speicher, &erg, kmp_glied(speicher, GLIED_KLAMMERAFFE, spanne));
        }

        else if (z.codepoint == '.')
        {
            z = kmp_lexer_weiter(lexer, 1);

            if (z.codepoint == '.')
            {
                kmp_lexer_weiter(lexer, 1);
                KMP_spanne_t spanne = kmp_spanne_zeichen(anfang, z);
                bss_feld_hinzufügen(speicher, &erg, kmp_glied(speicher, GLIED_PUNKT_PUNKT, spanne));
            }
            else
            {
                KMP_spanne_t spanne = kmp_spanne_zeichen(anfang, kmp_lexer_nächstes_zeichen(lexer, -1));
                bss_feld_hinzufügen(speicher, &erg, kmp_glied(speicher, GLIED_PUNKT, spanne));
            }
        }

        else if (z.codepoint == ',')
        {
            kmp_lexer_weiter(lexer, 1);
            KMP_spanne_t spanne = kmp_spanne_zeichen(anfang, z);
            bss_feld_hinzufügen(speicher, &erg, kmp_glied(speicher, GLIED_KOMMA, spanne));
        }

        else if (z.codepoint == ':')
        {
            kmp_lexer_weiter(lexer, 1);
            bss_feld_hinzufügen(speicher, &erg, kmp_glied(speicher, GLIED_DOPPELPUNKT, kmp_spanne_zeichen(anfang, z)));
        }

        else if (z.codepoint == ';')
        {
            kmp_lexer_weiter(lexer, 1);
            KMP_spanne_t spanne = kmp_spanne_zeichen(anfang, z);
            bss_feld_hinzufügen(speicher, &erg, kmp_glied(speicher, GLIED_STRICHPUNKT, spanne));
        }

        else if (z.codepoint == '|')
        {
            z = kmp_lexer_weiter(lexer, 1);

            if (z.codepoint == '>')
            {
                kmp_lexer_weiter(lexer, 1);
                KMP_spanne_t spanne = kmp_spanne_zeichen(anfang, z);
                bss_feld_hinzufügen(speicher, &erg, kmp_glied(speicher, GLIED_DREIECK_RECHTS, spanne));
            }
            else
            {
                KMP_spanne_t spanne = kmp_spanne_zeichen(anfang, kmp_lexer_nächstes_zeichen(lexer, -1));
                bss_feld_hinzufügen(speicher, &erg, kmp_glied(speicher, GLIED_BALKEN, spanne));
            }
        }

        else if (z.codepoint == '(')
        {
            kmp_lexer_weiter(lexer, 1);
            KMP_spanne_t spanne = kmp_spanne_zeichen(anfang, z);
            bss_feld_hinzufügen(speicher, &erg, kmp_glied(speicher, GLIED_RKLAMMER_AUF, spanne));
        }

        else if (z.codepoint == ')')
        {
            kmp_lexer_weiter(lexer, 1);
            KMP_spanne_t spanne = kmp_spanne_zeichen(anfang, z);
            bss_feld_hinzufügen(speicher, &erg, kmp_glied(speicher, GLIED_RKLAMMER_ZU, spanne));
        }

        else if (z.codepoint == '[')
        {
            kmp_lexer_weiter(lexer, 1);
            KMP_spanne_t spanne = kmp_spanne_zeichen(anfang, z);
            bss_feld_hinzufügen(speicher, &erg, kmp_glied(speicher, GLIED_EKLAMMER_AUF, spanne));
        }

        else if (z.codepoint == ']')
        {
            kmp_lexer_weiter(lexer, 1);
            KMP_spanne_t spanne = kmp_spanne_zeichen(anfang, z);
            bss_feld_hinzufügen(speicher, &erg, kmp_glied(speicher, GLIED_EKLAMMER_ZU, spanne));
        }

        else if (z.codepoint == '{')
        {
            kmp_lexer_weiter(lexer, 1);
            KMP_spanne_t spanne = kmp_spanne_zeichen(anfang, z);
            bss_feld_hinzufügen(speicher, &erg, kmp_glied(speicher, GLIED_GKLAMMER_AUF, spanne));
        }

        else if (z.codepoint == '}')
        {
            kmp_lexer_weiter(lexer, 1);
            KMP_spanne_t spanne = kmp_spanne_zeichen(anfang, z);
            bss_feld_hinzufügen(speicher, &erg, kmp_glied(speicher, GLIED_GKLAMMER_ZU, spanne));
        }

        else if (z.codepoint == '+')
        {
            z = kmp_lexer_weiter(lexer, 1);

            if (z.codepoint == '=')
            {
                kmp_lexer_weiter(lexer, 1);
                KMP_spanne_t spanne = kmp_spanne_zeichen(anfang, z);
                bss_feld_hinzufügen(speicher, &erg, kmp_glied(speicher, GLIED_ZUWEISUNG_PLUS, spanne));
            }
            else
            {
                KMP_spanne_t spanne = kmp_spanne_zeichen(anfang, z);
                bss_feld_hinzufügen(speicher, &erg, kmp_glied(speicher, GLIED_PLUS, spanne));
            }
        }

        else if (z.codepoint == '-')
        {
            z = kmp_lexer_weiter(lexer, 1);

            if (z.codepoint == '>')
            {
                kmp_lexer_weiter(lexer, 1);
                KMP_spanne_t spanne = kmp_spanne_zeichen(anfang, z);
                bss_feld_hinzufügen(speicher, &erg, kmp_glied(speicher, GLIED_PFEIL, spanne));
            }
            else
            {
                KMP_spanne_t spanne = kmp_spanne_zeichen(anfang, kmp_lexer_nächstes_zeichen(lexer, -1));
                bss_feld_hinzufügen(speicher, &erg, kmp_glied(speicher, GLIED_MINUS, spanne));
            }
        }

        else if (z.codepoint == '*')
        {
            kmp_lexer_weiter(lexer, 1);
            KMP_spanne_t spanne = kmp_spanne_zeichen(anfang, z);
            bss_feld_hinzufügen(speicher, &erg, kmp_glied(speicher, GLIED_STERN, spanne));
        }

        else if (z.codepoint == '/')
        {
            kmp_lexer_weiter(lexer, 1);
            KMP_spanne_t spanne = kmp_spanne_zeichen(anfang, z);
            bss_feld_hinzufügen(speicher, &erg, kmp_glied(speicher, GLIED_PISA, spanne));
        }

        else if (z.codepoint == '#')
        {
            z = kmp_lexer_weiter(lexer, 1);

            // INFO: kommentare
            if (z.codepoint == '#')
            {
                z = kmp_lexer_weiter(lexer, 1);

                g32 rekursion = 1;

                if (z.codepoint == '(')
                {
                    z = kmp_lexer_weiter(lexer, 1);

wiederholung:
                    while (rekursion)
                    {
                        // Prüfen auf Dateiende
                        if (z.codepoint == '\0')
                        {
                            // Kommentar wurde nicht geschlossen - hier könnte ein Fehler ausgegeben werden
                            break;
                        }

                        // Prüfen, ob der Kommentar ordnungsgemäß geschlossen wird
                        if (z.codepoint == '#' &&
                            kmp_lexer_nächstes_zeichen(lexer, 1).codepoint == '#' &&
                            kmp_lexer_nächstes_zeichen(lexer, 2).codepoint == ')')
                        {
                            z = kmp_lexer_weiter(lexer, 3);
                            rekursion--;
                        }
                        // Prüfen auf verschachtelte Kommentare
                        else if (z.codepoint == '#' &&
                                 kmp_lexer_nächstes_zeichen(lexer, 1).codepoint == '#' &&
                                 kmp_lexer_nächstes_zeichen(lexer, 2).codepoint == '(')
                        {
                            z = kmp_lexer_weiter(lexer, 3);
                            rekursion++;
                            goto wiederholung;
                        }
                        else
                        {
                            z = kmp_lexer_weiter(lexer, 1);
                        }
                    }
                }
                else
                {
                    // Einzeiliger Kommentar
                    while (z.codepoint != '\0' && z.codepoint != '\n' && z.codepoint != '\r')
                    {
                        z = kmp_lexer_weiter(lexer, 1);
                    }
                }
            }
            else
            {
                KMP_spanne_t spanne = kmp_spanne_zeichen(anfang, kmp_lexer_nächstes_zeichen(lexer, -1));
                bss_feld_hinzufügen(speicher, &erg, kmp_glied(speicher, GLIED_RAUTE, spanne));
            }
        }

        else if (z.codepoint == '!')
        {
            z = kmp_lexer_weiter(lexer, 1);

            if (z.codepoint == '=')
            {
                kmp_lexer_weiter(lexer, 1);
                KMP_spanne_t spanne = kmp_spanne_zeichen(anfang, z);
                bss_feld_hinzufügen(speicher, &erg, kmp_glied(speicher, GLIED_UNGLEICH, spanne));
            }
            else
            {
                KMP_spanne_t spanne = kmp_spanne_zeichen(anfang, kmp_lexer_nächstes_zeichen(lexer, -1));
                bss_feld_hinzufügen(speicher, &erg, kmp_glied(speicher, GLIED_AUSRUFEZEICHEN, spanne));
            }
        }

        else if (z.codepoint == '%')
        {
            kmp_lexer_weiter(lexer, 1);
            KMP_spanne_t spanne = kmp_spanne_zeichen(anfang, z);
            bss_feld_hinzufügen(speicher, &erg, kmp_glied(speicher, GLIED_PROZENT, spanne));
        }

        else if (z.codepoint == '&')
        {
            kmp_lexer_weiter(lexer, 1);
            KMP_spanne_t spanne = kmp_spanne_zeichen(anfang, z);
            bss_feld_hinzufügen(speicher, &erg, kmp_glied(speicher, GLIED_KAUFMANNSUND, spanne));
        }

        else if (z.codepoint == '=')
        {
            z = kmp_lexer_weiter(lexer, 1);

            if (z.codepoint == '=')
            {
                kmp_lexer_weiter(lexer, 1);
                KMP_spanne_t spanne = kmp_spanne_zeichen(anfang, z);
                bss_feld_hinzufügen(speicher, &erg, kmp_glied(speicher, GLIED_GLEICH, spanne));
            }
            else if (z.codepoint == '>')
            {
                kmp_lexer_weiter(lexer, 1);
                KMP_spanne_t spanne = kmp_spanne_zeichen(anfang, z);
                bss_feld_hinzufügen(speicher, &erg, kmp_glied(speicher, GLIED_FOLGERUNG, spanne));
            }
            else
            {
                KMP_spanne_t spanne = kmp_spanne_zeichen(anfang, kmp_lexer_nächstes_zeichen(lexer, -1));
                bss_feld_hinzufügen(speicher, &erg, kmp_glied(speicher, GLIED_ZUWEISUNG, spanne));
            }
        }

        else if (z.codepoint == '<')
        {
            z = kmp_lexer_weiter(lexer, 1);

            if (z.codepoint == '=')
            {
                kmp_lexer_weiter(lexer, 1);
                KMP_spanne_t spanne = kmp_spanne_zeichen(anfang, z);
                bss_feld_hinzufügen(speicher, &erg, kmp_glied(speicher, GLIED_KLEINER_GLEICH, spanne));
            }
            else if (z.codepoint == '|')
            {
                kmp_lexer_weiter(lexer, 1);
                KMP_spanne_t spanne = kmp_spanne_zeichen(anfang, z);
                bss_feld_hinzufügen(speicher, &erg, kmp_glied(speicher, GLIED_DREIECK_LINKS, spanne));
            }
            else
            {
                KMP_spanne_t spanne = kmp_spanne_zeichen(anfang, kmp_lexer_nächstes_zeichen(lexer, -1));
                bss_feld_hinzufügen(speicher, &erg, kmp_glied(speicher, GLIED_KLEINER, spanne));
            }
        }

        else if (z.codepoint == '>')
        {
            z = kmp_lexer_weiter(lexer, 1);

            if (z.codepoint == '=')
            {
                kmp_lexer_weiter(lexer, 1);
                KMP_spanne_t spanne = kmp_spanne_zeichen(anfang, z);
                bss_feld_hinzufügen(speicher, &erg, kmp_glied(speicher, GLIED_GRÖẞER_GLEICH, spanne));
            }
            else
            {
                KMP_spanne_t spanne = kmp_spanne_zeichen(anfang, kmp_lexer_nächstes_zeichen(lexer, -1));
                bss_feld_hinzufügen(speicher, &erg, kmp_glied(speicher, GLIED_GRÖẞER, spanne));
            }
        }

        // INFO: text
        else if (z.codepoint == L'»' || z.codepoint == '"')
        {
            char abschluss = z.codepoint == L'»' ? L'«' : '"';

            z = kmp_lexer_weiter(lexer, 1);
            anfang = z; // um das initiale » nicht in der länge mitzuzählen
            KMP_zeichen_t ende = z;

            while (z.codepoint != abschluss)
            {
                if (z.codepoint == '\\')
                {
                    z = kmp_lexer_weiter(lexer, 1);
                }

                ende = z;
                z = kmp_lexer_weiter(lexer, 1);
            }

            if (z.codepoint == abschluss)
            {
                kmp_lexer_weiter(lexer, 1);
            }

            // AUFGABE: hartkodierte -3 weil die anführungszeichen aus mehreren bytes bestehen.
            //          aber eigentlich sollte die kmp_lexer_nächstes_zeichen(lexer) funktion so implementiert werden,
            //          dass sie mit negativem versatz umgehen kann und die angabe die anzahl
            //          der zeichen ist, und nicht die anzahl der bytes; wir sollten einfach
            //          sagen können, dass wir das zeichen davor haben wollen.
            KMP_spanne_t spanne = kmp_spanne_zeichen(anfang, kmp_lexer_nächstes_zeichen(lexer, -3));
            bss_feld_hinzufügen(speicher, &erg, kmp_glied(speicher, GLIED_TEXT, spanne));
        }

        // INFO: bezeichner
        else if (kmp_zeichen_ist_letter(z) || kmp_zeichen_ist_emoji(z) || z.codepoint == '_')
        {
            while (kmp_zeichen_ist_letter(z) ||
                   kmp_zeichen_ist_emoji(z)  ||
                   kmp_zeichen_ist_ziffer(z) ||
                   kmp_zeichen_ist_zwj(z)    ||
                   kmp_zeichen_ist_kombo(z)  ||
                   z.codepoint == '_')
            {
                z = kmp_lexer_weiter(lexer, 1);
            }

            KMP_spanne_t spanne = kmp_spanne_zeichen(anfang, kmp_lexer_nächstes_zeichen(lexer, -1));
            bss_feld_hinzufügen(speicher, &erg, kmp_glied(speicher, GLIED_BEZEICHNER, spanne));
        }

        // INFO: zahlen
        else if (kmp_zeichen_ist_ziffer(z))
        {
            uint32_t wert = 0;
            KMP_zeichen_t ende = z;

            while (kmp_zeichen_ist_ziffer(z))
            {
                if (z.codepoint == '_')
                {
                    z = kmp_lexer_weiter(lexer, 1);
                    continue;
                }

                wert *= 10;
                wert += ziffern[z.codepoint];

                ende = z;
                z = kmp_lexer_weiter(lexer, 1);
            }

            if (z.codepoint == 'b')
            {
                z = kmp_lexer_weiter(lexer, 1);
                ende = z;
                n32 basis = wert;
                wert = 0;

                while (kmp_zeichen_ist_letter(z) || kmp_zeichen_ist_ziffer(z) || z.codepoint == '_')
                {
                    char aktuelles_zeichen = z.codepoint;
                    uint32_t ziffer_wert;

                    if (aktuelles_zeichen >= '0' && aktuelles_zeichen <= '9')
                    {
                        ziffer_wert = ziffern[(g32) aktuelles_zeichen];
                    }
                    else if (aktuelles_zeichen >= 'a' && aktuelles_zeichen <= 'f')
                    {
                        ziffer_wert = ziffern[(g32) aktuelles_zeichen];
                    }
                    else if (aktuelles_zeichen >= 'A' && aktuelles_zeichen <= 'F')
                    {
                        ziffer_wert = ziffern[(g32) aktuelles_zeichen];
                    }
                    else
                    {
                        break;
                    }

                    if (ziffer_wert >= basis)
                    {
                        break;
                    }

                    ende = z;
                    wert *= basis;
                    wert += ziffer_wert;
                    z = kmp_lexer_weiter(lexer, 1);
                }

                KMP_spanne_t spanne = kmp_spanne_zeichen(anfang, ende);

                bss_feld_hinzufügen(speicher, &erg, kmp_glied_ganzzahl(speicher, spanne, wert, basis));
            }
            else if (z.codepoint == ',')
            {
                kmp_lexer_weiter(lexer, 1);
                z = kmp_lexer_nächstes_zeichen(lexer, 0);
                ende = z;

                d32 dezimalwert = wert;
                g32 faktor = 10;
                while (kmp_zeichen_ist_ziffer(z) || z.codepoint == '_')
                {
                    if (z.codepoint == '_')
                    {
                        continue;
                    }

                    dezimalwert += ((d32) ziffern[z.codepoint] / faktor);
                    faktor *= 10;

                    ende = z;
                    z = kmp_lexer_weiter(lexer, 1);
                }

                KMP_spanne_t spanne = kmp_spanne_zeichen(anfang, ende);
                bss_feld_hinzufügen(speicher, &erg, kmp_glied_dezimalzahl(speicher, spanne, dezimalwert));
            }
            else
            {
                KMP_spanne_t spanne = kmp_spanne_zeichen(anfang, ende);
                bss_feld_hinzufügen(speicher, &erg, kmp_glied_ganzzahl(speicher, spanne, wert, 10));
            }
        }
        else
        {
            kmp_lexer_weiter(lexer, 1);
            KMP_spanne_t spanne = kmp_spanne_zeichen(anfang, z);
            bss_feld_hinzufügen(speicher, &erg, kmp_glied(speicher, GLIED_ILLEGAL, spanne));
        }
    }

    return erg;
}

void
kmp_lexer_leerzeichen_einlesen(KMP_lexer_t* lexer)
{
    KMP_zeichen_t z = kmp_lexer_nächstes_zeichen(lexer, 0);

    while (z.codepoint == ' '  ||
           z.codepoint == '\n' ||
           z.codepoint == '\r' ||
           z.codepoint == '\t')
    {
        z = kmp_lexer_weiter(lexer, 1);
    }
}

KMP_zeichen_t
kmp_lexer_nächstes_zeichen(KMP_lexer_t* lexer, g16 versatz)
{
    g32 byte_index = lexer->index + versatz;

    if (byte_index >= lexer->inhalt.größe || byte_index < 0)
    {
        return kmp_zeichen_mit_daten(bss_text("\0"), byte_index, 1, lexer->quelle, lexer->inhalt);
    }

    n8* bytes = (n8*) lexer->inhalt.daten + byte_index;
    n8 zeichen_länge = 1;

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

    if (byte_index + zeichen_länge > lexer->inhalt.größe)
    {
        zeichen_länge = 1;
    }

    return kmp_zeichen_mit_daten(
        bss_text(lexer->inhalt.daten + byte_index),
        byte_index, zeichen_länge, lexer->quelle, lexer->inhalt);
}

KMP_zeichen_t
kmp_lexer_weiter(KMP_lexer_t *lexer, g32 anzahl)
{
    for (g32 i = 0; i < anzahl; ++i)
    {
        lexer->index += kmp_lexer_nächstes_zeichen(lexer, 1).byte_länge;
    }

    return kmp_lexer_nächstes_zeichen(lexer, 1);
}


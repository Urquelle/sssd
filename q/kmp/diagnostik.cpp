#include "diagnostik.hpp"

#include <iostream>
#include <cstdio>

// Escape-Sequenzen für verschiedene Terminal-Typen
#define ANSI_HYPERLINK_START "\x1b]8;;"
#define ANSI_HYPERLINK_END "\x1b]8;;\a"
#define ANSI_HYPERLINK_MID "\a"

void datei_verknüpfung_ausgeben(const char* dateiname, int versatz);

Diagnostik::Diagnostik()
{
}

bool
Diagnostik::hat_meldungen()
{
    auto erg = _meldungen.size() > 0;

    return erg;
}

void
Diagnostik::melden(Diagnostik::Meldung meldung)
{
    _meldungen.push_back(meldung);
}

void
Diagnostik::melden(Spanne spanne, Fehler *fehler)
{
    _meldungen.push_back({
        .spanne = spanne,
        .fehler = fehler
    });
}

std::vector<Diagnostik::Meldung>
Diagnostik::meldungen()
{
    return _meldungen;
}

std::ostream& operator<<(std::ostream& ausgabe, const Diagnostik::Meldung& m)
{
    datei_verknüpfung_ausgeben(m.spanne.von().quelldatei().c_str(), 0);
    ausgabe << " fehler: " << m.fehler->text << std::endl;

    return ausgabe;
}

void datei_verknüpfung_ausgeben(const char* dateiname, int versatz)
{
    char pfad[1024];
    _fullpath(pfad, dateiname, sizeof(pfad));

    // Konvertiere Backslashes zu Forward Slashes
    for (char* p = pfad; *p; p++)
    {
        if (*p == '\\') *p = '/';
    }

    // Erstelle den file://-URL mit Byte-Offset
    // Format: file:///path/to/file#byte=N
    printf("%sfile://%s#byte=%d%s%s%s\n",
           ANSI_HYPERLINK_START,
           pfad,
           versatz,
           ANSI_HYPERLINK_MID,
           dateiname,
           ANSI_HYPERLINK_END);
}


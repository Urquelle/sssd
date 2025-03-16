#include "syntax.hpp"
#include "asb.hpp"
#include "glied.hpp"
#include "lexer.hpp"

#include <cassert>
#include <fstream>
#include <sstream>

Sss::Kmp::Ausdruck_Binär::Binär_Op glied_zu_binär_op(Sss::Kmp::Glied *glied);
Sss::Kmp::Ausdruck_Unär::Unär_Op   glied_zu_unär_op(Sss::Kmp::Glied *glied);

namespace Sss::Kmp {

const char * SCHLÜSSELWORT_SCHABLONE = "schablone";  // INFO: struct
const char * SCHLÜSSELWORT_MERKMAL   = "merkmal";    // INFO: trait
const char * SCHLÜSSELWORT_OPTION    = "option";     // INFO: enum
const char * SCHLÜSSELWORT_WEICHE    = "weiche";     // INFO: match
const char * SCHLÜSSELWORT_WENN      = "wenn";       // INFO: if
const char * SCHLÜSSELWORT_FÜR       = "für";        // INFO: for
const char * SCHLÜSSELWORT_SOLANGE   = "solange";    // INFO: while
const char * SCHLÜSSELWORT_SONST     = "sonst";      // INFO: else
const char * SCHLÜSSELWORT_BRAUCHE   = "brauche";    // INFO: import
const char * SCHLÜSSELWORT_LADE      = "lade";       // INFO: include
const char * SCHLÜSSELWORT_FINAL     = "final";      // INFO: defer
const char * SCHLÜSSELWORT_RES       = "res";        // INFO: resultat -> return
const char * SCHLÜSSELWORT_MARKE     = "marke";
const char * SCHLÜSSELWORT_WEITER    = "weiter";     // INFO: continue
const char * SCHLÜSSELWORT_SPRUNG    = "sprung";     // INFO: goto
const char * SCHLÜSSELWORT_RAUS      = "raus";       // INFO: break

Syntax::Syntax(std::vector<Glied *> glieder)
    : _glieder(glieder)
    , _glied_index(0)
{
}

Diagnostik
Syntax::diagnostik() const
{
    return _diagnostik;
}

void
Syntax::melden(Spanne spanne, Fehler *fehler)
{
    _diagnostik.melden(spanne, fehler);
    /*__debugbreak();*/
}

void
Syntax::melden(Anweisung * anweisung, Fehler *fehler)
{
    melden(anweisung->spanne(), fehler);
}

void
Syntax::melden(Ausdruck * ausdruck, Fehler *fehler)
{
    melden(ausdruck->spanne(), fehler);
}

void
Syntax::melden(Glied * glied, Fehler *fehler)
{
    melden(glied->spanne(), fehler);
}

bool
Syntax::dateiende()
{
    auto erg = ist(Glied::ENDE);

    return erg;
}

Asb
Syntax::starten(int32_t index)
{
    Asb erg;

    for (;;)
    {
        if (dateiende())
        {
            break;
        }

        auto ergebnis = anweisung_einlesen();

        if (ergebnis.schlecht())
        {
            melden(glied(), ergebnis.fehler());

            break;
        }

        auto *anweisung = ergebnis.wert();

        if (anweisung->art() == Asb_Knoten::ANWEISUNG_LADE)
        {
            auto dateiname = anweisung->als<Anweisung_Lade *>()->dateiname();

            std::ifstream datei(dateiname);
            std::stringstream inhalt;
            inhalt << datei.rdbuf();

            auto lexer = Lexer(dateiname, inhalt.str());
            auto glied = lexer.starten();

            auto syntax = Sss::Kmp::Syntax(glied);
            auto ast = syntax.starten();

            for (auto *a : ast.anweisungen)
            {
                erg.anweisungen.push_back(a);
            }

            continue;
        }

        erg.anweisungen.push_back(anweisung);
    }

    return erg;
}

Marke::Marke(Spanne spanne, std::string wert)
    : _spanne(spanne)
    , _wert(wert)
{
}

Spanne
Marke::spanne() const
{
    return _spanne;
}

std::string
Marke::wert() const
{
    return _wert;
}

std::vector<Marke *>
Syntax::marken_einlesen()
{
    std::vector<Marke *> erg;
    auto *anfang = glied();

    if (glied()->art() != Glied::RAUTE)
    {
        return erg;
    }

    if (!ist(Glied::BEZEICHNER, 1) || strcmp(glied(1)->text(), SCHLÜSSELWORT_MARKE) != 0)
    {
        return erg;
    }

    weiter(2);

    for (;;)
    {
        auto ausdruck = ausdruck_einlesen();

        if (ausdruck.schlecht())
        {
            break;
        }

        if (ausdruck.wert()->art() != Asb_Knoten::AUSDRUCK_BEZEICHNER)
        {
            break;
        }

        erg.push_back(new Marke(
            ausdruck.wert()->spanne(),
            ausdruck.wert()->als<Ausdruck_Bezeichner *>()->wert()
        ));

        if (!passt(Glied::KOMMA))
        {
            break;
        }
    }

    return erg;
}

// anweisung {{{
Ergebnis<Anweisung *>
Syntax::anweisung_einlesen()
{
    std::vector<Marke *> marken = marken_einlesen();

    Anweisung *anweisung = nullptr;

    if (ist(Glied::GKLAMMER_AUF))
    {
        anweisung = block_anweisung_einlesen();
    }

    else if (ist(Glied::KLEINER))
    {
        anweisung = markierung_anweisung_einlesen();
        erwarte(Glied::PUNKT);
    }

    else if (ist(Glied::BEZEICHNER) && strcmp(glied()->text(), SCHLÜSSELWORT_WENN) == 0)
    {
        anweisung = wenn_anweisung_einlesen();
    }

    else if (ist(Glied::BEZEICHNER) && strcmp(glied()->text(), SCHLÜSSELWORT_FÜR) == 0)
    {
        anweisung = für_anweisung_einlesen();
    }

    else if (ist(Glied::BEZEICHNER) && strcmp(glied()->text(), SCHLÜSSELWORT_SOLANGE) == 0)
    {
        anweisung = solange_anweisung_einlesen();
    }

    else if (ist(Glied::BEZEICHNER) && strcmp(glied()->text(), SCHLÜSSELWORT_WEICHE) == 0)
    {
        anweisung = weiche_anweisung_einlesen();
    }

    else if (ist(Glied::BEZEICHNER) && strcmp(glied()->text(), SCHLÜSSELWORT_FINAL) == 0)
    {
        anweisung = final_anweisung_einlesen();
    }

    else if (ist(Glied::BEZEICHNER) && strcmp(glied()->text(), SCHLÜSSELWORT_RES) == 0)
    {
        anweisung = res_anweisung_einlesen();
    }

    else if (ist(Glied::BEZEICHNER) && strcmp(glied()->text(), SCHLÜSSELWORT_WEITER) == 0)
    {
        anweisung = weiter_anweisung_einlesen();
        erwarte(Glied::PUNKT);
    }

    else if (ist(Glied::BEZEICHNER) && strcmp(glied()->text(), SCHLÜSSELWORT_SPRUNG) == 0)
    {
        anweisung = sprung_anweisung_einlesen();
        erwarte(Glied::PUNKT);
    }

    else if (ist(Glied::BEZEICHNER) && strcmp(glied()->text(), SCHLÜSSELWORT_RAUS) == 0)
    {
        anweisung = raus_anweisung_einlesen();
        erwarte(Glied::PUNKT);
    }

    else if (glied()->art()  == Glied::BEZEICHNER && glied(1)->art() == Glied::BALKEN)
    {
        anweisung = deklaration_anweisung_einlesen();
    }

    else if (glied()->art() == Glied::RAUTE)
    {
        if (glied(1)->art() == Glied::BEZEICHNER && strcmp(glied(1)->text(), SCHLÜSSELWORT_BRAUCHE) == 0)
        {
            auto ausdruck = basis_ausdruck_einlesen();

            if (ausdruck.schlecht() || ausdruck.wert()->art() != Ausdruck::AUSDRUCK_BRAUCHE)
            {
                assert(!"meldung erstatten");
            }

            anweisung = new Anweisung_Brauche(
                ausdruck.wert()->spanne(),
                ausdruck.wert()->als<Ausdruck_Brauche *>()->dateiname()
            );
        }
        else if (glied(1)->art() == Glied::BEZEICHNER && strcmp(glied(1)->text(), SCHLÜSSELWORT_LADE) == 0)
        {
            weiter(2);

            auto ausdruck = basis_ausdruck_einlesen();

            if (ausdruck.schlecht() || ausdruck.wert()->art() != Ausdruck::AUSDRUCK_TEXT)
            {
                assert(!"meldung erstatten");
            }

            anweisung = new Anweisung_Lade(
                ausdruck.wert()->spanne(),
                ausdruck.wert()->als<Ausdruck_Text *>()->wert()
            );
        }
        else if (glied(1)->art() == Glied::AUSRUFEZEICHEN)
        {
            auto ausdruck = basis_ausdruck_einlesen();

            if (ausdruck.schlecht())
            {
                assert(!"meldung erstatten");
            }

            erwarte(Glied::PUNKT);

            anweisung = new Anweisung_Ausdruck(
                ausdruck.wert()->spanne(),
                ausdruck.wert()
            );
        }
    }

    else
    {
        auto ausdruck = ausdruck_einlesen();

        if (ausdruck.gut())
        {
            if (glied()->art() >= Glied::ZUWEISUNG && glied()->art() <= Glied::ZUWEISUNG_PROZENT)
            {
                auto *op = weiter();
                auto rechts = ausdruck_einlesen();

                if (rechts.schlecht())
                {
                    assert(!"gültigen ausdruck erwartet");
                }

                return new Anweisung_Zuweisung(
                    Spanne(ausdruck.wert()->spanne().von(), rechts.wert()->spanne().bis()),
                    ausdruck.wert(), op, rechts.wert()
                );
            }
            else
            {
                anweisung = new Anweisung_Ausdruck(
                    ausdruck.wert()->spanne(),
                    ausdruck.wert()
                );
            }
        }
        else
        {
            return ausdruck.fehler();
        }
    }

    if (anweisung == nullptr)
    {
        return new Fehler("Konnte keine gültige Anweisung einlesen");
    }

    anweisung->marken_setzen(marken);

    return anweisung;
}

Anweisung *
Syntax::deklaration_anweisung_einlesen()
{
    auto *deklaration = deklaration_einlesen();

    return new Anweisung_Deklaration(
        deklaration->spanne(),
        deklaration
    );
}

Anweisung *
Syntax::block_anweisung_einlesen()
{
    auto *klammer_auf = erwarte(Glied::GKLAMMER_AUF);

    std::vector<Anweisung *> anweisungen;
    if (glied()->art() != Glied::GKLAMMER_ZU)
    {
        for (;;)
        {
            auto anweisung = anweisung_einlesen();

            if (anweisung.schlecht())
            {
                break;
            }

            passt(Glied::PUNKT);
            anweisungen.push_back(anweisung.wert());
        }
    }

    auto *klammer_zu = erwarte(Glied::GKLAMMER_ZU);

    return new Anweisung_Block(
        Spanne(klammer_auf->spanne().von(), klammer_zu->spanne().bis()),
        anweisungen
    );
}

Anweisung *
Syntax::wenn_anweisung_einlesen()
{
    auto *schlüsselwort = weiter();

    auto bedingung = ausdruck_einlesen();
    if (bedingung.schlecht())
    {
        assert(!"meldung erstatten");
        return nullptr;
    }

    auto rumpf = anweisung_einlesen();

    Anweisung_Wenn *sonst_anweisung = nullptr;
    if (glied()->art() == Glied::BEZEICHNER && strcmp(glied()->text(), "sonst") == 0)
    {
        auto *sonst = weiter();

        if (glied()->art() == Glied::BEZEICHNER && strcmp(glied()->text(), SCHLÜSSELWORT_WENN) == 0)
        {
            sonst_anweisung = (Anweisung_Wenn *) wenn_anweisung_einlesen();
        }
        else
        {
            auto sonst_rumpf = anweisung_einlesen();

            if (sonst_rumpf.schlecht())
            {
                assert(!"meldung erstatten");
            }

            sonst_anweisung = new Anweisung_Wenn(
                Spanne(sonst->spanne().von(), sonst_rumpf.wert()->spanne().bis()),
                new Ausdruck_Ganzzahl(sonst->spanne(), 1),
                sonst_rumpf.wert(),
                nullptr
            );
        }
    }

    return new Anweisung_Wenn(
        Spanne(schlüsselwort->spanne().von(), sonst_anweisung
               ? sonst_anweisung->spanne().bis()
               : rumpf.wert()->spanne().bis()),
        bedingung.wert(),
        rumpf.wert(),
        sonst_anweisung
    );
}

Anweisung *
Syntax::markierung_anweisung_einlesen()
{
    auto *kleiner_als = weiter();
    auto markierung = basis_ausdruck_einlesen();

    if (markierung.schlecht())
    {
        melden(glied(), markierung.fehler());
        return nullptr;
    }

    if (markierung.wert()->art() != Asb_Knoten::AUSDRUCK_BEZEICHNER)
    {
        melden(markierung.wert(), new Fehler("gültigen Bezeichner erwartet."));
        return nullptr;
    }

    auto größer_als = erwarte(Glied::GRÖẞER);

    auto *erg = new Anweisung_Markierung(
        Spanne(kleiner_als->spanne().von(), größer_als->spanne().bis()),
        markierung.wert()->als<Ausdruck_Bezeichner *>()->wert()
    );

    return erg;
}

Anweisung *
Syntax::für_anweisung_einlesen()
{
    auto *schlüsselwort = weiter();

    Ausdruck *index = nullptr;
    Ausdruck *bedingung = nullptr;

    auto ausdruck = basis_ausdruck_einlesen();
    if (ausdruck.schlecht())
    {
        assert(!"meldung erstatten");
        return nullptr;
    }

    bedingung = ausdruck.wert();
    if (passt(Glied::DOPPELPUNKT))
    {
        index = ausdruck.wert();
        ausdruck = ausdruck_einlesen();

        if (ausdruck.schlecht())
        {
            assert(!"meldung erstatten");
            return nullptr;
        }

        bedingung = ausdruck.wert();
    }

    auto rumpf = anweisung_einlesen();
    if (rumpf.schlecht())
    {
        assert(!"meldung erstatten");
        return nullptr;
    }

    return new Anweisung_Für(
        Spanne(schlüsselwort->spanne().von(), rumpf.wert()->spanne().bis()),
        index,
        bedingung,
        rumpf.wert()
    );
}

Anweisung *
Syntax::solange_anweisung_einlesen()
{
    auto *schlüsselwort = weiter();

    auto bedingung = ausdruck_einlesen();
    if (bedingung.schlecht())
    {
        melden(glied(), bedingung.fehler());
    }

    auto rumpf = anweisung_einlesen();
    if (rumpf.schlecht())
    {
        melden(glied(), rumpf.fehler());
    }

    return new Anweisung_Solange(schlüsselwort->spanne(), bedingung.wert(), rumpf.wert());
}

Anweisung *
Syntax::weiche_anweisung_einlesen()
{
    auto *schlüsselwort = weiter();

    auto erg = ausdruck_einlesen();
    if (erg.schlecht())
    {
        melden(glied(), erg.fehler());
    }

    auto *ausdruck = erg.wert();
    std::vector<Muster *> muster;

    if (passt(Glied::GKLAMMER_AUF))
    {
        for (;;)
        {
            erg = ausdruck_einlesen();

            if (erg.schlecht())
            {
                break;
            }

            auto *muster_ausdruck = erg.wert();

            erwarte(Glied::FOLGERUNG);

            auto anweisung = anweisung_einlesen();

            if (anweisung.schlecht())
            {
                melden(glied(), anweisung.fehler());
            }

            muster.push_back(new Muster(
                Spanne(muster_ausdruck->spanne(), anweisung.wert()->spanne()),
                muster_ausdruck,
                anweisung.wert()
            ));

            if (!passt(Glied::PUNKT))
            {
                break;
            }
        }

        erwarte(Glied::GKLAMMER_ZU);
    }

    return new Anweisung_Weiche(
        Spanne(),
        ausdruck,
        muster
    );
}

Anweisung *
Syntax::final_anweisung_einlesen()
{
    auto *schlüsselwort = weiter();

    auto anweisung = anweisung_einlesen();

    auto *erg = new Anweisung_Final(
        Spanne(schlüsselwort->spanne(), anweisung.wert()->spanne()),
        anweisung.wert()
    );

    return erg;
}

Anweisung *
Syntax::res_anweisung_einlesen()
{
    auto *schlüsselwort = weiter();

    auto ausdruck = ausdruck_einlesen();

    auto *erg = new Anweisung_Res(
        Spanne(schlüsselwort->spanne(), ausdruck.wert()->spanne()),
        ausdruck.wert()
    );

    return erg;
}

Anweisung *
Syntax::weiter_anweisung_einlesen()
{
    auto *schlüsselwort = weiter();

    auto *erg = new Anweisung_Weiter(schlüsselwort->spanne());

    return erg;
}

Anweisung *
Syntax::sprung_anweisung_einlesen()
{
    auto *schlüsselwort = weiter();

    auto ausdruck = ausdruck_einlesen();

    if (ausdruck.schlecht())
    {
        melden(glied(), ausdruck.fehler());
    }

    if (ausdruck.wert()->art() != Asb_Knoten::AUSDRUCK_BEZEICHNER)
    {
        melden(ausdruck.wert(), new Fehler("Bezeichner erwartet."));
    }

    auto *erg = new Anweisung_Sprung(
        Spanne(schlüsselwort->spanne(), ausdruck.wert()->spanne()),
        ausdruck.wert()->als<Ausdruck_Bezeichner *>()
    );

    return erg;
}

Anweisung *
Syntax::raus_anweisung_einlesen()
{
    auto *schlüsselwort = weiter();

    auto *erg = new Anweisung_Raus(schlüsselwort->spanne());

    return erg;
}
// }}}
// deklaration {{{
Deklaration *
Syntax::deklaration_einlesen(bool mit_abschluss)
{
    auto *anfang = glied();
    std::vector<std::string> namen;

    for (;;)
    {
        auto ausdruck = ausdruck_einlesen();

        if (ausdruck.schlecht() || ausdruck.wert()->art() != Ausdruck::AUSDRUCK_BEZEICHNER)
        {
            break;
        }

        namen.push_back(ausdruck.wert()->als<Ausdruck_Bezeichner *>()->wert());

        if (!passt(Glied::KOMMA))
        {
            break;
        }
    }

    if (namen.size() == 0)
    {
        return nullptr;
    }

    erwarte(Glied::BALKEN);
    Spezifizierung *spezifizierung = spezifizierung_einlesen();

    if (passt(Glied::ZUWEISUNG))
    {
        auto ausdruck = ausdruck_einlesen();

        if (ausdruck.schlecht())
        {
            assert(!"meldung erstatten");
        }

        if (mit_abschluss)
        {
            erwarte(Glied::PUNKT);
        }

        return new Deklaration_Variable(
            Spanne(anfang->spanne().von(), ausdruck.wert()->spanne().bis()),
            namen,
            spezifizierung,
            ausdruck.wert()
        );
    }

    else if (passt(Glied::BALKEN))
    {
        if (ist(Glied::RKLAMMER_AUF))
        {
            auto ausdruck_ergebnis = basis_ausdruck_einlesen();

            if (ausdruck_ergebnis.schlecht())
            {
                assert(!"schlecht");
            }

            auto *ausdruck = ausdruck_ergebnis.wert();

            if (ausdruck->art() == Asb_Knoten::AUSDRUCK_FUNKTION)
            {
                if (ausdruck->als<Ausdruck_Funktion *>()->rumpf() == nullptr)
                {
                    erwarte(Glied::PUNKT);
                }

                return new Deklaration_Funktion(
                    Spanne(anfang->spanne().von(), ausdruck->spanne().bis()),
                    namen[0],
                    spezifizierung,
                    ausdruck->als<Ausdruck_Funktion *>()
                );
            }
            else if (ausdruck->art() == Asb_Knoten::AUSDRUCK_OPTION)
            {
                return new Deklaration_Option(
                    Spanne(anfang->spanne().von(), ausdruck->spanne().bis()),
                    namen[0],
                    ausdruck->als<Ausdruck_Option *>()
                );
            }
            else
            {
                assert(0);
            }
        }
        else if (ist(Glied::BEZEICHNER) && strcmp(glied()->text(), SCHLÜSSELWORT_OPTION) == 0)
        {
            auto ausdruck_ergebnis = basis_ausdruck_einlesen();
            auto *ausdruck = ausdruck_ergebnis.wert();

            return new Deklaration_Option(
                Spanne(anfang->spanne().von(), ausdruck->spanne().bis()),
                namen[0],
                ausdruck->als<Ausdruck_Option *>()
            );
        }

        else if (ist(Glied::BEZEICHNER) && strcmp(glied()->text(), SCHLÜSSELWORT_SCHABLONE) == 0)
        {
            auto *schlüsselwort = weiter();

            erwarte(Glied::GKLAMMER_AUF);
            std::vector<Deklaration *> eigenschaften;

            for (;;)
            {
                auto *eigenschaft = deklaration_einlesen();

                if (eigenschaft == nullptr)
                {
                    break;
                }

                eigenschaften.push_back(eigenschaft);
            }

            auto *klammer_zu = erwarte(Glied::GKLAMMER_ZU);

            return new Deklaration_Schablone(
                Spanne(anfang->spanne().von(), klammer_zu->spanne().bis()),
                namen[0],
                eigenschaften
            );
        }

        else if (ist(Glied::RAUTE))
        {
            auto ausdruck = basis_ausdruck_einlesen();

            if (ausdruck.schlecht() || ausdruck.wert()->art() != Ausdruck::AUSDRUCK_BRAUCHE)
            {
                assert(!"meldung erstatten");
            }

            return new Deklaration_Brauche(
                Spanne(),
                namen[0],
                ausdruck.wert()->als<Ausdruck_Brauche *>()
            );
        }
    }

    assert(spezifizierung != nullptr);

    if (mit_abschluss)
    {
        erwarte(Glied::PUNKT);
    }

    return new Deklaration_Variable(
        Spanne(anfang->spanne().von(), spezifizierung->spanne().bis()),
        namen,
        spezifizierung,
        nullptr
    );
}
// }}}
// ausdruck {{{
Ergebnis<Ausdruck *>
Syntax::ausdruck_einlesen()
{
    auto erg = reihe_ausdruck_einlesen();

    return erg;
}

Ergebnis<Ausdruck *>
Syntax::reihe_ausdruck_einlesen()
{
    auto links = vergleich_ausdruck_einlesen();

    if (passt(Glied::PUNKT_PUNKT))
    {
        auto max = vergleich_ausdruck_einlesen();

        if (max.schlecht())
        {
            assert(!"meldung erstatten");
        }

        links = new Ausdruck_Reihe(
            Spanne(links.wert()->spanne().von(), max.wert()->spanne().bis()),
            links.wert(),
            max.wert()
        );
    }

    return links;
}

Ergebnis<Ausdruck *>
Syntax::vergleich_ausdruck_einlesen()
{
    auto links = bitschieben_ausdruck_einlesen();

    if (glied()->art() >= Glied::KLEINER && glied()->art() <= Glied::GRÖẞER)
    {
        auto *op = weiter();
        auto rechts = bitschieben_ausdruck_einlesen();

        if (rechts.schlecht())
        {
            assert(!"meldung erstatten");
        }

        links = new Ausdruck_Binär(
            Spanne(links.wert()->spanne().von(), rechts.wert()->spanne().bis()),
            glied_zu_binär_op(op),
            links.wert(),
            rechts.wert()
        );
    }

    return links;
}

Ergebnis<Ausdruck *>
Syntax::bitschieben_ausdruck_einlesen()
{
    auto links = add_ausdruck_einlesen();

    if (ist(Glied::DREIECK_LINKS) || ist(Glied::DREIECK_RECHTS))
    {
        auto *op = weiter();
        auto rechts = add_ausdruck_einlesen();

        if (rechts.schlecht())
        {
            assert(!"meldung erstatten");
        }

        links = new Ausdruck_Binär(
            Spanne(links.wert()->spanne().von(), rechts.wert()->spanne().bis()),
            glied_zu_binär_op(op),
            links.wert(),
            rechts.wert()
        );
    }

    return links;
}

Ergebnis<Ausdruck *>
Syntax::add_ausdruck_einlesen()
{
    auto links = mult_ausdruck_einlesen();

    while (glied()->art() == Glied::PLUS || glied()->art() == Glied::MINUS)
    {
        auto *glied_op = weiter();
        auto rechts = mult_ausdruck_einlesen();

        if (rechts.schlecht())
        {
            return rechts.fehler();
        }

        links = new Ausdruck_Binär(
            Spanne(links.wert()->spanne().von(), rechts.wert()->spanne().bis()),
            glied_zu_binär_op(glied_op), links.wert(), rechts.wert());
    }

    return links;
}

Ergebnis<Ausdruck *>
Syntax::mult_ausdruck_einlesen()
{
    auto links = aufruf_ausdruck_einlesen();

    while (glied()->art() == Glied::STERN       ||
           glied()->art() == Glied::DOPPELPUNKT ||
           glied()->art() == Glied::PROZENT)
    {
        auto *glied_op = weiter();
        auto rechts = aufruf_ausdruck_einlesen();

        links = new Ausdruck_Binär(
            Spanne(links.wert()->spanne().von(), rechts.wert()->spanne().bis()),
            glied_zu_binär_op(glied_op), links.wert(), rechts.wert());
    }

    return links;
}

Ergebnis<Ausdruck *>
Syntax::aufruf_ausdruck_einlesen()
{
    auto links = index_ausdruck_einlesen();

    if (glied()->art() == Glied::RKLAMMER_AUF)
    {
        auto *klammer_auf = weiter();
        std::vector<Ausdruck *> argumente;

        if (glied()->art() != Glied::RKLAMMER_ZU)
        {
            auto arg = ausdruck_einlesen();
            while (arg.gut())
            {
                argumente.push_back(arg.wert());

                if (!passt(Glied::STRICHPUNKT))
                {
                    break;
                }

                arg = ausdruck_einlesen();
            }
        }

        auto *klammer_zu = erwarte(Glied::RKLAMMER_ZU);

        links = new Ausdruck_Aufruf(
            Spanne(
                links.wert()->spanne().von(),
                klammer_zu->spanne().bis()),
            links.wert(),
            argumente
        );
    }

    return links;
}

Ergebnis<Ausdruck *>
Syntax::index_ausdruck_einlesen()
{
    auto links = eigenschaft_ausdruck_einlesen();

    while (glied()->art() == Glied::EKLAMMER_AUF)
    {
        auto *klammer_auf = weiter();
        auto index = ausdruck_einlesen();
        auto *klammer_zu = erwarte(Glied::EKLAMMER_ZU);

        links = new Ausdruck_Index(
            Spanne(klammer_auf->spanne().von(), klammer_zu->spanne().bis()),
            links.wert(), index.wert()
        );
    }

    return links;
}

Ergebnis<Ausdruck *>
Syntax::eigenschaft_ausdruck_einlesen()
{
    auto links = basis_ausdruck_einlesen();

    while (glied()->art() == Glied::PISA)
    {
        auto *punkt = weiter();

        auto feld = basis_ausdruck_einlesen();
        if (feld.schlecht())
        {
            assert(!"meldung erstatten");
        }

        if (feld.wert()->art() != Asb_Knoten::AUSDRUCK_BEZEICHNER)
        {
            assert(!"meldung erstetten");
        }

        links = new Ausdruck_Eigenschaft(
            Spanne(links.wert()->spanne().von(), feld.wert()->spanne().bis()),
            links.wert(),
            feld.wert()
        );
    }

    return links;
}

Ergebnis<Ausdruck *>
Syntax::basis_ausdruck_einlesen()
{
    auto t = glied();

    switch (t->art())
    {
        case Glied::MINUS:
        case Glied::TILDE:
        case Glied::AUSRUFEZEICHEN:
        {
            auto *op = weiter();
            auto ausdruck = basis_ausdruck_einlesen();

            if (ausdruck.schlecht())
            {
                return ausdruck.fehler();
            }

            auto *erg = new Ausdruck_Unär(
                Spanne(op->spanne().von(), ausdruck.wert()->spanne().bis()),
                glied_zu_unär_op(op),
                ausdruck.wert()
            );

            return erg;
        } break;

        case Glied::RAUTE:
        {
            auto *raute = weiter();

            if (glied()->art() == Glied::BEZEICHNER && strcmp(glied()->text(), SCHLÜSSELWORT_BRAUCHE) == 0)
            {
                weiter();

                auto dateiname = basis_ausdruck_einlesen();

                if (dateiname.schlecht() || dateiname.wert()->art() != Ausdruck::AUSDRUCK_TEXT)
                {
                    assert(!"meldung erstatten");
                }

                auto *erg = new Ausdruck_Brauche(
                    Spanne(raute->spanne().von(), dateiname.wert()->spanne().bis()),
                    dateiname.wert()->als<Ausdruck_Text *>()->wert()
                );

                return erg;
            }
            else if (glied()->art() == Glied::AUSRUFEZEICHEN)
            {
                weiter();

                auto ausdruck = ausdruck_einlesen();

                if (ausdruck.schlecht())
                {
                    assert(!"meldung erstatten");
                }

                auto *erg = new Ausdruck_Ausführen(
                    Spanne(raute->spanne().von(), ausdruck.wert()->spanne().bis()),
                    ausdruck.wert()
                );

                return erg;
            }
        } break;

        case Glied::GANZZAHL:
        {
            weiter();
            auto *erg = new Ausdruck_Ganzzahl(
                    t->spanne(), t->als<Glied_Ganzzahl *>()->wert());

            return erg;
        } break;

        case Glied::DEZIMALZAHL:
        {
            weiter();
            auto *erg = new Ausdruck_Dezimalzahl(
                    t->spanne(), t->als<Glied_Dezimalzahl *>()->wert());

            return erg;
        } break;

        case Glied::BEZEICHNER:
        {
            if (strcmp(t->text(), SCHLÜSSELWORT_SCHABLONE) == 0 && ist(Glied::GKLAMMER_AUF, 1))
            {
                auto *anfang = weiter();
                auto *klammer_auf = erwarte(Glied::GKLAMMER_AUF);
                std::vector<Deklaration *> eigenschaften;

                for (;;)
                {
                    auto *eigenschaft = deklaration_einlesen(false);

                    if (!eigenschaft)
                    {
                        break;
                    }

                    eigenschaften.push_back(eigenschaft);

                    if (!passt(Glied::PUNKT))
                    {
                        break;
                    }
                }

                auto *klammer_zu = erwarte(Glied::GKLAMMER_ZU);

                return new Ausdruck_Schablone(
                    Spanne(anfang->spanne().von(), klammer_zu->spanne().bis()),
                    eigenschaften
                );
            }
            else if (strcmp(t->text(), SCHLÜSSELWORT_OPTION) == 0 && ist(Glied::GKLAMMER_AUF, 1))
            {
                auto *anfang = weiter();
                auto *klammer_auf = erwarte(Glied::GKLAMMER_AUF);
                std::vector<Deklaration_Variable *> eigenschaften;

                for (;;)
                {
                    auto name = basis_ausdruck_einlesen();
                    if (name.schlecht() || name.wert()->art() != Asb_Knoten::AUSDRUCK_BEZEICHNER)
                    {
                        break;
                    }

                    Spezifizierung *spez = nullptr;
                    if (passt(Glied::BALKEN) && !ist(Glied::ZUWEISUNG))
                    {
                        spez = spezifizierung_einlesen();
                    }

                    Ausdruck *init = nullptr;
                    if (passt(Glied::ZUWEISUNG))
                    {
                        auto erg = ausdruck_einlesen();
                        init = erg.wert();
                    }

                    eigenschaften.push_back(new Deklaration_Variable(
                        name.wert()->spanne(),
                        {name.wert()->als<Ausdruck_Bezeichner *>()->wert()},
                        spez,
                        init
                    ));

                    if (!passt(Glied::PUNKT))
                    {
                        break;
                    }
                }

                auto *klammer_zu = erwarte(Glied::GKLAMMER_ZU);

                return new Ausdruck_Option(
                    Spanne(anfang->spanne().von(), klammer_zu->spanne().bis()),
                    eigenschaften
                );
            }
            else
            {
                weiter();

                return new Ausdruck_Bezeichner(t->spanne(), t->text());
            }
        } break;

        case Glied::TEXT:
        {
            weiter();
            auto *erg = new Ausdruck_Text(t->spanne(), t->text());

            return erg;
        } break;

        case Glied::RKLAMMER_AUF:
        {
            auto *klammer_auf = weiter();

            // Versuche, Parameter einer Funktionsdefinition zu parsen
            std::vector<Deklaration *> parameter;
            bool könnte_funktion_sein = true;
            int32_t vorheriger_index = _glied_index;

            if (glied()->art() != Glied::RKLAMMER_ZU)
            {
                for (;;)
                {
                    auto *param = deklaration_einlesen(false);

                    if (!param)
                    {
                        könnte_funktion_sein = false;
                        break;
                    }

                    parameter.push_back(param);

                    if (!passt(Glied::STRICHPUNKT))
                    {
                        break;
                    }
                }
            }

            // Prüfe, ob es eine Funktionsdefinition sein könnte
            if (könnte_funktion_sein && glied()->art() == Glied::RKLAMMER_ZU)
            {
                auto *klammer_zu = weiter();

                Spezifizierung *rückgabe = nullptr;
                if (passt(Glied::PFEIL))
                {
                    rückgabe = spezifizierung_einlesen();

                    if (rückgabe == nullptr)
                    {
                        assert(!"meldung erstatten");
                    }
                }

                if (glied()->art() != Glied::GKLAMMER_AUF)
                {
                    return new Ausdruck_Funktion(
                        Spanne(klammer_auf->spanne().von(), klammer_zu->spanne().bis()),
                        parameter,
                        rückgabe,
                        nullptr
                    );
                }

                Anweisung *rumpf = nullptr;
                if (glied()->art() == Glied::GKLAMMER_AUF)
                {
                    auto rumpf_anweisung = anweisung_einlesen();
                    if (rumpf_anweisung.gut())
                    {
                        rumpf = rumpf_anweisung.wert();
                    }
                    else
                    {
                        melden(glied(), rumpf_anweisung.fehler());
                    }
                }

                // Wenn Rückgabetyp oder Rumpf vorhanden ist, ist es eine Funktionsdefinition
                if (rückgabe || rumpf)
                {
                    auto *erg = new Ausdruck_Funktion(
                        Spanne(klammer_auf->spanne().von(), rumpf ? rumpf->spanne().bis() : (rückgabe ? rückgabe->spanne().bis() : klammer_zu->spanne().bis())),
                        parameter,
                        rückgabe,
                        rumpf
                    );

                    return erg;
                }
            }

            // Wenn es keine Funktionsdefinition ist, zurücksetzen und als normalen Ausdruck parsen
            _glied_index = vorheriger_index;
            auto ausdruck = ausdruck_einlesen();
            if (ausdruck.schlecht())
            {
                return ausdruck.fehler();
            }

            auto *klammer_zu = erwarte(Glied::RKLAMMER_ZU);
            if (!klammer_zu)
            {
                return new Fehler("Erwartete schließende Klammer ')'");
            }

            auto *erg = new Ausdruck_Klammer(
                Spanne(klammer_auf->spanne().von(), klammer_zu->spanne().bis()),
                ausdruck.wert()
            );

            return erg;
        } break;

        case Glied::GKLAMMER_AUF:
        {
            auto *klammer_auf = weiter();
            bool ist_benamt = false;

            std::vector<Ausdruck_Kompositum::Eigenschaft *> eigenschaften;
            if (!ist(Glied::GKLAMMER_ZU))
            {
                for (;;)
                {
                    if (passt(Glied::PISA))
                    {
                        ist_benamt = true;

                        auto a = basis_ausdruck_einlesen();
                        erwarte(Glied::ZUWEISUNG);
                        auto ausdruck = ausdruck_einlesen();

                        eigenschaften.push_back(new Ausdruck_Kompositum::Eigenschaft(
                            Spanne(a.wert()->spanne().von(), ausdruck.wert()->spanne().bis()),
                            a.wert()->als<Ausdruck_Bezeichner *>()->wert(),
                            ausdruck.wert()
                        ));
                    }
                    else
                    {
                        auto ausdruck = ausdruck_einlesen();

                        if (ausdruck.schlecht())
                        {
                            break;
                        }

                        eigenschaften.push_back(new Ausdruck_Kompositum::Eigenschaft(
                            ausdruck.wert()->spanne(),
                            ausdruck.wert()
                        ));
                    }

                    if (!passt(Glied::STRICHPUNKT))
                    {
                        break;
                    }
                }
            }

            auto *klammer_zu = erwarte(Glied::GKLAMMER_ZU);

            Spezifizierung *spezifizierung = nullptr;
            if (passt(Glied::KLEINER))
            {
                spezifizierung = spezifizierung_einlesen();
                erwarte(Glied::GRÖẞER);
            }

            auto *erg = new Ausdruck_Kompositum(
                Spanne(klammer_auf->spanne().von(), klammer_zu->spanne().bis()),
                eigenschaften,
                ist_benamt,
                spezifizierung);

            return erg;
        } break;
    }

    return new Fehler("Konnte keinen Ausdruck einlesen.");
}
// }}}
// spezifizierung {{{
Spezifizierung *
Syntax::spezifizierung_einlesen()
{
    if (glied()->art() == Glied::STERN)
    {
        auto *stern = weiter();
        auto *basis = spezifizierung_einlesen();

        return new Spezifizierung_Zeiger(
            Spanne(stern->spanne().von(), basis->spanne().bis()),
            basis
        );
    }

    else if (glied()->art() == Glied::EKLAMMER_AUF)
    {
        auto *klammer_auf = weiter();
        auto ausdruck = ausdruck_einlesen();
        auto *klammer_zu = erwarte(Glied::EKLAMMER_ZU);

        auto *basis = spezifizierung_einlesen();

        return new Spezifizierung_Feld(
            Spanne(klammer_auf->spanne().von(), basis->spanne().bis()),
            basis,
            ausdruck.gut() ? ausdruck.wert() : nullptr
        );
    }

    else if (glied()->art() == Glied::RKLAMMER_AUF)
    {
        auto *klammer_auf = weiter();

        std::vector<Spezifizierung *> parameter;
        if (glied()->art() != Glied::RKLAMMER_AUF)
        {
            for (;;)
            {
                if (glied()->art() == Glied::BEZEICHNER && glied(1)->art() == Glied::BALKEN)
                {
                    auto bezeichner = ausdruck_einlesen();
                    erwarte(Glied::BALKEN);
                }

                auto *spez = spezifizierung_einlesen();

                if (spez != nullptr)
                {
                    parameter.push_back(spez);
                }

                if (!passt(Glied::STRICHPUNKT))
                {
                    break;
                }
            }
        }

        auto *klammer_zu = erwarte(Glied::RKLAMMER_ZU);
        auto bis = klammer_zu->spanne().bis();

        Spezifizierung *rückgabe = nullptr;
        if (passt(Glied::PFEIL))
        {
            rückgabe = spezifizierung_einlesen();
            bis = rückgabe->spanne().bis();
        }

        return new Spezifizierung_Funktion(
            Spanne(klammer_auf->spanne().von(), bis),
            parameter,
            rückgabe
        );
    }

    else if (glied()->art() == Glied::BEZEICHNER)
    {
        auto *name = weiter();

        return new Spezifizierung_Bezeichner(
            name->spanne(),
            name->text()
        );
    }

    return nullptr;
}
// }}}
// hilfsfunktionen {{{
Glied *
Syntax::erwarte(Glied::Art art)
{
    if (glied()->art() == art)
    {
        auto *erg = glied();
        weiter();

        return erg;
    }

    return nullptr;
}

Glied *
Syntax::glied(int32_t versatz) const
{
    int32_t index = _glied_index + versatz;

    if (_glieder.size() <= (size_t) index)
    {
        index = (int32_t) _glieder.size() - 1;
    }

    auto *erg = _glieder.at(index);

    return erg;
}

bool
Syntax::ist(Glied::Art art, int32_t versatz)
{
    auto erg = glied(versatz)->art() == art;

    return erg;
}

Glied *
Syntax::weiter(int32_t anzahl)
{
    auto *erg = glied();
    _glied_index += anzahl;

    return erg;
}

bool
Syntax::passt(Glied::Art art)
{
    if (glied()->art() == art)
    {
        weiter();
        return true;
    }

    return false;
}
// }}}

}

Sss::Kmp::Ausdruck_Binär::Binär_Op
glied_zu_binär_op(Sss::Kmp::Glied *glied)
{
    using namespace Sss::Kmp;

    switch (glied->art())
    {
        case Glied::PLUS:           return Ausdruck_Binär::ADDITION;
        case Glied::MINUS:          return Ausdruck_Binär::SUBTRAKTION;
        case Glied::STERN:          return Ausdruck_Binär::MULTIPLIKATION;
        case Glied::DOPPELPUNKT:    return Ausdruck_Binär::DIVISION;
        case Glied::PROZENT:        return Ausdruck_Binär::MODULO;
        case Glied::KLEINER:        return Ausdruck_Binär::KLEINER;
        case Glied::KLEINER_GLEICH: return Ausdruck_Binär::KLEINER_GLEICH;
        case Glied::GLEICH:         return Ausdruck_Binär::GLEICH;
        case Glied::UNGLEICH:       return Ausdruck_Binär::UNGLEICH;
        case Glied::GRÖẞER_GLEICH:  return Ausdruck_Binär::GRÖẞER_GLEICH;
        case Glied::GRÖẞER:         return Ausdruck_Binär::GRÖẞER;
        case Glied::DREIECK_RECHTS: return Ausdruck_Binär::BIT_VERSATZ_RECHTS;
        case Glied::DREIECK_LINKS:  return Ausdruck_Binär::BIT_VERSATZ_LINKS;

        default:
            assert(!"unbekannter op");
            return Ausdruck_Binär::UNBEKANNT;
    }
}

Sss::Kmp::Ausdruck_Unär::Unär_Op
glied_zu_unär_op(Sss::Kmp::Glied *glied)
{
    using namespace Sss::Kmp;

    switch (glied->art())
    {
        case Glied::MINUS:          return Ausdruck_Unär::MINUS;
        case Glied::AUSRUFEZEICHEN: return Ausdruck_Unär::NEGIERUNG;
        case Glied::TILDE:          return Ausdruck_Unär::INVERTIERUNG;

        default:
            assert(!"unbekannter op");
            return Ausdruck_Unär::UNBEKANNT;
    }
}

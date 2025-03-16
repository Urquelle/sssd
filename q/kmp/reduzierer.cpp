#include "kmp/reduzierer.hpp"

#include <cassert>
#include <format>

int global_transformator_markierung_index = 0;
std::string markierung_erstellen();

namespace Sss::Kmp
{

Bestimmte_Anweisung *
Reduzierer::wenn_anweisung_transformieren(Bestimmte_Anweisung_Wenn *anweisung)
{
    auto *knoten = anweisung->asb_knoten();

    if (anweisung->sonst() == nullptr)
    {
        auto ende_markierung = markierung_erstellen();
        auto *erg = Block(knoten,
        {
            Sprung_Unwahr(knoten, ende_markierung, anweisung->bedingung()),
            anweisung->rumpf(),
            Markierung(knoten, ende_markierung)
        });

        return Transformator::anweisung_transformieren(erg);
    }
    else
    {
        auto sonst_markierung = markierung_erstellen();
        auto ende_markierung  = markierung_erstellen();

        auto *erg = Block(knoten,
        {
            Sprung_Unwahr(knoten, sonst_markierung, anweisung->bedingung()),
            anweisung->rumpf(),
            Sprung(knoten, ende_markierung),
            Markierung(knoten, sonst_markierung),
            anweisung->sonst(),
            Markierung(knoten, ende_markierung)
        });

        return Transformator::anweisung_transformieren(erg);
    }
}

Bestimmte_Anweisung *
Reduzierer::für_anweisung_transformieren(Bestimmte_Anweisung_Für *anweisung)
{
    Bestimmter_Ausdruck *von = nullptr;
    Bestimmter_Ausdruck *bis = nullptr;
    Bestimmter_Ausdruck *bedingung = nullptr;

    switch (anweisung->bedingung()->art())
    {
        case Asb_Knoten::AUSDRUCK_BINÄR:
        {
            von = anweisung->bedingung()->als<Bestimmter_Ausdruck_Binär *>()->links();
            bis = anweisung->bedingung()->als<Bestimmter_Ausdruck_Binär *>()->rechts();
        } break;

        case Asb_Knoten::AUSDRUCK_REIHE:
        {
            von = anweisung->bedingung()->als<Bestimmter_Ausdruck_Reihe *>()->min();
            bis = anweisung->bedingung()->als<Bestimmter_Ausdruck_Reihe *>()->max();
        } break;

        default:
        {
            assert(!"unbekannter ausdruck");
        } break;
    }

    auto *knoten = anweisung->asb_knoten()->als<Anweisung_Für *>();

    auto *erg = Block(knoten,
    {
        Variable_Deklaration(knoten->index(), anweisung->index(), von),
        Konstante_Deklaration(knoten->bedingung(), "bis", bis),
        Solange(
            knoten->rumpf(),
            Kleiner_Gleich(
                knoten->bedingung(),
                Variable(knoten, von),
                Variable(knoten, bis)
            ),
            Block(knoten,
            {
                anweisung->rumpf(),
                Markierung(knoten, anweisung->weiter_markierung()),
                Hochzählen(knoten, Variable(knoten, von))
            }),
            anweisung->weiter_markierung(),
            anweisung->raus_markierung()
        )
    });

    return Transformator::anweisung_transformieren(erg);
}

Bestimmte_Anweisung *
Reduzierer::solange_anweisung_transformieren(Bestimmte_Anweisung_Solange *anweisung)
{
    auto *knoten = anweisung->asb_knoten();
    auto rumpf_markierung = markierung_erstellen();

    Bestimmte_Anweisung *erg = Block(knoten,
    {
        Sprung(knoten, anweisung->weiter_markierung()),
        Markierung(knoten, rumpf_markierung),
        anweisung->rumpf(),
        Markierung(knoten, anweisung->weiter_markierung()),
        Sprung_Wahr(knoten, rumpf_markierung, anweisung->bedingung()),
        Markierung(knoten, anweisung->raus_markierung())
    });

    return Transformator::anweisung_transformieren(erg);
}

Bestimmte_Anweisung *
Reduzierer::bedingter_sprung_anweisung_transformieren(Bestimmte_Anweisung_Bedingter_Sprung *anweisung)
{
    Bestimmte_Anweisung *erg = nullptr;

    auto *bedingung = ausdruck_transformieren(anweisung->bedingung());
    if (bedingung == anweisung->bedingung())
    {
        return anweisung;
    }

    erg = new Bestimmte_Anweisung_Bedingter_Sprung(
        (Anweisung *) anweisung->asb_knoten(), anweisung->markierung(), bedingung);

    return erg;
}
#if 0
Bestimmter_Ausdruck *
Reduzierer::transformieren(Bestimmter_Ausdruck *ausdruck)
{
    auto *erg = Transformator::transformieren(ausdruck);

    return erg;
}
#endif
}

/* helfer {{{ */
std::string markierung_erstellen()
{
    auto erg = std::format("markierung_{}", global_transformator_markierung_index++);

    return erg;
}
/* }}} */


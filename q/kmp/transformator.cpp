#include "kmp/transformator.hpp"

#include "kmp/asb.hpp"
#include "kmp/bestimmter_asb.hpp"
#include "kmp/operand.hpp"

#include <cassert>

namespace Sss::Kmp
{
/* transformieren {{{ */
Bestimmte_Anweisung *
Transformator::anweisung_transformieren(Bestimmte_Anweisung *anweisung)
{
    if (anweisung->art() == Asb_Knoten::ANWEISUNG_FÜR)
    {
        für_anweisung_transformieren(anweisung->als<Bestimmte_Anweisung_Für *>());
    }

    else if (anweisung->art() == Asb_Knoten::ANWEISUNG_BLOCK)
    {
        block_anweisung_transformieren(anweisung->als<Bestimmte_Anweisung_Block *>());
    }

    else if (anweisung->art() == Asb_Knoten::ANWEISUNG_SOLANGE)
    {
        solange_anweisung_transformieren(anweisung->als<Bestimmte_Anweisung_Solange *>());
    }

    else if (anweisung->art() == Asb_Knoten::ANWEISUNG_WENN)
    {
        wenn_anweisung_transformieren(anweisung->als<Bestimmte_Anweisung_Wenn *>());
    }

    else if (anweisung->art() == Asb_Knoten::ANWEISUNG_BEDINGTER_SPRUNG)
    {
        bedingter_sprung_anweisung_transformieren(anweisung->als<Bestimmte_Anweisung_Bedingter_Sprung *>());
    }

    else if (anweisung->art() == Asb_Knoten::ANWEISUNG_DEKLARATION)
    {
        deklaration_anweisung_transformieren(anweisung->als<Bestimmte_Anweisung_Deklaration *>());
    }

    else if (anweisung->art() == Asb_Knoten::ANWEISUNG_WEICHE)
    {
        /*weiche_transformieren(anweisung->als<Bestimmte_Anweisung_Weiche *>());*/
    }

    return anweisung;
}

Bestimmte_Anweisung *
Transformator::deklaration_anweisung_transformieren(Bestimmte_Anweisung_Deklaration *anweisung)
{
    auto *deklaration = deklaration_transformieren(anweisung->deklaration());

    if (deklaration == anweisung->deklaration())
    {
        return anweisung;
    }

    return new Bestimmte_Anweisung_Deklaration(
        (Anweisung *) anweisung->asb_knoten(), deklaration);
}

Bestimmte_Anweisung *
Transformator::block_anweisung_transformieren(Bestimmte_Anweisung_Block *anweisung)
{
    bool geändert = false;
    std::vector<Bestimmte_Anweisung *> rumpf;

    for (int i = 0; i < anweisung->anweisungen().size(); ++i)
    {
        auto *alte_anweisung = anweisung->anweisungen().at(i);
        auto *neue_anweisung = anweisung_transformieren(alte_anweisung);

        if (neue_anweisung != alte_anweisung)
        {
            geändert = true;
        }

        rumpf.push_back(neue_anweisung);
    }

    if (!geändert)
    {
        return anweisung;
    }

    auto *erg = new Bestimmte_Anweisung_Block(
        (Anweisung *) anweisung->asb_knoten(), rumpf);

    return erg;
}

Bestimmte_Anweisung *
Transformator::wenn_anweisung_transformieren(Bestimmte_Anweisung_Wenn *anweisung)
{
    auto *bedingung = ausdruck_transformieren(anweisung->bedingung());
    auto *rumpf     = anweisung_transformieren(anweisung->rumpf());
    auto *sonst     = anweisung->sonst() == nullptr
                          ? anweisung->sonst()
                          : anweisung_transformieren(anweisung->sonst());

    if (bedingung == anweisung->bedingung() && rumpf == anweisung->rumpf() && sonst == anweisung->sonst())
    {
        return anweisung;
    }

    auto *erg = new Bestimmte_Anweisung_Wenn(
        (Anweisung *) anweisung->asb_knoten(), bedingung, rumpf, sonst);

    return erg;
}

Bestimmte_Anweisung *
Transformator::für_anweisung_transformieren(Bestimmte_Anweisung_Für *anweisung)
{
    auto *bedingung = ausdruck_transformieren(anweisung->bedingung());
    auto *rumpf     = anweisung_transformieren(anweisung->rumpf());

    if (bedingung == anweisung->bedingung() && rumpf == anweisung->rumpf())
    {
        return anweisung;
    }

    auto *erg = new Bestimmte_Anweisung_Für(
        (Anweisung *) anweisung->asb_knoten(),
        anweisung->index(), anweisung->bedingung(), anweisung->rumpf(),
        anweisung->weiter_markierung(), anweisung->raus_markierung()
    );

    return erg;
}

Bestimmte_Anweisung *
Transformator::solange_anweisung_transformieren(Bestimmte_Anweisung_Solange *anweisung)
{
    auto *bedingung = ausdruck_transformieren(anweisung->bedingung());
    auto *rumpf     = anweisung_transformieren(anweisung->rumpf());

    if (bedingung == anweisung->bedingung() && rumpf == anweisung->rumpf())
    {
        return anweisung;
    }

    auto *erg = new Bestimmte_Anweisung_Solange(
        (Anweisung *) anweisung->asb_knoten(),
        anweisung->bedingung(), anweisung->rumpf(),
        anweisung->weiter_markierung(), anweisung->raus_markierung()
    );

    return erg;
}

Bestimmte_Anweisung *
Transformator::bedingter_sprung_anweisung_transformieren(Bestimmte_Anweisung_Bedingter_Sprung *anweisung)
{
    auto *bedingung = ausdruck_transformieren(anweisung->bedingung());

    if (bedingung == anweisung->bedingung())
    {
        return anweisung;
    }

    auto *erg = new Bestimmte_Anweisung_Bedingter_Sprung(
        (Anweisung *) anweisung->asb_knoten(),
        anweisung->markierung(), anweisung->bedingung()
    );

    return erg;
}

Bestimmte_Deklaration *
Transformator::deklaration_transformieren(Bestimmte_Deklaration *deklaration)
{
    switch (deklaration->art())
    {
        case Asb_Knoten::DEKLARATION_FUNKTION:
        {
            auto *erg = funktion_deklaration_transformieren(deklaration->als<Bestimmte_Deklaration_Funktion *>());

            return erg;
        } break;

        case Asb_Knoten::DEKLARATION_VARIABLE:
        {
            auto *erg = variable_deklaration_transformieren(deklaration->als<Bestimmte_Deklaration *>());

            return erg;
        } break;

        default:
        {
            assert(!"unbekannte deklaration");
            return nullptr;
        } break;
    }

    return nullptr;
}

Bestimmte_Deklaration *
Transformator::funktion_deklaration_transformieren(Bestimmte_Deklaration_Funktion *deklaration)
{
    auto *rumpf = anweisung_transformieren(deklaration->rumpf());

    if (rumpf == deklaration->rumpf())
    {
        return deklaration;
    }

    auto *erg = new Bestimmte_Deklaration_Funktion(
        (Deklaration *) deklaration->asb_knoten(),
        deklaration->symbol(),
        deklaration->datentyp(),
        rumpf);

    return erg;
}

Bestimmte_Deklaration *
Transformator::variable_deklaration_transformieren(Bestimmte_Deklaration *deklaration)
{
    return deklaration;
}

Bestimmter_Ausdruck *
Transformator::ausdruck_transformieren(Bestimmter_Ausdruck *ausdruck)
{
    switch (ausdruck->art())
    {
        case Asb_Knoten::AUSDRUCK_BEZEICHNER:
        {
            return ausdruck;
        } break;

        case Asb_Knoten::AUSDRUCK_GANZZAHL:
        {
            return ausdruck;
        } break;

        case Asb_Knoten::AUSDRUCK_DEZIMALZAHL:
        {
            return ausdruck;
        } break;

        case Asb_Knoten::AUSDRUCK_TEXT:
        {
            return ausdruck;
        } break;

        case Asb_Knoten::AUSDRUCK_BINÄR:
        {
            auto *bin = ausdruck->als<Bestimmter_Ausdruck_Binär *>();

            auto *links = ausdruck_transformieren(bin->links());
            auto *rechts = ausdruck_transformieren(bin->rechts());

            if (links == bin->links() && rechts == bin->rechts())
            {
                return ausdruck;
            }

            auto *erg = new Bestimmter_Ausdruck_Binär(
                (Ausdruck_Binär *) bin->asb_knoten(),
                bin->operand(), bin->op(), links, rechts);

            return erg;
        } break;

        default:
        {
            assert(!"unbekannter ausdruck");
        } break;
    }

    return nullptr;
}
/* }}} */
/* ersteller {{{ */
Bestimmte_Anweisung *
Transformator::Block(Asb_Knoten *asb_knoten, std::vector<Bestimmte_Anweisung *> rumpf)
{
    auto *erg = new Bestimmte_Anweisung_Block((Anweisung *) asb_knoten, rumpf);

    return erg;
}

Bestimmte_Anweisung *
Transformator::Variable_Deklaration(Asb_Knoten *asb_knoten, Symbol *symbol, Bestimmter_Ausdruck *ausdruck)
{
    auto *deklaration = new Bestimmte_Deklaration((Deklaration *) asb_knoten, asb_knoten->art(), symbol, symbol->datentyp(), ausdruck);
    auto *erg = new Bestimmte_Anweisung_Deklaration((Anweisung *) asb_knoten, deklaration);

    return erg;
}

Bestimmte_Anweisung *
Transformator::Konstante_Deklaration(Asb_Knoten *asb_knoten, std::string name, Bestimmter_Ausdruck *ausdruck)
{
    Symbol *symbol = new Symbol(ausdruck->spanne(), Symbol::VARIABLE, Symbol::BEHANDELT, name);
    auto *deklaration = new Bestimmte_Deklaration(
        (Deklaration *) asb_knoten, asb_knoten->art(), symbol, ausdruck->operand()->datentyp(), ausdruck);
    auto *erg = new Bestimmte_Anweisung_Deklaration((Anweisung *) asb_knoten, deklaration);

    return erg;
}

Bestimmter_Ausdruck *
Transformator::Variable(Asb_Knoten *asb_knoten, Bestimmter_Ausdruck *ausdruck)
{

    Bestimmter_Ausdruck *erg = new Bestimmter_Ausdruck_Bezeichner(
        (Ausdruck_Bezeichner *) asb_knoten, ausdruck->als<Bestimmter_Ausdruck_Bezeichner *>()->operand());

    return erg;
}

Bestimmter_Ausdruck *
Transformator::Kleiner_Gleich(Asb_Knoten *asb_knoten, Bestimmter_Ausdruck *links, Bestimmter_Ausdruck *rechts)
{
    auto *operand = new Operand(System::Bool);

    Bestimmter_Ausdruck *erg = new Bestimmter_Ausdruck_Binär(
        (Ausdruck_Binär *)asb_knoten, operand, Ausdruck_Binär::KLEINER_GLEICH, links, rechts);

    return erg;
}

Bestimmte_Anweisung *
Transformator::Solange(Asb_Knoten *asb_knoten, Bestimmter_Ausdruck *bedingung, Bestimmte_Anweisung *rumpf, std::string weiter_markierung, std::string raus_markierung)
{
    Bestimmte_Anweisung *erg =
        new Bestimmte_Anweisung_Solange((Anweisung *) asb_knoten, bedingung, rumpf, weiter_markierung, raus_markierung);

    return erg;
}

Bestimmte_Anweisung *
Transformator::Markierung(Asb_Knoten *asb_knoten, std::string markierung)
{
    auto *erg =
        new Bestimmte_Anweisung_Markierung((Anweisung *) asb_knoten, markierung);

    return erg;
}

Bestimmte_Anweisung *
Transformator::Hochzählen(Asb_Knoten *asb_knoten, Bestimmter_Ausdruck *ausdruck)
{
    assert(!"impl");
    return nullptr;
}

Bestimmte_Anweisung *
Transformator::Sprung(Asb_Knoten *asb_knoten, std::string markierung)
{
    Bestimmte_Anweisung *erg = new Bestimmte_Anweisung_Sprung(
        (Anweisung *) asb_knoten, markierung
    );

    return erg;
}

Bestimmte_Anweisung *
Transformator::Sprung_Wahr(Asb_Knoten *asb_knoten, std::string markierung, Bestimmter_Ausdruck *bedingung)
{
    Bestimmte_Anweisung *erg = new Bestimmte_Anweisung_Bedingter_Sprung(
        (Anweisung *) asb_knoten, markierung, bedingung
    );

    return erg;
}

Bestimmte_Anweisung *
Transformator::Sprung_Unwahr(Asb_Knoten *asb_knoten, std::string markierung, Bestimmter_Ausdruck *bedingung)
{
    Bestimmte_Anweisung *erg = new Bestimmte_Anweisung_Bedingter_Sprung(
        (Anweisung *) asb_knoten, markierung, bedingung
    );

    return erg;
}
/* }}} */
}

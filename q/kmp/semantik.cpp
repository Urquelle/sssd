#include "kmp/semantik.hpp"

#include <iostream>
#include <cassert>
#include <fstream>
#include <sstream>
#include <format>
#include <stack>
#include <utility>

#include "kmp/asb.hpp"
#include "kmp/bestimmter_asb.hpp"
#include "kmp/lexer.hpp"
#include "kmp/syntax.hpp"
#include "kmp/datentyp.hpp"
#include "kmp/reduzierer.hpp"

int global_semantik_markierung_index = 0;
// INFO: pair<weiter_markierung, raus_markierung>
std::stack<std::pair<std::string, std::string>> global_markierungen;
std::string markierung_erstellen(std::string markierung);

#define SSS_HAUPTMETHODE "Haupt"

namespace Sss::Kmp {

Semantik::Semantik(Asb asb, Zone *system, Zone *global)
    : _asb(asb)
    , _system(system)
    , _global(global)
    , _hauptmethode(nullptr)
{
    _zone = _global;
}

// diagnostik {{{

void
Semantik::panik(Spanne spanne, std::string text)
{
    melden(spanne, text);

    for (auto meldung : _diagnostik.meldungen())
    {
         std::cout << meldung << std::endl;
    }

    __debugbreak();
}

void
Semantik::panik(Symbol *symbol, std::string text)
{
    panik(symbol->spanne(), text);
}

void
Semantik::panik(Ausdruck *ausdruck, std::string text)
{
    panik(ausdruck->spanne(), text);
}

void
Semantik::panik(Anweisung *anweisung, std::string text)
{
    panik(anweisung->spanne(), text);
}

void
Semantik::panik(Spezifizierung *spezifizierung, std::string text)
{
    panik(spezifizierung->spanne(), text);
}

void
Semantik::panik(Deklaration *deklaration, std::string text)
{
    panik(deklaration->spanne(), text);
}

void
Semantik::melden(Spanne spanne, std::string text)
{
    _diagnostik.melden(spanne, new Fehler(text));
}

void
Semantik::melden(Ausdruck *ausdruck, std::string text)
{
    melden(ausdruck->spanne(), text);
}

void
Semantik::melden(Anweisung *anweisung, std::string text)
{
    panik(anweisung->spanne(), text);
}

void
Semantik::melden(Spezifizierung *spezifizierung, std::string text)
{
    melden(spezifizierung->spanne(), text);
}

void
Semantik::melden(Symbol *symbol, std::string text)
{
    melden(symbol->spanne(), text);
}

void
Semantik::melden(Deklaration *deklaration, std::string text)
{
    melden(deklaration->spanne(), text);
}

Diagnostik
Semantik::diagnostik() const
{
    return _diagnostik;
}

// }}}

Zone *
Semantik::aktive_zone() const
{
    return _zone;
}

Zone *
Semantik::system() const
{
    return _system;
}

Zone *
Semantik::global() const
{
    return _global;
}

Bestimmter_Asb *
Semantik::starten(bool mit_hauptmethode)
{
    _bestimmter_asb = new Bestimmter_Asb(_system, _global);

    importe_registrieren();
    deklarationen_registrieren();
    symbole_analysieren(mit_hauptmethode);

    if (mit_hauptmethode)
    {
        if (_hauptmethode == nullptr)
        {
            melden(_asb.anweisungen[0]->spanne(), "Die Hauptmethode \"" SSS_HAUPTMETHODE "\" muss im Programm vorhanden sein.");
            return nullptr;
        }

        globale_anweisungen_der_hauptmethode_zuordnen();
    }

    anweisungen_reduzieren();

    return _bestimmter_asb;
}

void
Semantik::anweisungen_reduzieren()
{
    auto reduzierer = Reduzierer();
    std::vector<Bestimmte_Anweisung *> anweisungen;

    for (auto *anweisung : _bestimmter_asb->anweisungen())
    {
        auto *a = reduzierer.anweisung_transformieren(anweisung);
        anweisungen.push_back(a);
    }

    _bestimmter_asb->anweisungen_setzen(anweisungen);
}

void
Semantik::importe_registrieren()
{
    // AUFGABE: zirkuläre importe verhindern
    for (auto *anweisung: _asb.anweisungen)
    {
        if (anweisung->art() == Asb_Knoten::ANWEISUNG_BRAUCHE)
        {
            auto *import = anweisung->als<Anweisung_Brauche *>();
            import_verarbeiten(import->dateiname());
        }

        else if (anweisung->art() == Asb_Knoten::ANWEISUNG_DEKLARATION)
        {
            auto *deklaration = anweisung->als<Anweisung_Deklaration *>()->deklaration();

            if (deklaration->art() != Asb_Knoten::DEKLARATION_BRAUCHE)
            {
                continue;
            }

            auto *import = deklaration->als<Deklaration_Brauche *>();

            auto dateiname = import->ausdruck()->dateiname();

            std::ifstream datei(dateiname);
            std::stringstream inhalt;
            inhalt << datei.rdbuf();

            auto lexer = Lexer(dateiname, inhalt.str());
            auto token = lexer.starten();

            auto syntax = Syntax(token);
            auto ast = syntax.starten();

            auto semantik = Semantik(ast, _system, new Zone(import->name(), _system));
            auto *brauche_bestimmter_ast = semantik.starten(false);

            deklaration->symbol_setzen(
                new Symbol(deklaration->spanne(),
                           Symbol::MODUL, Symbol::BEHANDELT, import->name(), semantik.global()));

            if (!registrieren(deklaration->symbol()))
            {
                panik(deklaration, std::format("bezeichner {} bereits vorhanden.", import->name()));
            }
        }
    }
}

void
Semantik::deklarationen_registrieren()
{
    for (auto *anweisung : _asb.anweisungen)
    {
        if (anweisung->art() != Asb_Knoten::ANWEISUNG_DEKLARATION)
        {
            continue;
        }

        auto *deklaration = anweisung->als<Anweisung_Deklaration *>()->deklaration();

        for (auto name : deklaration->namen())
        {
            switch (deklaration->art())
            {
                case Asb_Knoten::DEKLARATION_BRAUCHE:
                {
                    // INFO: deklaration brauche wird bereits in der methode importe_registrieren behandelt.
                } break;

                case Asb_Knoten::DEKLARATION_VARIABLE:
                {
                    auto *symbol = new Symbol(deklaration->spanne(), Symbol::VARIABLE, Symbol::UNBEHANDELT, name);
                    deklaration->symbol_setzen(symbol);

                    if (!registrieren(symbol))
                    {
                        melden(symbol, std::format("{} ist bereits vorhanden.", symbol->name()));
                    }
                } break;

                case Asb_Knoten::DEKLARATION_FUNKTION:
                {
                    auto *funktion = deklaration->als<Deklaration_Funktion *>();
                    auto *datentyp = new Datentyp_Funktion();
                    auto *symbol = new Symbol(
                        deklaration->spanne(),
                        Symbol::FUNKTION, Symbol::UNBEHANDELT,
                        name,
                        datentyp);

                    datentyp->symbol_setzen(symbol);
                    datentyp->deklaration_setzen(deklaration);
                    deklaration->symbol_setzen(symbol);

                    if (!registrieren(symbol))
                    {
                        melden(symbol, std::format("{} ist in dieser zone bereits vorhanden", symbol->name()));
                    }
                } break;

                case Asb_Knoten::DEKLARATION_SCHABLONE:
                {
                    auto *datentyp = new Datentyp_Schablone(deklaration->als<Deklaration_Schablone *>());

                    auto *symbol = new Symbol(
                        deklaration->spanne(),
                        Symbol::DATENTYP, Symbol::BEHANDELT,
                        name,
                        datentyp);

                    datentyp->symbol_setzen(symbol);
                    datentyp->deklaration_setzen(deklaration);
                    deklaration->symbol_setzen(symbol);

                    _schablonen.push_back(deklaration->als<Deklaration_Schablone *>());

                    if (!registrieren(symbol))
                    {
                        melden(symbol, std::format("{} ist bereits vorhanden.", symbol->name()));
                    }
                } break;

                case Asb_Knoten::DEKLARATION_OPTION:
                {
                    auto *datentyp = new Datentyp_Option();
                    auto *symbol = new Symbol(
                        deklaration->spanne(),
                        Symbol::DATENTYP, Symbol::UNBEHANDELT,
                        name, datentyp);

                    datentyp->symbol_setzen(symbol);
                    datentyp->deklaration_setzen(deklaration);
                    deklaration->symbol_setzen(symbol);

                    if (!registrieren(symbol))
                    {
                        melden(symbol, std::format("{} ist bereits vorhanden.", symbol->name()));
                    }
                } break;

                default:
                {
                    assert(!"unbekannte deklaration");
                } break;
            }
        }
    }
}

void
Semantik::symbole_analysieren(bool mit_hauptmethode)
{
    for (auto *anweisung : _asb.anweisungen)
    {
        if (anweisung->art() != Asb_Knoten::ANWEISUNG_DEKLARATION)
        {
            continue;
        }

        auto *deklaration = anweisung->als<Anweisung_Deklaration *>()->deklaration();
        auto *symbol = deklaration->symbol();

        std::cout << "mit hauptmethode: " << mit_hauptmethode << " symbol: " << symbol->name() << std::endl;

        if (mit_hauptmethode)
        {
            if (symbol->name().compare(SSS_HAUPTMETHODE) == 0 && symbol->art() == Symbol::FUNKTION)
            {
                _hauptmethode = deklaration->symbol();
            }
        }

        auto *bestimmte_deklaration = symbol_analysieren(symbol, deklaration);

        if (bestimmte_deklaration != nullptr)
        {
            auto *bestimmte_anweisung =
                new Bestimmte_Anweisung_Deklaration(anweisung->als<Anweisung *>(), bestimmte_deklaration);

            _bestimmter_asb->anweisungen().push_back(bestimmte_anweisung);
        }
    }
}

Bestimmte_Deklaration *
Semantik::symbol_analysieren(Symbol *symbol, Deklaration *deklaration)
{
    if (symbol->status() == Symbol::BEHANDELT)
    {
        return nullptr;
    }

    if (symbol->status() == Symbol::IN_BEHANDLUNG)
    {
        melden(symbol, "Zirkuläre Abhängigkeit");

        return nullptr;
    }

    symbol->status_setzen(Symbol::IN_BEHANDLUNG);

    Bestimmte_Deklaration *erg = nullptr;

    switch (deklaration->art())
    {
        case Asb_Knoten::DEKLARATION_VARIABLE:
        {
            erg = variable_analysieren(symbol, deklaration->als<Deklaration_Variable *>());
        } break;

        case Asb_Knoten::DEKLARATION_FUNKTION:
        {
            erg = funktion_analysieren(symbol, deklaration->als<Deklaration_Funktion *>());
        } break;

        case Asb_Knoten::DEKLARATION_OPTION:
        {
            erg = option_analysieren(symbol, deklaration->als<Deklaration_Option *>());
        } break;

        case Asb_Knoten::DEKLARATION_SCHABLONE:
        {
            erg = schablone_analysieren(symbol, deklaration->als<Deklaration_Schablone *>());
        } break;

        default:
        {
            melden(deklaration, std::format("unbekannte deklaration"));
        } break;
    }

    symbol->deklaration_setzen(erg);
    symbol->status_setzen(Symbol::BEHANDELT);

    return erg;
}

void
Semantik::globale_anweisungen_der_hauptmethode_zuordnen()
{
    assert(_hauptmethode);
    auto *dekl = _hauptmethode->deklaration()->als<Bestimmte_Deklaration_Funktion *>();

    std::vector<Bestimmte_Anweisung *> globale_anweisungen;
    for (auto *anweisung : _asb.anweisungen)
    {
        if (anweisung->art() == Asb_Knoten::ANWEISUNG_DEKLARATION ||
            anweisung->art() == Asb_Knoten::ANWEISUNG_LADE        ||
            anweisung->art() == Asb_Knoten::ANWEISUNG_BRAUCHE)
        {
            continue;
        }

        Bestimmte_Anweisung *bestimmte_anweisung = nullptr;
        anweisung_analysieren((Anweisung *) anweisung, nullptr, &bestimmte_anweisung);
        assert(bestimmte_anweisung);

        globale_anweisungen.push_back(bestimmte_anweisung);
    }

    auto *globaler_block = new Bestimmte_Anweisung_Block(nullptr, globale_anweisungen);
    auto *neuer_rumpf = new Bestimmte_Anweisung_Block(nullptr, std::vector<Bestimmte_Anweisung *>(
        { globaler_block, dekl->rumpf() }));

    dekl->rumpf_setzen(neuer_rumpf);
}

Bestimmte_Deklaration *
Semantik::variable_analysieren(Symbol *symbol, Deklaration_Variable *deklaration)
{
    Datentyp *datentyp = nullptr;
    Bestimmte_Deklaration *erg = nullptr;
    Bestimmter_Ausdruck *initialisierung = nullptr;

    if (deklaration->spezifizierung() != nullptr)
    {
        datentyp = spezifizierung_analysieren(deklaration->spezifizierung());
        datentyp_abschließen(datentyp);
    }

    if (deklaration->initialisierung() != nullptr)
    {
        initialisierung = ausdruck_analysieren(deklaration->initialisierung());

        if (datentyp == nullptr)
        {
            datentyp = initialisierung->operand()->datentyp();
        }
        else
        {
            if (!Datentyp::datentypen_kompatibel(datentyp, initialisierung->operand()->datentyp()))
            {
                melden(deklaration->initialisierung(), std::format("initialisierung ist nicht mit dem deklarierten datentyp kompatibel"));
            }
        }
    }

    if (datentyp == nullptr)
    {
        melden(deklaration, "datentyp konnte nicht ermittelt oder abgeleitet werden.");

        return nullptr;
    }

    erg = new Bestimmte_Deklaration(deklaration, deklaration->art(), symbol, datentyp, initialisierung);

    return erg;
}

Bestimmte_Deklaration *
Semantik::funktion_analysieren(Symbol *symbol, Deklaration_Funktion *deklaration)
{
    auto *datentyp = symbol->datentyp()->als<Datentyp_Funktion *>();

    for (auto *parameter : deklaration->parameter())
    {
        auto *parameter_datentyp = spezifizierung_analysieren(parameter->spezifizierung());
        datentyp_abschließen(parameter_datentyp);

        if (parameter_datentyp == nullptr)
        {
            assert(!"meldung erstatten");
        }

        if (parameter->als<Deklaration_Variable *>()->initialisierung())
        {
            auto *parameter_op =
                ausdruck_analysieren(parameter->als<Deklaration_Variable *>()->initialisierung());
        }

        auto *parameter_symbol = new Symbol(
            parameter->spanne(),
            Symbol::VARIABLE, Symbol::BEHANDELT, parameter->namen()[0], parameter_datentyp);

        if (!symbol->zone()->registrieren(parameter_symbol))
        {
            melden(parameter, std::format("{} konnte nicht registriert werden.", parameter->namen()[0]));

            return nullptr;
        }

        datentyp->parameter_hinzufügen(parameter_datentyp);
    }

    Datentyp *rückgabe = System::Nihil;
    if (deklaration->rückgabe())
    {
        rückgabe = spezifizierung_analysieren(deklaration->rückgabe());
    }

    datentyp->rückgabe_setzen(rückgabe);

    // INFO: eine funktionsdeklaration kann ohne rumpf auskommen.
    Bestimmte_Anweisung *rumpf = nullptr;
    if (deklaration->rumpf() != nullptr)
    {
        auto *z = symbol->zone();
        z->über_setzen(aktive_zone());

        zone_betreten(symbol->zone());
            auto erg = anweisung_analysieren(deklaration->rumpf(), datentyp->rückgabe(), &rumpf);
#if 0
            auto reduzierer = Reduzierer();
            rumpf = reduzierer.anweisung_transformieren(rumpf);
#endif

            if (!erg && rückgabe != System::Nihil)
            {
                melden(symbol, std::format("nicht alle codepfade geben einen wert zurück."));
            }
        zone_verlassen();
    }

    auto *erg = new Bestimmte_Deklaration_Funktion(deklaration, symbol, datentyp, rumpf);

    return erg;
}

Bestimmte_Deklaration *
Semantik::option_analysieren(Symbol *symbol, Deklaration_Option *deklaration)
{
    auto *datentyp = symbol->datentyp()->als<Datentyp_Option *>();

    auto *z = symbol->zone();
    z->über_setzen(aktive_zone());

    zone_betreten(symbol->zone());

    for (auto *eigenschaft : deklaration->eigenschaften())
    {
        Datentyp *dt = nullptr;
        if (eigenschaft->spezifizierung())
        {
            dt = spezifizierung_analysieren(eigenschaft->spezifizierung());
        }

        Operand *op = nullptr;
        if (eigenschaft->initialisierung())
        {
            auto *ausdruck = ausdruck_analysieren(eigenschaft->initialisierung(), dt);
            op = ausdruck->operand();
        }

        for (auto name : eigenschaft->namen())
        {
            datentyp->eigenschaft_hinzufügen(dt, op, name);
        }
    }

    zone_verlassen();

    datentyp->abschließen();

    return new Bestimmte_Deklaration_Option(deklaration, symbol, datentyp);
}

Bestimmte_Deklaration *
Semantik::schablone_analysieren(Symbol *symbol, Deklaration_Schablone *deklaration, bool zirkularität_ignorieren)
{
    auto *datentyp = symbol->datentyp()->als<Datentyp_Schablone *>();

    if (datentyp->status() == Datentyp::BEHANDELT ||
        datentyp->status() == Datentyp::IN_BEHANDLUNG && zirkularität_ignorieren)
    {
        return nullptr;
    }

    if (datentyp->status() == Datentyp::IN_BEHANDLUNG)
    {
        panik(symbol, std::format("datentyp {} mit zirkulärer abhängigkeit.", symbol->name()));
    }

    datentyp->status_setzen(Datentyp::IN_BEHANDLUNG);

    assert(symbol != nullptr);
    assert(datentyp != nullptr);

    size_t größe = 0;
    size_t versatz = 0;

    auto *z = symbol->zone();
    z->über_setzen(aktive_zone());

    zone_betreten(z);

    for (auto *eigenschaft : deklaration->eigenschaften())
    {
        auto *dt = deklaration_analysieren(eigenschaft);

        for (auto name : eigenschaft->namen())
        {
            größe += dt->datentyp()->größe();
            if (!datentyp->eigenschaft_hinzufügen(dt->datentyp(), versatz, name))
            {
                panik(eigenschaft, std::format("{} bereits vorhanden.", name));
            }
            versatz += dt->datentyp()->größe();
        }
    }

    zone_verlassen();

    datentyp->größe_setzen(größe);
    datentyp->status_setzen(Datentyp::BEHANDELT);

    auto *erg = new Bestimmte_Deklaration_Schablone(deklaration, symbol, datentyp);

    return erg;
}

Bestimmter_Ausdruck *
Semantik::muster_analysieren(Ausdruck *muster, Datentyp *datentyp)
{
    switch (muster->art())
    {
        case Asb_Knoten::AUSDRUCK_BEZEICHNER:
        {
            auto name = muster->als<Ausdruck_Bezeichner *>()->wert();
            auto *symbol = new Symbol(muster->spanne(), Symbol::VARIABLE, Symbol::BEHANDELT, name, datentyp);
            if (!registrieren(symbol))
            {
                melden(muster, std::format("bezeichner {} konnte nicht registriert werden.", name));
            }

            auto *erg = new Bestimmter_Ausdruck_Bezeichner((Ausdruck_Bezeichner *) muster, new Operand(symbol));

            return erg;
        } break;

        case Asb_Knoten::AUSDRUCK_KOMPOSITUM:
        {
            if (datentyp->art() != Datentyp::SCHABLONE)
            {
                melden(muster, std::format("kompositum ausdruck kann nur bei einer schablone angewendet werden"));
            }

            auto *schablone = datentyp->als<Datentyp_Schablone *>();
            auto *kompositum = muster->als<Ausdruck_Kompositum *>();

            std::vector<Bestimmter_Ausdruck_Kompositum::Bestimmte_Eigenschaft *> eigenschaften;
            for (auto *kompositum_eigenschaft : kompositum->eigenschaften())
            {
                bool eigenschaft_gefunden = false;

                for (auto *schablone_eigenschaft : schablone->eigenschaften())
                {
                    if (kompositum_eigenschaft->name() == schablone_eigenschaft->name())
                    {
                        auto *symbol = new Symbol(
                                kompositum_eigenschaft->spanne(),
                                Symbol::VARIABLE,
                                Symbol::BEHANDELT,
                                kompositum_eigenschaft->name(),
                                schablone_eigenschaft->datentyp());

                        if (!registrieren(symbol))
                        {
                            melden(kompositum_eigenschaft->spanne(),
                                  std::format("bezeichner {} konnte nicht registriert werden.", kompositum_eigenschaft->name()));
                        }

                        auto kompositum_eigenschaft_ausdruck = ausdruck_analysieren(kompositum_eigenschaft->ausdruck());

                        if (Datentyp::datentypen_kompatibel(
                            schablone_eigenschaft->datentyp(),
                            kompositum_eigenschaft_ausdruck->operand()->datentyp()) == Datentyp::INKOMPATIBEL)
                        {
                            melden(kompositum_eigenschaft->spanne(), std::format(
                                "datentyp des ausdrucks {} ist nicht kompatibel mit dem datentyp der eigenschaft {}",
                                    kompositum_eigenschaft_ausdruck->operand()->datentyp()->symbol()->name(),
                                    schablone_eigenschaft->datentyp()->symbol()->name()));
                        }

                        eigenschaft_gefunden = true;

                        kompositum_eigenschaft_ausdruck->operand()->symbol_setzen(symbol);

                        eigenschaften.push_back(new Bestimmter_Ausdruck_Kompositum::Bestimmte_Eigenschaft(
                            kompositum_eigenschaft_ausdruck->operand()
                        ));

                        break;
                    }
                }

                if (!eigenschaft_gefunden)
                {
                    melden(kompositum_eigenschaft->spanne(),
                          std::format("eigenschaft {} in der schablone {} nicht vorhanden",
                              kompositum_eigenschaft->name(),
                              schablone->symbol()->name()));
                }
            }

            auto *erg = new Bestimmter_Ausdruck_Kompositum(
                (Ausdruck_Kompositum *) muster,
                new Operand(datentyp),
                eigenschaften);

            return erg;
        } break;

        default:
        {
            assert(!"unbekannter ausdruck");
            return nullptr;
        } break;
    }
}

Bestimmte_Deklaration *
Semantik::deklaration_analysieren(Deklaration *deklaration)
{
    switch (deklaration->art())
    {
        case Asb_Knoten::DEKLARATION_VARIABLE:
        {
            auto *spezifizierung  = deklaration->als<Deklaration_Variable *>()->spezifizierung();
            auto *initialisierung = deklaration->als<Deklaration_Variable *>()->initialisierung();

            Datentyp *dt_spezifizierung = nullptr;
            if (spezifizierung)
            {
                dt_spezifizierung = spezifizierung_analysieren(spezifizierung);
                datentyp_abschließen(dt_spezifizierung);
            }

            Datentyp *dt_initialisierung = dt_spezifizierung;
            if (initialisierung)
            {
                auto *ausdruck = ausdruck_analysieren(initialisierung, dt_spezifizierung);
                dt_initialisierung = ausdruck->operand()->datentyp();
                datentyp_abschließen(dt_initialisierung);
            }

            if (dt_spezifizierung == nullptr && dt_initialisierung != nullptr)
            {
                dt_spezifizierung = dt_initialisierung;
            }

            if (Datentyp::datentypen_kompatibel(dt_spezifizierung, dt_initialisierung) == Datentyp::INKOMPATIBEL)
            {
                melden(deklaration,
                        "datentyp der spezifizierung und der initialisierung sind nicht kompatibel.");
            }

            auto *datentyp = dt_spezifizierung
                    ? dt_spezifizierung
                    : dt_initialisierung;

            std::vector<Symbol *> symbole;
            for (auto name : deklaration->namen())
            {
                auto *sym = new Symbol(deklaration->spanne(), Symbol::VARIABLE, Symbol::BEHANDELT, name, datentyp);
                if (!registrieren(sym))
                {
                    assert(!"meldung erstatten");
                }

                symbole.push_back(sym);
            }

            auto *erg = new Bestimmte_Deklaration(deklaration, symbole, datentyp);

            return erg;
        } break;

        case Asb_Knoten::DEKLARATION_SCHABLONE:
        {
            _schablonen.push_back(deklaration->als<Deklaration_Schablone *>());
        } break;

        default: assert(!"unbekannte deklaration");
    }

    return nullptr;
}

Datentyp *
Semantik::spezifizierung_analysieren(Spezifizierung *spezifizierung)
{
    switch (spezifizierung->art())
    {
        case Asb_Knoten::SPEZIFIZIERUNG_BEZEICHNER:
        {
            auto name = spezifizierung->als<Spezifizierung_Bezeichner *>()->name();
            auto *symbol = bezeichner_analysieren(name);

            if (symbol == nullptr)
            {
                panik(spezifizierung, std::format("{} ist kein bekannter Datentyp.", name));
            }

            return symbol->datentyp();
        } break;

        case Asb_Knoten::SPEZIFIZIERUNG_FELD:
        {
            auto *feld = spezifizierung->als<Spezifizierung_Feld *>();

            auto *basis_datentyp = spezifizierung_analysieren(feld->basis());
            Bestimmter_Ausdruck *index = nullptr;
            Datentyp *index_datentyp = nullptr;

            if (feld->index())
            {
                index = ausdruck_analysieren(feld->index());

                if (index != nullptr && index->operand() != nullptr && index->operand()->datentyp() != nullptr)
                {
                    index_datentyp = index->operand()->datentyp();
                }
            }

            return new Datentyp_Feld(basis_datentyp, index_datentyp);
        } break;

        case Asb_Knoten::SPEZIFIZIERUNG_ZEIGER:
        {
            auto *basis_datentyp =
                spezifizierung_analysieren(spezifizierung->als<Spezifizierung_Zeiger *>()->basis());

            return new Datentyp_Zeiger(basis_datentyp);
        } break;

        case Asb_Knoten::SPEZIFIZIERUNG_FUNKTION:
        {
            std::vector<Datentyp *> parameter;
            Datentyp *rückgabe = nullptr;

            for (auto *param : spezifizierung->als<Spezifizierung_Funktion *>()->parameter())
            {
                auto *param_datentyp = spezifizierung_analysieren(param);

                if (param_datentyp == nullptr)
                {
                    assert(!"meldung erstatten");
                }

                parameter.push_back(param_datentyp);
            }

            if (spezifizierung->als<Spezifizierung_Funktion *>()->rückgabe())
            {
                rückgabe = spezifizierung_analysieren(spezifizierung->als<Spezifizierung_Funktion *>()->rückgabe());
            }

            return new Datentyp_Funktion(parameter, rückgabe);
        } break;

        default: assert(!"unbekannte spezifizierung");
    }

    return nullptr;
}

Symbol *
Semantik::bezeichner_analysieren(std::string bezeichner)
{
    auto *erg = _zone->suchen(bezeichner);

    return erg;
}

void
Semantik::datentyp_abschließen(Datentyp *datentyp, bool basis_eines_zeigers)
{
    if (datentyp->abgeschlossen())
    {
        return;
    }

    switch (datentyp->art())
    {
        case Datentyp::SCHABLONE:
        {
            schablone_analysieren(datentyp->symbol(), datentyp->deklaration()->als<Deklaration_Schablone *>(), basis_eines_zeigers);
        } break;

        case Datentyp::FELD:
        {
            /*assert(!"Feld Datentyp abschließen");*/
        } break;

        case Datentyp::ZEIGER:
        {
            auto *zeiger = datentyp->als<Datentyp_Zeiger *>();

            datentyp_abschließen(zeiger->basis(), true);
        } break;

        /*default: assert(!"implementieren"); break;*/
    }
}

Bestimmter_Ausdruck *
Semantik::ausdruck_analysieren(Ausdruck *ausdruck, Datentyp *erwarteter_datentyp)
{
    switch (ausdruck->art())
    {
        case Asb_Knoten::AUSDRUCK_DEZIMALZAHL:
        {
            auto *dez = ausdruck->als<Ausdruck_Dezimalzahl *>();
            auto *op = new Operand(System::D32, Operand::LITERAL | Operand::ARITHMETISCH);
            op->wert_setzen(new Operand::Wert(dez->wert()));

            auto *erg = new Bestimmter_Ausdruck_Dezimalzahl(
                ausdruck->als<Ausdruck_Dezimalzahl *>(), op
            );

            return erg;
        } break;

        case Asb_Knoten::AUSDRUCK_GANZZAHL:
        {
            auto *ganz = ausdruck->als<Ausdruck_Ganzzahl *>();
            auto *op = new Operand(System::G32, Operand::LITERAL | Operand::ARITHMETISCH);
            op->wert_setzen(new Operand::Wert(ganz->wert()));

            auto *erg = new Bestimmter_Ausdruck_Ganzzahl(
                ausdruck->als<Ausdruck_Ganzzahl *>(), op
            );

            return erg;
        } break;

        case Asb_Knoten::AUSDRUCK_TEXT:
        {
            auto *text = ausdruck->als<Ausdruck_Text *>();
            auto *op = new Operand(System::Text, Operand::LITERAL);
            op->wert_setzen(new Operand::Wert(text->wert()));

            auto *erg = new Bestimmter_Ausdruck_Text(
                ausdruck->als<Ausdruck_Text *>(), op
            );

            return erg;
        } break;

        case Asb_Knoten::AUSDRUCK_BEZEICHNER:
        {
            auto *bezeichner = ausdruck->als<Ausdruck_Bezeichner *>();
            auto *symbol = bezeichner_analysieren(bezeichner->wert());

            if (symbol == nullptr)
            {
                melden(ausdruck,
                        std::format("{} konnte in der aktuellen zone nicht augelöst werden.", bezeichner->wert()));
            }

            auto merkmale = 0;
            if (symbol->datentyp())
            {
                merkmale = symbol->datentyp()->ist_arithmetisch() ? Operand::ARITHMETISCH : 0;
            }

            auto *erg = new Bestimmter_Ausdruck_Bezeichner(
                ausdruck->als<Ausdruck_Bezeichner *>(),
                new Operand(symbol, merkmale)
            );

            return erg;
        } break;

        case Asb_Knoten::AUSDRUCK_INDEX:
        {
            auto *index_ausdruck = ausdruck->als<Ausdruck_Index *>();

            auto *basis = ausdruck_analysieren(index_ausdruck->basis());
            if (basis == nullptr)
            {
                return basis;
            }

            auto *index = ausdruck_analysieren(index_ausdruck->index());

            if (index == nullptr)
            {
                return index;
            }

            auto *erg = new Bestimmter_Ausdruck_Index(
                index_ausdruck, index->operand(), basis, index
            );
        } break;

        case Asb_Knoten::AUSDRUCK_UNÄR:
        {
            auto *unär = ausdruck->als<Ausdruck_Unär *>();
            auto *bestimmter_ausdruck = ausdruck_analysieren(unär->ausdruck());

            auto *erg = new Bestimmter_Ausdruck_Unär(
                unär,
                bestimmter_ausdruck->operand(),
                unär->op(),
                bestimmter_ausdruck
            );

            return erg;
        } break;

        case Asb_Knoten::AUSDRUCK_BINÄR:
        {
            auto *bin = ausdruck->als<Ausdruck_Binär *>();

            auto *links  = ausdruck_analysieren(bin->links());
            auto *rechts = ausdruck_analysieren(bin->rechts());

            if (bin->op() >= Ausdruck_Binär::ADDITION && bin->op() <= Ausdruck_Binär::MODULO ||
                bin->op() >= Ausdruck_Binär::BIT_VERSATZ_LINKS && bin->op() <= Ausdruck_Binär::BIT_VERSATZ_RECHTS)
            {
                if (!links->operand()->ist_arithmetisch())
                {
                    panik(bin->links(), "arithmetischen operanden erwartet");
                }

                if (!rechts->operand()->ist_arithmetisch())
                {
                    panik(bin->links(), "arithmetischen operanden erwartet");
                }

                if (Datentyp::datentypen_kompatibel(
                        links->operand()->datentyp(),
                        rechts->operand()->datentyp()) == Datentyp::INKOMPATIBEL)
                {
                    panik(ausdruck, "datentypen inkompatibel");
                }

                auto *erg = new Bestimmter_Ausdruck_Binär(
                    bin,
                    new Operand(links->operand()->datentyp()),
                    bin->op(),
                    links, rechts
                );

                return erg;
            }
            else if (bin->op() >= Ausdruck_Binär::GLEICH && bin->op() <= Ausdruck_Binär::GRÖẞER_GLEICH)
            {
                if (Datentyp::datentypen_kompatibel(
                        links->operand()->datentyp(),
                        rechts->operand()->datentyp()) == Datentyp::INKOMPATIBEL)
                {
                    panik(ausdruck, "datentypen inkompatibel");
                }

                auto *erg = new Bestimmter_Ausdruck_Binär(
                    bin,
                    new Operand(System::Bool),
                    bin->op(),
                    links, rechts
                );

                return erg;
            }
            else
            {
                assert(!"kein passender operand");
            }
        } break;

        case Asb_Knoten::AUSDRUCK_AUFRUF:
        {
            auto *aufruf = ausdruck->als<Ausdruck_Aufruf *>();

            auto *basis = ausdruck_analysieren(aufruf->basis());
            assert(basis != nullptr);

            assert(basis != nullptr);

            if (basis->operand()->symbol()->art() != Symbol::FUNKTION)
            {
                assert(!"meldung erstatten");
            }

            auto *funktion = basis->operand()->datentyp()->als<Datentyp_Funktion *>();
            if (funktion->parameter().size() != aufruf->argumente().size())
            {
                assert(!"meldung erstatten");
            }

            std::vector<Bestimmter_Ausdruck *> argumente;
            for (int i = 0; i < funktion->parameter().size(); ++i)
            {
                auto *parameter = funktion->parameter()[i];
                auto *argument = aufruf->argumente()[i];

                auto *bestimmtes_arg = ausdruck_analysieren(argument);

                if (Datentyp::datentypen_kompatibel(
                        parameter,
                        bestimmtes_arg->operand()->datentyp()) == Datentyp::INKOMPATIBEL)
                {
                    assert(!"meldung erstatten");
                }

                argumente.push_back(bestimmtes_arg);
            }

            auto *erg = new Bestimmter_Ausdruck_Aufruf(
                aufruf,
                new Operand(funktion->rückgabe()),
                basis,
                argumente
            );

            return erg;
        } break;

        case Asb_Knoten::AUSDRUCK_EIGENSCHAFT:
        {
            auto *eigenschaft = ausdruck->als<Ausdruck_Eigenschaft *>();
            auto *basis = ausdruck_analysieren(eigenschaft->basis());

            if (basis == nullptr)
            {
                assert(!"meldung erstatten");
            }

            if (basis->operand()->symbol() != nullptr &&
                basis->operand()->symbol()->art() == Symbol::MODUL)
            {
                assert(eigenschaft->eigenschaft()->art() == Asb_Knoten::AUSDRUCK_BEZEICHNER);
                auto *symbol =
                    basis->operand()->symbol()->zone()->suchen(
                            eigenschaft->eigenschaft()->als<Ausdruck_Bezeichner *>()->wert());

                auto *erg = new Bestimmter_Ausdruck_Eigenschaft(
                    eigenschaft,
                    new Operand(symbol),
                    basis, symbol
                );

                return erg;
            }
            else
            {
                if (basis->operand()->datentyp() == nullptr)
                {
                    assert(!"meldung erstatten");
                }

                datentyp_abschließen(basis->operand()->datentyp());

                assert(eigenschaft->eigenschaft()->art() == Asb_Knoten::AUSDRUCK_BEZEICHNER);
                auto name = eigenschaft->eigenschaft()->als<Ausdruck_Bezeichner *>()->wert();
                auto *symbol = basis->operand()->datentyp()->symbol()->zone()->suchen(name);

                if (symbol == nullptr)
                {
                    panik(eigenschaft->eigenschaft(),
                          std::format("eigenschaft {} ist nicht teil der datenstruktur {}",
                                      name, basis->operand()->datentyp()->symbol()->name()));
                }

                auto *erg = new Bestimmter_Ausdruck_Eigenschaft(
                    eigenschaft,
                    new Operand(symbol),
                    basis, symbol
                );

                return erg;
            }
        } break;

        case Asb_Knoten::AUSDRUCK_KOMPOSITUM:
        {
            auto *kompositum = ausdruck->als<Ausdruck_Kompositum *>();
            Datentyp *datentyp = nullptr;

            if (kompositum->spezifizierung() == nullptr && erwarteter_datentyp == nullptr)
            {
                datentyp = new Datentyp_Schablone(nullptr);
                datentyp->symbol_setzen(new Symbol(kompositum->spanne(), Symbol::DATENTYP, Symbol::BEHANDELT, "<anonymer datentyp>"));
                datentyp->symbol()->datentyp_setzen(datentyp);

                auto *z = datentyp->symbol()->zone();
                z->über_setzen(aktive_zone());

                size_t versatz = 0;
                size_t größe = 0;

                zone_betreten(z);
                    for (auto *eigenschaft : kompositum->eigenschaften())
                    {
                        auto *op = ausdruck_analysieren(eigenschaft->ausdruck());
                        datentyp->als<Datentyp_Schablone *>()->eigenschaft_hinzufügen(
                            op->operand()->datentyp(), versatz, eigenschaft->name());

                        if (!registrieren(new Symbol(eigenschaft->spanne(), Symbol::VARIABLE, Symbol::BEHANDELT, eigenschaft->name())))
                        {
                            panik(eigenschaft->spanne(), std::format("{} kann hier nicht verwendet werden", eigenschaft->name()));
                        }

                        versatz += op->operand()->datentyp()->größe();
                        größe += op->operand()->datentyp()->größe();
                    }
                zone_verlassen();

                datentyp->größe_setzen(größe);
                datentyp->abschließen();
            }

            if (erwarteter_datentyp)
            {
                if (kompositum->spezifizierung())
                {
                    auto *deklarierter_datentyp = spezifizierung_analysieren(kompositum->spezifizierung());

                    if (deklarierter_datentyp == nullptr)
                    {
                        assert(!"meldung erstatten");
                    }

                    datentyp_abschließen(deklarierter_datentyp);

                    if (Datentyp::datentypen_kompatibel(deklarierter_datentyp, erwarteter_datentyp) == Datentyp::INKOMPATIBEL)
                    {
                        panik(kompositum->spezifizierung(), "Datentypen inkompatibel");
                    }
                }

                datentyp = erwarteter_datentyp;
            }
            else
            {
                Datentyp *deklarierter_datentyp = nullptr;

                if (kompositum->spezifizierung())
                {
                    deklarierter_datentyp = spezifizierung_analysieren(kompositum->spezifizierung());
                    datentyp_abschließen(deklarierter_datentyp);
                }

                if (deklarierter_datentyp == nullptr)
                {
#if 0
                    // INFO: anonymen datentyp erstellen
                    std::vector<Datentyp_Anon::Eigenschaft *> eigenschaften;
                    size_t versatz = 0;

                    for (auto eigenschaft : kompositum->eigenschaften())
                    {
                        if (!eigenschaft->ist_benamt())
                        {
                            assert(!"anonymer datentyp darf nur benamte eigenschaften enthalten.");
                        }

                        auto *eigenschaft_op = ausdruck_analysieren(eigenschaft->ausdruck());

                        if (eigenschaft_op->datentyp() == nullptr)
                        {
                            assert(!"datentyp konnte nicht hergeleitet werden.");
                        }

                        eigenschaften.push_back(new Datentyp_Anon::Eigenschaft(eigenschaft_op->datentyp(), versatz, eigenschaft->name()));
                        versatz += eigenschaft_op->datentyp()->größe();
                    }

                    datentyp = new Datentyp_Anon(eigenschaften);
#endif
                }
                else
                {
                    datentyp = deklarierter_datentyp;
                }
            }

            if (kompositum->ist_benamt() && datentyp->art() != Datentyp::SCHABLONE)
            {
                melden(kompositum, "benamte eigenschaften können nur mit schablonen verwendet werden");
                assert(!"meldung erstatten");
            }

            std::vector<Bestimmter_Ausdruck_Kompositum::Bestimmte_Eigenschaft *> eigenschaften;
            for (auto *eigenschaft : kompositum->eigenschaften())
            {
                if (kompositum->ist_benamt() && !eigenschaft->ist_benamt())
                {
                    melden(eigenschaft->ausdruck(),
                           "wenn eine eigenschaft benamt wird, müssen alle eigenschaften benamt werden.");
                }

                auto *schablone = datentyp->als<Datentyp_Schablone *>();
                datentyp_abschließen(schablone);

                if (eigenschaft->ist_benamt())
                {
                    bool gefunden = false;

                    for (auto *schablone_eigenschaft : schablone->eigenschaften())
                    {
                        if (strcmp(schablone_eigenschaft->name().c_str(), eigenschaft->name().c_str()) == 0)
                        {
                            gefunden = true;
                            break;
                        }
                    }

                    // AUFGABE: mehrfache verwendung der selben eigenschaft prüfen

                    if (!gefunden)
                    {
                        panik(eigenschaft->ausdruck(),
                               std::format("eigenschaft {} konnte in der schablone nicht gefunden werden.",
                                           eigenschaft->name()));
                    }

                    auto *symbol = schablone->symbol()->zone()->suchen(eigenschaft->name());
                    if (symbol == nullptr)
                    {
                        panik(eigenschaft->ausdruck(),
                               std::format("eigenschaft {} konnte in der schablone nicht gefunden werden.",
                                           eigenschaft->name()));
                    }

                    eigenschaften.push_back(new Bestimmter_Ausdruck_Kompositum::Bestimmte_Eigenschaft(
                        new Operand(symbol)
                    ));
                }
            }

            auto *erg = new Bestimmter_Ausdruck_Kompositum(
                kompositum,
                new Operand(datentyp),
                eigenschaften
            );

            return erg;
        } break;

        default:
        {
            assert(!"unbekannter ausdruck");
        } break;
    }

    return nullptr;
}

bool
Semantik::anweisung_analysieren(Anweisung *anweisung, Datentyp *ziel_typ, Bestimmte_Anweisung **erg_param)
{
    assert(erg_param);
    bool erg = false;

    switch (anweisung->art())
    {
        case Asb_Knoten::ANWEISUNG_AUSDRUCK:
        {
            auto *ausdruck = ausdruck_analysieren(anweisung->als<Anweisung_Ausdruck *>()->ausdruck());

            if (ausdruck != nullptr)
            {
                *erg_param = new Bestimmte_Anweisung_Ausdruck(anweisung, ausdruck);
            }
        } break;

        case Asb_Knoten::ANWEISUNG_BLOCK:
        {
            std::vector<Bestimmte_Anweisung *> block_anweisungen;
            for (auto *a : anweisung->als<Anweisung_Block *>()->anweisungen())
            {
                Bestimmte_Anweisung *block_anweisung = nullptr;
                erg = anweisung_analysieren(a, ziel_typ, &block_anweisung) || erg;
                assert(block_anweisung);
                block_anweisungen.push_back(block_anweisung);
            }

            *erg_param = new Bestimmte_Anweisung_Block(anweisung, block_anweisungen);
        } break;

        case Asb_Knoten::ANWEISUNG_DEKLARATION:
        {
            auto *bestimmte_deklaration =
                    deklaration_analysieren(anweisung->als<Anweisung_Deklaration *>()->deklaration());

            *erg_param = new Bestimmte_Anweisung_Deklaration(anweisung, bestimmte_deklaration);
        } break;

        case Asb_Knoten::ANWEISUNG_SOLANGE:
        {
            auto *solange = anweisung->als<Anweisung_Solange *>();

            std::string weiter_markierung = markierung_erstellen("weiter");
            std::string raus_markierung = markierung_erstellen("raus");

            auto *bedingung = ausdruck_analysieren(solange->bedingung());

            zone_betreten(new Zone("solange", aktive_zone()));
                global_markierungen.push(std::make_pair(weiter_markierung, raus_markierung));
                Bestimmte_Anweisung *rumpf = nullptr;
                erg = anweisung_analysieren(solange->rumpf(), ziel_typ, &rumpf) && erg;
                global_markierungen.pop();
            zone_verlassen();

            *erg_param = new Bestimmte_Anweisung_Solange(
                    anweisung, bedingung, rumpf, weiter_markierung, raus_markierung);
        } break;

        case Asb_Knoten::ANWEISUNG_FÜR:
        {
            auto *für = anweisung->als<Anweisung_Für *>();
            Symbol *für_index_symbol = nullptr;

            std::string weiter_markierung = markierung_erstellen("weiter");
            std::string raus_markierung = markierung_erstellen("raus");

            zone_betreten(new Zone("für", aktive_zone()));
                if (für->index() != nullptr)
                {
                    assert(für->index()->art() == Asb_Knoten::AUSDRUCK_BEZEICHNER);
                    auto für_index_symbol_name = für->index()->als<Ausdruck_Bezeichner *>()->wert();
                    für_index_symbol = new Symbol(
                        für->index()->spanne(),
                        Symbol::VARIABLE, Symbol::BEHANDELT, für_index_symbol_name
                    );

                    if (!registrieren(für_index_symbol))
                    {
                        melden(für->index(), std::format("variablenname {} bereits vergeben", für_index_symbol_name));
                    }
                }
                else
                {
                    für_index_symbol = new Symbol(
                        Spanne(),
                        Symbol::VARIABLE, Symbol::BEHANDELT, "it"
                    );

                    if (!registrieren(für_index_symbol))
                    {
                        melden(für->index(), std::format("variablenname it bereits vergeben"));
                    }
                }

                auto *bedingung = ausdruck_analysieren(für->bedingung());
                Bestimmte_Anweisung *für_rumpf = nullptr;

                global_markierungen.push(std::make_pair(weiter_markierung, raus_markierung));
                erg = anweisung_analysieren(für->rumpf(), ziel_typ, &für_rumpf) && erg;
                global_markierungen.pop();
            zone_verlassen();

            *erg_param = new Bestimmte_Anweisung_Für(
                anweisung, für_index_symbol, bedingung, für_rumpf, weiter_markierung, raus_markierung);
        } break;

        case Asb_Knoten::ANWEISUNG_WEITER:
        {
            if (global_markierungen.empty())
            {
                melden(anweisung, "Kein gültiger Bereich für diese Anweisung.");
            }

            auto markierungen = global_markierungen.top();
            std::string weiter_markierung = markierungen.first;

            *erg_param = new Bestimmte_Anweisung_Sprung(anweisung, weiter_markierung);
        } break;

        case Asb_Knoten::ANWEISUNG_RAUS:
        {
            if (global_markierungen.empty())
            {
                melden(anweisung, "Kein gültiger Bereich für diese Anweisung.");
            }

            auto markierungen = global_markierungen.top();
            std::string raus_markierung = markierungen.second;

            *erg_param = new Bestimmte_Anweisung_Sprung(anweisung, raus_markierung);
        } break;

        case Asb_Knoten::ANWEISUNG_RES:
        {
            auto *res = anweisung->als<Anweisung_Res *>();

            auto *ausdruck = ausdruck_analysieren(res->ausdruck());

            if (ziel_typ == nullptr)
            {
                melden(anweisung, std::format("nur eine funktion kann ein ergebnis zurückgeben."));
            }

            if (Datentyp::datentypen_kompatibel(ziel_typ, ausdruck->operand()->datentyp()) == Datentyp::INKOMPATIBEL)
            {
                melden(anweisung,
                   std::format("der datentyp der rückgabe {}, ist nicht mit dem deklarierten rückgabedatentyp {} kompatibel.", ausdruck->operand()->datentyp()->symbol()->name(), ziel_typ->symbol()->name()));
            }

            *erg_param = new Bestimmte_Anweisung_Res(anweisung, ausdruck);
            erg = true;
        } break;

        case Asb_Knoten::ANWEISUNG_WENN:
        {
            auto *wenn = anweisung->als<Anweisung_Wenn *>();
            auto *bedingung = ausdruck_analysieren(wenn->bedingung());

            if (bedingung->operand()->datentyp()->art() != Datentyp::BOOL)
            {
                assert(!"meldung erstatten");
            }

            zone_betreten();
                Bestimmte_Anweisung *rumpf = nullptr;
                erg = anweisung_analysieren(anweisung->als<Anweisung_Wenn *>()->rumpf(), ziel_typ, &rumpf);
            zone_verlassen();

            Bestimmte_Anweisung *sonst = nullptr;
            if (anweisung->als<Anweisung_Wenn *>()->sonst())
            {
                erg = anweisung_analysieren(anweisung->als<Anweisung_Wenn *>()->sonst(), ziel_typ, &sonst) && erg;
            }

            *erg_param = new Bestimmte_Anweisung_Wenn(anweisung, bedingung, rumpf, sonst);
        } break;

        case Asb_Knoten::ANWEISUNG_WEICHE:
        {
            auto *weiche = anweisung->als<Anweisung_Weiche *>();

            auto ausdruck = ausdruck_analysieren(weiche->ausdruck());

            std::vector<Bestimmtes_Muster *> bestimmte_muster;
            for (auto *muster: weiche->muster())
            {
                zone_betreten(new Zone("muster", aktive_zone()));

                Bestimmter_Ausdruck *bestimmter_ausdruck =
                        muster_analysieren(muster->muster(), ausdruck->operand()->datentyp());

                Bestimmte_Anweisung *rumpf = nullptr;
                erg = anweisung_analysieren(muster->anweisung(), ziel_typ, &rumpf) && erg;

                zone_verlassen();

                bestimmte_muster.push_back(new Bestimmtes_Muster(
                    muster->muster(), bestimmter_ausdruck, rumpf));
            }

            *erg_param = new Bestimmte_Anweisung_Weiche(
                anweisung, ausdruck, bestimmte_muster);
        } break;

        case Asb_Knoten::ANWEISUNG_ZUWEISUNG:
        {
            auto *zuweisung = anweisung->als<Anweisung_Zuweisung *>();

            auto links  = ausdruck_analysieren(zuweisung->links());
            datentyp_abschließen(links->operand()->datentyp());

            auto rechts = ausdruck_analysieren(zuweisung->rechts());
            datentyp_abschließen(rechts->operand()->datentyp());

            if (Datentyp::datentypen_kompatibel(links->operand()->datentyp(), rechts->operand()->datentyp()) == Datentyp::INKOMPATIBEL)
            {
                panik(zuweisung, std::format("datentypen sind nicht kompatibel."));
            }

            *erg_param = new Bestimmte_Anweisung_Zuweisung(
                anweisung, links, zuweisung->op(), rechts);
        } break;

        case Asb_Knoten::ANWEISUNG_FINAL:
        {
            auto *final = anweisung->als<Anweisung_Final *>();

            Bestimmte_Anweisung *final_anweisung = nullptr;
            anweisung_analysieren(final->anweisung(), ziel_typ, &final_anweisung);

            *erg_param = new Bestimmte_Anweisung_Final(anweisung, final_anweisung);
        } break;

        default:
        {
            assert(!"unbekannte anweisung");
        } break;
    }

    return erg;
}

void
Semantik::import_verarbeiten(std::string dateiname)
{
    std::ifstream datei(dateiname);
    std::stringstream inhalt;
    inhalt << datei.rdbuf();

    auto lexer = Lexer(dateiname, inhalt.str());
    auto token = lexer.starten();

    auto syntax = Syntax(token);
    auto ast = syntax.starten();

    auto semantik = Semantik(ast, _system, new Zone("import", _system));
    auto *bestimmter_ast = semantik.starten(false);

    for (auto [name, symbol] : semantik.global()->symbole())
    {
        if (!registrieren(symbol))
        {
            panik(symbol, "Symbol konnte nicht registriert werden.");
        }
    }
}

bool
Semantik::registrieren(Symbol *symbol)
{
    if (_zone == nullptr)
    {
        return false;
    }

    auto erg = _zone->registrieren(symbol);

    return erg;
}

void
Semantik::zone_betreten(Zone *zone)
{
    // ACHTUNG: an dieser stelle geht uns die vorher aktive zone verloren,
    //          wenn die übergebene zone ihren über nicht gesetzt hat.
    if (zone == nullptr)
    {
        zone = new Zone();
    }

    _zone = zone;
}

void
Semantik::zone_verlassen()
{
    _zone = _zone->über();
}

bool
Semantik::operanden_kompatibel(Operand *ziel, Operand *quelle)
{
    return false;
}

void
Semantik::system_zone_initialisieren(Zone *zone)
{
#define I(N, D) \
    auto *sym_##N = new Symbol(Spanne(), Symbol::DATENTYP, Symbol::BEHANDELT, #N, System::D); \
    System::D->symbol_setzen(sym_##N); \
    zone->registrieren(sym_##N)

    I(n8, N8);
    I(n16, N16);
    I(n32, N32);
    I(n64, N64);
    I(n128, N128);

    I(g8, G8);
    I(g16, G16);
    I(g32, G32);
    I(g64, G64);
    I(g128, G128);

    I(d32, D32);
    I(d64, D64);

    I(text, Text);
    I(bool, Bool);
#undef I
}

}

std::string markierung_erstellen(std::string markierung)
{
    std::string erg = std::format("{}_{}", markierung, global_semantik_markierung_index++);

    return erg;
}

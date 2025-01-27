#include "semantik.hpp"

#include <iostream>
#include <cassert>
#include <fstream>
#include <sstream>
#include <format>

#include "kmp/ast.hpp"
#include "kmp/lexer.hpp"
#include "kmp/syntax.hpp"
#include "kmp/datentyp.hpp"

namespace Sss::Kmp {

Datentyp * global_datentyp_g8     = new Datentyp(Datentyp::GANZE_ZAHL, 1, true);
Datentyp * global_datentyp_g16    = new Datentyp(Datentyp::GANZE_ZAHL, 2, true);
Datentyp * global_datentyp_g32    = new Datentyp(Datentyp::GANZE_ZAHL, 4, true);
Datentyp * global_datentyp_g64    = new Datentyp(Datentyp::GANZE_ZAHL, 8, true);
Datentyp * global_datentyp_g128   = new Datentyp(Datentyp::GANZE_ZAHL, 16, true);
Datentyp * global_datentyp_d32    = new Datentyp(Datentyp::DEZIMAL_ZAHL, 4, true);
Datentyp * global_datentyp_d64    = new Datentyp(Datentyp::DEZIMAL_ZAHL, 8, true);
Datentyp * global_datentyp_text   = new Datentyp(Datentyp::TEXT, 8, true);
Datentyp * global_datentyp_bool   = new Datentyp(Datentyp::GANZE_ZAHL, 4, true);
Datentyp * global_datentyp_nihil  = new Datentyp(Datentyp::NIHIL, 0, true);

Semantik::Semantik(Ast ast, Zone *system, Zone *global)
    : _ast(ast)
    , _system(system)
    , _global(global)
{
    _zone = _global;
}

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
Semantik::panik(Anweisung *anweisung, std::string text)
{
    panik(anweisung->spanne(), text);
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
Semantik::melden(Spezifizierung *spezifizierung, std::string text)
{
    melden(spezifizierung->spanne(), text);
}

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

Ast
Semantik::starten()
{
    importe_registrieren();
    deklarationen_registrieren();
    symbole_analysieren();

    return _ast;
}

void
Semantik::importe_registrieren()
{
    // AUFGABE: zirkuläre importe verhindern

    for (auto *anweisung: _ast.anweisungen)
    {
        if (anweisung->art() == Ast_Knoten::ANWEISUNG_BRAUCHE)
        {
            auto *import = anweisung->als<Anweisung_Brauche *>();
            import_verarbeiten(import->dateiname());
        }

        else if (anweisung->art() == Ast_Knoten::ANWEISUNG_DEKLARATION)
        {
            auto *deklaration = anweisung->als<Anweisung_Deklaration *>()->deklaration();

            if (deklaration->art() != Ast_Knoten::DEKLARATION_BRAUCHE)
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
            ast = semantik.starten();

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
    for (auto *anweisung : _ast.anweisungen)
    {
        if (anweisung->art() != Ast_Knoten::ANWEISUNG_DEKLARATION)
        {
            continue;
        }

        auto *deklaration = anweisung->als<Anweisung_Deklaration *>()->deklaration();

        for (auto name : deklaration->namen())
        {
            switch (deklaration->art())
            {
                case Ast_Knoten::DEKLARATION_BRAUCHE:
                {
                    // INFO: deklaration brauche wird bereits in der methode importe_registrieren behandelt.
                } break;

                case Ast_Knoten::DEKLARATION_VARIABLE:
                {
                    auto *symbol = new Symbol(deklaration->spanne(), Symbol::VARIABLE, Symbol::UNBEHANDELT, name);
                    deklaration->symbol_setzen(symbol);

                    if (!registrieren(symbol))
                    {
                        assert(!"meldung erstatten");
                    }
                } break;

                case Ast_Knoten::DEKLARATION_FUNKTION:
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
                        assert(!"meldung erstatten");
                    }
                } break;

                case Ast_Knoten::DEKLARATION_SCHABLONE:
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
                       assert(!"meldung erstatten");
                    }
                } break;

                case Ast_Knoten::DEKLARATION_OPTION:
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
                       assert(!"meldung erstatten");
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
Semantik::symbole_analysieren()
{
    for (auto *anweisung : _ast.anweisungen)
    {
        if (anweisung->art() != Ast_Knoten::ANWEISUNG_DEKLARATION)
        {
            continue;
        }

        auto *deklaration = anweisung->als<Anweisung_Deklaration *>()->deklaration();

        symbol_analysieren(deklaration->symbol(), deklaration);
    }
}

void
Semantik::symbol_analysieren(Symbol *symbol, Deklaration *deklaration)
{
    if (symbol->status() == Symbol::BEHANDELT)
    {
        return;
    }

    if (symbol->status() == Symbol::IN_BEHANDLUNG)
    {
        panik(symbol, "Zirkuläre Abhängigkeit");
    }

    symbol->status_setzen(Symbol::IN_BEHANDLUNG);

    switch (deklaration->art())
    {
        case Ast_Knoten::DEKLARATION_VARIABLE:
        {
            variable_analysieren(symbol, deklaration->als<Deklaration_Variable *>());
        } break;

        case Ast_Knoten::DEKLARATION_FUNKTION:
        {
            funktion_analysieren(symbol, deklaration->als<Deklaration_Funktion *>());
        } break;

        case Ast_Knoten::DEKLARATION_OPTION:
        {
            opt_analysieren(symbol, deklaration->als<Deklaration_Option *>());
        } break;

        case Ast_Knoten::DEKLARATION_SCHABLONE:
        {
            schablone_analysieren(symbol, deklaration->als<Deklaration_Schablone *>());
        } break;

        default:
        {
            assert(!"unbekannte deklaration");
        } break;
    }

    symbol->status_setzen(Symbol::BEHANDELT);
}

void
Semantik::variable_analysieren(Symbol *symbol, Deklaration_Variable *deklaration)
{
    Datentyp *datentyp = nullptr;

    if (deklaration->spezifizierung() != nullptr)
    {
        datentyp = spezifizierung_analysieren(deklaration->spezifizierung());
        datentyp_abschließen(datentyp);
    }
    else if (deklaration->initialisierung() != nullptr)
    {
        auto *op = ausdruck_analysieren(deklaration->initialisierung());
        datentyp = op->datentyp();
    }

    if (datentyp == nullptr)
    {
        assert(!"datentyp konnte nicht ermittelt oder abgeleitet werden.");
    }
}

void
Semantik::funktion_analysieren(Symbol *symbol, Deklaration_Funktion *deklaration)
{
    auto *funktion = symbol->datentyp()->als<Datentyp_Funktion *>();

    for (auto *parameter : deklaration->parameter())
    {
        auto *datentyp = spezifizierung_analysieren(parameter->spezifizierung());
        datentyp_abschließen(datentyp);

        if (datentyp == nullptr)
        {
            assert(!"meldung erstatten");
        }

        if (parameter->als<Deklaration_Variable *>()->initialisierung())
        {
            auto *parameter_op =
                ausdruck_analysieren(parameter->als<Deklaration_Variable *>()->initialisierung());
        }

        if (!symbol->zone()->registrieren(
                new Symbol(
                    parameter->spanne(),
                    Symbol::VARIABLE, Symbol::BEHANDELT, parameter->namen()[0], datentyp)))
        {
            assert(!"meldung erstatten");
        }

        funktion->parameter_hinzufügen(datentyp);
    }

    Datentyp *rückgabe = global_datentyp_nihil;
    if (deklaration->rückgabe())
    {
        rückgabe = spezifizierung_analysieren(deklaration->rückgabe());
    }

    funktion->rückgabe_setzen(rückgabe);

    // INFO: eine funktionsdeklaration kann ohne rumpf auskommen.
    if (deklaration->rumpf() != nullptr)
    {
        auto *z = symbol->zone();
        z->über_setzen(aktive_zone());

        zone_betreten(symbol->zone());

        // AUFGABE: überprüfen ob alle pfade einen wert zurückgeben
        auto erg = anweisung_analysieren(deklaration->rumpf(), funktion->rückgabe());
        if (!erg && rückgabe != global_datentyp_nihil)
        {
            panik(symbol, std::format("nicht alle codepfade geben einen wert zurück."));
        }

        zone_verlassen();
    }
}

void
Semantik::opt_analysieren(Symbol *symbol, Deklaration_Option *deklaration)
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
            op = ausdruck_analysieren(eigenschaft->initialisierung(), dt);
        }

        for (auto name : eigenschaft->namen())
        {
            datentyp->eigenschaft_hinzufügen(dt, op, name);
        }
    }

    zone_verlassen();

    datentyp->abschließen();
}

void
Semantik::schablone_analysieren(Symbol *symbol, Deklaration_Schablone *schablone, bool zirkularität_ignorieren)
{
    auto *datentyp = symbol->datentyp()->als<Datentyp_Schablone *>();

    if (datentyp->status() == Datentyp::BEHANDELT ||
        datentyp->status() == Datentyp::IN_BEHANDLUNG && zirkularität_ignorieren)
    {
        return;
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

    for (auto *eigenschaft : schablone->eigenschaften())
    {
        auto *dt = deklaration_analysieren(eigenschaft);

        for (auto name : eigenschaft->namen())
        {
            größe += dt->größe();
            if (!datentyp->eigenschaft_hinzufügen(dt, versatz, name))
            {
                panik(eigenschaft, std::format("{} bereits vorhanden.", name));
            }
            versatz += dt->größe();
        }
    }

    zone_verlassen();

    datentyp->größe_setzen(größe);
    datentyp->status_setzen(Datentyp::BEHANDELT);
}

void
Semantik::muster_analysieren(Ausdruck *muster, Datentyp *datentyp)
{
    switch (muster->art())
    {
        case Ast_Knoten::AUSDRUCK_BEZEICHNER:
        {
            auto name = muster->als<Ausdruck_Bezeichner *>()->wert();
            if (!registrieren(new Symbol(muster->spanne(), Symbol::VARIABLE, Symbol::BEHANDELT, name, datentyp)))
            {
                panik(muster, std::format("bezeichner {} konnte nicht registriert werden.", name));
            }
        } break;

        case Ast_Knoten::AUSDRUCK_KOMPOSITUM:
        {
            if (datentyp->art() != Datentyp::SCHABLONE)
            {
                panik(muster, std::format("kompositum ausdruck kann nur bei einer schablone angewendet werden"));
            }

            auto *schablone = datentyp->als<Datentyp_Schablone *>();
            auto *kompositum = muster->als<Ausdruck_Kompositum *>();

            for (auto *kompositum_eigenschaft : kompositum->eigenschaften())
            {
                bool eigenschaft_gefunden = false;

                for (auto *schablone_eigenschaft : schablone->eigenschaften())
                {
                    if (kompositum_eigenschaft->name() == schablone_eigenschaft->name())
                    {
                        if (!registrieren(
                            new Symbol(
                                kompositum_eigenschaft->spanne(),
                                Symbol::VARIABLE,
                                Symbol::BEHANDELT,
                                kompositum_eigenschaft->name(),
                                schablone_eigenschaft->datentyp())))
                        {
                            panik(kompositum_eigenschaft->spanne(),
                                  std::format("bezeichner {} konnte nicht registriert werden.", kompositum_eigenschaft->name()));
                        }

                        eigenschaft_gefunden = true;
                        break;
                    }
                }

                if (!eigenschaft_gefunden)
                {
                    panik(kompositum_eigenschaft->spanne(),
                          std::format("eigenschaft {} in der schablone {} nicht vorhanden",
                              kompositum_eigenschaft->name(),
                              schablone->symbol()->name()));
                }
            }
        } break;

        default: assert(!"unbekannter ausdruck");
    }
}

Datentyp *
Semantik::deklaration_analysieren(Deklaration *deklaration)
{
    switch (deklaration->art())
    {
        case Ast_Knoten::DEKLARATION_VARIABLE:
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
                auto *op = ausdruck_analysieren(initialisierung, dt_spezifizierung);
                dt_initialisierung = op->datentyp();
                datentyp_abschließen(dt_initialisierung);
            }

            if (dt_spezifizierung == nullptr && dt_initialisierung != nullptr)
            {
                dt_spezifizierung = dt_initialisierung;
            }

            if (Datentyp::datentypen_kompatibel(dt_spezifizierung, dt_initialisierung) == Datentyp::INKOMPATIBEL)
            {
                panik(deklaration, "datentyp der spezifizierung und der initialisierung sind nicht kompatibel.");
            }

            auto *erg = dt_spezifizierung ? dt_spezifizierung : dt_initialisierung;

            for (auto name : deklaration->namen())
            {
                auto *sym = new Symbol(deklaration->spanne(), Symbol::VARIABLE, Symbol::BEHANDELT, name, erg);
                if (!registrieren(sym))
                {
                    assert(!"meldung erstatten");
                }
            }

            return erg;
        } break;

        case Ast_Knoten::DEKLARATION_SCHABLONE:
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
        case Ast_Knoten::SPEZIFIZIERUNG_BEZEICHNER:
        {
            auto name = spezifizierung->als<Spezifizierung_Bezeichner *>()->name();
            auto *symbol = bezeichner_analysieren(name);

            if (symbol == nullptr)
            {
                panik(spezifizierung, std::format("{} ist kein bekannter Datentyp.", name));
            }

            return symbol->datentyp();
        } break;

        case Ast_Knoten::SPEZIFIZIERUNG_FELD:
        {
            auto *basis_datentyp =
                    spezifizierung_analysieren(spezifizierung->als<Spezifizierung_Feld *>()->basis());

            auto *index =
                    ausdruck_analysieren(spezifizierung->als<Spezifizierung_Feld *>()->index());

            return new Datentyp_Feld(basis_datentyp, index->datentyp());
        } break;

        case Ast_Knoten::SPEZIFIZIERUNG_ZEIGER:
        {
            auto *basis_datentyp =
                spezifizierung_analysieren(spezifizierung->als<Spezifizierung_Zeiger *>()->basis());

            return new Datentyp_Zeiger(basis_datentyp);
        } break;

        case Ast_Knoten::SPEZIFIZIERUNG_FUNKTION:
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

Operand *
Semantik::ausdruck_analysieren(Ausdruck *ausdruck, Datentyp *erwarteter_datentyp)
{
    switch (ausdruck->art())
    {
        case Ast_Knoten::AUSDRUCK_DEZIMALZAHL:
        {
            return new Operand(global_datentyp_d32, Operand::LITERAL | Operand::ARITHMETISCH);
        } break;

        case Ast_Knoten::AUSDRUCK_GANZZAHL:
        {
            return new Operand(global_datentyp_g32, Operand::LITERAL | Operand::ARITHMETISCH);
        } break;

        case Ast_Knoten::AUSDRUCK_TEXT:
        {
            auto *erg = new Operand(global_datentyp_text, Operand::LITERAL);

            return erg;
        } break;

        case Ast_Knoten::AUSDRUCK_BEZEICHNER:
        {
            auto *symbol = bezeichner_analysieren(ausdruck->als<Ausdruck_Bezeichner *>()->wert());

            if (symbol == nullptr)
            {
                assert(!"meldung erstatten");
            }

            auto *erg = new Operand(symbol);

            return erg;
        } break;

        case Ast_Knoten::AUSDRUCK_UNÄR:
        {
            auto *erg = ausdruck_analysieren(ausdruck->als<Ausdruck_Unär *>()->ausdruck());

            return erg;
        } break;

        case Ast_Knoten::AUSDRUCK_BINÄR:
        {
            auto *bin = ausdruck->als<Ausdruck_Binär *>();

            auto *links  = ausdruck_analysieren(bin->links());
            auto *rechts = ausdruck_analysieren(bin->rechts());

            if (bin->op() >= Ausdruck_Binär::ADDITION && bin->op() <= Ausdruck_Binär::MODULO ||
                bin->op() >= Ausdruck_Binär::BIT_SCHIEB_L && bin->op() <= Ausdruck_Binär::BIT_SCHIEB_R)
            {
                if (!links->ist_arithmetisch())
                {
                    panik(bin->links(), "arithmetischen operanden erwartet");
                }

                if (!rechts->ist_arithmetisch())
                {
                    panik(bin->links(), "arithmetischen operanden erwartet");
                }

                if (Datentyp::datentypen_kompatibel(links->datentyp(), rechts->datentyp()) == Datentyp::INKOMPATIBEL)
                {
                    panik(ausdruck, "datentypen inkompatibel");
                }

                auto *erg = new Operand(links->datentyp());

                return erg;
            }
            else if (bin->op() >= Ausdruck_Binär::GLEICH && bin->op() <= Ausdruck_Binär::GRÖẞER_GLEICH)
            {
                if (Datentyp::datentypen_kompatibel(links->datentyp(), rechts->datentyp()) == Datentyp::INKOMPATIBEL)
                {
                    panik(ausdruck, "datentypen inkompatibel");
                }

                auto *erg = new Operand(global_datentyp_bool);

                return erg;
            }
            else
            {
                assert(!"kein passender operand");
            }
        } break;

        case Ast_Knoten::AUSDRUCK_AUFRUF:
        {
            auto *aufruf = ausdruck->als<Ausdruck_Aufruf *>();

            auto *basis = ausdruck_analysieren(aufruf->basis());
            assert(basis != nullptr);

            assert(basis != nullptr);

            if (basis->symbol()->art() != Symbol::FUNKTION)
            {
                assert(!"meldung erstatten");
            }

            auto *funktion = basis->datentyp()->als<Datentyp_Funktion *>();
            if (funktion->parameter().size() != aufruf->argumente().size())
            {
                assert(!"meldung erstatten");
            }

            for (int i = 0; i < funktion->parameter().size(); ++i)
            {
                auto *parameter = funktion->parameter()[i];
                auto *argument = aufruf->argumente()[i];

                auto *arg_op = ausdruck_analysieren(argument);

                if (Datentyp::datentypen_kompatibel(parameter, arg_op->datentyp()) == Datentyp::INKOMPATIBEL)
                {
                    assert(!"meldung erstatten");
                }
            }

            return new Operand(funktion->rückgabe());
        } break;

        case Ast_Knoten::AUSDRUCK_EIGENSCHAFT:
        {
            auto *eigenschaft = ausdruck->als<Ausdruck_Eigenschaft *>();
            auto *basis = ausdruck_analysieren(eigenschaft->basis());

            if (basis == nullptr)
            {
                assert(!"meldung erstatten");
            }

            if (basis->symbol() != nullptr && basis->symbol()->art() == Symbol::MODUL)
            {
                assert(eigenschaft->eigenschaft()->art() == Ast_Knoten::AUSDRUCK_BEZEICHNER);
                auto *symbol =
                    basis->symbol()->zone()->suchen(
                            eigenschaft->eigenschaft()->als<Ausdruck_Bezeichner *>()->wert());

                return new Operand(symbol);
            }
            else
            {
                if (basis->datentyp() == nullptr)
                {
                    assert(!"meldung erstatten");
                }

                datentyp_abschließen(basis->datentyp());

                assert(eigenschaft->eigenschaft()->art() == Ast_Knoten::AUSDRUCK_BEZEICHNER);
                auto name = eigenschaft->eigenschaft()->als<Ausdruck_Bezeichner *>()->wert();
                auto *symbol = basis->datentyp()->symbol()->zone()->suchen(name);

                if (symbol == nullptr)
                {
                    panik(eigenschaft->eigenschaft(),
                          std::format("eigenschaft {} ist nicht teil der datenstruktur {}",
                                      name, basis->datentyp()->symbol()->name()));
                }

                return new Operand(symbol);
            }
        } break;

        case Ast_Knoten::AUSDRUCK_KOMPOSITUM:
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
                            op->datentyp(), versatz, eigenschaft->name());

                        if (!registrieren(new Symbol(eigenschaft->spanne(), Symbol::VARIABLE, Symbol::BEHANDELT, eigenschaft->name())))
                        {
                            panik(eigenschaft->spanne(), std::format("{} kann hier nicht verwendet werden", eigenschaft->name()));
                        }

                        versatz += op->datentyp()->größe();
                        größe += op->datentyp()->größe();
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

                    if (!gefunden)
                    {
                        melden(eigenschaft->ausdruck(),
                               std::format("eigenschaft {} konnte in der schablone nicht gefunden werden.",
                                           eigenschaft->name()));
                    }
                }
            }

            return new Operand(datentyp);
        } break;

        default:
        {
            assert(!"unbekannter ausdruck");
            return nullptr;
        } break;
    }

    return nullptr;
}

bool
Semantik::anweisung_analysieren(Anweisung *anweisung, Datentyp *über)
{
    bool erg = false;

    switch (anweisung->art())
    {
        case Ast_Knoten::ANWEISUNG_AUSDRUCK:
        {
            ausdruck_analysieren(anweisung->als<Anweisung_Ausdruck *>()->ausdruck());
        } break;

        case Ast_Knoten::ANWEISUNG_BLOCK:
        {
            for (auto *a : anweisung->als<Anweisung_Block *>()->anweisungen())
            {
                erg = anweisung_analysieren(a, über) || erg;
            }
        } break;

        case Ast_Knoten::ANWEISUNG_DEKLARATION:
        {
            deklaration_analysieren(anweisung->als<Anweisung_Deklaration *>()->deklaration());
        } break;

        case Ast_Knoten::ANWEISUNG_FÜR:
        {
            auto *für = anweisung->als<Anweisung_Für *>();

            zone_betreten(new Zone("für", aktive_zone()));
                if (für->index() != nullptr)
                {
                    assert(für->index()->art() == Ast_Knoten::AUSDRUCK_BEZEICHNER);
                    registrieren(new Symbol(
                        für->index()->spanne(),
                        Symbol::VARIABLE, Symbol::BEHANDELT, für->index()->als<Ausdruck_Bezeichner *>()->wert()
                    ));
                }
                else
                {
                    registrieren(new Symbol(
                        Spanne(),
                        Symbol::VARIABLE, Symbol::BEHANDELT, "it"
                    ));
                }

                ausdruck_analysieren(für->bedingung());
                erg = anweisung_analysieren(für->rumpf());
            zone_verlassen();
        } break;

        case Ast_Knoten::ANWEISUNG_RES:
        {
            auto *res = anweisung->als<Anweisung_Res *>();

            auto *op = ausdruck_analysieren(res->ausdruck());

            if (über == nullptr)
            {
                panik(anweisung, std::format("nur eine funktion kann ein ergebnis zurückgeben."));
            }

            if (Datentyp::datentypen_kompatibel(über, op->datentyp()) == Datentyp::INKOMPATIBEL)
            {
                panik(anweisung, std::format("der datentyp der rückgabe {}, ist nicht mit dem deklarierten rückgabedatentyp {} kompatibel.", op->datentyp()->symbol()->name(), über->symbol()->name()));
            }

            erg = true;
        } break;

        case Ast_Knoten::ANWEISUNG_WENN:
        {
            auto *wenn = anweisung->als<Anweisung_Wenn *>();
            auto *bedingung = wenn->bedingung();

            auto *op = ausdruck_analysieren(bedingung);

            if (op->datentyp()->art() != Datentyp::BOOL)
            {
                assert(!"meldung erstatten");
            }

            zone_betreten();
                erg = anweisung_analysieren(anweisung->als<Anweisung_Wenn *>()->rumpf());
            zone_verlassen();

            if (anweisung->als<Anweisung_Wenn *>()->sonst())
            {
                erg = anweisung_analysieren(anweisung->als<Anweisung_Wenn *>()->sonst()) && erg;
            }
        } break;

        case Ast_Knoten::ANWEISUNG_WEICHE:
        {
            auto *weiche = anweisung->als<Anweisung_Weiche *>();

            auto ausdruck_op = ausdruck_analysieren(weiche->ausdruck());

            for (auto *muster: weiche->muster())
            {
                zone_betreten(new Zone("muster", aktive_zone()));

                muster_analysieren(muster->muster(), ausdruck_op->datentyp());
                erg = anweisung_analysieren(muster->anweisung()) && erg;

                zone_verlassen();
            }
        } break;

        case Ast_Knoten::ANWEISUNG_ZUWEISUNG:
        {
            auto *zuweisung = anweisung->als<Anweisung_Zuweisung *>();

            auto links  = ausdruck_analysieren(zuweisung->links());
            datentyp_abschließen(links->datentyp());

            auto rechts = ausdruck_analysieren(zuweisung->rechts());
            datentyp_abschließen(rechts->datentyp());

            if (Datentyp::datentypen_kompatibel(links->datentyp(), rechts->datentyp()) == Datentyp::INKOMPATIBEL)
            {
                panik(zuweisung, std::format("datentypen sind nicht kompatibel."));
            }
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
    ast = semantik.starten();

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
    zone->registrieren(new Symbol(Spanne(), Symbol::DATENTYP, Symbol::BEHANDELT, "n8",   global_datentyp_g8));
    zone->registrieren(new Symbol(Spanne(), Symbol::DATENTYP, Symbol::BEHANDELT, "n16",  global_datentyp_g16));
    zone->registrieren(new Symbol(Spanne(), Symbol::DATENTYP, Symbol::BEHANDELT, "n32",  global_datentyp_g32));
    zone->registrieren(new Symbol(Spanne(), Symbol::DATENTYP, Symbol::BEHANDELT, "n64",  global_datentyp_g64));
    zone->registrieren(new Symbol(Spanne(), Symbol::DATENTYP, Symbol::BEHANDELT, "n128", global_datentyp_g128));

    zone->registrieren(new Symbol(Spanne(), Symbol::DATENTYP, Symbol::BEHANDELT, "g8",   global_datentyp_g8));
    zone->registrieren(new Symbol(Spanne(), Symbol::DATENTYP, Symbol::BEHANDELT, "g16",  global_datentyp_g16));
    zone->registrieren(new Symbol(Spanne(), Symbol::DATENTYP, Symbol::BEHANDELT, "g32",  global_datentyp_g32));
    zone->registrieren(new Symbol(Spanne(), Symbol::DATENTYP, Symbol::BEHANDELT, "g64",  global_datentyp_g64));
    zone->registrieren(new Symbol(Spanne(), Symbol::DATENTYP, Symbol::BEHANDELT, "g128", global_datentyp_g128));

    zone->registrieren(new Symbol(Spanne(), Symbol::DATENTYP, Symbol::BEHANDELT, "d32", global_datentyp_d32));
    zone->registrieren(new Symbol(Spanne(), Symbol::DATENTYP, Symbol::BEHANDELT, "d64", global_datentyp_d64));

    zone->registrieren(new Symbol(Spanne(), Symbol::DATENTYP, Symbol::BEHANDELT, "text", global_datentyp_text));
    zone->registrieren(new Symbol(Spanne(), Symbol::DATENTYP, Symbol::BEHANDELT, "bool", global_datentyp_bool));
}

}

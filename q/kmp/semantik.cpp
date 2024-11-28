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

Datentyp * global_datentyp_g8     = new Datentyp(Datentyp::GANZE_ZAHL, 1);
Datentyp * global_datentyp_g16    = new Datentyp(Datentyp::GANZE_ZAHL, 2);
Datentyp * global_datentyp_g32    = new Datentyp(Datentyp::GANZE_ZAHL, 4);
Datentyp * global_datentyp_g64    = new Datentyp(Datentyp::GANZE_ZAHL, 8);
Datentyp * global_datentyp_g128   = new Datentyp(Datentyp::GANZE_ZAHL, 16);
Datentyp * global_datentyp_d32    = new Datentyp(Datentyp::DEZIMAL_ZAHL, 4);
Datentyp * global_datentyp_d64    = new Datentyp(Datentyp::DEZIMAL_ZAHL, 8);
Datentyp * global_datentyp_text   = new Datentyp(Datentyp::TEXT, 8);
Datentyp * global_datentyp_nichts = new Datentyp(Datentyp::NICHTS, 0);

Semantik::Semantik(Ast ast, Zone *system, Zone *global)
    : _ast(ast)
    , _system(system)
    , _global(global)
{
    _zone = _global;
}

void
Semantik::melden(Spanne spanne, std::string text)
{
    _diagnostik.melden(spanne, new Fehler(text));

    for (auto meldung : _diagnostik.meldungen())
    {
         std::cout << meldung << std::endl;
    }

    __debugbreak();
}

void
Semantik::melden(Ausdruck *ausdruck, std::string text)
{
    melden(ausdruck->spanne(), text);
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
    analyse_abschließen();

    return _ast;
}

void
Semantik::importe_registrieren()
{
    for (auto *anweisung: _ast.anweisungen)
    {
        if (anweisung->art() == Ast_Knoten::ANWEISUNG_BRAUCHE)
        {
            auto *import = anweisung->als<Anweisung_Brauche *>();
            auto dateiname = import->dateiname();

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
                    assert(!"meldung erstatten");
                }
            }
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
                new Symbol(Symbol::MODUL, Symbol::BEHANDELT, import->name(), semantik.global()));

            if (!registrieren(deklaration->symbol()))
            {
                assert(!"meldung erstatten");
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
                case Ast_Knoten::DEKLARATION_VARIABLE:
                {
                    auto *symbol = new Symbol(Symbol::VARIABLE, Symbol::UNBEHANDELT, name);
                    deklaration->symbol_setzen(symbol);

                    if (!registrieren(symbol))
                    {
                        assert(!"meldung erstatten");
                    }
                } break;

                case Ast_Knoten::DEKLARATION_FUNKTION:
                {
                    auto *funktion = deklaration->als<Deklaration_Funktion *>();
                    auto *symbol = new Symbol(Symbol::FUNKTION, Symbol::UNBEHANDELT,
                                              name, new Datentyp_Funktion());

                    deklaration->symbol_setzen(symbol);

                    if (!registrieren(symbol))
                    {
                        assert(!"meldung erstatten");
                    }
                } break;

                case Ast_Knoten::DEKLARATION_SCHABLONE:
                {
                    auto *symbol = new Symbol(
                        Symbol::DATENTYP, Symbol::BEHANDELT,
                        name, new Datentyp_Schablone(deklaration->als<Deklaration_Schablone *>()));

                    deklaration->symbol_setzen(symbol);

                    _schablonen.push_back(deklaration->als<Deklaration_Schablone *>());

                    if (!registrieren(symbol))
                    {
                       assert(!"meldung erstatten");
                    }
                } break;

                case Ast_Knoten::DEKLARATION_OPT:
                {
                    auto *symbol = new Symbol(
                        Symbol::DATENTYP, Symbol::UNBEHANDELT,
                        name, new Datentyp_Opt());

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
Semantik::analyse_abschließen()
{
    for (auto *schablone : _schablonen)
    {
        schablone_abschließen(schablone);
    }
}

void
Semantik::schablone_abschließen(Deklaration_Schablone *schablone)
{
    Symbol *symbol = schablone->symbol();
    auto *datentyp = symbol->datentyp()->als<Datentyp_Schablone *>();

    if (datentyp->abgeschlossen())
    {
        return;
    }

    assert(symbol != nullptr);
    assert(datentyp != nullptr);

    size_t größe = 0;
    size_t versatz = 0;

    auto *z = symbol->zone();
    z->über_setzen(aktive_zone());

    zone_betreten(symbol->zone());

    for (auto *eigenschaft : schablone->eigenschaften())
    {
        auto *dt = deklaration_analysieren(eigenschaft);

        for (auto name : eigenschaft->namen())
        {
            größe += dt->größe();
            datentyp->eigenschaft_hinzufügen(dt, versatz, name);
            versatz += dt->größe();
        }
    }

    zone_verlassen();

    datentyp->größe_setzen(größe);
    datentyp->abschließen();
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
        assert(!"zirkularität");
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

        case Ast_Knoten::DEKLARATION_OPT:
        {
            opt_analysieren(symbol, deklaration->als<Deklaration_Opt *>());
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
                new Symbol(Symbol::VARIABLE, Symbol::BEHANDELT, parameter->namen()[0], datentyp)))
        {
            assert(!"meldung erstatten");
        }

        funktion->parameter_hinzufügen(datentyp);
    }

    Datentyp *rückgabe = global_datentyp_nichts;
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
        anweisung_analysieren(deklaration->rumpf());
        zone_verlassen();
    }
}

void
Semantik::opt_analysieren(Symbol *symbol, Deklaration_Opt *deklaration)
{
    // AUFGABE: eigenschaften durchgehen
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
            }

            Datentyp *dt_initialisierung = dt_spezifizierung;
            if (initialisierung)
            {
                auto *op = ausdruck_analysieren(initialisierung, dt_spezifizierung);
                dt_initialisierung = op->datentyp();
            }

            if (dt_spezifizierung == nullptr && dt_initialisierung != nullptr)
            {
                dt_spezifizierung = dt_initialisierung;
            }

            if (!datentypen_kompatibel(dt_spezifizierung, dt_initialisierung))
            {
                assert(!"meldung erstatten");
            }

            auto *erg = dt_spezifizierung ? dt_spezifizierung : dt_initialisierung;

            for (auto name : deklaration->namen())
            {
                auto *sym = new Symbol(Symbol::VARIABLE, Symbol::BEHANDELT, name, erg);
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
            auto *symbol = bezeichner_analysieren(spezifizierung->als<Spezifizierung_Bezeichner *>()->name());

            if (symbol == nullptr)
            {
                assert(!"meldung erstatten");
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
Semantik::datentyp_abschließen(Datentyp *datentyp)
{
    switch (datentyp->art())
    {
        case Datentyp::SCHABLONE:
        {
            schablone_abschließen(datentyp->als<Datentyp_Schablone *>()->deklaration());
        } break;

        case Datentyp::FUNKTION:
        {
            // AUFGABE: abschließen
            assert(!"abschließen");
        } break;

        default: break;
    }
}

Operand *
Semantik::ausdruck_analysieren(Ausdruck *ausdruck, Datentyp *erwarteter_datentyp)
{
    switch (ausdruck->art())
    {
        case Ast_Knoten::AUSDRUCK_DEZIMALZAHL:
        {
            return new Operand(global_datentyp_d32, Operand::LITERAL);
        } break;

        case Ast_Knoten::AUSDRUCK_GANZZAHL:
        {
            return new Operand(global_datentyp_g32, Operand::LITERAL);
        } break;

        case Ast_Knoten::AUSDRUCK_TEXT:
        {
            return new Operand(global_datentyp_text, Operand::LITERAL);
        } break;

        case Ast_Knoten::AUSDRUCK_BEZEICHNER:
        {
            auto *symbol = bezeichner_analysieren(ausdruck->als<Ausdruck_Bezeichner *>()->wert());

            if (symbol == nullptr)
            {
                assert(!"meldung erstatten");
            }

            return new Operand(symbol);
        } break;

        case Ast_Knoten::AUSDRUCK_UNÄR:
        {
            auto *op = ausdruck_analysieren(ausdruck->als<Ausdruck_Unär *>()->ausdruck());

            return op;
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

                if (!datentypen_kompatibel(parameter, arg_op->datentyp()))
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

                assert(!"implementieren");
            }
        } break;

        case Ast_Knoten::AUSDRUCK_KOMPOSITUM:
        {
            auto *kompositum = ausdruck->als<Ausdruck_Kompositum *>();

            if (kompositum->spezifizierung() == nullptr && erwarteter_datentyp == nullptr)
            {
                assert(!"meldung erstatten");
            }

            Datentyp *datentyp = nullptr;
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

                    if (!datentypen_kompatibel(deklarierter_datentyp, erwarteter_datentyp))
                    {
                        assert(!"meldung erstatten");
                    }
                }

                datentyp = erwarteter_datentyp;
            }
            else
            {
                auto *deklarierter_datentyp = spezifizierung_analysieren(kompositum->spezifizierung());
                datentyp_abschließen(deklarierter_datentyp);

                if (deklarierter_datentyp == nullptr)
                {
                    assert(!"meldung erstatten");
                }

                datentyp = deklarierter_datentyp;
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

            assert(!"implementieren");
            return nullptr;
        } break;

        default:
        {
            assert(!"unbekannter ausdruck");
            return nullptr;
        } break;
    }

    return nullptr;
}

void
Semantik::anweisung_analysieren(Anweisung *anweisung)
{
    switch (anweisung->art())
    {
        case Ast_Knoten::ANWEISUNG_BLOCK:
        {
            for (auto *a : anweisung->als<Anweisung_Block *>()->anweisungen())
            {
                anweisung_analysieren(a);
            }
        } break;

        case Ast_Knoten::ANWEISUNG_AUSDRUCK:
        {
            ausdruck_analysieren(anweisung->als<Anweisung_Ausdruck *>()->ausdruck());
        } break;

        case Ast_Knoten::ANWEISUNG_DEKLARATION:
        {
            deklaration_analysieren(anweisung->als<Anweisung_Deklaration *>()->deklaration());
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
                anweisung_analysieren(anweisung->als<Anweisung_Wenn *>()->rumpf());
            zone_verlassen();

            if (anweisung->als<Anweisung_Wenn *>()->sonst())
            {
                anweisung_analysieren(anweisung->als<Anweisung_Wenn *>()->sonst());
            }
        } break;

        case Ast_Knoten::ANWEISUNG_FÜR:
        {
            auto *für = anweisung->als<Anweisung_Für *>();

            zone_betreten();
                if (für->index() != nullptr)
                {
                    assert(für->index()->art() == Ast_Knoten::AUSDRUCK_BEZEICHNER);
                    registrieren(new Symbol(
                        Symbol::VARIABLE, Symbol::BEHANDELT, für->index()->als<Ausdruck_Bezeichner *>()->wert()
                    ));
                }
                else
                {
                    registrieren(new Symbol(
                        Symbol::VARIABLE, Symbol::BEHANDELT, "it"
                    ));
                }

                ausdruck_analysieren(für->bedingung());
                anweisung_analysieren(für->rumpf());
            zone_verlassen();
        } break;
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
Semantik::datentypen_kompatibel(Datentyp *links, Datentyp *rechts)
{
    // INFO: wenn einer davon null ist => ungültig
    if (links == nullptr || rechts == nullptr)
    {
        return false;
    }

    // INFO: wenn beide vom gleichen typ sind => dann kompatibel
    if (links == rechts)
    {
        return true;
    }

    // INFO: wenn die art gleich ist und der rechte datentyp in den linken
    //       reinpasst => dann kompatibel
    if (links->art() == rechts->art() && links->größe() >= rechts->größe())
    {
        return true;
    }

    // INFO: bool und ganze- und natürliche zahlen sind kompatibel
    if (links->art() == Datentyp::BOOL && rechts->art() == Datentyp::GANZE_ZAHL ||
        links->art() == Datentyp::GANZE_ZAHL && rechts->art() == Datentyp::BOOL)
    {
        return true;
    }

    return false;
}

bool
Semantik::operanden_kompatibel(Operand *links, Operand *rechts)
{
    return false;
}

void
Semantik::system_zone_initialisieren(Zone *zone)
{
    zone->registrieren(new Symbol(Symbol::DATENTYP, Symbol::BEHANDELT, "n8",   global_datentyp_g8));
    zone->registrieren(new Symbol(Symbol::DATENTYP, Symbol::BEHANDELT, "n16",  global_datentyp_g16));
    zone->registrieren(new Symbol(Symbol::DATENTYP, Symbol::BEHANDELT, "n32",  global_datentyp_g32));
    zone->registrieren(new Symbol(Symbol::DATENTYP, Symbol::BEHANDELT, "n64",  global_datentyp_g64));
    zone->registrieren(new Symbol(Symbol::DATENTYP, Symbol::BEHANDELT, "n128", global_datentyp_g128));

    zone->registrieren(new Symbol(Symbol::DATENTYP, Symbol::BEHANDELT, "g8",   global_datentyp_g8));
    zone->registrieren(new Symbol(Symbol::DATENTYP, Symbol::BEHANDELT, "g16",  global_datentyp_g16));
    zone->registrieren(new Symbol(Symbol::DATENTYP, Symbol::BEHANDELT, "g32",  global_datentyp_g32));
    zone->registrieren(new Symbol(Symbol::DATENTYP, Symbol::BEHANDELT, "g64",  global_datentyp_g64));
    zone->registrieren(new Symbol(Symbol::DATENTYP, Symbol::BEHANDELT, "g128", global_datentyp_g128));

    zone->registrieren(new Symbol(Symbol::DATENTYP, Symbol::BEHANDELT, "d32", global_datentyp_d32));
    zone->registrieren(new Symbol(Symbol::DATENTYP, Symbol::BEHANDELT, "d64", global_datentyp_d64));

    zone->registrieren(new Symbol(Symbol::DATENTYP, Symbol::BEHANDELT, "text", global_datentyp_text));
    zone->registrieren(new Symbol(Symbol::DATENTYP, Symbol::BEHANDELT, "bool", new Datentyp(Datentyp::BOOL, 4)));
}

}

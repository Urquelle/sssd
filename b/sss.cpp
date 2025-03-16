#include "dienste/kmd.cpp"

#include "kmp/asb.hpp"
#include "kmp/spanne.cpp"
#include "kmp/fehler.cpp"
#include "kmp/ergebnis.cpp"
#include "kmp/diagnostik.cpp"
#include "kmp/zone.cpp"
#include "kmp/operand.cpp"
#include "kmp/symbol.cpp"
#include "kmp/datentyp.cpp"
#include "kmp/zeichen.cpp"
#include "kmp/glied.cpp"
#include "kmp/lexer.cpp"
#include "kmp/asb.cpp"
#include "kmp/syntax.cpp"
#include "kmp/bestimmter_asb.cpp"
#include "kmp/semantik.cpp"
#include "kmp/transformator.cpp"
#include "kmp/reduzierer.cpp"
#include "kmp/interpret.cpp"

#include "vm/hauptspeicher.cpp"
#include "vm/anweisung.cpp"
#include "vm/operand.cpp"
#include "vm/cpu.cpp"

#include <windows.h>

#define INHALT_GRÖSSE 1024*1024*10
char inhalt[INHALT_GRÖSSE];

int main (int argc, char *argv[])
{
    using namespace Sss::Kmp;

    SetConsoleOutputCP(CP_UTF8);

    size_t inhalt_versatz = 0;

    auto *system = new Zone("system");
    auto *global = new Zone("global", system);
    Semantik::system_zone_initialisieren(system);

    bool beenden = false;

    Kmd kmd = {};

    kmd_marke(&kmd, "r");
    kmd_marke(&kmd, "repl");
    kmd_param(&kmd, "s", "ausführen");
    kmd_param(&kmd, "start", "ausführen");

    bool repl = false;
    if (!kmd_einlesen(&kmd, argc, argv))
    {
        //
    }

    if (kmd_gesetzt(&kmd, "r") || kmd_gesetzt(&kmd, "repl"))
    {
        while (!beenden)
        {
            std::cout << "\n>> ";
            gets_s(inhalt + inhalt_versatz, INHALT_GRÖSSE);
            inhalt_versatz = strlen(inhalt) - 1;

            auto lexer = Lexer("<repl>", inhalt);
            auto token = lexer.starten();

            auto syntax = Syntax(token);
            auto asb = syntax.starten();

            for (auto anweisung : asb.anweisungen)
            {
                anweisung->ausgeben(0, std::cout);
            }

            if (syntax.diagnostik().hat_meldungen())
            {
                for (auto meldung : syntax.diagnostik().meldungen())
                {
                     std::cerr << meldung << std::endl;
                }

                syntax.diagnostik().löschen();
                continue;
            }

            auto semantik = Semantik(asb, system, global);
            auto *bestimmter_asb = semantik.starten(false);
        }
    }
    else
    {
        if (kmd_freie_werte_anzahl(&kmd) == 0)
        {
            std::cerr << "Fehler: es wurde keine Datei übergeben." << std::endl;
            exit(1);
        }

        auto dateiname = kmd_freier_wert(&kmd, 0);
        std::ifstream datei(dateiname);
        std::stringstream code;
        code << datei.rdbuf();

        auto lexer = Lexer(dateiname, code.str());
        auto token = lexer.starten();

        auto syntax = Syntax(token);
        auto ast = syntax.starten();

        if (syntax.diagnostik().hat_meldungen())
        {
            for (auto meldung : syntax.diagnostik().meldungen())
            {
                 std::cerr << meldung << std::endl;
            }

            syntax.diagnostik().löschen();
            exit(1);
        }

        auto semantik = Semantik(ast, system, global);
        semantik.system_zone_initialisieren(semantik.system());
        auto *bestimmter_asb = semantik.starten(true);

        if (semantik.diagnostik().hat_meldungen())
        {
            for (auto meldung : semantik.diagnostik().meldungen())
            {
                 std::cerr << meldung << std::endl;
            }

            semantik.diagnostik().löschen();
        }
    }

    return 0;
}

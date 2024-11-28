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
#include "kmp/ast.cpp"
#include "kmp/syntax.cpp"
#include "kmp/semantik.cpp"

#include <fstream>
#include <sstream>
#include <windows.h>

int main (int argc, char *argv[])
{
    using namespace Sss::Kmp;

    const char *dateiname = "test_direktive.sss";

    SetConsoleOutputCP(CP_UTF8);

    std::ifstream datei(dateiname);
    std::stringstream inhalt;
    inhalt << datei.rdbuf();

    auto lexer = Lexer(dateiname, inhalt.str());
    auto token = lexer.starten();

    auto syntax = Syntax(token);
    auto ast = syntax.starten();

    auto *system = new Zone("system");
    auto *global = new Zone("global", system);

    auto semantik = Semantik(ast, system, global);
    semantik.system_zone_initialisieren(semantik.system());

    ast = semantik.starten();

    return 0;
}


#include "kmp/spanne.cpp"
#include "kmp/fehler.cpp"
#include "kmp/ergebnis.cpp"
#include "kmp/diagnostik.cpp"
#include "kmp/zeichen.cpp"
#include "kmp/glied.cpp"
#include "kmp/lexer.cpp"
#include "kmp/ast.cpp"
#include "kmp/syntax.cpp"

#include <fstream>
#include <sstream>
#include <windows.h>

int main (int argc, char *argv[])
{
    const char *dateiname = "test_deklaration.sss";

    SetConsoleOutputCP(CP_UTF8);

    std::ifstream datei(dateiname);
    std::stringstream inhalt;
    inhalt << datei.rdbuf();

    auto lexer = Sss::Kmp::Lexer(dateiname, inhalt.str());
    auto token = lexer.starten();

    auto syntax = Sss::Kmp::Syntax(token);

    for (;;)
    {
        auto *deklaration = syntax.deklaration_einlesen();

        if (deklaration == nullptr)
        {
            break;
        }

        deklaration->ausgeben(0, std::cout);
        std::cout << std::endl;
    }

    return 0;
}


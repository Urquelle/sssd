#include "spanne.cpp"
#include "diagnostik.cpp"
#include "zeichen.cpp"
#include "token.cpp"
#include "lexer.cpp"

#include <fstream>
#include <sstream>
#include <windows.h>

int main (int argc, char *argv[]) {
    const char *dateiname = "test_lexer.sss";

    SetConsoleOutputCP(CP_UTF8);

    std::ifstream datei(dateiname);
    std::stringstream inhalt;
    inhalt << datei.rdbuf();

    auto lexer = Sss::Lexer(dateiname, inhalt.str());
    auto token = lexer.starten();

    for (auto *t : token)
    {
         t->ausgeben(std::cout);
         std::cout << std::endl;
    }

    return 0;
}

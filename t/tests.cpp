#include "kmp/spanne.cpp"
#include "kmp/fehler.cpp"
#include "kmp/ergebnis.cpp"
#include "kmp/diagnostik.cpp"
#include "kmp/zeichen.cpp"
#include "kmp/glied.cpp"
#include "kmp/lexer.cpp"
#include "kmp/syntax.cpp"
#include "kmp/asb.cpp"
#include "kmp/datentyp.cpp"
#include "kmp/symbol.cpp"
#include "kmp/zone.cpp"

#include "lexer_tests.cpp"
#include "syntax_tests.cpp"

#include <windows.h>

int main() {
    SetConsoleOutputCP(CP_UTF8);

    Lexer_Test lexer_prüfungen;
    lexer_prüfungen.run_all_tests();

    std::cout << "\n--- Syntax Prüfungen ---\n";
    Syntax_Test syntax_prüfungen;
    syntax_prüfungen.alle_prüfungen_ausführen();

    return 0;
}

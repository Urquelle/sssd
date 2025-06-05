#include "kmp/spanne.c"
#include "kmp/fehler.c"
#include "kmp/ergebnis.c"
#include "kmp/diagnostik.c"
#include "kmp/zeichen.c"
#include "kmp/glied.c"
#include "kmp/lexer.c"
#include "kmp/syntax.c"
#include "kmp/asb.c"
#include "kmp/datentyp.c"
#include "kmp/symbol.c"
#include "kmp/zone.c"

#include "lexer_tests.c"
#include "syntax_tests.c"

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

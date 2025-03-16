#include <iostream>

#include "kmp/glied.hpp"
#include "kmp/lexer.hpp"
#include "kmp/syntax.hpp"
#include "kmp/asb.hpp"

#include "makros.hpp"

using namespace Sss::Kmp;

class Syntax_Test
{
private:
    int prüfungen_ausgeführt = 0;
    int prüfungen_bestanden = 0;

    const std::string FEHLER_PARSEN_AUSDRUCK            = "Fehler beim Parsen des Ausdrucks";
    const std::string FEHLER_PARSEN_ANWEISUNG           = "Fehler beim Parsen der Anweisung";
    const std::string FEHLER_PARSEN_SPEZIFIZIERUNG      = "Fehler beim Parsen der Spezifizierung";
    const std::string FEHLER_FALSCHER_AUSDRUCKSTYP      = "Falscher Ausdruckstyp";
    const std::string FEHLER_FALSCHER_ANWEISUNGSTYP     = "Falscher Anweisungstyp";

    std::vector<Glied*> lex(const std::string& input)
    {
        Lexer lexer("test.txt", input);

        return lexer.starten();
    }

    Ausdruck * ausdruck_einlesen(const std::string& input)
    {
        auto glieder = lex(input);

        Syntax syntax(glieder);
        auto result = syntax.ausdruck_einlesen();
        TEST_ANNAHME(result.gut(), FEHLER_PARSEN_AUSDRUCK);

        return result.wert();
    }

    Anweisung * anweisung_einlesen(const std::string& input)
    {
        auto glieder = lex(input);

        Syntax syntax(glieder);
        auto result = syntax.anweisung_einlesen();
        TEST_ANNAHME(result.gut(), FEHLER_PARSEN_ANWEISUNG);
        TEST_ANNAHME_GL(Glied::ENDE, syntax.glied()->art(), "Falsches Glied");

        return result.wert();
    }

    Spezifizierung * spezifizierung_einlesen(const std::string& input)
    {
        auto glieder = lex(input);

        Syntax syntax(glieder);
        auto* result = syntax.spezifizierung_einlesen();
        TEST_ANNAHME(result != nullptr, FEHLER_PARSEN_SPEZIFIZIERUNG);

        return result;
    }

    void aufräumen(Ausdruck *ausdruck)
    {
        delete ausdruck;
    }

    void aufräumen(Anweisung *anweisung)
    {
        delete anweisung;
    }

    void aufräumen(Spezifizierung* spez)
    {
        delete spez;
    }

    void aufräumen(std::vector<Glied*>& glieder)
    {
        for (auto* glied : glieder)
        {
            delete glied;
        }

        glieder.clear();
    }

public:
    void prüfung_ausführen(const std::string& name, void (Syntax_Test::*test)())
    {
        prüfungen_ausgeführt++;
        try
        {
            std::cout << "Prüfung läuft: " << name << "... ";
            (this->*test)();
            prüfungen_bestanden++;
            std::cout << "ERFOLG\n";
        }
        catch (const std::exception& e)
        {
            std::cout << "FEHLSCHLAG: " << e.what() << "\n";
        }
    }

    void ganzzahl_literal_prüfen()
    {
        auto expr = ausdruck_einlesen("42");
        TEST_ANNAHME_GL(Asb_Knoten::AUSDRUCK_GANZZAHL, expr->art(), FEHLER_FALSCHER_AUSDRUCKSTYP);
        auto ganzzahl = expr->als<Ausdruck_Ganzzahl*>();
        TEST_ANNAHME_GL(42, ganzzahl->wert(), "Falscher Ganzzahlwert");

        aufräumen(expr);
    }

    void dezimal_literal_prüfen()
    {
        auto expr = ausdruck_einlesen("3,14");
        TEST_ANNAHME_GL(Asb_Knoten::AUSDRUCK_DEZIMALZAHL, expr->art(), FEHLER_FALSCHER_AUSDRUCKSTYP);
        auto dezimal = expr->als<Ausdruck_Dezimalzahl*>();
        float diff = std::abs(3.14f - dezimal->wert());
        TEST_ANNAHME(diff < 0.001, "Falscher Dezimalwert");
        aufräumen(expr);
    }

    void bezeichner_prüfen()
    {
        auto expr = ausdruck_einlesen("variable");
        TEST_ANNAHME_GL(Asb_Knoten::AUSDRUCK_BEZEICHNER, expr->art(), FEHLER_FALSCHER_AUSDRUCKSTYP);
        auto bez = expr->als<Ausdruck_Bezeichner*>();
        TEST_ANNAHME_TXTGL("variable", bez->wert().c_str(), "Falscher Bezeichnerwert");
        aufräumen(expr);
    }

    void text_literal_prüfen()
    {
        auto expr = ausdruck_einlesen("»Hallo«");
        TEST_ANNAHME_GL(Asb_Knoten::AUSDRUCK_TEXT, expr->art(), FEHLER_FALSCHER_AUSDRUCKSTYP);
        auto text = expr->als<Ausdruck_Text*>();
        TEST_ANNAHME_TXTGL("Hallo", text->wert().c_str(), "Falscher Textwert");
        aufräumen(expr);
    }

    void binär_ausdruck_prüfen()
    {
        auto expr = ausdruck_einlesen("1 + 2");
        TEST_ANNAHME_GL(Asb_Knoten::AUSDRUCK_BINÄR, expr->art(), FEHLER_FALSCHER_AUSDRUCKSTYP);
        auto bin = expr->als<Ausdruck_Binär*>();
        TEST_ANNAHME_GL(Ausdruck_Binär::ADDITION, bin->op(), "Falscher Operator");
        TEST_ANNAHME_GL(Asb_Knoten::AUSDRUCK_GANZZAHL, bin->links()->art(), "Falscher linker Operantentyp");
        TEST_ANNAHME_GL(Asb_Knoten::AUSDRUCK_GANZZAHL, bin->rechts()->art(), "Falscher rechter Operantentyp");
        TEST_ANNAHME_GL(1, bin->links()->als<Ausdruck_Ganzzahl*>()->wert(), "Falscher linker Wert");
        TEST_ANNAHME_GL(2, bin->rechts()->als<Ausdruck_Ganzzahl*>()->wert(), "Falscher rechter Wert");
        aufräumen(expr);
    }

    void unär_ausdruck_prüfen()
    {
        auto expr = ausdruck_einlesen("-5");
        TEST_ANNAHME_GL(Asb_Knoten::AUSDRUCK_UNÄR, expr->art(), FEHLER_FALSCHER_AUSDRUCKSTYP);
        auto unär = expr->als<Ausdruck_Unär*>();
        TEST_ANNAHME_GL(Ausdruck_Unär::MINUS, unär->op(), "Falscher Operator");
        TEST_ANNAHME_GL(Asb_Knoten::AUSDRUCK_GANZZAHL, unär->ausdruck()->art(), "Falscher Operantentyp");
        TEST_ANNAHME_GL(5, unär->ausdruck()->als<Ausdruck_Ganzzahl*>()->wert(), "Falscher Wert");
        aufräumen(expr);
    }

    void klammer_ausdruck_prüfen()
    {
        auto expr = ausdruck_einlesen("(3 + 4)");
        TEST_ANNAHME_GL(Asb_Knoten::AUSDRUCK_KLAMMER, expr->art(), FEHLER_FALSCHER_AUSDRUCKSTYP);
        auto klammer = expr->als<Ausdruck_Klammer*>();
        TEST_ANNAHME_GL(Asb_Knoten::AUSDRUCK_BINÄR, klammer->ausdruck()->art(), "Falscher innerer Ausdruckstyp");
        auto bin = klammer->ausdruck()->als<Ausdruck_Binär*>();
        TEST_ANNAHME_GL(Ausdruck_Binär::ADDITION, bin->op(), "Falscher Operator");
        TEST_ANNAHME_GL(3, bin->links()->als<Ausdruck_Ganzzahl*>()->wert(), "Falscher linker Wert");
        TEST_ANNAHME_GL(4, bin->rechts()->als<Ausdruck_Ganzzahl*>()->wert(), "Falscher rechter Wert");
        aufräumen(expr);
    }

    void index_ausdruck_prüfen()
    {
        auto expr = ausdruck_einlesen("array[5]");
        TEST_ANNAHME_GL(Asb_Knoten::AUSDRUCK_INDEX, expr->art(), FEHLER_FALSCHER_AUSDRUCKSTYP);
        auto index = expr->als<Ausdruck_Index*>();
        TEST_ANNAHME_GL(Asb_Knoten::AUSDRUCK_BEZEICHNER, index->basis()->art(), "Falscher Basistyp");
        TEST_ANNAHME_TXTGL("array", index->basis()->als<Ausdruck_Bezeichner*>()->wert().c_str(), "Falscher Basisname");
        TEST_ANNAHME_GL(Asb_Knoten::AUSDRUCK_GANZZAHL, index->index()->art(), "Falscher Indextyp");
        TEST_ANNAHME_GL(5, index->index()->als<Ausdruck_Ganzzahl*>()->wert(), "Falscher Indexwert");
        aufräumen(expr);
    }

    void eigenschaft_ausdruck_prüfen()
    {
        auto expr = ausdruck_einlesen("objekt/feld");
        TEST_ANNAHME_GL(Asb_Knoten::AUSDRUCK_EIGENSCHAFT, expr->art(), FEHLER_FALSCHER_AUSDRUCKSTYP);
        auto eigenschaft = expr->als<Ausdruck_Eigenschaft*>();
        TEST_ANNAHME_GL(Asb_Knoten::AUSDRUCK_BEZEICHNER, eigenschaft->basis()->art(), "Falscher Basistyp");
        TEST_ANNAHME_TXTGL("objekt", eigenschaft->basis()->als<Ausdruck_Bezeichner*>()->wert().c_str(), "Falscher Basisname");
        TEST_ANNAHME_GL(Asb_Knoten::AUSDRUCK_BEZEICHNER, eigenschaft->eigenschaft()->art(), "Falscher Eigenschaftstyp");
        TEST_ANNAHME_TXTGL("feld", eigenschaft->eigenschaft()->als<Ausdruck_Bezeichner*>()->wert().c_str(), "Falscher Eigenschaftsname");
        aufräumen(expr);
    }

    void aufruf_ausdruck_prüfen()
    {
        auto expr = ausdruck_einlesen("funktion(1; 2)");
        TEST_ANNAHME_GL(Asb_Knoten::AUSDRUCK_AUFRUF, expr->art(), FEHLER_FALSCHER_AUSDRUCKSTYP);
        auto aufruf = expr->als<Ausdruck_Aufruf*>();
        TEST_ANNAHME_GL(Asb_Knoten::AUSDRUCK_BEZEICHNER, aufruf->basis()->art(), "Falscher Basistyp");
        TEST_ANNAHME_TXTGL("funktion", aufruf->basis()->als<Ausdruck_Bezeichner*>()->wert().c_str(), "Falscher Funktionsname");
        auto args = aufruf->argumente();
        TEST_ANNAHME_GL(2, args.size(), "Falsche Anzahl an Argumenten");
        TEST_ANNAHME_GL(Asb_Knoten::AUSDRUCK_GANZZAHL, args[0]->art(), "Falscher Argumenttyp 1");
        TEST_ANNAHME_GL(1, args[0]->als<Ausdruck_Ganzzahl*>()->wert(), "Falscher Argumentwert 1");
        TEST_ANNAHME_GL(Asb_Knoten::AUSDRUCK_GANZZAHL, args[1]->art(), "Falscher Argumenttyp 2");
        TEST_ANNAHME_GL(2, args[1]->als<Ausdruck_Ganzzahl*>()->wert(), "Falscher Argumentwert 2");
        aufräumen(expr);
    }

    void reihe_ausdruck_prüfen()
    {
        auto expr = ausdruck_einlesen("1..5");
        TEST_ANNAHME_GL(Asb_Knoten::AUSDRUCK_REIHE, expr->art(), FEHLER_FALSCHER_AUSDRUCKSTYP);
        auto reihe = expr->als<Ausdruck_Reihe *>();
        TEST_ANNAHME_GL(Asb_Knoten::AUSDRUCK_GANZZAHL, reihe->min()->art(), "Falscher Min-Typ");
        TEST_ANNAHME_GL(1, reihe->min()->als<Ausdruck_Ganzzahl*>()->wert(), "Falscher Min-Wert");
        TEST_ANNAHME_GL(Asb_Knoten::AUSDRUCK_GANZZAHL, reihe->max()->art(), "Falscher Max-Typ");
        TEST_ANNAHME_GL(5, reihe->max()->als<Ausdruck_Ganzzahl*>()->wert(), "Falscher Max-Wert");
        aufräumen(expr);
    }

    void import_ausdruck_prüfen()
    {
        auto expr = ausdruck_einlesen("#brauche »modul«");
        TEST_ANNAHME_GL(Asb_Knoten::AUSDRUCK_BRAUCHE, expr->art(), FEHLER_FALSCHER_AUSDRUCKSTYP);
        auto brauche = expr->als<Ausdruck_Brauche*>();
        TEST_ANNAHME_TXTGL("modul", brauche->dateiname().c_str(), "Falscher Dateiname");
        aufräumen(expr);
    }

    void ausführen_ausdruck_prüfen()
    {
        auto expr = ausdruck_einlesen("#! »kommando«");
        TEST_ANNAHME_GL(Asb_Knoten::AUSDRUCK_AUSFÜHREN, expr->art(), FEHLER_FALSCHER_AUSDRUCKSTYP);
        auto ausführen = expr->als<Ausdruck_Ausführen*>();
        TEST_ANNAHME_GL(Asb_Knoten::AUSDRUCK_TEXT, ausführen->ausdruck()->art(), FEHLER_FALSCHER_AUSDRUCKSTYP);
        TEST_ANNAHME_TXTGL("kommando", ausführen->ausdruck()->als<Ausdruck_Text*>()->wert().c_str(), "Falscher Kommando-Text");
        aufräumen(expr);
    }

    void kompositum_ausdruck_prüfen()
    {
        auto expr = ausdruck_einlesen("{1; /feld = 2}");
        TEST_ANNAHME_GL(Asb_Knoten::AUSDRUCK_KOMPOSITUM, expr->art(), FEHLER_FALSCHER_AUSDRUCKSTYP);
        auto kompositum = expr->als<Ausdruck_Kompositum*>();
        auto eigenschaften = kompositum->eigenschaften();
        TEST_ANNAHME_GL(2, eigenschaften.size(), "Falsche Anzahl an Eigenschaften");
        TEST_ANNAHME(!eigenschaften[0]->ist_benamt(), "Erste Eigenschaft sollte unbenannt sein");
        TEST_ANNAHME_GL(1, eigenschaften[0]->ausdruck()->als<Ausdruck_Ganzzahl*>()->wert(), "Falscher Wert erste Eigenschaft");
        TEST_ANNAHME(eigenschaften[1]->ist_benamt(), "Zweite Eigenschaft sollte benannt sein");
        TEST_ANNAHME_TXTGL("feld", eigenschaften[1]->name().c_str(), "Falscher Name zweite Eigenschaft");
        TEST_ANNAHME_GL(2, eigenschaften[1]->ausdruck()->als<Ausdruck_Ganzzahl*>()->wert(), "Falscher Wert zweite Eigenschaft");
        aufräumen(expr);
    }

    void funktion_ausdruck_prüfen()
    {
        auto expr = ausdruck_einlesen("(x | g32) -> g32 { res 42 }");
        TEST_ANNAHME_GL(Asb_Knoten::AUSDRUCK_FUNKTION, expr->art(), FEHLER_FALSCHER_AUSDRUCKSTYP);
        auto funktion = expr->als<Ausdruck_Funktion *>();
        auto params = funktion->parameter();
        TEST_ANNAHME_GL(1, params.size(), "Falsche Anzahl an Parametern");
        TEST_ANNAHME_TXTGL("x", params[0]->namen()[0].c_str(), "Falscher Parametername");
        TEST_ANNAHME_GL(Asb_Knoten::SPEZIFIZIERUNG_BEZEICHNER, funktion->rückgabe()->art(), "Falscher Rückgabetyp");
        TEST_ANNAHME_TXTGL("g32", funktion->rückgabe()->als<Spezifizierung_Bezeichner *>()->name().c_str(), "Falscher Rückgabe-Name");
        TEST_ANNAHME_GL(Asb_Knoten::ANWEISUNG_BLOCK, funktion->rumpf()->art(), "Falscher Rumpf-Typ");
        TEST_ANNAHME_GL(Asb_Knoten::ANWEISUNG_RES, funktion->rumpf()->als<Anweisung_Block *>()->anweisungen()[0]->art(), "Falscher Rumpf-Typ");
        aufräumen(expr);
    }

    void schablone_ausdruck_prüfen()
    {
        auto expr = ausdruck_einlesen("schablone { x | g32. }");
        TEST_ANNAHME_GL(Asb_Knoten::AUSDRUCK_SCHABLONE, expr->art(), FEHLER_FALSCHER_AUSDRUCKSTYP);
        auto schablone = expr->als<Ausdruck_Schablone*>();
        auto eigenschaften = schablone->eigenschaften();
        TEST_ANNAHME_GL(1, eigenschaften.size(), "Falsche Anzahl an Eigenschaften");
        TEST_ANNAHME_TXTGL("x", eigenschaften[0]->namen()[0].c_str(), "Falscher Eigenschaftsname");
        TEST_ANNAHME_GL(Asb_Knoten::SPEZIFIZIERUNG_BEZEICHNER, eigenschaften[0]->spezifizierung()->art(), "Falscher Spezifizierungstyp");
        TEST_ANNAHME_TXTGL("g32", eigenschaften[0]->spezifizierung()->als<Spezifizierung_Bezeichner*>()->name().c_str(), "Falscher Typname");
        aufräumen(expr);
    }

    void option_ausdruck_prüfen()
    {
        auto expr = ausdruck_einlesen("option { A | g32 = 1. }");
        TEST_ANNAHME_GL(Asb_Knoten::AUSDRUCK_OPTION, expr->art(), FEHLER_FALSCHER_AUSDRUCKSTYP);
        auto option = expr->als<Ausdruck_Option*>();
        auto eigenschaften = option->eigenschaften();
        TEST_ANNAHME_GL(1, eigenschaften.size(), "Falsche Anzahl an Eigenschaften");
        TEST_ANNAHME_TXTGL("A", eigenschaften[0]->namen()[0].c_str(), "Falscher Variantenname");
        TEST_ANNAHME_GL(Asb_Knoten::SPEZIFIZIERUNG_BEZEICHNER, eigenschaften[0]->spezifizierung()->art(), "Falscher Spezifizierungstyp");
        TEST_ANNAHME_TXTGL("g32", eigenschaften[0]->spezifizierung()->als<Spezifizierung_Bezeichner*>()->name().c_str(), "Falscher Typname");
        TEST_ANNAHME_GL(1, eigenschaften[0]->initialisierung()->als<Ausdruck_Ganzzahl*>()->wert(), "Falscher Initialisierungswert");
        aufräumen(expr);
    }

    void zuweisung_anweisung_prüfen()
    {
        auto anweisung = anweisung_einlesen("x = 42");
        TEST_ANNAHME_GL(Asb_Knoten::ANWEISUNG_ZUWEISUNG, anweisung->art(), FEHLER_FALSCHER_ANWEISUNGSTYP);
        auto zuweisung = anweisung->als<Anweisung_Zuweisung*>();
        TEST_ANNAHME_GL(Glied::ZUWEISUNG, zuweisung->op()->art(), "Falscher Zuweisungsoperator");
        TEST_ANNAHME_GL(Asb_Knoten::AUSDRUCK_BEZEICHNER, zuweisung->links()->art(), "Falscher linker Ausdruckstyp");
        TEST_ANNAHME_TXTGL("x", zuweisung->links()->als<Ausdruck_Bezeichner*>()->wert().c_str(), "Falscher Bezeichner");
        TEST_ANNAHME_GL(Asb_Knoten::AUSDRUCK_GANZZAHL, zuweisung->rechts()->art(), "Falscher rechter Ausdruckstyp");
        TEST_ANNAHME_GL(42, zuweisung->rechts()->als<Ausdruck_Ganzzahl*>()->wert(), "Falscher Wert");
        aufräumen(anweisung);
    }

    void wenn_anweisung_prüfen()
    {
        auto anweisung = anweisung_einlesen("wenn x { 42 } sonst { 0 }");
        TEST_ANNAHME_GL(Asb_Knoten::ANWEISUNG_WENN, anweisung->art(), FEHLER_FALSCHER_ANWEISUNGSTYP);
        auto wenn = anweisung->als<Anweisung_Wenn*>();
        TEST_ANNAHME_GL(Asb_Knoten::AUSDRUCK_BEZEICHNER, wenn->bedingung()->art(), "Falscher Bedingungstyp");
        TEST_ANNAHME_GL(Asb_Knoten::ANWEISUNG_BLOCK, wenn->rumpf()->art(), "Falscher Rumpftyp");
        TEST_ANNAHME(wenn->sonst() != nullptr, "Sonst-Zweig sollte vorhanden sein");
        TEST_ANNAHME_GL(Asb_Knoten::ANWEISUNG_BLOCK, wenn->sonst()->rumpf()->art(), "Falscher Sonst-Rumpftyp");
        aufräumen(anweisung);
    }

    void für_anweisung_prüfen()
    {
        auto anweisung = anweisung_einlesen("für i : 1..5 { x }");
        TEST_ANNAHME_GL(Asb_Knoten::ANWEISUNG_FÜR, anweisung->art(), FEHLER_FALSCHER_ANWEISUNGSTYP);
        auto für = anweisung->als<Anweisung_Für*>();
        TEST_ANNAHME_GL(Asb_Knoten::AUSDRUCK_BEZEICHNER, für->index()->art(), "Falscher Indextyp");
        TEST_ANNAHME_TXTGL("i", für->index()->als<Ausdruck_Bezeichner*>()->wert().c_str(), "Falscher Indexname");
        TEST_ANNAHME_GL(Asb_Knoten::AUSDRUCK_REIHE, für->bedingung()->art(), "Falscher Bedingungstyp");
        TEST_ANNAHME_GL(Asb_Knoten::ANWEISUNG_BLOCK, für->rumpf()->art(), "Falscher Rumpftyp");
        aufräumen(anweisung);
    }

    void solange_anweisung_prüfen()
    {
        auto anweisung = anweisung_einlesen("solange x < y { x = x + 1. }");
        TEST_ANNAHME_GL(Asb_Knoten::ANWEISUNG_SOLANGE, anweisung->art(), FEHLER_FALSCHER_ANWEISUNGSTYP);
        auto solange = anweisung->als<Anweisung_Solange *>();
        TEST_ANNAHME_GL(Asb_Knoten::AUSDRUCK_BINÄR, solange->bedingung()->art(), "Falscher Indextyp");
        TEST_ANNAHME_TXTGL("x", solange->bedingung()->als<Ausdruck_Binär *>()->links()->als<Ausdruck_Bezeichner *>()->wert().c_str(), "Falscher Indexname");
        TEST_ANNAHME_TXTGL("y", solange->bedingung()->als<Ausdruck_Binär *>()->rechts()->als<Ausdruck_Bezeichner *>()->wert().c_str(), "Falscher Indexname");
        TEST_ANNAHME_GL(Asb_Knoten::ANWEISUNG_BLOCK, solange->rumpf()->art(), "Falscher Rumpftyp");
        aufräumen(anweisung);
    }

    void weiter_anweisung_prüfen()
    {
        auto anweisung = anweisung_einlesen("weiter.");

        TEST_ANNAHME_GL(Asb_Knoten::ANWEISUNG_WEITER, anweisung->art(), FEHLER_FALSCHER_ANWEISUNGSTYP);

        aufräumen(anweisung);
    }

    void raus_anweisung_prüfen()
    {
        auto anweisung = anweisung_einlesen("raus.");

        TEST_ANNAHME_GL(Asb_Knoten::ANWEISUNG_RAUS, anweisung->art(), FEHLER_FALSCHER_ANWEISUNGSTYP);

        aufräumen(anweisung);
    }

    void sprung_anweisung_prüfen()
    {
        auto anweisung = anweisung_einlesen("sprung zur_marke.");
        TEST_ANNAHME_GL(Asb_Knoten::ANWEISUNG_SPRUNG, anweisung->art(), FEHLER_FALSCHER_ANWEISUNGSTYP);
        auto sprung = anweisung->als<Anweisung_Sprung *>();
        TEST_ANNAHME_GL(Asb_Knoten::AUSDRUCK_BEZEICHNER, sprung->markierung()->art(), "Falscher Indextyp");
        TEST_ANNAHME_TXTGL("zur_marke", sprung->markierung()->wert().c_str(), "Falscher Indexname");

        aufräumen(anweisung);
    }

    void markierung_anweisung_prüfen()
    {
        auto anweisung = anweisung_einlesen("<markierung1>.");
        TEST_ANNAHME_GL(Asb_Knoten::ANWEISUNG_MARKIERUNG, anweisung->art(), FEHLER_FALSCHER_ANWEISUNGSTYP);
        auto markierung = anweisung->als<Anweisung_Markierung *>();
        TEST_ANNAHME_TXTGL("markierung1", markierung->markierung().c_str(), "Falscher Indexname");

        aufräumen(anweisung);
    }

    void block_anweisung_prüfen()
    {
        auto anweisung = anweisung_einlesen("{ x = 1. y = 2 }");
        TEST_ANNAHME_GL(Asb_Knoten::ANWEISUNG_BLOCK, anweisung->art(), FEHLER_FALSCHER_ANWEISUNGSTYP);
        auto block = anweisung->als<Anweisung_Block*>();
        auto anweisungen = block->anweisungen();
        TEST_ANNAHME_GL(2, anweisungen.size(), "Falsche Anzahl an Anweisungen");
        TEST_ANNAHME_GL(Asb_Knoten::ANWEISUNG_ZUWEISUNG, anweisungen[0]->art(), "Falscher Typ der ersten Anweisung");
        TEST_ANNAHME_GL(Asb_Knoten::ANWEISUNG_ZUWEISUNG, anweisungen[1]->art(), "Falscher Typ der zweiten Anweisung");
        aufräumen(anweisung);
    }

    void deklaration_variable_anweisung_prüfen()
    {
        auto anweisung = anweisung_einlesen("x | g32 = 42");
        TEST_ANNAHME_GL(Asb_Knoten::ANWEISUNG_DEKLARATION, anweisung->art(), FEHLER_FALSCHER_ANWEISUNGSTYP);
        auto dek = anweisung->als<Anweisung_Deklaration*>();
        auto decl = dek->deklaration()->als<Deklaration_Variable*>();
        TEST_ANNAHME_GL(Asb_Knoten::DEKLARATION_VARIABLE, decl->art(), "Falscher Deklarationstyp");
        TEST_ANNAHME_GL(1, decl->namen().size(), "Falsche Anzahl an Namen");
        TEST_ANNAHME_TXTGL("x", decl->namen()[0].c_str(), "Falscher Variablenname");
        TEST_ANNAHME_GL(Asb_Knoten::SPEZIFIZIERUNG_BEZEICHNER, decl->spezifizierung()->art(), "Falscher Spezifizierungstyp");
        TEST_ANNAHME_GL(42, decl->initialisierung()->als<Ausdruck_Ganzzahl*>()->wert(), "Falscher Initialisierungswert");
        aufräumen(anweisung);
    }

    void deklaration_funktion_anweisung_prüfen()
    {
        auto anweisung = anweisung_einlesen("xyz || (a| g32 = -314; b| d64; c| n32 = 15) -> g32 { res 10. }");
        TEST_ANNAHME_GL(Asb_Knoten::ANWEISUNG_DEKLARATION, anweisung->art(), FEHLER_FALSCHER_ANWEISUNGSTYP);
        auto dek = anweisung->als<Anweisung_Deklaration*>();
        auto decl = dek->deklaration()->als<Deklaration_Funktion *>();
        TEST_ANNAHME_GL(Asb_Knoten::DEKLARATION_FUNKTION, decl->art(), "Falscher Deklarationstyp");
        TEST_ANNAHME_GL(1, decl->namen().size(), "Falsche Anzahl an Namen");
        TEST_ANNAHME_TXTGL("xyz", decl->namen()[0].c_str(), "Falscher Variablenname");
        TEST_ANNAHME_GL(3, decl->parameter().size(), "Falsche Anzahl an Parametern");
        TEST_ANNAHME_TXTGL("a", decl->parameter()[0]->namen()[0].c_str(), "Falscher Name des Parameters");
        TEST_ANNAHME_GL(Spezifizierung::SPEZIFIZIERUNG_BEZEICHNER, decl->parameter()[0]->spezifizierung()->art(), "Falscher Spezifizierungstyp");
        TEST_ANNAHME_TXTGL("g32", decl->parameter()[0]->spezifizierung()->als<Spezifizierung_Bezeichner *>()->name().c_str(), "Falscher Name der Spezifizierung");
        TEST_ANNAHME_TXTGL("b", decl->parameter()[1]->namen()[0].c_str(), "Falscher Name des Parameters");
        TEST_ANNAHME_GL(Spezifizierung::SPEZIFIZIERUNG_BEZEICHNER, decl->parameter()[1]->spezifizierung()->art(), "Falscher Spezifizierungstyp");
        TEST_ANNAHME_TXTGL("d64", decl->parameter()[1]->spezifizierung()->als<Spezifizierung_Bezeichner *>()->name().c_str(), "Falscher Name der Spezifizierung");
        TEST_ANNAHME_TXTGL("c", decl->parameter()[2]->namen()[0].c_str(), "Falscher Name des Parameters");
        TEST_ANNAHME_GL(Spezifizierung::SPEZIFIZIERUNG_BEZEICHNER, decl->parameter()[2]->spezifizierung()->art(), "Falscher Spezifizierungstyp");
        TEST_ANNAHME_TXTGL("n32", decl->parameter()[2]->spezifizierung()->als<Spezifizierung_Bezeichner *>()->name().c_str(), "Falscher Name der Spezifizierung");
        TEST_ANNAHME_NGL(nullptr, decl->rückgabe(), "Falscher Datentyp der Funktionsrückgabe");
        TEST_ANNAHME_GL(Spezifizierung::SPEZIFIZIERUNG_BEZEICHNER, decl->rückgabe()->art(), "Falscher Spezifizierungstyp");
        TEST_ANNAHME_NGL(nullptr, decl->rumpf(), "Falscher Datentyp des Funktionsrumpfs");

        aufräumen(anweisung);
    }

    void deklaration_funktion_signatur_anweisung_prüfen()
    {
        auto anweisung = anweisung_einlesen("xyz || (a| g32 = -314; b| d64; c| n32 = 15) -> g32.");
        TEST_ANNAHME_GL(Asb_Knoten::ANWEISUNG_DEKLARATION, anweisung->art(), FEHLER_FALSCHER_ANWEISUNGSTYP);
        auto dek = anweisung->als<Anweisung_Deklaration*>();
        auto decl = dek->deklaration()->als<Deklaration_Funktion *>();
        TEST_ANNAHME_GL(Asb_Knoten::DEKLARATION_FUNKTION, decl->art(), "Falscher Deklarationstyp");
        TEST_ANNAHME_GL(1, decl->namen().size(), "Falsche Anzahl an Namen");
        TEST_ANNAHME_TXTGL("xyz", decl->namen()[0].c_str(), "Falscher Variablenname");
        TEST_ANNAHME_GL(3, decl->parameter().size(), "Falsche Anzahl an Parametern");
        TEST_ANNAHME_TXTGL("a", decl->parameter()[0]->namen()[0].c_str(), "Falscher Name des Parameters");
        TEST_ANNAHME_GL(Spezifizierung::SPEZIFIZIERUNG_BEZEICHNER, decl->parameter()[0]->spezifizierung()->art(), "Falscher Spezifizierungstyp");
        TEST_ANNAHME_TXTGL("g32", decl->parameter()[0]->spezifizierung()->als<Spezifizierung_Bezeichner *>()->name().c_str(), "Falscher Name der Spezifizierung");
        TEST_ANNAHME_TXTGL("b", decl->parameter()[1]->namen()[0].c_str(), "Falscher Name des Parameters");
        TEST_ANNAHME_GL(Spezifizierung::SPEZIFIZIERUNG_BEZEICHNER, decl->parameter()[1]->spezifizierung()->art(), "Falscher Spezifizierungstyp");
        TEST_ANNAHME_TXTGL("d64", decl->parameter()[1]->spezifizierung()->als<Spezifizierung_Bezeichner *>()->name().c_str(), "Falscher Name der Spezifizierung");
        TEST_ANNAHME_TXTGL("c", decl->parameter()[2]->namen()[0].c_str(), "Falscher Name des Parameters");
        TEST_ANNAHME_GL(Spezifizierung::SPEZIFIZIERUNG_BEZEICHNER, decl->parameter()[2]->spezifizierung()->art(), "Falscher Spezifizierungstyp");
        TEST_ANNAHME_TXTGL("n32", decl->parameter()[2]->spezifizierung()->als<Spezifizierung_Bezeichner *>()->name().c_str(), "Falscher Name der Spezifizierung");
        TEST_ANNAHME_NGL(nullptr, decl->rückgabe(), "Falscher Datentyp der Funktionsrückgabe");
        TEST_ANNAHME_GL(Spezifizierung::SPEZIFIZIERUNG_BEZEICHNER, decl->rückgabe()->art(), "Falscher Spezifizierungstyp");
        TEST_ANNAHME_GL(nullptr, decl->rumpf(), "Falscher Datentyp des Funktionsrumpfs");

        aufräumen(anweisung);
    }

    void res_anweisung_prüfen()
    {
        auto anweisung = anweisung_einlesen("res 42");
        TEST_ANNAHME_GL(Asb_Knoten::ANWEISUNG_RES, anweisung->art(), FEHLER_FALSCHER_ANWEISUNGSTYP);
        auto res = anweisung->als<Anweisung_Res*>();
        TEST_ANNAHME_GL(Asb_Knoten::AUSDRUCK_GANZZAHL, res->ausdruck()->art(), FEHLER_FALSCHER_AUSDRUCKSTYP);
        TEST_ANNAHME_GL(42, res->ausdruck()->als<Ausdruck_Ganzzahl*>()->wert(), "Falscher Wert");
        aufräumen(anweisung);
    }

    void bezeichner_spezifizierung_prüfen()
    {
        auto spez = spezifizierung_einlesen("g32");
        TEST_ANNAHME_GL(Asb_Knoten::SPEZIFIZIERUNG_BEZEICHNER, spez->art(), "Falscher Spezifizierungstyp");
        auto bez = spez->als<Spezifizierung_Bezeichner*>();
        TEST_ANNAHME_TXTGL("g32", bez->name().c_str(), "Falscher Bezeichnername");
        aufräumen(spez);
    }

    void zeiger_spezifizierung_prüfen()
    {
        auto spez = spezifizierung_einlesen("*g32");
        TEST_ANNAHME_GL(Asb_Knoten::SPEZIFIZIERUNG_ZEIGER, spez->art(), "Falscher Spezifizierungstyp");
        auto zeiger = spez->als<Spezifizierung_Zeiger*>();
        TEST_ANNAHME_GL(Asb_Knoten::SPEZIFIZIERUNG_BEZEICHNER, zeiger->basis()->art(), "Falscher Basistyp");
        TEST_ANNAHME_TXTGL("g32", zeiger->basis()->als<Spezifizierung_Bezeichner*>()->name().c_str(), "Falscher Basisname");
        aufräumen(spez);
    }

    void feld_spezifizierung_prüfen()
    {
        auto spez = spezifizierung_einlesen("[5]g32");
        TEST_ANNAHME_GL(Asb_Knoten::SPEZIFIZIERUNG_FELD, spez->art(), "Falscher Spezifizierungstyp");
        auto feld = spez->als<Spezifizierung_Feld*>();
        TEST_ANNAHME_GL(Asb_Knoten::SPEZIFIZIERUNG_BEZEICHNER, feld->basis()->art(), "Falscher Basistyp");
        TEST_ANNAHME_TXTGL("g32", feld->basis()->als<Spezifizierung_Bezeichner*>()->name().c_str(), "Falscher Basisname");
        TEST_ANNAHME_GL(Asb_Knoten::AUSDRUCK_GANZZAHL, feld->index()->art(), "Falscher Indextyp");
        TEST_ANNAHME_GL(5, feld->index()->als<Ausdruck_Ganzzahl*>()->wert(), "Falscher Indexwert");
        aufräumen(spez);
    }

    void funktion_spezifizierung_prüfen()
    {
        auto spez = spezifizierung_einlesen("(g32; *u8) -> bool");
        TEST_ANNAHME_GL(Asb_Knoten::SPEZIFIZIERUNG_FUNKTION, spez->art(), "Falscher Spezifizierungstyp");
        auto funk = spez->als<Spezifizierung_Funktion*>();
        auto params = funk->parameter();
        TEST_ANNAHME_GL(2, params.size(), "Falsche Anzahl an Parametern");
        TEST_ANNAHME_GL(Asb_Knoten::SPEZIFIZIERUNG_BEZEICHNER, params[0]->art(), "Falscher Typ des ersten Parameters");
        TEST_ANNAHME_TXTGL("g32", params[0]->als<Spezifizierung_Bezeichner*>()->name().c_str(), "Falscher Name des ersten Parameters");
        TEST_ANNAHME_GL(Asb_Knoten::SPEZIFIZIERUNG_ZEIGER, params[1]->art(), "Falscher Typ des zweiten Parameters");
        TEST_ANNAHME_GL(Asb_Knoten::SPEZIFIZIERUNG_BEZEICHNER, funk->rückgabe()->art(), "Falscher Rückgabetyp");
        TEST_ANNAHME_TXTGL("bool", funk->rückgabe()->als<Spezifizierung_Bezeichner*>()->name().c_str(), "Falscher Rückgabename");
        aufräumen(spez);
    }

    void alle_prüfungen_ausführen()
    {
        std::cout << "\n--- Ausdruck Prüfungen ---\n";

        prüfung_ausführen("Ganzzahlausdruck", &Syntax_Test::ganzzahl_literal_prüfen);
        prüfung_ausführen("Dezimalzahlausdruck", &Syntax_Test::dezimal_literal_prüfen);
        prüfung_ausführen("Bezeichnerausdruck", &Syntax_Test::bezeichner_prüfen);
        prüfung_ausführen("Textausdruck", &Syntax_Test::text_literal_prüfen);
        prüfung_ausführen("Binärausdruck", &Syntax_Test::binär_ausdruck_prüfen);
        prüfung_ausführen("Unärausdruck", &Syntax_Test::unär_ausdruck_prüfen);
        prüfung_ausführen("Klammerausdruck", &Syntax_Test::klammer_ausdruck_prüfen);
        prüfung_ausführen("Indexausdruck", &Syntax_Test::index_ausdruck_prüfen);
        prüfung_ausführen("Eigenschaftsausdruck", &Syntax_Test::eigenschaft_ausdruck_prüfen);
        prüfung_ausführen("Aufrufausdruck", &Syntax_Test::aufruf_ausdruck_prüfen);
        prüfung_ausführen("Reihenausdruck", &Syntax_Test::reihe_ausdruck_prüfen);
        prüfung_ausführen("Importausdruck", &Syntax_Test::import_ausdruck_prüfen);
        prüfung_ausführen("Ausführungsausdruck", &Syntax_Test::ausführen_ausdruck_prüfen);
        prüfung_ausführen("Kompositumsausdruck", &Syntax_Test::kompositum_ausdruck_prüfen);
        prüfung_ausführen("Funktionsausdruck", &Syntax_Test::funktion_ausdruck_prüfen);
        prüfung_ausführen("Schablonenausdruck", &Syntax_Test::schablone_ausdruck_prüfen);
        prüfung_ausführen("Optionsausdruck", &Syntax_Test::option_ausdruck_prüfen);

        std::cout << "\n--- Anweisung Prüfungen ---\n";

        prüfung_ausführen("Zuweisungsanweisung", &Syntax_Test::zuweisung_anweisung_prüfen);
        prüfung_ausführen("Wenn-Anweisung", &Syntax_Test::wenn_anweisung_prüfen);
        prüfung_ausführen("Für-Anweisung", &Syntax_Test::für_anweisung_prüfen);
        prüfung_ausführen("Solange-Anweisung", &Syntax_Test::solange_anweisung_prüfen);
        prüfung_ausführen("Block-Anweisung", &Syntax_Test::block_anweisung_prüfen);
        prüfung_ausführen("Variable Deklarationsanweisung", &Syntax_Test::deklaration_variable_anweisung_prüfen);
        prüfung_ausführen("Funktionssignatur Deklarationsanweisung", &Syntax_Test::deklaration_funktion_signatur_anweisung_prüfen);
        prüfung_ausführen("Funktion Deklarationsanweisung", &Syntax_Test::deklaration_funktion_anweisung_prüfen);
        prüfung_ausführen("Res-Anweisung", &Syntax_Test::res_anweisung_prüfen);
        prüfung_ausführen("Weiter-Anweisung", &Syntax_Test::weiter_anweisung_prüfen);
        prüfung_ausführen("Raus-Anweisung", &Syntax_Test::raus_anweisung_prüfen);
        prüfung_ausführen("Sprung-Anweisung", &Syntax_Test::sprung_anweisung_prüfen);
        prüfung_ausführen("Markierung-Anweisung", &Syntax_Test::markierung_anweisung_prüfen);

        std::cout << "\n--- Spezifizierung Prüfungen ---\n";

        prüfung_ausführen("Bezeichner-Spezifizierung", &Syntax_Test::bezeichner_spezifizierung_prüfen);
        prüfung_ausführen("Zeiger-Spezifizierung", &Syntax_Test::zeiger_spezifizierung_prüfen);
        prüfung_ausführen("Feld-Spezifizierung", &Syntax_Test::feld_spezifizierung_prüfen);
        prüfung_ausführen("Funktions-Spezifizierung", &Syntax_Test::funktion_spezifizierung_prüfen);

        std::cout << "\n_syntax Prüfungsergebnis: " << prüfungen_bestanden << "/"
                  << prüfungen_ausgeführt << " Prüfungen bestanden\n";
    }
};

#include <iostream>

#include "kmp/glied.hpp"
#include "kmp/lexer.hpp"

#include "makros.hpp"

using namespace Sss::Kmp;

class Lexer_Test
{
private:
    int prüfungen_ausgeführt = 0;
    int prüfungen_bestanden = 0;

    std::vector<Glied*> lex(const std::string& input)
    {
        Lexer lexer("test.txt", input);

        return lexer.starten();
    }

    void aufräumen(std::vector<Glied*>& erg)
    {
        for (auto* glied : erg)
        {
            delete glied;
        }

        erg.clear();
    }

public:
    void run_test(const std::string& name, void (Lexer_Test::*test)())
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

    void einfache_glieder_prüfen()
    {
        auto erg = lex("@ . , : ; | ( ) [ ] { } + - * / # ! % & = < >");

        TEST_ANNAHME_GL(24, erg.size(), "Falsche Anzahl an Gliedern");
        TEST_ANNAHME_GL(Glied::KLAMMERAFFE, erg[0]->art(), "Falsche Gliedart");
        TEST_ANNAHME_GL(Glied::PUNKT, erg[1]->art(), "Falsche Gliedart");
        TEST_ANNAHME_GL(Glied::KOMMA, erg[2]->art(), "Falsche Gliedart");
        TEST_ANNAHME_GL(Glied::DOPPELPUNKT, erg[3]->art(), "Falsche Gliedart");
        TEST_ANNAHME_GL(Glied::STRICHPUNKT, erg[4]->art(), "Falsche Gliedart");
        TEST_ANNAHME_GL(Glied::BALKEN, erg[5]->art(), "Falsche Gliedart");
        TEST_ANNAHME_GL(Glied::RKLAMMER_AUF, erg[6]->art(), "Falsche Gliedart");
        TEST_ANNAHME_GL(Glied::RKLAMMER_ZU, erg[7]->art(), "Falsche Gliedart");
        TEST_ANNAHME_GL(Glied::EKLAMMER_AUF, erg[8]->art(), "Falsche Gliedart");
        TEST_ANNAHME_GL(Glied::EKLAMMER_ZU, erg[9]->art(), "Falsche Gliedart");
        TEST_ANNAHME_GL(Glied::GKLAMMER_AUF, erg[10]->art(), "Falsche Gliedart");
        TEST_ANNAHME_GL(Glied::GKLAMMER_ZU, erg[11]->art(), "Falsche Gliedart");
        TEST_ANNAHME_GL(Glied::PLUS, erg[12]->art(), "Falsche Gliedart");
        TEST_ANNAHME_GL(Glied::MINUS, erg[13]->art(), "Falsche Gliedart");
        TEST_ANNAHME_GL(Glied::STERN, erg[14]->art(), "Falsche Gliedart");
        TEST_ANNAHME_GL(Glied::PISA, erg[15]->art(), "Falsche Gliedart");
        TEST_ANNAHME_GL(Glied::RAUTE, erg[16]->art(), "Falsche Gliedart");
        TEST_ANNAHME_GL(Glied::AUSRUFEZEICHEN, erg[17]->art(), "Falsche Gliedart");
        TEST_ANNAHME_GL(Glied::PROZENT, erg[18]->art(), "Falsche Gliedart");
        TEST_ANNAHME_GL(Glied::KAUFMANNSUND, erg[19]->art(), "Falsche Gliedart");
        TEST_ANNAHME_GL(Glied::ZUWEISUNG, erg[20]->art(), "Falsche Gliedart");
        TEST_ANNAHME_GL(Glied::KLEINER, erg[21]->art(), "Falsche Gliedart");
        TEST_ANNAHME_GL(Glied::GRÖẞER, erg[22]->art(), "Falsche Gliedart");
        TEST_ANNAHME_GL(Glied::ENDE, erg[23]->art(), "Falsche Gliedart");

        aufräumen(erg);
    }

    void zusammengesetzte_glieder_prüfen()
    {
        auto erg = lex(".. += -> |> <| <= >= != == =>");

        TEST_ANNAHME_GL(11, erg.size(), "Falsche Anzahl der Glieder");
        TEST_ANNAHME_GL(Glied::PUNKT_PUNKT, erg[0]->art(), "Falsche Gliedart");
        TEST_ANNAHME_GL(Glied::ZUWEISUNG_PLUS, erg[1]->art(), "Falsche Gliedart");
        TEST_ANNAHME_GL(Glied::PFEIL, erg[2]->art(), "Falsche Gliedart");
        TEST_ANNAHME_GL(Glied::DREIECK_RECHTS, erg[3]->art(), "Falsche Gliedart");
        TEST_ANNAHME_GL(Glied::DREIECK_LINKS, erg[4]->art(), "Falsche Gliedart");
        TEST_ANNAHME_GL(Glied::KLEINER_GLEICH, erg[5]->art(), "Falsche Gliedart");
        TEST_ANNAHME_GL(Glied::GRÖẞER_GLEICH, erg[6]->art(), "Falsche Gliedart");
        TEST_ANNAHME_GL(Glied::UNGLEICH, erg[7]->art(), "Falsche Gliedart");
        TEST_ANNAHME_GL(Glied::GLEICH, erg[8]->art(), "Falsche Gliedart");
        TEST_ANNAHME_GL(Glied::FOLGERUNG, erg[9]->art(), "Falsche Gliedart");
        TEST_ANNAHME_GL(Glied::ENDE, erg[10]->art(), "Falsche Gliedart");

        aufräumen(erg);
    }

    void nummern_prüfen()
    {
        auto erg = lex("123 45,67 10b1000");

        TEST_ANNAHME_GL(4, erg.size(), "Falsche Anzahl der Glieder");
        TEST_ANNAHME_GL(Glied::GANZZAHL, erg[0]->art(), "Falsche Gliedart");
        TEST_ANNAHME_GL(123, ((Glied_Ganzzahl *) erg[0])->wert(), "Falscher Ganzzahlwert");
        TEST_ANNAHME_GL(Glied::DEZIMALZAHL, erg[1]->art(), "Falsche Gliedart");
        float dezimal_diff = std::abs(45.67f - ((Glied_Dezimalzahl *) erg[1])->wert());
        TEST_ANNAHME(dezimal_diff < 0.001, "Falscher Dezimalwert");
        TEST_ANNAHME_GL(Glied::GANZZAHL, erg[2]->art(), "Falsche Gliedart");
        TEST_ANNAHME_GL(10, ((Glied_Ganzzahl *) erg[2])->basis(), "Falscher Basiswert");
        TEST_ANNAHME_GL(1000, ((Glied_Ganzzahl *) erg[2])->wert(), "Falscher Wert");

        aufräumen(erg);
    }

    void bezeichner_prüfen()
    {
        auto erg = lex("wie_gehts Hallo123 👨‍⚕️🙉😅 ᚠᚢᚦᚨᚱᚲᚷᚹᚻᚾᛁᛃᛇᛈᛉᛋᛏᛒ カタカナ");

        TEST_ANNAHME_GL(6, erg.size(), "Falsche Anzahl der Glieder");
        TEST_ANNAHME_GL(Glied::BEZEICHNER, erg[0]->art(), "Falsche Gliedart");
        TEST_ANNAHME_TXTGL("wie_gehts", erg[0]->text(), "Falscher Bezeichner");
        TEST_ANNAHME_GL(Glied::BEZEICHNER, erg[1]->art(), "Falsche Gliedart");
        TEST_ANNAHME_TXTGL("Hallo123", erg[1]->text(), "Falscher Bezeichner");
        TEST_ANNAHME_GL(Glied::BEZEICHNER, erg[2]->art(), "Falsche Gliedart");
        TEST_ANNAHME_TXTGL("👨‍⚕️🙉😅", erg[2]->text(), "Falscher Bezeichner");
        TEST_ANNAHME_GL(Glied::BEZEICHNER, erg[3]->art(), "Falsche Gliedart");
        TEST_ANNAHME_TXTGL("ᚠᚢᚦᚨᚱᚲᚷᚹᚻᚾᛁᛃᛇᛈᛉᛋᛏᛒ", erg[3]->text(), "Falscher Bezeichner");
        TEST_ANNAHME_GL(Glied::BEZEICHNER, erg[4]->art(), "Falsche Gliedart");
        TEST_ANNAHME_TXTGL("カタカナ", erg[4]->text(), "Falscher Bezeichner");

        aufräumen(erg);
    }

    void text_prüfen()
    {
        auto erg = lex("»Hallo Welt« »Escaped \\\" Quote«");

        TEST_ANNAHME_GL(3, erg.size(), "Falsche Anzahl der Glieder");
        TEST_ANNAHME_GL(Glied::TEXT, erg[0]->art(), "Falsche Gliedart");
        TEST_ANNAHME_TXTGL("Hallo Welt", erg[0]->text(), "Wrong text content");
        TEST_ANNAHME_GL(Glied::TEXT, erg[1]->art(), "Falsche Gliedart");
        TEST_ANNAHME_TXTGL("Escaped \\\" Quote", erg[1]->text(), "Wrong text content");

        aufräumen(erg);
    }

    void test_kommentare()
    {
        auto erg = lex("## Comment\n code ##( nested ##( comment ##) ##) more");

        TEST_ANNAHME_GL(3, erg.size(), "Falsche Anzahl der Glieder");
        TEST_ANNAHME_GL(Glied::BEZEICHNER, erg[0]->art(), "Falsche Gliedart");
        TEST_ANNAHME_TXTGL("code", erg[0]->text(), "Wrong identifier text");
        TEST_ANNAHME_GL(Glied::BEZEICHNER, erg[1]->art(), "Falsche Gliedart");
        TEST_ANNAHME_TXTGL("more", erg[1]->text(), "Wrong identifier text");

        aufräumen(erg);
    }

    void test_dateiende()
    {
        auto erg = lex("test");

        TEST_ANNAHME_GL(2, erg.size(), "Falsche Anzahl der Glieder");
        TEST_ANNAHME_GL(Glied::BEZEICHNER, erg[0]->art(), "Falsche Gliedart");
        TEST_ANNAHME_GL(Glied::ENDE, erg[1]->art(), "Falsche Gliedart");

        aufräumen(erg);
    }

    void run_all_tests()
    {
        run_test("Einfache Glieder", &Lexer_Test::einfache_glieder_prüfen);
        run_test("Zusammengesetzte Glieder", &Lexer_Test::zusammengesetzte_glieder_prüfen);
        run_test("Zahlen", &Lexer_Test::nummern_prüfen);
        run_test("Bezeichner", &Lexer_Test::bezeichner_prüfen);
        run_test("Text", &Lexer_Test::text_prüfen);
        run_test("Kommentare", &Lexer_Test::test_kommentare);
        run_test("Dateiende", &Lexer_Test::test_dateiende);

        std::cout << "\n_prüfungsergebnis: " << prüfungen_bestanden << "/"
                  << prüfungen_ausgeführt << " Prüfungen bestanden\n";
    }
};


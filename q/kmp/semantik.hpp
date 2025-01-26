#pragma once

#include "ast.hpp"
#include "diagnostik.hpp"
#include "operand.hpp"
#include "symbol.hpp"
#include "zone.hpp"

namespace Sss::Kmp {

class Semantik
{
public:
    Semantik(Ast ast, Zone *system = nullptr, Zone *global = nullptr);

    Ast starten();

    Zone * aktive_zone() const;
    Zone * system() const;
    Zone * global() const;

    bool registrieren(std::string name, Symbol *symbol);
    bool registrieren(Symbol *symbol);
    bool ist_registriert(std::string name);

    void system_zone_initialisieren(Zone *zone);
    void importe_registrieren();
    void deklarationen_registrieren();
    void symbole_analysieren();
    void datentyp_abschließen(Datentyp *datentyp, bool basis_eines_zeigers = false);

    void symbol_analysieren(Symbol *symbol, Deklaration *deklaration);
    void variable_analysieren(Symbol *symbol, Deklaration_Variable *deklaration);
    void funktion_analysieren(Symbol *symbol, Deklaration_Funktion *deklaration);
    void opt_analysieren(Symbol *symbol, Deklaration_Opt *deklaration);
    void schablone_analysieren(Symbol *symbol, Deklaration_Schablone *deklaration, bool zirkularität_ignorieren = false);
    void muster_analysieren(Ausdruck *muster, Datentyp *datentyp);

    void       anweisung_analysieren(Anweisung *anweisung, Datentyp *über = nullptr);
    Datentyp * deklaration_analysieren(Deklaration *deklaration);
    Datentyp * spezifizierung_analysieren(Spezifizierung *spezifizierung);
    Operand  * ausdruck_analysieren(Ausdruck *ausdruck, Datentyp *erwarteter_datentyp = nullptr);
    Symbol   * bezeichner_analysieren(std::string bezeichner);

    void import_verarbeiten(std::string dateiname);

    bool operanden_kompatibel(Operand *ziel, Operand *quelle);

    void zone_betreten(Zone *zone = nullptr);
    void zone_verlassen();

    void panik(Spanne spanne, std::string text);
    void panik(Symbol *symbol, std::string text);
    void panik(Ausdruck *ausdruck, std::string text);
    void panik(Spezifizierung *spezifizierung, std::string text);
    void panik(Deklaration *deklaration, std::string text);
    void panik(Anweisung *anweisung, std::string text);

    void melden(Spanne spanne, std::string text);
    void melden(Ausdruck *ausdruck, std::string text);
    void melden(Spezifizierung *spezifizierung, std::string text);

private:
    Zone *_zone;
    Zone *_system;
    Zone *_global;

    Diagnostik _diagnostik;
    Ast _ast;

    std::vector<Deklaration_Schablone *> _schablonen;
};

}

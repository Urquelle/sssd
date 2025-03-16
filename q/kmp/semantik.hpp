#pragma once

#include "kmp/asb.hpp"
#include "kmp/diagnostik.hpp"
#include "kmp/operand.hpp"
#include "kmp/symbol.hpp"
#include "kmp/zone.hpp"
#include "kmp/bestimmter_asb.hpp"

namespace Sss::Kmp {

class Semantik
{
public:
    Semantik(Asb ast, Zone *system = nullptr, Zone *global = nullptr);

    static void system_zone_initialisieren(Zone *zone);

    Bestimmter_Asb * starten(bool mit_hauptmethode);

    Zone * aktive_zone() const;
    Zone * system() const;
    Zone * global() const;

    bool registrieren(std::string name, Symbol *symbol);
    bool registrieren(Symbol *symbol);
    bool ist_registriert(std::string name);

    void importe_registrieren();
    void deklarationen_registrieren();
    void symbole_analysieren(bool mit_hauptmethode);
    void datentyp_abschließen(Datentyp *datentyp, bool basis_eines_zeigers = false);
    void globale_anweisungen_der_hauptmethode_zuordnen();
    void anweisungen_reduzieren();

    Bestimmte_Deklaration * symbol_analysieren(Symbol *symbol, Deklaration *deklaration);
    Bestimmte_Deklaration * variable_analysieren(Symbol *symbol, Deklaration_Variable *deklaration);
    Bestimmte_Deklaration * funktion_analysieren(Symbol *symbol, Deklaration_Funktion *deklaration);
    Bestimmte_Deklaration * option_analysieren(Symbol *symbol, Deklaration_Option *deklaration);
    Bestimmte_Deklaration * schablone_analysieren(Symbol *symbol, Deklaration_Schablone *deklaration, bool zirkularität_ignorieren = false);

    bool                    anweisung_analysieren(Anweisung *anweisung, Datentyp *über = nullptr, Bestimmte_Anweisung **bestimmte_anweisung = nullptr);
    Bestimmte_Deklaration * deklaration_analysieren(Deklaration *deklaration);
    Datentyp              * spezifizierung_analysieren(Spezifizierung *spezifizierung);
    Bestimmter_Ausdruck   * ausdruck_analysieren(Ausdruck *ausdruck, Datentyp *erwarteter_datentyp = nullptr);
    Symbol                * bezeichner_analysieren(std::string bezeichner);
    Bestimmter_Ausdruck   * muster_analysieren(Ausdruck *muster, Datentyp *datentyp);

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
    void melden(Anweisung *anweisung, std::string text);
    void melden(Spezifizierung *spezifizierung, std::string text);
    void melden(Symbol *symbol, std::string text);
    void melden(Deklaration *deklaration, std::string text);

    Diagnostik diagnostik() const;

private:
    Zone *_zone;
    Zone *_system;
    Zone *_global;

    Diagnostik _diagnostik;
    Asb _asb;
    Bestimmter_Asb *_bestimmter_asb;

    Symbol *_hauptmethode;
    std::vector<Deklaration_Schablone *> _schablonen;
};

}

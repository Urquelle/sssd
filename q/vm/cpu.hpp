#pragma once

#include <cstdint>
#include <string>

#include "vm/register.hpp"
#include "vm/laufwerk.hpp"
#include "vm/anweisung.hpp"

namespace Sss::Vm {

class Cpu
{
public:
    Cpu(Laufwerk *laufwerk);

    Laufwerk *laufwerk() const;

    void ausführen(uint16_t adresse);
    void schritt();

    std::optional<Anweisung *> dekodieren(uint16_t wert);

    void panik(std::string meldung);

    void stand_speichern();
    void stand_laden();

    uint16_t pop();
    bool push(uint16_t wert);

    uint16_t reg(Register r) const;
    void reg(Register r, uint16_t wert);
    void reg(Register r, Schalter wert);

    bool beenden() const;
    void beenden(bool wert);

private:
    uint16_t _stapel_rahmen_größe = { 0 };
    Laufwerk *_laufwerk = { nullptr };
    uint16_t _regs[16] = { 0 };
    bool _beenden = { false };

};

}

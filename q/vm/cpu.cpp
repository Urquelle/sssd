#include "vm/cpu.hpp"

#include <format>
#include <cassert>
#include <iostream>

namespace Sss::Vm {

Cpu::Cpu(Laufwerk *laufwerk)
    : _laufwerk(laufwerk)
{
    for (int i = (int) Register::R1; i < (int) Register::SCHLT; ++i)
    {
        _regs[i] = 0;
    }

    reg(Register::SCHLT, Schalter::Null);
    reg(Register::SP, 0xFFFF - 1);
    reg(Register::FP, reg(Register::SP));
}

Laufwerk *
Cpu::laufwerk() const
{
    return _laufwerk;
}

uint16_t
Cpu::reg(Register r) const
{
    auto erg = (uint16_t) _regs[(int) r];

    return erg;
}

void
Cpu::reg(Register r, uint16_t wert)
{
    _regs[(int) r] = wert;
}

void
Cpu::reg(Register r, Schalter wert)
{
    _regs[(int) r] = (uint16_t) wert;
}

bool
Cpu::beenden() const
{
    return _beenden;
}

void
Cpu::beenden(bool wert)
{
    _beenden = wert;
}

void
Cpu::ausführen(uint16_t adresse)
{
    _regs[(int) Register::IP] = adresse;

    while (!_beenden)
    {
        schritt();
    }
}

void
Cpu::schritt()
{
    auto adr = _regs[(int) Register::IP];

    auto wert = _laufwerk->lesen2(adr);
    if (!wert.has_value())
    {
        panik(std::format("daten konnten nicht gelesen werden adr: {}", adr));
    }

    auto instr = dekodieren(wert.value());
    if (!instr.has_value())
    {
        panik(std::format("ungültige instruktion adr: {}", adr));
    }

    instr.value()->ausführen(this);
}

std::optional<Vm::Anweisung *>
Cpu::dekodieren(uint16_t daten)
{
    return std::nullopt;
}

void
Cpu::panik(std::string meldung)
{
    std::cerr << meldung;
    __debugbreak();
}

void
Cpu::stand_speichern()
{
    for (uint8_t reg_index = (int) Register::R1; reg_index <= (int) Register::R8; ++reg_index)
    {
        push(_regs[reg_index]);
    }

    push(_regs[reg(Register::IP)]);
    push(_stapel_rahmen_größe + 2);

    _regs[(int) Register::FP] = reg(Register::SP);
    _stapel_rahmen_größe = 0;
}

void
Cpu::stand_laden()
{
    auto fp = reg(Register::FP);
    _regs[(int) Register::SP] = fp;

    _stapel_rahmen_größe = pop();
    auto lokal_stapel_rahmen_größe = _stapel_rahmen_größe;

    _regs[(int) Register::IP] = pop();

    for (uint8_t reg_index = (int) Register::R8; reg_index >= (int) Register::R1; --reg_index)
    {
        _regs[reg_index] = pop();
    }

    auto z_args = pop();

    for (uint8_t args_index = 0; args_index < z_args; ++args_index)
    {
        pop();
    }

    _regs[(int) Register::FP] = fp + lokal_stapel_rahmen_größe;
}

uint16_t
Cpu::pop()
{
    _regs[(int) Register::SP] += 2;
    auto erg = _laufwerk->lesen2(reg(Register::SP));
    _stapel_rahmen_größe -= 2;

    return erg.value();
}

bool
Cpu::push(uint16_t wert)
{
    if (_laufwerk->schreiben2(reg(Register::SP), wert))
    {
        _regs[(int) Register::SP] -= 2;
        _stapel_rahmen_größe += 2;

        return true;
    }

    return false;
}

}

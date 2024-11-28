#include "vm/anweisung.hpp"
#include "vm/cpu.hpp"

#include <numeric>

namespace Sss::Vm {

void
schalter_setzen(Cpu *cpu, Register reg)
{
    if (cpu->reg(reg) == 0)
    {
        cpu->reg(Register::SCHLT, (int) Schalter::Null);
    }

    // falls das linkeste bit eine 1 ist, dann ist
    // die zahl negativ
    else if (cpu->reg(reg) >> 15)
    {
        cpu->reg(Register::SCHLT, (int) Schalter::Negativ);
    }
    else
    {
        cpu->reg(Register::SCHLT, (int) Schalter::Positiv);
    }
}

uint16_t
vorzeichen_erweitern(uint16_t x, uint8_t z_bits)
{
    if ((x >> (z_bits - 1)) & 1)
    {
        x |= (0xFFFF << z_bits);
    }

    return x;
}

Anweisung::Anweisung(Anweisung::Operation operation, std::vector<Operand *> operanden)
    : _operanden(std::move(operanden))
{
    _größe = std::accumulate(
        _operanden.begin(), _operanden.end(), 0, 
        [](int summe, const Operand *op)
        {
            return summe + (int) op->größe();
        }
    );
}

auto
Anweisung::operanden() const
{
    return _operanden;
}

std::optional<Operand *>
Anweisung::operand(int index)
{
    if (index >= _operanden.size())
    {
        return std::nullopt;
    }

    return _operanden.at(index);
}

uint16_t
Anweisung::wert(Cpu *cpu, Operand *op)
{
    uint16_t erg = 0;

    switch (op->art())
    {
        case Operand::Art::Reg:
        {
            erg = cpu->reg(op->als<Operand_Reg *>()->wert());
        } break;

        case Operand::Art::Imm:
        {
            erg = op->als<Operand_Imm *>()->wert();
        } break;

        case Operand::Art::Adr:
        {
            auto werg = cpu->laufwerk()->lesen2(
                op->als<Operand_Adr *>()->wert()
            );

            if (werg.has_value())
            {
                cpu->panik("daten konnten nicht gelesen werden");
            }

            erg = werg.value();
        } break;
    }

    return erg;
}

auto
Anweisung::größe() const
{
    return _größe;
}

Anweisung_Hlt::Anweisung_Hlt()
    : Anweisung({})
{
}

void
Anweisung_Hlt::ausführen(Cpu *cpu)
{
    cpu->beenden(true);
}

Anweisung_Add::Anweisung_Add(Operand *links, Operand *rechts)
    : Anweisung({ links, rechts })
{
}

void
Anweisung_Add::ausführen(Cpu *cpu)
{
    auto links = operand(0);
    auto rechts = operand(1);

    if (!links.has_value() || !rechts.has_value())
    {
        return;
    }

    uint16_t wert_links = wert(cpu, links.value());
    uint16_t wert_rechts = wert(cpu, rechts.value());
    uint16_t wert = wert_links + wert_rechts;

    cpu->reg(Register::ACU, wert);

    schalter_setzen(cpu, Register::SCHLT);
}

Anweisung_Sub::Anweisung_Sub(Operand *links, Operand *rechts)
    : Anweisung({ links, rechts })
{
}

void
Anweisung_Sub::ausführen(Cpu *cpu)
{
    auto links = operand(0);
    auto rechts = operand(1);

    if (!links.has_value() || !rechts.has_value())
    {
        return;
    }

    uint16_t wert_links = wert(cpu, links.value());
    uint16_t wert_rechts = wert(cpu, rechts.value());
    uint16_t wert = wert_links - wert_rechts;

    cpu->reg(Register::ACU, wert);

    schalter_setzen(cpu, Register::SCHLT);
}

Anweisung_Mul::Anweisung_Mul(Operand *links, Operand *rechts)
    : Anweisung({ links, rechts })
{
}

void
Anweisung_Mul::ausführen(Cpu *cpu)
{
    auto links = operand(0);
    auto rechts = operand(1);

    if (!links.has_value() || !rechts.has_value())
    {
        return;
    }

    uint16_t wert_links = wert(cpu, links.value());
    uint16_t wert_rechts = wert(cpu, rechts.value());
    uint16_t wert = wert_links * wert_rechts;

    cpu->reg(Register::ACU, wert);

    schalter_setzen(cpu, Register::SCHLT);
}

Anweisung_Div::Anweisung_Div(Operand *links, Operand *rechts)
    : Anweisung({ links, rechts })
{
}

void
Anweisung_Div::ausführen(Cpu *cpu)
{
    auto links = operand(0);
    auto rechts = operand(1);

    if (!links.has_value() || !rechts.has_value())
    {
        return;
    }

    uint16_t wert_links = wert(cpu, links.value());
    uint16_t wert_rechts = wert(cpu, rechts.value());

    if (wert_rechts == 0)
    {
        cpu->panik("division durch 0");
    }

    uint16_t wert = wert_links / wert_rechts;

    cpu->reg(Register::ACU, wert);

    schalter_setzen(cpu, Register::SCHLT);
}

Anweisung_Mov::Anweisung_Mov(Operand *ziel, Operand *quelle)
    : Anweisung({ ziel, quelle })
{
}

void
Anweisung_Mov::ausführen(Cpu *cpu)
{
    auto ziel = operand(0);
    auto quelle = operand(1);

    if (!ziel.has_value() || !quelle.has_value())
    {
        return;
    }

    uint16_t w = wert(cpu, quelle.value());

    switch (ziel.value()->art())
    {
        case Operand::Art::Adr:
        {
            if (quelle.value()->größe() == 1)
            {
                cpu->laufwerk()->schreiben1(ziel.value()->als<Operand_Adr *>()->wert(), w);
            }
            else
            {
                cpu->laufwerk()->schreiben2(ziel.value()->als<Operand_Adr *>()->wert(), w);
            }
        } break;

        case Operand::Art::Reg:
        {
            cpu->reg(ziel.value()->als<Operand_Reg *>()->wert(), w);
        } break;

        default:
        {
            cpu->panik("ungültiger zieloperand");
        } break;
    }
}

Anweisung_Push::Anweisung_Push(Operand *quelle)
    : Anweisung({ quelle })
{
}

void
Anweisung_Push::ausführen(Cpu *cpu)
{
    auto quelle = operand(0);

    if (!quelle.has_value())
    {
        return;
    }

    uint16_t w = wert(cpu, quelle.value());

    cpu->push(w);
}

Anweisung_Pop::Anweisung_Pop(Operand *ziel)
    : Anweisung({ ziel })
{
}

void
Anweisung_Pop::ausführen(Cpu *cpu)
{
    auto ziel = operand(0);

    if (!ziel.has_value())
    {
        return;
    }

    uint16_t w = cpu->pop();

    switch (ziel.value()->art())
    {
        case Operand::Art::Reg:
        {
            cpu->reg(ziel.value()->als<Operand_Reg *>()->wert(), w);
        } break;

        case Operand::Art::Adr:
        {
            cpu->laufwerk()->schreiben2(ziel.value()->als<Operand_Adr *>()->wert(), w);
        } break;

        default:
        {
            cpu->panik("ungültiger zieloperand");
        } break;
    }

    cpu->push(w);
}

}

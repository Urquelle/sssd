#include "vm/anweisung.hpp"
#include "vm/cpu.hpp"

#include <numeric>

namespace Sss::Vm {

#define IF_SCHREIBE1(L, A, W) do{if(!(laufwerk)->schreiben1(A,W)){return false;}else{A += 1;return true;}}while(0)
#define IF_SCHREIBE2(L, A, W) do{if(!(laufwerk)->schreiben1(A,W)){return false;}else{A += 2;return true;}}while(0)

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

Anweisung::Anweisung(std::vector<Operand *> operanden)
    : _operanden(std::move(operanden))
{
    // INFO: größe in bytes = opcode(1 byte) + summe der operandengrößen
    _größe = 1 + std::accumulate(
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

uint8_t
Anweisung_Hlt::kodieren(Laufwerk *laufwerk, std::vector<Operand *> operanden, uint16_t adresse)
{
    laufwerk->schreiben1(adresse, OP_HLT);

    return 1;
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

uint8_t
Anweisung_Add::kodieren(Laufwerk *laufwerk, std::vector<Operand *> operanden, uint16_t adresse)
{
    uint16_t adr = adresse;

    if (operanden.size() < 2)
    {
        return 0;
    }

    if (operanden[0]->art() == Operand::Art::Reg)
    {
        if (operanden[1]->art() == Operand::Art::Reg)
        {
            auto reg1 = operanden[0]->als<Operand_Reg *>();
            auto reg2 = operanden[1]->als<Operand_Reg *>();

            IF_SCHREIBE1(laufwerk, adr, OP_SUB_REG_REG);
            IF_SCHREIBE1(laufwerk, adr, (uint8_t) reg1->wert());
            IF_SCHREIBE1(laufwerk, adr, (uint8_t) reg2->wert());
        }
        else if (operanden[1]->art() == Operand::Art::Imm)
        {
            auto reg = operanden[0]->als<Operand_Reg *>();
            auto lit = operanden[1]->als<Operand_Imm *>();

            IF_SCHREIBE1(laufwerk, adr, OP_SUB_REG_LIT);
            IF_SCHREIBE1(laufwerk, adr, (uint8_t) reg->wert());
            IF_SCHREIBE2(laufwerk, adr, lit->wert());
        }
    }
    else if (operanden[0]->art() == Operand::Art::Imm)
    {
        if (operanden[1]->art() == Operand::Art::Reg)
        {
            auto lit = operanden[0]->als<Operand_Imm *>();
            auto reg = operanden[1]->als<Operand_Reg *>();

            IF_SCHREIBE1(laufwerk, adr, OP_SUB_LIT_REG);
            IF_SCHREIBE2(laufwerk, adr, lit->wert());
            IF_SCHREIBE1(laufwerk, adr, (uint8_t) reg->wert());
        }
    }

    return (uint16_t) größe();
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

uint8_t
Anweisung_Sub::kodieren(Laufwerk *laufwerk, std::vector<Operand *> operanden, uint16_t adresse)
{
    auto adr = adresse;

    if (operanden.size() < 2)
    {
        return 0;
    }

    if (operanden[0]->art() == Operand::Art::Reg)
    {
        if (operanden[1]->art() == Operand::Art::Reg)
        {
            auto reg1 = operanden[0]->als<Operand_Reg *>();
            auto reg2 = operanden[1]->als<Operand_Reg *>();

            IF_SCHREIBE1(laufwerk, adr, OP_SUB_REG_REG);
            IF_SCHREIBE1(laufwerk, adr, (uint8_t) reg1->wert());
            IF_SCHREIBE1(laufwerk, adr, (uint8_t) reg2->wert());
        }
        else if (operanden[1]->art() == Operand::Art::Imm)
        {
            auto reg = operanden[0]->als<Operand_Reg *>();
            auto imm = operanden[1]->als<Operand_Imm *>();

            IF_SCHREIBE1(laufwerk, adr, OP_SUB_REG_LIT);
            IF_SCHREIBE1(laufwerk, adr, (uint8_t) reg->wert());
            IF_SCHREIBE2(laufwerk, adr, imm->wert());
        }
    }
    else if (operanden[0]->art() == Operand::Art::Imm)
    {
        if (operanden[1]->art() == Operand::Art::Reg)
        {
            auto imm = operanden[0]->als<Operand_Imm *>();
            auto reg = operanden[1]->als<Operand_Reg *>();

            IF_SCHREIBE1(laufwerk, adr, OP_SUB_LIT_REG);
            IF_SCHREIBE2(laufwerk, adr, imm->wert());
            IF_SCHREIBE1(laufwerk, adr, (uint8_t) reg->wert());
        }
    }

    return (uint16_t) größe();
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

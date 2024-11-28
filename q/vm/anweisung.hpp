#pragma once

#include <vector>
#include <optional>

#include "vm/operand.hpp"

namespace Sss::Vm {

class Cpu;

class Anweisung
{
public:
    Anweisung(std::vector<Operand *> operanden);
    auto operation() const;
    auto operanden() const;
    std::optional<Operand *> operand(int index);
    auto größe() const;
    virtual void ausführen(Cpu *cpu) = 0;
    uint16_t wert(Cpu *cpu, Operand *op);

private:
    size_t _größe;
    std::vector<Operand *> _operanden;
};

class Anweisung_Hlt : public Anweisung
{
public:
    Anweisung_Hlt();

    void ausführen(Cpu *cpu) override;
};

class Anweisung_Add : public Anweisung
{
public:
    Anweisung_Add(Operand *links, Operand *rechts);

    void ausführen(Cpu *cpu) override;
};

class Anweisung_Sub : public Anweisung
{
public:
    Anweisung_Sub(Operand *links, Operand *rechts);

    void ausführen(Cpu *cpu) override;
};

class Anweisung_Mul : public Anweisung
{
public:
    Anweisung_Mul(Operand *links, Operand *rechts);

    void ausführen(Cpu *cpu) override;
};

class Anweisung_Div : public Anweisung
{
public:
    Anweisung_Div(Operand *links, Operand *rechts);

    void ausführen(Cpu *cpu) override;
};

class Anweisung_Mov : public Anweisung
{
public:
    Anweisung_Mov(Operand *ziel, Operand *quelle);

    void ausführen(Cpu *cpu) override;
};

class Anweisung_Push : public Anweisung
{
public:
    Anweisung_Push(Operand *quelle);

    void ausführen(Cpu *cpu) override;
};

class Anweisung_Pop : public Anweisung
{
public:
    Anweisung_Pop(Operand *ziel);

    void ausführen(Cpu *cpu) override;
};

}

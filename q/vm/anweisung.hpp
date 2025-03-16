#pragma once

#include <vector>
#include <optional>
#include <string>

#include "vm/operand.hpp"
#include "vm/laufwerk.hpp"

namespace Sss::Vm {

class Cpu;

#define Op_Liste \
    X(OP_HLT,                 0, "hlt") \
    /* mov */                           \
    X(OP_MOV_REG_LIT,         1, "mov") \
    X(OP_MOV_REG_REG,         2, "mov") \
    X(OP_MOV_REG_ADR,         3, "mov") \
    X(OP_MOV_ADR_REG,         4, "mov") \
    X(OP_MOV_ADR_LIT,         5, "mov") \
    X(OP_MOV_REG_REG_ZGR,     6, "mov") \
    X(OP_MOV_REG_LIT_VER_REG, 7, "mov") \
    /* arithmetik */                    \
    X(OP_ADD_REG_REG,         8, "add") \
    X(OP_ADD_REG_LIT,         9, "add") \
    X(OP_SUB_REG_REG,        10, "sub") \
    X(OP_SUB_REG_LIT,        11, "sub") \
    X(OP_SUB_LIT_REG,        12, "sub") \
    X(OP_INC_REG,            13, "inc") \
    X(OP_DEC_REG,            14, "dec") \
    X(OP_MUL_REG_REG,        15, "mul") \
    X(OP_MUL_REG_LIT,        16, "mul") \
    /* ... */                           \
    X(OP_PSH_LIT,            17, "psh") \
    X(OP_PSH_REG,            18, "psh") \
    X(OP_POP,                19, "pop") \
    /* verzweigung */                   \
    X(OP_JNE_LIT,            20, "jne") \
    X(OP_JNE_REG,            21, "jne") \
    X(OP_JEQ_REG,            22, "jeq") \
    X(OP_JEQ_LIT,            23, "jeq") \
    X(OP_JLT_REG,            24, "jlt") \
    X(OP_JLT_LIT,            25, "jlt") \
    X(OP_JGT_REG,            26, "jgt") \
    X(OP_JGT_LIT,            27, "jgt") \
    X(OP_JLE_REG,            28, "jle") \
    X(OP_JLE_LIT,            29, "jle") \
    X(OP_JGE_REG,            30, "jge") \
    X(OP_JGE_LIT,            31, "jge") \
    /* ... */                           \
    X(OP_CAL_ADR,            32, "cal") \
    X(OP_CAL_REG,            33, "cal") \
    X(OP_RET,                34, "ret") \
    /* shift */                         \
    X(OP_LSF_REG_LIT,        35, "lsf") \
    X(OP_LSF_REG_REG,        36, "lsf") \
    X(OP_RSF_REG_LIT,        37, "rsf") \
    X(OP_RSF_REG_REG,        38, "rsf") \
    /* logik */                         \
    X(OP_AND_REG_LIT,        39, "and") \
    X(OP_AND_REG_REG,        40, "and") \
    X(OP_OR_REG_LIT,         41, "or")  \
    X(OP_OR_REG_REG,         42, "or")  \
    X(OP_XOR_REG_LIT,        43, "xor") \
    X(OP_XOR_REG_REG,        44, "xor") \
    /* ... */                           \
    X(OP_NOT,                45, "not") \
    X(OP_INT,                46, "int") \
    X(OP_RTI,                47, "rti") \
    X(OP_BRK,                48, "brk")

enum Op : std::int8_t
{
    #define X(Name, Wert, ...) Name = Wert,
    Op_Liste
    #undef X

    Z_OPS,
};

class Anweisung
{
public:
    enum Art
    {
        HLT,
        NOP,
        MOV,
        ADD,
        SUB,
        MUL,
        PSH,
        POP,
        JMP,
    };

    Anweisung(std::vector<Operand *> operanden);
    auto operation() const;
    auto operanden() const;
    std::optional<Operand *> operand(int index);
    auto größe() const;
    virtual void ausführen(Cpu *cpu) = 0;
    uint16_t wert(Cpu *cpu, Operand *op);

    virtual uint8_t kodieren(Laufwerk *laufwerk, std::vector<Operand *> operanden, uint16_t adresse) = 0;

private:
    size_t _größe;
    std::vector<Operand *> _operanden;
};

class Anweisung_Hlt : public Anweisung
{
public:
    Anweisung_Hlt();

    void ausführen(Cpu *cpu) override;
    uint8_t kodieren(Laufwerk *laufwerk, std::vector<Operand *> operanden, uint16_t adresse) override;
};

class Anweisung_Add : public Anweisung
{
public:
    Anweisung_Add(Operand *links, Operand *rechts);

    void ausführen(Cpu *cpu) override;
    uint8_t kodieren(Laufwerk *laufwerk, std::vector<Operand *> operanden, uint16_t adresse) override;
};

class Anweisung_Sub : public Anweisung
{
public:
    Anweisung_Sub(Operand *links, Operand *rechts);

    void ausführen(Cpu *cpu) override;
    uint8_t kodieren(Laufwerk *laufwerk, std::vector<Operand *> operanden, uint16_t adresse) override;
};

class Anweisung_Mul : public Anweisung
{
public:
    Anweisung_Mul(Operand *links, Operand *rechts);

    void ausführen(Cpu *cpu) override;
    uint8_t kodieren(Laufwerk *laufwerk, std::vector<Operand *> operanden, uint16_t adresse) override;
};

class Anweisung_Div : public Anweisung
{
public:
    Anweisung_Div(Operand *links, Operand *rechts);

    void ausführen(Cpu *cpu) override;
    uint8_t kodieren(Laufwerk *laufwerk, std::vector<Operand *> operanden, uint16_t adresse) override;
};

class Anweisung_Mov : public Anweisung
{
public:
    Anweisung_Mov(Operand *ziel, Operand *quelle);

    void ausführen(Cpu *cpu) override;
    uint8_t kodieren(Laufwerk *laufwerk, std::vector<Operand *> operanden, uint16_t adresse) override;
};

class Anweisung_Push : public Anweisung
{
public:
    Anweisung_Push(Operand *quelle);

    void ausführen(Cpu *cpu) override;
    uint8_t kodieren(Laufwerk *laufwerk, std::vector<Operand *> operanden, uint16_t adresse) override;
};

class Anweisung_Pop : public Anweisung
{
public:
    Anweisung_Pop(Operand *ziel);

    void ausführen(Cpu *cpu) override;
    uint8_t kodieren(Laufwerk *laufwerk, std::vector<Operand *> operanden, uint16_t adresse) override;
};

class Anweisung_Jmp : public Anweisung
{
public:
    Anweisung_Jmp(std::string markierung);

    void ausführen(Cpu *cpu) override;
    uint8_t kodieren(Laufwerk *laufwerk, std::vector<Operand *> operanden, uint16_t adresse) override;

private:
    std::string _markierung;
};

}

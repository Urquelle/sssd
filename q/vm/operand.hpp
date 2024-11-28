#pragma once

#include <cstdint>

#include "vm/register.hpp"

namespace Sss::Vm {

class Operand
{
public:
    enum class Art
    {
        Reg = 1,
        Imm = 2,
        Adr = 3,
    };

    static Operand * Adr(uint16_t wert);
    static Operand * Reg(uint8_t wert);
    static Operand * Imm(uint16_t wert);

    Operand(Art art);

    Art art() const;
    size_t größe() const;

    template<typename T> T als();

private:
    Art _art;
    size_t _größe;
};

class Operand_Adr : public Operand
{
public:
    Operand_Adr(uint16_t wert);

    uint16_t wert() const;

private:
    uint16_t _wert;
};

class Operand_Imm : public Operand
{
public:
    Operand_Imm(uint16_t wert);

    uint16_t wert() const;

private:
    uint16_t _wert;
};

class Operand_Reg : public Operand
{
public:
    Operand_Reg(Register wert);

    Register wert() const;

private:
    Register _wert;
};

}

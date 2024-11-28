#include "vm/operand.hpp"

namespace Sss::Vm {

Operand::Operand(Operand::Art art)
    : _art(art)
{
    switch (_art)
    {
        case Art::Reg: _größe = 1; break;
        case Art::Adr: _größe = 2; break;
        case Art::Imm: _größe = 2; break;
    }
}

Operand::Art
Operand::art() const
{
    return _art;
}

size_t
Operand::größe() const
{
    return _größe;
}

template<typename T>
T Operand::als()
{
    auto erg = static_cast<T> (this);

    return erg;
}

Operand_Adr::Operand_Adr(uint16_t wert)
    : Operand(Operand::Art::Adr)
    , _wert(wert)
{
}

uint16_t
Operand_Adr::wert() const
{
    return _wert;
}

Operand_Imm::Operand_Imm(uint16_t wert)
    : Operand(Operand::Art::Imm)
    , _wert(wert)
{
}

uint16_t
Operand_Imm::wert() const
{
    return _wert;
}

Operand_Reg::Operand_Reg(Register wert)
    : Operand(Operand::Art::Reg)
    , _wert(wert)
{
}

Register
Operand_Reg::wert() const
{
    return _wert;
}

}

#pragma once

#include <cstdint>

namespace Sss::Vm {

enum class Schalter : uint16_t
{
    Null    = 0,
    Negativ = 1,
    Positiv = 2,
};

enum class Register : uint16_t
{
    // allgemeine register
    R1 = 1, R2 = 2, R3 = 3, R4 = 4,
    R5 = 5, R6 = 6, R7 = 7, R8 = 8,

    // spezialregister
    ACU, IP, SP, FP, SCHLT
};

}

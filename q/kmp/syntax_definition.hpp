#pragma once

namespace Sss {

class Bindung
{

};

enum class Assoziativität
{
    Links  = 1,
    Rechts = 2
};

class Syntax_Regel
{
public:
    Syntax_Regel(Assoziativität assoziativität, Bindung bindung);
};

class Syntax_Definition
{
public:
    Syntax_Definition();
};

}

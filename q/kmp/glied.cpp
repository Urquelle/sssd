#include "glied.hpp"

#include <ostream>

namespace Sss::Kmp {

Glied::Glied(Glied::Art art, Spanne spanne)
    : _art(art)
    , _spanne(spanne)
{
    _text = text_kopieren(spanne);
}

Glied::Art
Glied::art() const
{
    return _art;
}

Spanne
Glied::spanne() const
{
    return _spanne;
}

char *
Glied::text() const
{
    return _text;
}

template<typename T>
T Glied::als()
{
    return static_cast<T> (this);
}

void
Glied::ausgeben(std::ostream &ausgabe)
{
    switch (_art)
    {
    #define X(N, W, B) case N: ausgabe << "\033[1;34m" B "\033[0m"; break;
        Glied_Art
    #undef X
    }

    ausgabe << ": " << _text;
}

Glied *
Glied::Ganzzahl(Spanne spanne, int32_t wert, int32_t basis)
{
    auto *erg = new Glied_Ganzzahl(spanne, wert, basis);

    return erg;
}

Glied *
Glied::Dezimalzahl(Spanne spanne, float wert)
{
    auto *erg = new Glied_Dezimalzahl(spanne, wert);

    return erg;
}

Glied *
Glied::Bezeichner(Spanne spanne)
{
    auto *erg = new Glied(Glied::BEZEICHNER, spanne);

    return erg;
}

Glied *
Glied::Text(Spanne spanne)
{
    auto *erg = new Glied(Glied::TEXT, spanne);

    return erg;
}

char *
Glied::text_kopieren(Spanne spanne)
{
    auto utf8_text = spanne.von().utf8();
    auto text_länge = spanne.länge();

    auto *erg = new char[text_länge + 1];
    memcpy(erg, utf8_text.data(), text_länge);
    erg[text_länge] = 0;

    return erg;
}

Glied_Ganzzahl::Glied_Ganzzahl(Spanne spanne, int32_t wert, int32_t basis)
    : Glied(Glied::GANZZAHL, spanne)
    , _wert(wert)
    , _basis(basis)
{
}

int32_t
Glied_Ganzzahl::wert() const
{
    return _wert;
}

int32_t
Glied_Ganzzahl::basis() const
{
    return _basis;
}

Glied_Dezimalzahl::Glied_Dezimalzahl(Spanne spanne, float wert)
    : Glied(Glied::DEZIMALZAHL, spanne)
    , _wert(wert)
{
}

float
Glied_Dezimalzahl::wert() const
{
    return _wert;
}

}

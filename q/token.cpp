#include "token.hpp"

#include <ostream>

namespace Sss {

Token::Token(Token::Art art, Spanne spanne)
    : _art(art)
    , _spanne(spanne)
{
    _text = text_kopieren(spanne);
}

Token::Art
Token::art() const
{
    return _art;
}

Spanne
Token::spanne() const
{
    return _spanne;
}

char *
Token::text() const
{
    return _text;
}

template<typename T>
T Token::als()
{
    return static_cast<T> (this);
}

void
Token::ausgeben(std::ostream &ausgabe)
{
    switch (_art)
    {
    #define X(N, W, B) case N: ausgabe << "\033[1;34m" B "\033[0m"; break;
        Token_Art
    #undef X
    }

    ausgabe << ": " << _text;
}

Token *
Token::Ganzzahl(Spanne spanne, int32_t wert, int32_t basis)
{
    auto *erg = new Token_Ganzzahl(spanne, wert, basis);

    return erg;
}

Token *
Token::Dezimalzahl(Spanne spanne, float wert)
{
    auto *erg = new Token_Dezimalzahl(spanne, wert);

    return erg;
}

Token *
Token::Bezeichner(Spanne spanne)
{
    auto *erg = new Token(Token::BEZEICHNER, spanne);

    return erg;
}

Token *
Token::Text(Spanne spanne)
{
    auto *erg = new Token(Token::TEXT, spanne);

    return erg;
}

char *
Token::text_kopieren(Spanne spanne)
{
    auto utf8_text = spanne.von().utf8();
    auto text_länge = spanne.länge();

    auto *erg = new char[text_länge + 1];
    memcpy(erg, utf8_text.data(), text_länge);
    erg[text_länge] = 0;

    return erg;
}

Token_Ganzzahl::Token_Ganzzahl(Spanne spanne, int32_t wert, int32_t basis)
    : Token(Token::GANZZAHL, spanne)
    , _wert(wert)
    , _basis(basis)
{
}

int32_t
Token_Ganzzahl::wert() const
{
    return _wert;
}

int32_t
Token_Ganzzahl::basis() const
{
    return _basis;
}

Token_Dezimalzahl::Token_Dezimalzahl(Spanne spanne, float wert)
    : Token(Token::DEZIMALZAHL, spanne)
    , _wert(wert)
{
}

float
Token_Dezimalzahl::wert() const
{
    return _wert;
}

}

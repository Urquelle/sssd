#pragma once

#include <cstdint>
#include <string>
#include <string_view>

class Zeichen
{
public:
    Zeichen();
    Zeichen(const Zeichen& z);
    Zeichen(std::string_view utf8_char, uint32_t versatz, uint8_t byte_länge, std::string& quelldatei, std::string& text);

    std::string text() const;
    std::string_view utf8() const;
    uint32_t codepoint() const;
    bool ist_ascii() const;
    bool ist_letter() const;
    bool ist_emoji() const;
    bool ist_ziffer() const;
    bool ist_zwj() const;
    bool ist_kombo() const;
    uint8_t byte_länge() const;
    uint32_t versatz() const;
    std::string quelldatei() const;

private:
    std::string_view _utf8_char;
    std::string _quelldatei;
    uint32_t _codepoint;
    std::string _text;
    uint32_t _versatz;
    uint8_t _byte_länge;
};

inline bool operator==(const Zeichen& lhs, const Zeichen& rhs);
inline bool operator!=(const Zeichen& lhs, const Zeichen& rhs);

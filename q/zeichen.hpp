#pragma once

#include <cstdint>
#include <string>

class Zeichen
{
public:
    Zeichen();
    Zeichen(const Zeichen& z);
    Zeichen(std::string_view utf8_char, uint32_t byte_index, std::string& q, std::string& text);

    std::string text() const;
    std::string_view utf8() const;
    uint32_t codepoint() const;
    bool ist_ascii() const;
    bool ist_letter() const;
    bool ist_emoji() const;
    bool ist_ziffer() const;
    uint8_t byte_länge() const;

private:
    std::string_view _utf8_char;
    uint32_t _codepoint;
    std::string _text;
    uint8_t _byte_länge;
};

inline bool operator==(const Zeichen& lhs, const Zeichen& rhs);
inline bool operator!=(const Zeichen& lhs, const Zeichen& rhs);


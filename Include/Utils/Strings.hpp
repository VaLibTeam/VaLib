// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <Types/BasicTypedef.hpp>
#include <Types/Error.hpp>

namespace strings {

inline bool isAlpha(char c) { return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'); }
inline bool isSpace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r';
}
inline bool isNumber(char c) { return c >= '0' && c <= '9'; }

inline char numberToChar(int64 num) {
    if (num >= 0 && num <= 9) {
        return static_cast<char>('0' + num);
    }
    throw ValueError("Input number is out of range (0-9)");
}

} // namespace strings
// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <VaLib/Types/BasicTypedef.hpp>
#include <VaLib/Types/Error.hpp>

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

inline VaString escape(const VaString& input) {
    VaString result;
    result.reserve(len(input));
    for (char c : input) {
        switch (c) {
        case '\n': result += "\\n"; break;
        case '\t': result += "\\t"; break;
        case '\r': result += "\\r"; break;
        case '\v': result += "\\v"; break;
        case '\f': result += "\\f"; break;
        case '\\': result += "\\\\"; break;
        case '\"': result += "\\\""; break;
        case '\'': result += "\\\'"; break;
        default: result += c;
        }
    }
    return result;
}

} // namespace strings
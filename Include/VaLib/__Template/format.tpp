// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <VaLib/Types/String.hpp>

#include <VaLib/Utils/Strings.hpp>
#include <VaLib/Utils/ToString.hpp>

#ifdef VaLib_USE_CONCEPTS
#include <VaLib/Types/BasicConcepts.hpp>
#include <VaLib/Types/Stringer.hpp>
#endif

#include <cstdio>
#include <iostream>

namespace va {

inline VaString sprintf(const VaString& format) { return format; }

template <typename T, typename... Args>
VaString sprintf(const VaString& format, T value, Args... args) {
    Size pos = format.find("%");
    if (pos == VaString::npos || pos == len(format) - 1) {
        return format;
    }

    Size specifierPos = pos + 1;
    while (specifierPos < len(format) && !strings::isAlpha(format[specifierPos])) {
        specifierPos++;
    }

    if (specifierPos >= len(format)) {
        return format;
    }

    VaString formatFlags = format.substr(pos + 1, specifierPos - pos - 1);
    char specifier = format[specifierPos];

    VaString before = format.substr(0, pos);
    VaString after = format.substr(specifierPos + 1);
    VaString replacement;

    switch (specifier) {
    case 'd':
        if constexpr (tt::IsIntegral<T>) {
            bool leftJustify = !formatFlags.isEmpty() && formatFlags[0] == '-';
            bool zeroPad = !formatFlags.isEmpty() && formatFlags[0] == '0';

            Size width = 0;
            if (!formatFlags.isEmpty()) {
                Size startIndex = (leftJustify || zeroPad) ? 1 : 0;
                try {
                    width = std::stoi(formatFlags.substr(startIndex).toStdString());
                } catch (...) {
                    width = 0;
                }
            }

            replacement = va::toString(static_cast<int>(value));

            if (width > 0 && len(replacement) < width) {
                Size padSize = width - len(replacement);
                VaString padding(padSize, zeroPad ? '0' : ' ');

                if (leftJustify) {
                    replacement = replacement + padding;
                } else {
                    replacement = padding + replacement;
                }
            }
        } else {
            replacement = "InvalidInt";
        }
        break;
    case 'f':
        if constexpr (tt::IsFloatingPoint<T>) {
            replacement = va::toString((float64)value);
        } else {
            replacement = "InvalidFloat";
        }
        break;
    case 's':
        if constexpr (tt::IsSame<T, const char*> || tt::IsSame<T, char*>) {
            replacement = VaString(value);
        } else if constexpr (tt::IsSame<T, VaString>) {
            replacement = VaString(value);
        } else if constexpr (tt::IsSame<T, VaImmutableString>) {
            replacement = VaString(value);
        #ifdef VaLib_USE_CONCEPTS
        } else if constexpr (VaStringer<T>) {
            replacement = value.toString();
        #endif
        } else {
            replacement = "InvalidStr";
        }
        break;
    case 'q':
        if constexpr (tt::IsSame<T, const char*> || tt::IsSame<T, char*>) {
            replacement = VaString(value);
        } else if constexpr (tt::IsSame<T, VaString>) {
            replacement = VaString(value);
        } else if constexpr (tt::IsSame<T, VaImmutableString>) {
            replacement = VaString(value);
        #ifdef VaLib_USE_CONCEPTS
        } else if constexpr (VaStringer<T>) {
            replacement = value.toString();
        #endif
        } else {
            replacement = "InvalidStr";
        }

        replacement = va::quote(replacement);
        break;
    case 'c':
        if constexpr (tt::IsSame<T, char>) {
            replacement = VaString(1, value);
        } else {
            replacement = "InvalidChar";
        }
        break;
    case 't':
        if constexpr (tt::IsSame<T, bool>) {
            replacement = value ? "true" : "false";
        } else {
            replacement = "InvalidBool";
        }
        break;

    default: replacement = VaString("%") + formatFlags + specifier;
    }

    return before + replacement + sprintf(after, args...);
}

template <typename T, typename... Args>
inline void printf(const VaString& format, T value, Args... args) {
    std::cout << va::sprintf(format, value, args...);
}

inline void printf(VaString format) {
    std::cout << format;
}

template <typename T, typename... Args>
inline void printlnf(const VaString& format, T value, Args... args) {
    std::cout << va::sprintf(format, value, args...) << "\n";
}

inline void printlnf(VaString format) {
    std::cout << format << "\n";
}

inline void printlnf() {
    std::cout << "\n";
}

} // namespace va

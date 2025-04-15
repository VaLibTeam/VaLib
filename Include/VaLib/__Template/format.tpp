// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <VaLib/Types/String.hpp>
#include <VaLib/Utils/Strings.hpp>

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
        if constexpr (std::is_integral<T>::value) {
            bool leftJustify = !formatFlags.isEmpty() && formatFlags[0] == '-';
            bool zeroPad = !formatFlags.isEmpty() && formatFlags[0] == '0';

            Size width = 0;
            if (!formatFlags.isEmpty()) {
                Size startIndex = (leftJustify || zeroPad) ? 1 : 0;
                try {
                    width = std::stoi(formatFlags.substr(startIndex).toCStyleString());
                } catch (...) {
                    width = 0;
                }
            }

            replacement = VaString(std::to_string((int)value));

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
        if constexpr (std::is_floating_point<T>::value) {
            replacement = VaString(std::to_string((float64)value));
        } else {
            replacement = "InvalidFloat";
        }
        break;
    case 's':
        if constexpr (std::is_same<T, const char*>::value || std::is_same<T, char*>::value) {
            replacement = VaString(value);
        } else if constexpr (std::is_same<T, VaString>::value) {
            replacement = VaString(value);
        } else if constexpr (std::is_same<T, VaImmutableString>::value) {
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
        if constexpr (std::is_same<T, const char*>::value || std::is_same<T, char*>::value) {
            replacement = VaString(value);
        } else if constexpr (std::is_same<T, VaString>::value) {
            replacement = VaString(value);
        } else if constexpr (std::is_same<T, VaImmutableString>::value) {
            replacement = VaString(value);
        #ifdef VaLib_USE_CONCEPTS
        } else if constexpr (VaStringer<T>) {
            replacement = value.toString();
        #endif
        } else {
            replacement = "InvalidStr";
        }
        
        replacement = VaString("\"") + strings::escape(replacement) + "\"";
        break;
    case 'c':
        if constexpr (std::is_same<T, char>::value) {
            replacement = VaString(1, value);
        } else {
            replacement = "InvalidChar";
        }
        break;
    case 't':
        if constexpr (std::is_same<T, bool>::value) {
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

} // namespace va
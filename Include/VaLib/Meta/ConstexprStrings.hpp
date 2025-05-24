// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

namespace va {

constexpr bool constexprStrEq(const char* a, const char* b) {
    while (*a && *a == *b) a++, b++;
    return *a == *b;
}

}

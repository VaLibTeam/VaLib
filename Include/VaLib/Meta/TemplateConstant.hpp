// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <type_traits>

template <auto Val>
struct VaTemplateConstant {
    using Type = decltype(Val);
    static constexpr Type value = Val;

    VaTemplateConstant() = default;
    VaTemplateConstant(Type val) {
        static_assert(val == value, "Invalid value");
    }
};

template <auto Val>
class VaIntegeralConstant : public VaTemplateConstant<Val> {
    static_assert(std::is_integral_v<decltype(Val)>, "Val must be integral");
};

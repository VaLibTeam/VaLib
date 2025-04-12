// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <VaLib/Types/BasicTypedef.hpp>

#include <ostream>
#include <string>

#include <VaLib/Types/ImmutableString.hpp>
#include <VaLib/Types/String.hpp>

namespace va {

VaString toString(int64 num) {
    if (num == 0) {
        return VaString("0");
    }

    char tmp[20];
    int index = 0;

    while (num > 0) {
        tmp[index++] = (num % 10) + '0';
        num /= 10;
    }

    VaString result;
    for (int i = index - 1; i >= 0; i--) {
        result += tmp[i];
    }

    return result;
}

VaString toString(VaImmutableString str) { return VaString(str); }

} // namespace va

// #ifdef VaLib_USE_CONCEPTS
#include <concepts>

template <typename T>
concept StdStringer = requires(T t) {
    { t.toStdString() } -> std::convertible_to<VaString>;
};

template <typename T>
concept Stringer = requires(T t) {
    { t.toString() } -> std::convertible_to<VaString>;
};

template <typename T>
concept ImmutableStringer = requires(T t) {
    { t.toImmutableString() } -> std::same_as<VaImmutableString>;
};

template <StdStringer T>
std::ostream& operator<<(std::ostream& os, T v) {
    os << v.toStdString();
    return os;
}

template <Stringer T>
std::ostream& operator<<(std::ostream& os, T v) {
    os << v.toString();
    return os;
}

template <ImmutableStringer T>
std::ostream& operator<<(std::ostream& os, T v) {
    os << v.toImmutableString().toStdString();
}

template <typename T>
concept ConvertibleToString = Stringer<T> || requires(T t) {
    { va::toString(t) } -> std::convertible_to<VaString>;
};

// #endif
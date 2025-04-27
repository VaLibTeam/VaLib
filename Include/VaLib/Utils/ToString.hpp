// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#ifdef VaLib_USE_CONCEPTS
#include <VaLib/Types/BasicConcepts.hpp>
#include <VaLib/Types/Stringer.hpp>
#endif

#include <VaLib/Types/BasicTypedef.hpp>

#include <VaLib/Types/ImmutableString.hpp>
#include <VaLib/Types/String.hpp>

#include <VaLib/Types/List.hpp>
#include <VaLib/Types/Tuple.hpp>

#include <vector>
#include <string>

namespace va {

VaString toString(int64 num);
inline VaString toString(int32 num) { return toString((int64)num); }
inline VaString toString(int16 num) { return toString((int64)num); }
inline VaString toString(int8 num) { return toString((int64)num); }

VaString toString(uint64 num);
inline VaString toString(uint32 num) { return toString((uint64)num); }
inline VaString toString(uint16 num) { return toString((uint64)num); }
inline VaString toString(uint8 num) { return toString((uint64)num); }

constexpr int AUTO_PRECISION = -1;
VaString toString(float64 num, int precision = AUTO_PRECISION);
inline VaString toString(float32 num, int precision = AUTO_PRECISION) {
    return toString((float64)num, precision);
}

inline VaString toString(VaList<VaString> lst) {
    return "[" + lst.join(", ") + "]";
}

inline VaString toString(std::vector<VaString> lst) {
    if (lst.size() == 0) return "[]";

    VaString result = lst[0];
    for (Size i = 1; i < lst.size(); ++i) {
        result += ", " + lst[i];
    }

    return "[" + result + "]";
}

template <typename T>
inline VaString toString(VaList<T> lst) {
    if (len(lst) == 0) return "[]";

    VaString result = toString(lst[0]);
    for (Size i = 1; i < len(lst); ++i) {
        result += ", " + toString(lst[i]);
    }

    return "[" + result + "]";
}

template <typename T>
inline VaString toString(std::vector<T> lst) {
    if (lst.size() == 0) return "[]";

    VaString result = toString(lst[0]);
    for (Size i = 1; i < lst.size(); ++i) {
        result += ", " + toString(lst[i]);
    }

    return "[" + result + "]";
}

template <typename... Ts>
inline VaString toString(VaTuple<Ts...> tp) {
    VaString result = toString(tp[0]);
    tp.forEach([&](const auto& elm) {
        result += ", " + toString(elm);
    });

    return "(" + result + ")";
}

inline VaString toString(VaImmutableString str) { return VaString(str); }
inline VaString toString(std::string str) { return VaString(str); }
inline VaString toString(const char* str) { return VaString(str); }

} // namespace va

#ifdef VaLib_USE_CONCEPTS
#include <concepts>

template <StdStringer T>
std::ostream& operator<<(std::ostream& os, T v) {
    os << v.toStdString();
    return os;
}

template <VaStringer T>
std::ostream& operator<<(std::ostream& os, T v) {
    os << v.toString();
    return os;
}

template <VaImmutableStringer T>
std::ostream& operator<<(std::ostream& os, T v) {
    os << v.toImmutableString().toStdString();
}

template <typename T>
concept ConvertibleToString = VaStringer<T> || requires(T t) {
    { va::toString(t) } -> std::convertible_to<VaString>;
};

#endif

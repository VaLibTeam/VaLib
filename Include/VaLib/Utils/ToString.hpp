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
#include <VaLib/Utils/Strings.hpp>

#include <VaLib/Types/List.hpp>
#include <VaLib/Types/Tuple.hpp>

#include <vector>
#include <string>

namespace va {

#ifdef VaLib_USE_CONCEPTS

template <typename T>
VaString toString(T value) {
    if constexpr (VaStringer<T>) {
        return value.toString();
    } else if constexpr (StdStringer<T>) {
        return VaString(value.toStdString());
    } else if constexpr (VaImmutableStringer<T>) {
        return VaString(value.toImmutableString());
    } else {
        static_assert(sizeof(T) == 0, "Unsupported type for toString");
    }
}

#endif

inline VaString quote(const VaString& str) {
    return "\"" + strings::escape(str) + "\"";
}

inline VaString toString(const VaString& str, bool needQuote = false) {
    return needQuote ? quote(str) : str;
}
inline VaString toString(const VaImmutableString& str, bool needQuote = false) {
    return needQuote ? quote(str) : VaString(str);
}
inline VaString toString(const std::string& str, bool needQuote = false) {
    return needQuote ? quote(VaString(str)) : VaString(str);
}
inline VaString toString(const char* str, bool needQuote = false) {
    return needQuote ? quote(VaString(str)) : VaString(str);
}

inline VaString toString(char ch, bool needQuote = false) {
    return needQuote ? "\'" + VaString(ch) + '\'' : VaString(ch);
}

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
    VaString result;
    tp.forEachIndexed([&](auto index, const auto& elm) {
        if (index > 0) {
            result += ", ";
        }

        result += toString(elm);
    });

    return "(" + result + ")";
}

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

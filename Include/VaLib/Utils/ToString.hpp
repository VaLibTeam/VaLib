// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#ifdef VaLib_USE_CONCEPTS
#include <VaLib/Types/BasicConcepts.hpp>
#endif

#include <VaLib/Types/BasicTypedef.hpp>
#include <VaLib/Types/Stringer.hpp>

#include <ostream>
#include <string>

#include <VaLib/Types/ImmutableString.hpp>
#include <VaLib/Types/String.hpp>

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
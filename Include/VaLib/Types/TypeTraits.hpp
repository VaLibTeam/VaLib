// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once
#include <type_traits>

using TrueType = std::true_type;
using FalseType = std::false_type;

namespace va {

// --- Has Less Than
namespace detail {
template<typename T, typename = decltype(std::declval<T>() < std::declval<T>())>
TrueType HasLessThanHelper(int);

template<typename T>
FalseType HasLessThanHelper(...);
}

template<typename T>
struct HasLessThan: decltype(detail::HasLessThanHelper<T>(0)) {};

template<typename T>
constexpr bool HasLessThanV = HasLessThan<T>::value;

// --- Has Greater Than
namespace detail {
template<typename T, typename = decltype(std::declval<T>() > std::declval<T>())>
TrueType HasGreaterThanHelper(int);

template<typename T>
FalseType HasGreaterThanHelper(...);
}

template<typename T>
struct HasGreaterThan: decltype(detail::HasGreaterThanHelper<T>(0)) {};

template<typename T>
constexpr bool HasGreaterThanV = HasGreaterThan<T>::value;

// --- Has Equality Operator
template <typename T, typename = void>
struct HasEqualityOperator: FalseType {};

template <typename T>
struct HasEqualityOperator<T, std::void_t<decltype(std::declval<T>() == std::declval<T>())>>: TrueType {};

template <typename T>
inline constexpr bool HasEqualityOperatorV = HasEqualityOperator<T>::value;

// --- Has Inequality Operator
template <typename T, typename = void>
struct HasInequalityOperator: FalseType {};

template <typename T>
struct HasInequalityOperator<T, std::void_t<decltype(std::declval<T>() != std::declval<T>())>>
    : TrueType {};

template <typename T>
inline constexpr bool HasInequalityOperatorV = HasInequalityOperator<T>::value;

}

// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <type_traits>

namespace va {

// --- Has Less Than
namespace detail {
template<typename T, typename = decltype(std::declval<T>() < std::declval<T>())>
std::true_type HasLessThanHelper(int);

template<typename T>
std::false_type HasLessThanHelper(...);
}

template<typename T>
struct HasLessThan : decltype(detail::HasLessThanHelper<T>(0)) {};

template<typename T>
constexpr bool HasLessThanV = HasLessThan<T>::value;

// --- Has Greater Than
namespace detail {
template<typename T, typename = decltype(std::declval<T>() > std::declval<T>())>
std::true_type HasGreaterThanHelper(int);

template<typename T>
std::false_type HasGreaterThanHelper(...);
}

template<typename T>
struct HasGreaterThan : decltype(detail::HasGreaterThanHelper<T>(0)) {};

template<typename T>
constexpr bool HasGreaterThanV = HasGreaterThan<T>::value;

// --- Has Equality Operator
template <typename T, typename = void>
struct HasEqualityOperator: std::false_type {};

template <typename T>
struct HasEqualityOperator<T, std::void_t<decltype(std::declval<T>() == std::declval<T>())>>
    : std::true_type {};

template <typename T>
inline constexpr bool HasEqualityOperatorV = HasEqualityOperator<T>::value;

// --- Has Inequality Operator
template <typename T, typename = void>
struct HasInequalityOperator: std::false_type {};

template <typename T>
struct HasInequalityOperator<T, std::void_t<decltype(std::declval<T>() != std::declval<T>())>>
    : std::true_type {};

template <typename T>
inline constexpr bool HasInequalityOperatorV = HasInequalityOperator<T>::value;

}

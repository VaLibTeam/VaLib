// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#ifdef VaLib_USE_CONCEPTS
#include <VaLib/Types/BasicConcepts.hpp>
#include <concepts>
#endif

#include <VaLib/Meta/BasicDefine.hpp>
#include <VaLib/Types/String.hpp>

#include <functional>

TODO(maqi-x, "Implement VaHash without using std::hash");
template <typename T>
struct VaHash: std::hash<T> {};

template <>
struct VaHash<VaString> {
    Size operator()(const VaString& str) const { return str.hash(); }
};

#ifdef VaLib_USE_CONCEPTS
template <typename T>
concept HasHashMethod = requires(T t) {
    { t.hash() } -> std::convertible_to<std::size_t>;
};

template <HasHashMethod T>
struct VaHash<T> {
    Size operator()(const T& t) const noexcept {
        return t.hash();
    }
};

#elif __cplusplus >= CPP20
template <typename T, typename = void>
struct hasHashMethod : FalseType {};

template <typename T>
struct hasHashMethod<T, std::void_t<decltype(std::declval<T>().hash())>>: TrueType {};

template <typename T>
constexpr bool hasHashMethodV = hasHashMethod<T>::value;

template <typename T>
requires hasHashMethodV<T>
struct VaHash<T> {
    Size operator()(const T& value) { return value.hash(); }
};
#endif

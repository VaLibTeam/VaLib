// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <Types/List.hpp>
#include <Types/Pair.hpp>
#include <Types/Stack.hpp>
#include <Types/Tuple.hpp>

namespace va {

template <typename... Args>
constexpr auto mkTuple(Args&&... args) {
    return VaTuple<Args...>::Make(args...);
}

template <Size N>
constexpr auto mkString(const char (&str)[N]) {
    return VaString::Make<N>();
}

template <Size N>
constexpr auto mkImmutableString(const char (&str)[N]) {
    return VaImmutableString::Make<N>();
}

template <typename T1, typename T2>
constexpr auto mkPair(T1 first, T2 second) {
    return VaPair<T1, T2>(first, second);
}

template <typename T, typename... Args,
          typename = std::enable_if_t<(std::is_constructible_v<T, Args> && ...)>>
constexpr auto mkList(Args... args) {
    return VaList<Args...>(args...);
}

} // namespace va
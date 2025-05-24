// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <VaLib/Types/ImmutableString.hpp>
#include <VaLib/Types/List.hpp>
#include <VaLib/Types/Pair.hpp>
#include <VaLib/Types/String.hpp>
#include <VaLib/Types/Tuple.hpp>

#include <VaLib/FuncTools/Func.hpp>

namespace va {

template<typename... Args>
auto mkTuple(Args&&... args) {
    return VaTuple<std::decay_t<Args>...>(std::forward<Args>(args)...);
}

template <Size N>
inline auto mkString(const char (&str)[N]) {
    return VaString::Make<N>(str);
}

template <Size N>
inline auto mkImmutableString(const char (&str)[N]) {
    return VaImmutableString::Make<N>(str);
}

template <typename T1, typename T2>
inline auto mkPair(T1 first, T2 second) {
    return VaPair<T1, T2>(first, second);
}

#if __cplusplus >= CPP17
    template <
        typename T, typename... Args,
        typename = tt::EnableIf<(std::is_constructible_v<T, Args> && ...)>
    >
    inline auto mkList(Args... args) {
        return VaList<Args...>(args...);
    }
#endif

template <typename R, typename... Args>
inline auto mkFunc(R (*func)(Args...)) {
    return VaFunc<R(Args...)>(func);
}

} // namespace va

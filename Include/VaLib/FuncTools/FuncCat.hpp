// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <VaLib/Types/BasicTypedef.hpp>
#include <VaLib/Types/Tuple.hpp>
#include <VaLib/Utils/Make.hpp>

#include <functional>

namespace va {

namespace detail {

/// @brief select first N args from tuple
template<Size... Is, typename Tuple>
auto tuple_take(Tuple&& tpl, std::index_sequence<Is...>) {
    return std::make_tuple(std::get<Is>(std::forward<Tuple>(tpl))...);
}

/// @brief drop first N args from tuple
template<Size Offset, Size... Is, typename Tuple>
auto tupleDrop(Tuple&& tpl, std::index_sequence<Is...>) {
    return std::make_tuple(std::get<Offset + Is>(std::forward<Tuple>(tpl))...);
}

// get arity of a function pointer
template<typename T>
struct FuncTraits;

template<typename R, typename... Args>
struct FuncTraits<R(*)(Args...)> {
    static constexpr Size arity = sizeof...(Args);
};

}

// Main fncat
template<typename F1, typename F2>
auto fncat(F1 f1, F2 f2) {
    constexpr Size N1 = detail::FuncTraits<F1>::arity;
    constexpr Size N2 = detail::FuncTraits<F2>::arity;

    return [f1, f2](auto&&... allArgs) {
        static_assert(sizeof...(allArgs) == N1 + N2, "Wrong number of arguments");

        auto all = std::make_tuple(std::forward<decltype(allArgs)>(allArgs)...);

        auto args1 = tuple_take(all, std::make_index_sequence<N1>{});
        auto args2 = detail::tupleDrop<N1>(all, std::make_index_sequence<N2>{});

        auto r1 = std::apply(f1, args1);
        auto r2 = std::apply(f2, args2);

        return std::make_tuple(r1, r2);
    };
}

}

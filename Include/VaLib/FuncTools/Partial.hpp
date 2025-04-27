// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <VaLib/Types/Tuple.hpp>
#include <VaLib/Utils/Make.hpp>

template <typename Func, typename... BoundArgs>
class VaPartial {
  protected:
    Func func;
    VaTuple<BoundArgs...> boundArgs;

  public:
    VaPartial(Func f, BoundArgs... args) : func(f), boundArgs(va::mkTuple(args...)) {}

    template <typename... CallArgs>
    auto operator()(CallArgs... args) {
        return va::apply(func, boundArgs + va::mkTuple(args...));
    }

    template <typename... CallArgs>
    auto operator()(CallArgs... args) const {
        return va::apply(func, boundArgs + va::mkTuple(args...));
    }
};

namespace va {

template <typename Func, typename... BoundArgs>
auto partial(Func func, BoundArgs... args) {
    return [func, ...boundArgs = args](auto&&... callArgs) {
        return func(boundArgs..., std::forward<decltype(callArgs)>(callArgs)...);
    };
}

} // namespace va

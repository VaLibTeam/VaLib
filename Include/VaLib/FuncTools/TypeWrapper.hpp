// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <VaLib/Types/BasicTypedef.hpp>
#include <VaLib/FuncTools/Func.hpp>

#if __cplusplus >= CPP17

template <auto F>
struct VaTypeWrapper {
    template <typename... Args>
    decltype(auto) operator()(Args&&... args) const {
        return F(std::forward<Args>(args)...);
    }

    template <typename... Args>
    decltype(auto) call(Args&&... args) const {
        return F(std::forward<Args>(args)...);
    }
};

namespace va {

template<typename T>
struct UnwrapHelper {
    static constexpr auto get() {
        return [](auto&&... args) -> decltype(auto) {
            return T{}(std::forward<decltype(args)>(args)...);
        };
    }
};

template<auto F>
struct UnwrapHelper<VaTypeWrapper<F>> {
    static constexpr auto get() {
        return F;
    }
};

template<typename T>
constexpr auto unwrapFunc() {
    return UnwrapHelper<T>::get();
}

}

#endif

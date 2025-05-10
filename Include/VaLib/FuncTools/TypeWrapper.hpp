// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <VaLib/Types/BasicTypedef.hpp>
#include <VaLib/FuncTools/Func.hpp>

#if __cplusplus >= CPP17

template<auto F>
struct VaTypeWrapper {
    template<typename... Args>
    decltype(auto) operator()(Args&&... args) const {
        return F(std::forward<Args>(args)...);
    }

    template<typename... Args>
    decltype(auto) call(Args&&... args) const {
        return F(std::forward<Args>(args)...);
    }
};

#endif

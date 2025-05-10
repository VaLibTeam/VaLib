// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <VaLib/FuncTools/Func.hpp>
#include <cstdio>

template <typename Cls, typename Signature>
class VaMethod;

template <typename Cls, typename R, typename... Args>
class VaMethod<Cls, R(Args...)> {
  protected:
    using MethodPtr = R (Cls::*)(Args...);
    MethodPtr method;

  public:
    VaMethod(MethodPtr m) : method(m) {}

    R call(Cls& obj, Args... args) const {
        return (obj.*method)(std::forward<Args>(args)...);
    }

    R operator()(Cls& obj, Args... args) const {
        return (obj.*method)(std::forward<Args>(args)...);
    }

    VaFunc<R(Args...)> bind(Cls& obj) const {
        return [&obj, m = method](Args... args) -> R {
            return (obj.*m)(std::forward<Args>(args)...);
        };
    }
};

template <typename Cls, typename R, typename... Args>
class VaMethod<const Cls, R(Args...)> {
  protected:
    using MethodPtr = R (Cls::*)(Args...) const;
    MethodPtr method;

  public:
    VaMethod(MethodPtr m) : method(m) {}

    R call(const Cls& obj, Args... args) const {
        return (obj.*method)(std::forward<Args>(args)...);
    }

    R operator()(const Cls& obj, Args... args) const {
        return (obj.*method)(std::forward<Args>(args)...);
    }

    VaFunc<R(Args...)> bind(const Cls& obj) const {
        return [&obj, m = method](Args... args) -> R {
            return (obj.*m)(std::forward<Args>(args)...);
        };
    }
};

namespace va {

template <typename Cls, typename R, typename... Args>
VaFunc<R(Args...)> bindMethod(VaMethod<Cls, R(Args...)> method, Cls& obj) {
    return method.bind(obj);
}

template <typename Cls, typename R, typename... Args>
VaFunc<R(Args...)> bindMethod(VaMethod<const Cls, R(Args...)> method, const Cls& obj) {
    return method.bind(obj);
}

}

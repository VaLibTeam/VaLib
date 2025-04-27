// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <VaLib/Types/Tuple.hpp>

template <typename T>
inline constexpr std::type_identity<T> Type{};

template <typename... Types>
struct VaBox {
    VaTuple<Types...> values{};

    template <typename T>
    VaBox& operator=(T&& v) {
        // Find the matching type in tuple and assign
        std::get<std::remove_cv_t<std::remove_reference_t<T>>>(values) = std::forward<T>(v);
        return *this;
    }

    template <typename T>
    operator T() const {
        return values.template get<T>();
    }

    template <typename T>
    T& get() {
        return values.template get<T>();
    }

    template <typename T>
    const T& get() const {
        return values.template get<T>();
    }

    template <typename T>
    T& operator[](std::type_identity<T>) {
        return values.template get<T>();
    }

    template <typename T>
    const T& operator[](std::type_identity<T>) const {
        return values.template get<T>();
    }
};

// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <VaLib/Types/TypeBox.hpp>
#include <VaLib/Types/Tuple.hpp>

template <typename T>
inline constexpr VaTypeBox<T> Type{};

template <typename... Types>
struct VaBox {
    VaTuple<Types...> values{};

    template <typename T>
    VaBox& operator=(T&& v) {
        // Find the matching type in tuple and assign
        std::get< typename std::remove_cv< typename std::remove_reference<T>::type >::type >(values)
            = std::forward<T>(v);

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
    T& operator[](VaTypeBox<T>) {
        return values.template get<T>();
    }

    template <typename T>
    const T& operator[](VaTypeBox<T>) const {
        return values.template get<T>();
    }
};

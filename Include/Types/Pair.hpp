// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <Types/BasicTypedef.hpp>

#include <cstddef>
#include <utility>

template <typename T1, typename T2>
class VaPair {
  public:
    T1 first;
    T2 second;

    VaPair() noexcept = default;
    VaPair(T1 a1, T2 a2) noexcept : first(a1), second(a2) {}
    VaPair(std::pair<T1, T2> p) noexcept : first(p.first), second(p.second) {}

    inline bool operator==(const VaPair<T1, T2>& other) const noexcept {
        return first == other.first && second == other.second;
    }

    inline bool operator!=(const VaPair<T1, T2>& other) const noexcept { return !(*this == other); }

    template <typename U1 = T1, typename U2 = T2,
              typename = std::enable_if_t<std::is_same_v<U1, U2>>>
    inline void swap() noexcept {
        std::swap(first, second);
    }

    template <int N>
    auto& get() noexcept {
        static_assert(N >= 0 && N < 2, "VaPair::get: invalid argument");
        if constexpr (N == 0)
            return first;
        else if constexpr (N == 1)
            return second;
    }

    template <int N>
    const auto& get() const noexcept {
        static_assert(N >= 0 && N < 2, "VaPair::get: invalid argument");
        if constexpr (N == 0)
            return first;
        else if constexpr (N == 1)
            return second;
    }
};

namespace std {

template <typename T1, typename T2>
struct tuple_size<VaPair<T1, T2>>: integral_constant<Size, 2> {};

template <typename T1, typename T2>
struct tuple_element<0, VaPair<T1, T2>> {
    using type = T1;
};

template <typename T1, typename T2>
struct tuple_element<1, VaPair<T1, T2>> {
    using type = T2;
};

} // namespace std

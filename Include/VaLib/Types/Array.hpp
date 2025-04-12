// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <VaLib/Types/List.hpp>
#include <VaLib/Types/BasicTypedef.hpp>
#include <VaLib/Types/Error.hpp>

namespace va::detail {
    template <typename T, Size N>
    class __BasicArray {
        static_assert(N > 0, "Size must be greater than 0");
    
      protected:
        T data[N];
    
      public:
        template <typename... Args, typename = std::enable_if_t<sizeof...(Args) == N>>
        constexpr __BasicArray(Args&&... args) : data{std::forward<Args>(args)...} {}

        // Element access
        constexpr T& operator[](Size index) noexcept { return data[index]; }
        constexpr const T& operator[](Size index) const noexcept { return data[index]; }
    
        T& at(Size index) {
            if (index >= N) throw IndexOutOfRangeError(N, index);
            return data[index];
        }
    
        const T& at(Size index) const {
            if (index >= N) throw IndexOutOfRangeError(N, index);
            return data[index];
        }
    
        template <Size L>
        __BasicArray<T, N + L> operator+(const __BasicArray<T, L>& other) const {
            __BasicArray<T[N + L], 0> result;
            std::copy(begin(), end(), result.begin());
            std::copy(other.begin(), other.end(), result.begin() + N);
            return result;
        }
    
        constexpr T* begin() noexcept { return data; }
        constexpr T* end() noexcept { return data + N; }
        constexpr const T* begin() const noexcept { return data; }
        constexpr const T* end() const noexcept { return data + N; }
    
        constexpr Size size() const noexcept { return N; }
    
        constexpr T* dataPtr() noexcept { return data; }
        constexpr const T* dataPtr() const noexcept { return data; }
    
        constexpr T& front() noexcept { return data[0]; }
        constexpr T& back() noexcept { return data[N - 1]; }
        constexpr const T& front() const noexcept { return data[0]; }
        constexpr const T& back() const noexcept { return data[N - 1]; }
    
        constexpr void fill(const T& value) {
            for (Size i = 0; i < N; i++) data[i] = value;
        }
    
        constexpr void swap(__BasicArray& other) noexcept {
            for (Size i = 0; i < N; i++) {
                T tmp = std::move(data[i]);
                data[i] = std::move(other.data[i]);
                other.data[i] = std::move(tmp);
            }
        }
    
        // Structured bindings support
        template <Size I>
        constexpr T& get() noexcept {
            static_assert(I < N, "Index out of bounds");
            return data[I];
        }
    
        template <Size I>
        constexpr const T& get() const noexcept {
            static_assert(I < N, "Index out of bounds");
            return data[I];
        }
    
        // Comparisons
        friend bool operator==(const __BasicArray& lhs, const __BasicArray& rhs) {
            if constexpr (std::is_trivially_copyable_v<T>) {
                return std::memcmp(lhs.data, rhs.data, N * sizeof(T)) == 0;
            } else {
                for (Size i = 0; i < N; i++) {
                    if (lhs[i] != rhs[i]) return false;
                }
                return true;
            }
        }
    
        friend inline bool operator!=(const __BasicArray& lhs, const __BasicArray& rhs) { return !(lhs == rhs); }

        friend bool operator==(const __BasicArray<T, N>& lhs, const VaList<T>& rhs) {
            if (N != len(rhs)) {
                
            }
        }
    };
}

template <typename T, Size N = 0>
class VaArray;

template <typename T, Size N>
class VaArray: public va::detail::__BasicArray<T, N> {
  public:
    using va::detail::__BasicArray<T, N>::__BasicArray; // Inherit constructors

    friend inline Size len(const VaArray& arr) {
        return N;
    }
};

template <typename T, Size N>
class VaArray<T[N], 0>: public va::detail::__BasicArray<T, N> {
  public:
    using va::detail::__BasicArray<T, N>::__BasicArray; // Inherit constructors

    friend inline Size len(const VaArray& arr) {
        return N;
    }
};

namespace std {

// for structured bindings
template <typename T, Size N>
struct tuple_size<VaArray<T[N]>>: std::integral_constant<Size, N> {};

template <Size I, typename T, Size N>
struct tuple_element<I, VaArray<T[N], 0>> {
    using type = T;
};

}
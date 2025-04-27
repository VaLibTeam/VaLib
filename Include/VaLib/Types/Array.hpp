// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include "VaLib/Types/TypeTraits.hpp"
#include <VaLib/Types/BasicTypedef.hpp>
#include <VaLib/Types/Error.hpp>
#include <VaLib/Types/List.hpp>

#include <compare>
#include <type_traits>

namespace va::detail {
/**
 * @brief Basic array implementation
 * @tparam T Type of elements stored in array
 * @tparam N Size of array
 *
 * @note This is an internal implementation class, use VaArray in public API
 * @warning The class name starts with double underscore indicating it's implementation detail
 */
template <typename T, Size N>
class __BasicArray {
    static_assert(N > 0, "Size must be greater than 0");

  protected:
    T data[N];

  public:
    /**
     * @brief Constructs array with given arguments
     * @tparam Args Types of arguments
     * @param args Arguments to initialize array elements
     *
     * @note Number of arguments must exactly match array size
     * @warning Enabled only when sizeof...(Args) == N
     */
    template <typename... Args, typename = std::enable_if_t<sizeof...(Args) == N>>
    constexpr __BasicArray(Args&&... args) : data{std::forward<Args>(args)...} {}

    /**
     * @brief Accesses element without bounds checking
     * @param index Index of element to access
     * @return Reference to requested element
     *
     * @warning No bounds checking performed
     */
    constexpr T& operator[](Size index) noexcept { return data[index]; }

    /**
     * @brief Accesses element without bounds checking (const version)
     * @param index Index of element to access
     * @return Const reference to requested element
     *
     * @warning No bounds checking performed
     */
    constexpr const T& operator[](Size index) const noexcept { return data[index]; }

    /**
     * @brief Accesses element with bounds checking
     * @param index Index of element to access
     * @return Reference to requested element
     *
     * @throw IndexOutOfRangeError if index >= N
     * @note Safer but slower than operator[]
     */
    T& at(Size index) {
        if (index >= N) throw IndexOutOfRangeError(N, index);
        return data[index];
    }

    /**
     * @brief Accesses element with bounds checking (const version)
     * @param index Index of element to access
     * @return Const reference to requested element
     *
     * @throw IndexOutOfRangeError if index >= N
     * @note Safer but slower than operator[]
     */
    const T& at(Size index) const {
        if (index >= N) throw IndexOutOfRangeError(N, index);
        return data[index];
    }

    /**
     * @brief Concatenates two arrays
     * @tparam L Size of second array
     * @param other Array to concatenate with
     * @return New array containing elements from both arrays
     *
     * @note Resulting array size is N + L
     * @warning May be expensive for large arrays
     */
    template <Size L>
    __BasicArray<T, N + L> operator+(const __BasicArray<T, L>& other) const {
        __BasicArray<T[N + L], 0> result;
        std::copy(this->begin(), this->end(), result.begin());
        std::copy(other.begin(), other.end(), result.begin() + N);
        return result;
    }

    /**
     * @brief Returns pointer to underlying data
     * @return Pointer to data array
     *
     * @note Useful for interoperability with C APIs
     */
    constexpr T* dataPtr() noexcept { return data; }

    /**
     * @brief Returns pointer to underlying data (const version)
     * @return Const pointer to data array
     */
    constexpr const T* dataPtr() const noexcept { return data; }

    /**
     * @brief Accesses first element
     * @return Reference to first element
     */
    constexpr T& front() noexcept { return data[0]; }

    /**
     * @brief Accesses last element
     * @return Reference to last element
     */
    constexpr T& back() noexcept { return data[N - 1]; }

    /**
     * @brief Accesses first element (const version)
     * @return Const reference to first element
     */
    constexpr const T& front() const noexcept { return data[0]; }

    /**
     * @brief Accesses last element (const version)
     * @return Const reference to last element
     */
    constexpr const T& back() const noexcept { return data[N - 1]; }

    /**
     * @brief Fills array with given value
     * @param value Value to fill array with
     *
     * @note Overwrites all existing elements
     */
    constexpr void fill(const T& value) {
        if constexpr (std::is_trivially_copyable_v<T>) {
            std::memset(data, value, sizeof(T) * N);
        } else {
            for (Size i = 0; i < N; i++) {
                data[i] = value;
            }
        }
    }

    /**
     * @brief Swaps contents with another array
     * @param other Array to swap with
     *
     * @note Performs element-wise swap
     * @warning May be expensive for non-trivial types
     */
    constexpr void swap(__BasicArray& other) noexcept {
        for (Size i = 0; i < N; i++) {
            T tmp = std::move(data[i]);
            data[i] = std::move(other.data[i]);
            other.data[i] = std::move(tmp);
        }
    }

    /**
     * @brief Gets element at compile-time index
     * @tparam I Index to access
     * @return Reference to element at index I
     *
     * @note Enables structured bindings support
     * @warning Compile-time checked, fails if I >= N
     */
    template <Size I>
    constexpr T& get() noexcept {
        static_assert(I < N, "Index out of bounds");
        return data[I];
    }

    /**
     * @brief Gets element at compile-time index (const version)
     * @tparam I Index to access
     * @return Const reference to element at index I
     */
    template <Size I>
    constexpr const T& get() const noexcept {
        static_assert(I < N, "Index out of bounds");
        return data[I];
    }

  public operators:
    /**
     * @brief Equality comparison
     * @param lhs First array to compare
     * @param rhs Second array to compare
     * @return true if all elements are equal
     *
     * @note Uses memcmp for trivially copyable types for optimization
     */
    friend bool operator==(const __BasicArray& lhs, const __BasicArray& rhs) {
        if constexpr (!va::HasEqualityOperatorV<T> && !va::HasInequalityOperatorV<T>) {
            for (Size i = 0; i < N; i++) {
                if (!(lhs[i] == rhs[i])) return false;
            }
            return true;
        } else if constexpr (va::HasInequalityOperatorV<T>) {
            for (Size i = 0; i < N; i++) {
                if (lhs[i] != rhs[i]) return false;
            }
            return true;
        } else {
            return std::memcmp(lhs.data, rhs.data, N * sizeof(T)) == 0;
        }
    }

    /**
     * @brief Inequality comparison
     * @param lhs First array to compare
     * @param rhs Second array to compare
     * @return true if any elements differ
     */
    friend inline bool operator!=(const __BasicArray& lhs, const __BasicArray& rhs) {
        return !(lhs == rhs);
    }

    /**
     * @brief Comparison with VaList
     * @param lhs Array to compare
     * @param rhs VaList to compare
     * @return true if equal
     */
    friend bool operator==(const __BasicArray<T, N>& lhs, const VaList<T>& rhs) {
        if (len(rhs) != N) {
            return false;
        }

        if constexpr (!va::HasEqualityOperatorV<T>) {
            return std::memcmp(lhs.data, rhs.data, N * sizeof(T)) == 0;
        } else {
            for (Size i = 0; i < N; i++) {
                if (lhs.data[i] != rhs[i]) return false;
            }
            return true;
        }
    }

  public iterators:
    using Iterator = T*;
    using ConstIterator = const T*;
    using ReverseIterator = std::reverse_iterator<Iterator>;
    using ConstReverseIterator = std::reverse_iterator<ConstIterator>;

    constexpr Iterator begin() noexcept { return data; }
    constexpr Iterator end() noexcept { return data + N; }

    constexpr ConstIterator begin() const noexcept { return data; }
    constexpr ConstIterator end() const noexcept { return data + N; }

    constexpr ConstIterator cbegin() const noexcept { return data; }
    constexpr ConstIterator cend() const noexcept { return data + N; }

    constexpr ReverseIterator rbegin() noexcept { return ReverseIterator(end()); }
    constexpr ReverseIterator rend() noexcept { return ReverseIterator(begin()); }

    constexpr ConstReverseIterator rbegin() const noexcept { return ConstReverseIterator(end()); }
    constexpr ConstReverseIterator rend() const noexcept { return ConstReverseIterator(begin()); }

    constexpr ConstReverseIterator crbegin() const noexcept { return ConstReverseIterator(cend()); }
    constexpr ConstReverseIterator crend() const noexcept { return ConstReverseIterator(cbegin()); }
};

} // namespace va::detail

/**
 * @brief Public array type
 * @tparam T Type of elements stored in array
 * @tparam N Size of array (0 for automatic size deduction)
 *
 * @note This is the public interface for fixed-size arrays
 */
template <typename T, Size N = 0>
class VaArray;

/**
 * @brief Public array type specialization
 * @tparam T Type of elements stored in array
 * @tparam N Size of array
 */
template <typename T, Size N>
class VaArray: public va::detail::__BasicArray<T, N> {
  public:
    using va::detail::__BasicArray<T, N>::__BasicArray; // Inherit constructors

    /**
     * @brief Gets array length
     * @param arr Array to check
     * @return Size of array
     */
    friend inline Size len(const VaArray& arr) { return N; }
};

/**
 * @brief Public array type specialization for size deduction
 * @tparam T Type of elements stored in array
 * @tparam N Automatically deduced size of array
 */
template <typename T, Size N>
class VaArray<T[N], 0>: public va::detail::__BasicArray<T, N> {
  public:
    using va::detail::__BasicArray<T, N>::__BasicArray; // Inherit constructors

    /**
     * @brief Gets array length
     * @param arr Array to check
     * @return Size of array
     */
    friend inline Size len(const VaArray& arr) { return N; }
};

namespace std {

// for structured bindings
template <typename T, Size N>
struct tuple_size<VaArray<T[N]>>: std::integral_constant<Size, N> {};

template <Size I, typename T, Size N>
struct tuple_element<I, VaArray<T[N], 0>> {
    using type = T;
};

} // namespace std

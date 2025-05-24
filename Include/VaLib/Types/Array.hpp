// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <VaLib/Types/TypeTraits.hpp>
#include <VaLib/Types/BasicTypedef.hpp>

#include <VaLib/Types/Error.hpp>
#include <VaLib/Types/List.hpp>

/**
 * @brief Basic array implementation
 * @tparam T Type of elements stored in array
 * @tparam N Size of array
 *
 * @note This is an internal implementation class, use VaArray in public API
 * @warning The class name starts with double underscore indicating it's implementation detail
 */
template <typename T, Size N>
class VaArray {
    static_assert(N > 0, "Size must be greater than 0");

  protected:
    T data[N];

  public:
    /**
     * @brief Constructs a VaArray from an initializer list
     * @param init Initializer list containing elements to initialize the array
     *
     * @note If the initializer list contains fewer elements than the array size,
     *       the remaining elements are default-initialized.
     * @warning If the initializer list contains more elements than the array size,
     *          excess elements are ignored.
     */
    constexpr VaArray(std::initializer_list<T> init) {
        Size i = 0;
        for (auto&& v: init) {
            data[i++] = v;
        }

        while (i < N) {
            data[i++] = T{};
        }
    }

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
     * @throws IndexOutOfRangeError if index >= N
     * @note Safer but slower than operator[]
     */
    inline T& at(Size index) {
        if (index >= N) throw IndexOutOfRangeError(N, index);
        return data[index];
    }

    /**
     * @brief Accesses element with bounds checking (const version)
     * @param index Index of element to access
     * @return Const reference to requested element
     *
     * @throws IndexOutOfRangeError if index >= N
     * @note Safer but slower than operator[]
     */
    inline const T& at(Size index) const {
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
    VaArray<T, N+L> operator+(const VaArray<T, L>& other) const {
        VaArray<T, N + L> result;
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
        for (Size i = 0; i < N; i++) {
            data[i] = value;
        }
    }

    /**
     * @brief Swaps contents with another array
     * @param other Array to swap with
     *
     * @note Performs element-wise swap
     * @warning May be expensive for non-trivial types
     */
    constexpr void swap(VaArray& other) noexcept {
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
    friend bool operator==(const VaArray& lhs, const VaArray& rhs) {
        #if __cplusplus >= CPP17
            if constexpr (tt::IsTriviallyCopyable<T>) {
                return std::memcmp(lhs.data, rhs.data, N * sizeof(T)) == 0;
            } else if constexpr (tt::HasEqualityOperator<T>::value) {
                for (Size i = 0; i < N; ++i) {
                    if (!(lhs.data[i] == rhs.data[i])) return false;
                }
                return true;
            } else {
                static_assert(sizeof(T) == -1, "Cannot compare arrays whose elements are not comparable");
            }
        #else
            if (tt::IsTriviallyCopyable<T>) {
                return std::memcmp(lhs.data, rhs.data, N * sizeof(T)) == 0;
            } else {
                for (Size i = 0; i < N; ++i) {
                    if (!(lhs.data[i] == rhs.data[i])) return false;
                }
                return true;
            }
        #endif
    }

    /**
     * @brief Inequality comparison
     * @param lhs First array to compare
     * @param rhs Second array to compare
     * @return true if any elements differ
     */
    friend inline bool operator!=(const VaArray& lhs, const VaArray& rhs) {
        return !(lhs == rhs);
    }

    /**
     * @brief Comparison with VaList
     * @param lhs Array to compare
     * @param rhs VaList to compare
     * @return true if equal
     */
    friend bool operator==(const VaArray<T, N>& lhs, const VaList<T>& rhs) {
        if (len(rhs) != N) {
            return false;
        }

        #if __cplusplus >= CPP17
            if constexpr (tt::IsTriviallyCopyable<T>) {
                return std::memcmp(lhs.data, rhs.dataPtr(), N * sizeof(T)) == 0;
            } else if constexpr (tt::HasEqualityOperator<T>::value) {
                for (Size i = 0; i < N; ++i) {
                    if (!(lhs.data[i] == rhs[i])) return false;
                }
                return true;
            } else {
                static_assert(sizeof(T) == -1, "Cannot compare arrays whose elements are not comparable");
            }
        #else
            if (tt::IsTriviallyCopyable<T>) {
                return std::memcmp(lhs.data, rhs.dataPtr(), N * sizeof(T)) == 0;
            } else {
                for (Size i = 0; i < N; ++i) {
                    if (!(lhs.data[i] == rhs[i])) return false;
                }
                return true;
            }
        #endif
    }

  public friends:
    friend inline Size len(const VaArray& arr) { return N; }

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

namespace std {

// for structured bindings
template <typename T, Size N>
struct tuple_size<VaArray<T, N>>: std::integral_constant<Size, N> {};

template <Size I, typename T, Size N>
struct tuple_element<I, VaArray<T, N>> {
    using type = T;
};

} // namespace std

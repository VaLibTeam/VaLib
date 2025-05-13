// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <VaLib/Types/BasicTypedef.hpp>

#include <utility>

/**
 * @class VaPair A simple value pair container.
 * @tparam T1 Type of the first element
 * @tparam T2 Type of the second element
 */
template <typename T1, typename T2>
class VaPair {
  public:
    T1 first;  ///< The first element of the pair
    T2 second; ///< The second element of the pair

    /**
     * @brief Default constructor.

     * @note Both elements will be value-initialized.
     */
    VaPair() noexcept = default;

    /**
     * @brief Constructs a pair with specified values.
     * @param a1 Value for the first element
     * @param a2 Value for the second element
     *
     * @note This constructor is noexcept as long as T1 and T2's copy constructors are noexcept.
     */
    VaPair(T1 a1, T2 a2) noexcept : first(a1), second(a2) {}

    template <typename U1, typename U2>
    VaPair(U1&& a1, U2&& a2) : first(std::forward<U1>(a1)), second(std::forward<U2>(a2)) {}

    /**
     * @brief Constructs from std::pair.
     * @param p std::pair to copy values from
     *
     * @note Conversion is noexcept as long as T1 and T2's copy constructors are noexcept.
     */
    VaPair(std::pair<T1, T2> p) noexcept : first(p.first), second(p.second) {}

    /**
     * @brief Equality comparison operator.
     * @param other Pair to compare with
     * @return true if both elements are equal
     *
     * @note Requires both T1 and T2 to have operator== defined.
     * @warning Comparison might not work as expected for floating-point types due to precision issues.
     */
    inline bool operator==(const VaPair<T1, T2>& other) const noexcept {
        return first == other.first && second == other.second;
    }

    /**
     * @brief Inequality comparison operator.
     * @param other Pair to compare with
     * @return true if elements are not equal
     *
     * @note Simply negates the result of operator==.
     */
    inline bool operator!=(const VaPair<T1, T2>& other) const noexcept { return !(*this == other); }

    /**
     * @brief Swaps the first and second elements.
     * @tparam U1 First type (automatically deduced)
     * @tparam U2 Second type (automatically deduced)
     *
     * @note This method is only available when T1 and T2 are the same type.
     * @warning Using this with different types will cause a compilation error.
     */
    template <typename U1 = T1, typename U2 = T2,
        typename = std::enable_if_t<std::is_same_v<U1, U2>>>
    inline void swap() noexcept {
        std::swap(first, second);
    }

    /**
     * @brief Gets element by index.
     * @tparam N Index of the element (0 or 1)
     * @return Reference to the requested element
     *
     * @note Compile-time checked - N must be 0 or 1.
     * @warning Will cause a compilation error if N is out of bounds.
     */
    template <int N>
    auto& get() noexcept {
        static_assert(N >= 0 && N < 2, "VaPair::get: invalid argument");
        if constexpr (N == 0) {
            return first;
        } else if constexpr (N == 1) {
            return second;
        }
    }

    /**
     * @brief Gets element by index (const version).
     * @tparam N Index of the element (0 or 1)
     * @return Const reference to the requested element
     *
     * @note Compile-time checked - N must be 0 or 1.
     * @warning Will cause a compilation error if N is out of bounds.
     */
    template <int N>
    const auto& get() const noexcept {
        static_assert(N >= 0 && N < 2, "VaPair::get: invalid argument");
        if constexpr (N == 0) {
            return first;
        } else if constexpr (N == 1) {
            return second;
        }
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

// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <VaLib/Types/BasicTypedef.hpp>

/**
 * @class VaTuple Variadic tuple class template.
 * 
 * @note This class supports variadic storage and provides methods for element access,
 * iteration, and functional operations.
 */
template <typename... Types>
class VaTuple;

/**
 * @brief Specialization of VaTuple for an empty tuple.
 * 
 * @note This is the recursion base case used for inheritance chaining.
 */
template <>
class VaTuple<> {
  public:
    /**
     * @brief Constructs a VaTuple from arguments.
     * 
     * @tparam Args Argument types.
     * @param args Arguments to construct the tuple from.
     * @return VaTuple of decayed types.
     * 
     * @note This method deduces and decays argument types to form a new tuple.
     */
    template <typename... Args>
    static auto Make(Args&&... args) {
        return VaTuple<std::decay_t<Args>...>(std::forward<Args>(args)...);
    }

    /**
     * @brief Applies a function to each element (no-op for empty tuple).
     * 
     * @tparam Func Callable type.
     * @param func Function to apply.
     * 
     * @note This overload is a no-op because the tuple is empty.
     */
    template <typename Func>
    void forEach(Func&&) {}

    /**
     * @brief Applies a function to each element (const version).
     * 
     * @tparam Func Callable type.
     * @param func Function to apply.
     * 
     * @note This overload is a no-op because the tuple is empty.
     */
    template <typename Func>
    void forEach(Func&&) const {}

    /**
     * @brief Applies a function to each element with index (no-op for empty tuple).
     * 
     * @tparam Func Callable type.
     * @tparam I Start index (default 0).
     * @param func Function to apply with index.
     * 
     * @note This overload is a no-op because the tuple is empty.
     */
    template <typename Func, Size I = 0>
    void forEachIndexed(Func&& func) {}

    /**
     * @brief Applies a function to each element with index (const version).
     * 
     * @tparam Func Callable type.
     * @tparam I Start index (default 0).
     * @param func Function to apply with index.
     * 
     * @note This overload is a no-op because the tuple is empty.
     */
    template <typename Func, Size I = 0>
    void forEachIndexed(Func&& func) const {}
};

/**
 * @brief Variadic recursive tuple implementation.
 * @tparam Head Type of the current head element.
 * @tparam Tail Types of the remaining elements.
 * 
 * @note This implementation recursively inherits from VaTuple<Tail...>.
 */
template <typename Head, typename... Tail>
class VaTuple<Head, Tail...>: protected VaTuple<Tail...> {
  protected:
    Head head;

  public:
    /**
     * @brief Constructs a VaTuple from const references.
     * @param head First element.
     * @param tail Remaining elements.
     */
    VaTuple(const Head& head, const Tail&... tail) : VaTuple<Tail...>(tail...), head(head) {}

    /**
     * @brief Constructs a VaTuple from rvalue references.
     * @param head First element.
     * @param tail Remaining elements.
     */
    VaTuple(Head&& head, Tail&&... tail)
        : VaTuple<Tail...>(std::forward<Tail>(tail)...), head(std::forward<Head>(head)) {}

    /**
     * @brief Creates a VaTuple by decaying and forwarding arguments.
     * @tparam Args Argument types.
     * @param args Arguments to store in tuple.
     * @return New VaTuple with decayed argument types.
     */
    template <typename... Args>
    static auto Make(Args&&... args) {
        return VaTuple<std::decay_t<Args>...>(std::forward<Args>(args)...);
    }

    /**
     * @brief Returns a reference to the first element.
     * @return Reference to the first element.
     */
    auto& first() { return head; }

    /**
     * @brief Returns a const reference to the first element.
     * @return Const reference to the first element.
     */
    const auto& first() const { return head; }

    /**
     * @brief Returns a reference to the second element.
     * @return Reference to the second element.
     * 
     * @warning Asserts at compile time if the tuple has fewer than two elements.
     */
    auto& second() {
        static_assert(sizeof...(Tail) >= 1, "Tuple does not have a second element.");
        return VaTuple<Tail...>::template get<0>();
    }

    /**
     * @brief Returns a const reference to the second element.
     * @return Const reference to the second element.
     * 
     * @warning Asserts at compile time if the tuple has fewer than two elements.
     */
    const auto& second() const {
        static_assert(sizeof...(Tail) >= 1, "Tuple does not have a second element.");
        return VaTuple<Tail...>::template get<0>();
    }

    /**
     * @brief Returns a reference to the third element.
     * @return Reference to the third element.
     * 
     * @warning Asserts at compile time if the tuple has fewer than three elements.
     */
    auto& third() {
        static_assert(sizeof...(Tail) >= 2, "Tuple does not have a third element.");
        return VaTuple<Tail...>::template get<1>();
    }

    /**
     * @brief Returns a const reference to the third element.
     * @return Const reference to the third element.
     * 
     * @warning Asserts at compile time if the tuple has fewer than three elements.
     */
    const auto& third() const {
        static_assert(sizeof...(Tail) >= 2, "Tuple does not have a third element.");
        return VaTuple<Tail...>::template get<1>();
    }

    /**
     * @brief Applies a function to each element in order.
     * @tparam Func Callable type.
     * @param func Function to apply.
     */
    template <typename Func>
    void forEach(Func&& func) {
        func(head);
        VaTuple<Tail...>::forEach(std::forward<Func>(func));
    }

    /**
     * @brief Applies a function to each element in order (const version).
     * @tparam Func Callable type.
     * @param func Function to apply.
     */
    template <typename Func>
    void forEach(Func&& func) const {
        func(head);
        VaTuple<Tail...>::forEach(std::forward<Func>(func));
    }

    /**
     * @brief Applies a function to each element with index.
     * @tparam Func Callable type.
     * @tparam I Start index (default 0).
     * @param func Function to apply with index.
     */
    template <typename Func, Size I = 0>
    void forEachIndexed(Func&& func) {
        func(std::integral_constant<Size, I>{}, head);
        VaTuple<Tail...>::template forEachIndexed<Func, I + 1>(std::forward<Func>(func));
    }

    /**
     * @brief Applies a function to each element with index (const version).
     * @tparam Func Callable type.
     * @tparam I Start index (default 0).
     * @param func Function to apply with index.
     */
    template <typename Func, Size I = 0>
    void forEachIndexed(Func&& func) const {
        func(std::integral_constant<Size, I>{}, head);
        VaTuple<Tail...>::template forEachIndexed<Func, I + 1>(std::forward<Func>(func));
    }

    /**
     * @brief Retrieves the element at index I.
     * @tparam I Index of the element.
     * @return Reference to the element at index I.
     * 
     * @note This function uses recursion and constexpr evaluation.
     */
    template <Size I>
    auto& get() {
        if constexpr (I == 0) {
            return head;
        } else {
            return VaTuple<Tail...>::template get<I - 1>();
        }
    }

    /**
     * @brief Retrieves the element at index I (const version).
     * @tparam I Index of the element.
     * @return Const reference to the element at index I.
     */
    template <Size I>
    const auto& get() const {
        if constexpr (I == 0) {
            return head;
        } else {
            return VaTuple<Tail...>::template get<I - 1>();
        }
    }

    /**
     * @brief Alias for the type of the I-th element.
     * @tparam I Index of the element.
     */
    template <Size I>
    using Element = typename std::tuple_element<I, VaTuple>::type;

    template <typename... Types1, typename... Types2, Size... I1, Size... I2>
    friend auto concatenate(const VaTuple<Types1...>&, const VaTuple<Types2...>&,
                            std::index_sequence<I1...>, std::index_sequence<I2...>);

    template <typename... Types1, typename... Types2>
    friend auto operator+(const VaTuple<Types1...>&, const VaTuple<Types2...>&);
};

/**
 * @brief Concatenates two VaTuples into one.
 * @tparam Types1 Types of the first tuple.
 * @tparam Types2 Types of the second tuple.
 * @tparam I1 Index sequence for first tuple.
 * @tparam I2 Index sequence for second tuple.
 * @param lhs Left tuple.
 * @param rhs Right tuple.
 * @return New VaTuple with all elements from both.
 */
template <typename... Types1, typename... Types2, Size... I1, Size... I2>
inline auto concatenate(const VaTuple<Types1...>& lhs, const VaTuple<Types2...>& rhs,
                        std::index_sequence<I1...>, std::index_sequence<I2...>) {
    return VaTuple<Types1..., Types2...>(lhs.template get<I1>()..., rhs.template get<I2>()...);
}

/**
 * @brief Concatenation operator for VaTuples.
 * @param lhs Left tuple.
 * @param rhs Right tuple.
 * @return New VaTuple combining both tuples.
 */
template <typename... Types1, typename... Types2>
inline auto operator+(const VaTuple<Types1...>& lhs, const VaTuple<Types2...>& rhs) {
    return concatenate(lhs, rhs, std::make_index_sequence<sizeof...(Types1)>{},
                       std::make_index_sequence<sizeof...(Types2)>{});
}

namespace std {

template <Size I, typename T>
struct tuple_element;

template <Size I, typename Head, typename... Tail>
struct tuple_element<I, VaTuple<Head, Tail...>>: tuple_element<I - 1, VaTuple<Tail...>> {};

template <typename Head, typename... Tail>
struct tuple_element<0, VaTuple<Head, Tail...>> {
    using type = Head;
};

template <typename... Ts>
struct tuple_size<VaTuple<Ts...>>: std::integral_constant<Size, sizeof...(Ts)> {};

template <Size I, typename T>
using tuple_element_t = typename tuple_element<I, T>::type;

template <Size I, typename... Types>
decltype(auto) get(VaTuple<Types...>& tuple) {
    return tuple.template get<I>();
}

template <Size I, typename... Types>
decltype(auto) get(const VaTuple<Types...>& tuple) {
    return tuple.template get<I>();
}

template <Size I, typename... Types>
decltype(auto) get(VaTuple<Types...>&& tuple) {
    return std::move(tuple).template get<I>();
}

template <Size I, typename... Types>
decltype(auto) get(const VaTuple<Types...>&& tuple) {
    return std::move(tuple).template get<I>();
}

} // namespace std
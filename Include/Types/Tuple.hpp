// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <Types/BasicTypedef.hpp>

template <typename... Types>
class VaTuple;

// Base case: empty tuple
template <>
class VaTuple<> {
  public:
    template <typename Func>
    void forEach(Func&&) {
        // nothing to do
    }

    template <typename Func>
    void forEach(Func&&) const {
        // nothing to do
    }

    template <typename Func, std::size_t I = 0>
    void forEachIndexed(Func&& func) {
        // nothing to do
    }

    template <typename Func, std::size_t I = 0>
    void forEachIndexed(Func&& func) const {
        // nothing to do
    }

    template <typename... Args>
    static auto Make(Args&&... args) {
        return VaTuple<std::decay_t<Args>...>(std::forward<Args>(args)...);
    }
};

// Recursive case: tuple with one or more elements
template <typename Head, typename... Tail>
class VaTuple<Head, Tail...>: protected VaTuple<Tail...> {
  protected:
    Head head;

  public:
    VaTuple(const Head& head, const Tail&... tail) : VaTuple<Tail...>(tail...), head(head) {}

    VaTuple(Head&& head, Tail&&... tail)
        : VaTuple<Tail...>(std::forward<Tail>(tail)...), head(std::forward<Head>(head)) {}

    // Methods to get the first, second, and third elements
    auto& first() { return head; }

    const auto& first() const { return head; }

    auto& second() {
        static_assert(sizeof...(Tail) >= 1, "Tuple does not have a second element.");
        return VaTuple<Tail...>::template get<0>();
    }

    const auto& second() const {
        static_assert(sizeof...(Tail) >= 1, "Tuple does not have a second element.");
        return VaTuple<Tail...>::template get<0>();
    }

    auto& third() {
        static_assert(sizeof...(Tail) >= 2, "Tuple does not have a third element.");
        return VaTuple<Tail...>::template get<1>();
    }

    const auto& third() const {
        static_assert(sizeof...(Tail) >= 2, "Tuple does not have a third element.");
        return VaTuple<Tail...>::template get<1>();
    }

    template <typename Func>
    void forEach(Func&& func) {
        func(head);
        VaTuple<Tail...>::forEach(std::forward<Func>(func));
    }

    template <typename Func>
    void forEach(Func&& func) const {
        func(head);
        VaTuple<Tail...>::forEach(std::forward<Func>(func));
    }

    template <typename Func, std::size_t I = 0>
    void forEachIndexed(Func&& func) {
        func(std::integral_constant<std::size_t, I>{}, head);
        VaTuple<Tail...>::template forEachIndexed<Func, I + 1>(std::forward<Func>(func));
    }

    template <typename Func, std::size_t I = 0>
    void forEachIndexed(Func&& func) const {
        func(std::integral_constant<std::size_t, I>{}, head);
        VaTuple<Tail...>::template forEachIndexed<Func, I + 1>(std::forward<Func>(func));
    }

    // get method using recursive inheritance
    template <Size I>
    auto& get() {
        if constexpr (I == 0) {
            return head;
        } else {
            return VaTuple<Tail...>::template get<I - 1>();
        }
    }

    template <Size I>
    const auto& get() const {
        if constexpr (I == 0) {
            return head;
        } else {
            return VaTuple<Tail...>::template get<I - 1>();
        }
    }

    template <std::size_t I>
    using Element = typename std::tuple_element<I, VaTuple>::type;

    template <typename... Args>
    static auto Make(Args&&... args) {
        return VaTuple<std::decay_t<Args>...>(std::forward<Args>(args)...);
    }
};

template <typename... Types1, typename... Types2, std::size_t... I1, std::size_t... I2>
auto concatenate(const VaTuple<Types1...>& lhs, const VaTuple<Types2...>& rhs,
                 std::index_sequence<I1...>, std::index_sequence<I2...>) {
    return VaTuple<Types1..., Types2...>(lhs.template get<I1>()..., rhs.template get<I2>()...);
}

template <typename... Types1, typename... Types2>
auto operator+(const VaTuple<Types1...>& lhs, const VaTuple<Types2...>& rhs) {
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
struct tuple_size<VaTuple<Ts...>>: std::integral_constant<std::size_t, sizeof...(Ts)> {};

template <Size I, typename T>
using tuple_element_t = typename tuple_element<I, T>::type;

template <std::size_t I, typename... Types>
decltype(auto) get(VaTuple<Types...>& tuple) {
    return tuple.template get<I>();
}

template <std::size_t I, typename... Types>
decltype(auto) get(const VaTuple<Types...>& tuple) {
    return tuple.template get<I>();
}

template <std::size_t I, typename... Types>
decltype(auto) get(VaTuple<Types...>&& tuple) {
    return std::move(tuple).template get<I>();
}

template <std::size_t I, typename... Types>
decltype(auto) get(const VaTuple<Types...>&& tuple) {
    return std::move(tuple).template get<I>();
}

} // namespace std
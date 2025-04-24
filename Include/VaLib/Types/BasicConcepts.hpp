// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <stddef.h>
#include <stdint.h>
#include <string>

#include <concepts>

typedef size_t Size;

namespace va {

template <typename T>
concept Addable = requires(T x, T y) {
    { x + y } -> std::convertible_to<T>;
    { x += y } -> std::convertible_to<T&>;
};

template <typename T>
concept Ordered = requires(T a, T b) {
    { a < b } -> std::convertible_to<bool>;
    { a > b } -> std::convertible_to<bool>;
};

template <typename It, typename T>
concept Iterator = requires(It it) {
    { ++it } -> std::same_as<It&>;
    { it++ } -> std::same_as<It>;

    { *it } -> std::convertible_to<T&>;
};

template <typename It, typename T>
concept ConstIterator = requires(It it) {
    { ++it } -> std::same_as<It&>;
    { it++ } -> std::same_as<It>;

    { *it } -> std::convertible_to<const T&>;
};

template <typename T, typename V>
concept IterableWith = requires(T t) {
    requires Iterator<decltype(t.begin()), V>;
    requires Iterator<decltype(t.end()), V>;
} && requires(const T ct) {
    requires ConstIterator<decltype(ct.begin()), V>;
    requires ConstIterator<decltype(ct.end()), V>;
};

template <typename R, typename... Args>
concept Callable = requires(R (*func)(Args...), Args... args) {
    { func(args...) } -> std::convertible_to<R>;
};

template <typename T>
concept DefaultConstructible = requires { T{}; };

template <typename T>
concept CopyConstructible = requires(T a) { T(a); };

template <typename T>
concept MoveConstructible = requires(T a) { T(std::move(a)); };

template <typename T>
concept EqualityComparable = requires(T a, T b) {
    { a == b } -> std::convertible_to<bool>;
    { a != b } -> std::convertible_to<bool>;
};

template <typename T>
concept Swappable = requires(T a, T b) { std::swap(a, b); };

template <typename T>
concept Hashable = requires(T a) {
    { std::hash<T>{}(a) } -> std::convertible_to<Size>;
};

} // namespace va

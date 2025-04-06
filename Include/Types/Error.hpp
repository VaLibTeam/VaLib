// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <Types/String.hpp>
#include <format.hpp>
#include <type_traits>
#include <variant>

#ifdef VaLib_USE_CONCEPTS
template <typename T>
concept error = requires(T t) {
    { t.what() } -> std::same_as<VaString>;
};
#endif

class BaseError {
  protected:
    VaString msg;

  public:
    BaseError() = default;
    explicit BaseError(VaString m);
    virtual ~BaseError() = default;

    VaString what() const;
};

class ValueError: public BaseError {
  public:
    using BaseError::BaseError;
};

class TypeError: public BaseError {
  public:
    using BaseError::BaseError;
};

class IndexOutOfTheRangeError: public BaseError {
  public:
    IndexOutOfTheRangeError(VaString m = "index out of the range");
    IndexOutOfTheRangeError(Size range, Size index);
};

template <typename T, typename E = BaseError>
class Result {
  protected:
    std::variant<T, E> value;

  public:
    Result(T v) : value(v) {}
    Result(E e) : value(e) {}

    bool isOk() const { return std::holds_alternative<T>(value); }
    bool isErr() const { return std::holds_alternative<E>(value); }

    T unwrap() const { return std::get<T>(value); }
    E unwrapErr() const { return std::get<E>(value); }
    T unwrapOr(T fallback) const { return isOk() ? unwrap() : fallback; }
};
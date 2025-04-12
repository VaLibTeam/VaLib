// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <VaLib/Types/String.hpp>
#include <type_traits>
#include <variant>

#ifdef VaLib_USE_CONCEPTS
template <typename T>
concept error = requires(T t) {
    { t.what() } -> std::same_as<VaString>;
};
#endif

/**
 * @brief Base class for all error types.
 */
class BaseError {
  protected:
    /**
     * @brief error message
     */
    VaString msg;

  public:
    /**
     * @brief Default constructor.
     */
    BaseError() = default;

    /**
     * @brief Constructor with an error message.
     * @param m The error message.
     */
    explicit BaseError(VaString m);

    /**
     * @brief Virtual destructor.
     */
    virtual ~BaseError() = default;

    /**
     * @brief Get the error message.
     * @return The error message as a VaString.
     */
    VaString what() const;
};

/**
 * @brief Error class for value-related errors.
 */
class ValueError: public BaseError {
  public:
    using BaseError::BaseError;
};

/**
 * @brief Error class for type-related errors.
 */
class TypeError: public BaseError {
  public:
    using BaseError::BaseError;
};

/**
 * @brief Error class for index out of range errors.
 */
class IndexOutOfRangeError: public BaseError {
  public:
    /**
     * @brief Constructor with a default error message.
     * @param m The error message (default: "index out of the range").
     */
    IndexOutOfRangeError(VaString m = "index out of the range");

    /**
     * @brief Constructor with range and index information.
     * @param range The valid range.
     * @param index The invalid index.
     */
    IndexOutOfRangeError(Size range, Size index);
};

/**
 * @brief Error class for null pointer errors.
 */
class NullPointerError: public BaseError {
  public:
    /**
         * @brief Constructor with a default error message.
         * @param m The error message (default: "null pointer error").
         */
    NullPointerError(VaString m = "null pointer error") : BaseError(m) {}
};

/**
 * @brief Error class for division by zero errors.
 */
class DivisionByZeroError: public BaseError {
  public:
    /**
         * @brief Constructor with a default error message.
         * @param m The error message (default: "division by zero").
         */
    DivisionByZeroError(VaString m = "division by zero") : BaseError(m) {}
};

/**
 * @brief Error class for file not found errors.
 */
class FileNotFoundError: public BaseError {
  public:
    /**
         * @brief Constructor with a default error message.
         * @param m The error message (default: "file not found").
         */
    FileNotFoundError(VaString m = "file not found") : BaseError(m) {}
};

/**
 * @brief Error class for permission denied errors.
 */
class PermissionError: public BaseError {
  public:
    /**
         * @brief Constructor with a default error message.
         * @param m The error message (default: "permission denied").
         */
    PermissionError(VaString m = "permission denied") : BaseError(m) {}
};

/**
 * @brief A class representing a result that can either hold a value or an error.
 * @tparam T The type of the value.
 * @tparam E The type of the error (default: BaseError).
 */
template <typename T, typename E = BaseError>
class Result {
  protected:
    /**
     * @brief The value or error.
     */
    std::variant<T, E> value;

  public:
    /**
     * @brief Constructor for a successful result.
     * @param v The value.
     */
    Result(T v) : value(v) {}

    /**
     * @brief Constructor for an error result.
     * @param e The error.
     */
    Result(E e) : value(e) {}

    /**
     * @brief Check if the result is successful.
     * @return True if the result holds a value, false otherwise.
     */
    bool isOk() const { return std::holds_alternative<T>(value); }

    /**
     * @brief Check if the result is an error.
     * @return True if the result holds an error, false otherwise.
     */
    bool isErr() const { return std::holds_alternative<E>(value); }

    /**
     * @brief Get the value if the result is successful.
     * @return The value.
     * 
     * @throws @ref ValueError if the result holds an error.
     */
    T unwrap() const {
        if (!isOk()) {
            throw ValueError("Attempted to unwrap a Result that holds an error.");
        }
        return std::get<T>(value);
    }

    /**
     * @brief Get the error if the result is an error.
     * @return The error.
     * 
     * @throws @ref ValueError if the result holds a value.
     */
    E unwrapErr() const {
        if (!isErr()) {
            throw ValueError("Attempted to unwrapErr a Result that holds a value.");
        }
        return std::get<E>(value);
    }

    /**
     * @brief Get the value or a fallback if the result is an error.
     * @param fallback The fallback value.
     * @return The value if the result is successful, otherwise the fallback.
     */
    T unwrapOr(T fallback) const { return isOk() ? unwrap() : fallback; }
};
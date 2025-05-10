// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <VaLib/Types/String.hpp>

#include <iostream>

#define THROWIT                                                                                    \
    virtual void throwIt() const override { throw *this; }

template <typename T, typename = void>
struct hasThrowIt: FalseType {};

template <typename T>
struct hasThrowIt<T, decltype(std::declval<T>().throwIt(), void())>: TrueType {};

template <typename T>
constexpr bool hasThrowItV = hasThrowIt<T>::value;

#ifdef VaLib_USE_CONCEPTS
template <typename T>
concept Error = requires(T err) {
    { err.what() } -> std::same_as<VaString>;
};
#endif

/**
 * @brief Base class for all error types.
 */
class VaBaseError {
  protected:
    VaString msg; ///< error message

  public:
    /**
     * @brief Default constructor.
     */
    VaBaseError() = default;

    /**
     * @brief Constructor with an error message.
     * @param m The error message.
     */
    explicit VaBaseError(VaString m);

    /**
     * @brief Virtual destructor.
     */
    virtual ~VaBaseError() = default;

    virtual void throwIt() const { throw *this; };

    /**
     * @brief Get the error message.
     * @return The error message as a VaString.
     */
    virtual VaString what() const;
};

/**
 * @brief Error class for value-related errors.
 */
class ValueError: public VaBaseError {
  public:
    using VaBaseError::VaBaseError;
    THROWIT;
};

/**
 * @brief Error class for arguments-related errors.
 */
class InvalidArgsError: public ValueError {
  public:
    using ValueError::ValueError;
    THROWIT;
};

/**
 * @brief Error class for type-related errors.
 */
class TypeError: public VaBaseError {
  public:
    using VaBaseError::VaBaseError;
    THROWIT;
};

class IndexError: public VaBaseError {
  public:
    using VaBaseError::VaBaseError;
    THROWIT;
};

/**
 * @brief Error class for index out of range errors.
 */
class IndexOutOfRangeError: public IndexError {
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
    THROWIT;
};

/**
 * @brief Error class for null pointer errors.
 */
class NullPointerError: public VaBaseError {
  public:
    /**
     * @brief Constructor with a default error message.
     * @param m The error message (default: "null pointer error").
     */
    NullPointerError(VaString m = "null pointer error") : VaBaseError(m) {}
    THROWIT;
};

/**
 * @brief Error class for division by zero errors.
 */
class DivisionByZeroError: public VaBaseError {
  public:
    /**
     * @brief Constructor with a default error message.
     * @param m The error message (default: "division by zero").
     */
    DivisionByZeroError(VaString m = "division by zero") : VaBaseError(m) {}
    THROWIT;
};

class KeyNotFoundError: public VaBaseError {
  public:
    /**
     * @brief Constructor with a default error message.
     * @param m The error message (default: "key not found").
     */
    KeyNotFoundError(VaString m = "key not found") : VaBaseError(m) {}
    THROWIT;
};

class InvalidCastError: public VaBaseError {
  public:
    /**
     * @brief Constructor with a default error message.
     * @param m The error message (default: "invalid cast").
     */
    InvalidCastError(VaString m = "invalid cast") : VaBaseError(m) {}
    THROWIT;
};

class InvalidAnyCastError: public VaBaseError {
  public:
    InvalidAnyCastError(VaString m = "invalid any cast") : VaBaseError(m) {}
};

class InvalidVariantCastError: public VaBaseError {
  public:
    InvalidVariantCastError(VaString m = "invalid variant cast") : VaBaseError(m) {}
    THROWIT;
};

/**
 * @brief Error class for file not found errors.
 */
class FileNotFoundError: public VaBaseError {
  public:
    /**
     * @brief Constructor with a default error message.
     * @param m The error message (default: "file not found").
     */
    FileNotFoundError(VaString m = "file not found") : VaBaseError(m) {}
    THROWIT;
};

/**
 * @brief Error class for permission denied errors.
 */
class PermissionError: public VaBaseError {
  public:
    /**
     * @brief Constructor with a default error message.
     * @param m The error message (default: "permission denied").
     */
    PermissionError(VaString m = "permission denied") : VaBaseError(m) {}
    THROWIT;
};

/**
 * @brief A class representing a result that can either hold a value or an error.
 * @tparam T The type of the value.
 * @tparam E The type of the error (default: VaBaseError).
 *
 * This class uses a union to store either a value of type T or a pointer to an error of type E.
 * The error is stored as a pointer to allow polymorphic behavior (i.e. inheritance).
 * Make sure the error type E has a copy constructor if you plan to copy VaResult objects.
 */
template <typename T, typename E = VaBaseError>
class VaResult {
  protected:
    /**
     * @brief The union that stores either the value or the error pointer.
     */
    union {
        T val;  ///< The value, if the result is successful.
        E* err; ///< The error pointer, if the result is an error.
    };

    bool ok; ///< True if the result holds a value, false if it holds an error.

    /**
     * @brief Internal helper to clean up the error if present.
     */
    void destroy() noexcept(
        std::is_nothrow_destructible_v<T> && std::is_nothrow_destructible_v<E>) {
        if (ok) {
            val.~T();
        } else {
            delete err;
        }
    }

  public:
    /**
     * @brief Constructor for a successful result.
     * @param v The value.
     */
    VaResult(T v) : ok(true) { new (&val) T(std::move(v)); }

    template <typename U, typename = std::enable_if_t<std::is_constructible_v<T, U&&> &&
                                                      !std::is_same_v<std::decay_t<U>, VaResult>>>
    VaResult(U&& v) : ok(true) {
        new (&val) T(std::forward<U>(v));
    }

    /**
     * @brief Constructor for an error result.
     * @param e A pointer to the error object (must be allocated on the heap).
     */
    VaResult(E* e) : err(e), ok(false) {}

    /**
     * @brief Constructor for an error result.
     * @param e An error object.
     */
    VaResult(E&& e) : err(new E(std::move(e))), ok(false) {}

    /**
     * @brief Destructor. Frees the error if present.
     */
    ~VaResult() noexcept(std::is_nothrow_destructible_v<T> && std::is_nothrow_destructible_v<E>) {
        destroy();
    }

    /**
     * @brief Copy constructor.
     * @param other The other VaResult to copy.
     *
     * The error is deep-copied using the copy constructor of type E.
     */
    VaResult(const VaResult& other) : ok(other.ok) {
        if (ok) {
            new (&val) T(other.val);
        } else {
            err = other.err ? new E(*other.err) : nullptr;
        }
    }

    /**
     * @brief Move constructor.
     * @param other The other VaResult to move from.
     */
    VaResult(VaResult&& other) noexcept : ok(other.ok) {
        if (ok) {
            new (&val) T(std::move(other.val));
        } else {
            err = other.err;
            other.err = nullptr;
        }
    }

    /**
     * @brief Copy assignment operator.
     * @param other The other VaResult to copy from.
     * @return Reference to this.
     */
    VaResult& operator=(const VaResult& other) {
        if (this == &other) return *this;
        this->destroy();

        ok = other.ok;
        if (ok) {
            new (&val) T(other.val);
        } else {
            err = other.err ? new E(*other.err) : nullptr;
        }
        return *this;
    }

    /**
     * @brief Move assignment operator.
     * @param other The other VaResult to move from.
     * @return Reference to this.
     */
    VaResult& operator=(VaResult&& other) noexcept {
        if (this == &other) return *this;
        this->destroy();

        ok = other.ok;
        if (ok) {
            new (&val) T(std::move(other.val));
        } else {
            err = other.err;
            other.err = nullptr;
        }
        return *this;
    }

    /**
     * @brief Check if the result is successful.
     * @return True if the result holds a value, false otherwise.
     */
    inline bool isOk() const { return ok; }

    /**
     * @brief Check if the result is an error.
     * @return True if the result holds an error, false otherwise.
     */
    inline bool isErr() const { return !ok; }

    /**
     * @brief Get the value if the result is successful.
     * @return The value.
     *
     * @throws ValueError if the result holds an error.
     */
    inline const T& unwrap() const {
        if (!ok) {
            throw ValueError("Attempted to unwrap a VaResult that holds an error");
        }
        return val;
    }

    /**
     * @brief Get the error if the result is an error.
     * @return A pointer to the error.
     *
     * @throws ValueError if the result holds a value.
     */
    inline const E* unwrapErr() const {
        if (ok) {
            throw ValueError("Attempted to unwrapErr a VaResult that holds a value");
        }
        return err;
    }

    /**
     * @brief Get the value or a fallback if the result is an error.
     * @param fallback The fallback value.
     * @return The value if the result is successful, otherwise the fallback.
     */
    inline const T& unwrapOr(const T& fallback) const { return ok ? val : fallback; }

    /**
     * @brief Throws the stored error by calling its `throwIt()` method.
     *
     * @throws ValueError if the result holds a value.
     * @throws Any exception thrown by the error's `throwIt()` method.
     * @note This overload is enabled only if the error type E has a member function `throwIt()`.
     */
    template <typename U = E>
    std::enable_if_t<hasThrowItV<U>, void> throwErr() {
        if (ok) {
            throw ValueError("Attempted to throw a VaResult that holds a value");
        }

        err->throwIt();
    }

    /**
     * @brief Throws the stored error by value.
     *
     * @throws ValueError if the result holds a value.
     * @throws A copy of the error object (thrown by value).
     * @note This overload is enabled only if the error type E does not have a `throwIt()` method.
     */
    template <typename U = E>
    std::enable_if_t<!hasThrowItV<U>, void> throwErr() {
        if (ok) {
            throw ValueError("Attempted to throw a VaResult that holds a value");
        }

        throw *err;
    }
};

/**
 * @brief Specialization of VaResult for the case when there is no value (void).
 * @tparam E The type of the error (default: VaBaseError).
 *
 * This specialization is used when the successful result does not hold any value,
 * only indicating success or failure. The error is still stored as a heap-allocated
 * pointer to support polymorphism (i.e. inheritance).
 *
 * Make sure the error type E has a copy constructor if you plan to copy VaResult objects.
 */
template <typename E>
class VaResult<void, E> {
  protected:
    E* err = nullptr; ///< The pointer to the error object (heap-allocated).
    bool ok;          ///< True if the result holds a value, false if it holds an error.

    /**
     * @brief Internal helper to clean up the error if present.
     */
    void destroy() noexcept(std::is_nothrow_destructible_v<E>) {
        if (!ok) {
            delete err;
        }
    }

  public:
    /**
     * @brief Constructor for a successful result (no value).
     */
    VaResult() : ok(true) {}

    /**
     * @brief Constructor for an error result.
     * @param e A pointer to the error object (must be allocated on the heap).
     */
    VaResult(E* e) : err(e), ok(false) {}

    /**
     * @brief Constructor for an error result.
     * @param e An error object.
     */
    VaResult(E&& e) : err(new E(std::move(e))), ok(false) {}

    /**
     * @brief Destructor. Frees the error if present.
     */
    ~VaResult() noexcept(std::is_nothrow_destructible_v<E>) { destroy(); }

    /**
     * @brief Copy constructor.
     * @param other The other VaResult to copy.
     *
     * The error is deep-copied using the copy constructor of type E.
     */
    VaResult(const VaResult& other) : ok(other.ok) {
        if (!ok) {
            err = other.err ? new E(*other.err) : nullptr;
        }
    }

    /**
     * @brief Move constructor.
     * @param other The other VaResult to move from.
     */
    VaResult(VaResult&& other) noexcept : ok(other.ok), err(other.err) { other.err = nullptr; }

    /**
     * @brief Copy assignment operator.
     * @param other The other VaResult to copy from.
     * @return Reference to this.
     */
    VaResult& operator=(const VaResult& other) {
        if (this == &other) return *this;
        destroy();

        ok = other.ok;
        if (!ok) {
            err = other.err ? new E(*other.err) : nullptr;
        } else {
            err = nullptr;
        }
        return *this;
    }

    /**
     * @brief Move assignment operator.
     * @param other The other VaResult to move from.
     * @return Reference to this.
     */
    VaResult& operator=(VaResult&& other) noexcept {
        if (this == &other) return *this;
        destroy();

        ok = other.ok;
        err = other.err;
        other.err = nullptr;
        return *this;
    }

    /**
     * @brief Check if the result is successful.
     * @return True if the result indicates success, false otherwise.
     */
    inline bool isOk() const { return ok; }

    /**
     * @brief Check if the result is an error.
     * @return True if the result holds an error, false otherwise.
     */
    inline bool isErr() const { return !ok; }

    /**
     * @brief Ensure the result is successful.
     *
     * @throws ValueError if the result holds an error.
     *
     * This method is used to assert that the result indicates success.
     * Since there is no value (void), it does not return anything.
     */
    inline void unwrap() const {
        if (!ok) {
            throw ValueError("Attempted to unwrap a VaResult that holds an error");
        }
    }

    /**
     * @brief Get the error if the result is an error.
     * @return A pointer to the error.
     *
     * @throws ValueError if the result indicates success.
     */
    inline const E* unwrapErr() const {
        if (ok) {
            throw ValueError("Attempted to unwrapErr a VaResult that holds a value");
        }
        return err;
    }

    /**
     * @brief Throws the stored error by calling its `throwIt()` method.
     *
     * @throws ValueError if the result indicates success.
     * @throws Any exception thrown by the error's `throwIt()` method.
     * @note This overload is enabled only if the error type E has a member function `throwIt()`.
     */
    template <typename U = E>
    std::enable_if_t<hasThrowItV<U>, void> throwErr() {
        if (ok) {
            throw ValueError("Attempted to throw a VaResult that holds a value");
        }

        err->throwIt();
    }

    /**
     * @brief Throws the stored error by value.
     *
     * @throws ValueError if the result indicates success.
     * @throws A copy of the error object (thrown by value).
     * @note This overload is enabled only if the error type E does not have a `throwIt()` method.
     */
    template <typename U = E>
    std::enable_if_t<!hasThrowItV<U>, void> throwErr() {
        if (ok) {
            throw ValueError("Attempted to throw a VaResult that holds a value");
        }

        throw *err;
    }
};

#undef THROWIT

namespace va {

inline void terminateHandler() {
    try {
        throw;
    } catch (const VaBaseError& err) {
        std::cerr << " what(): " << err.what();
    } catch (const std::exception& err) {
        std::cerr << " what(): " << err.what();
    }

    std::abort();
}

} // namespace va

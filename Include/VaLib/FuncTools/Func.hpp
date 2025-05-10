// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <VaLib/Types/BasicConcepts.hpp>
#include <VaLib/Types/Error.hpp>

#include <memory>
#include <type_traits>
#include <utility>

template <typename Signature>
class VaFunc;

template <typename R, typename... Args>
class VaFunc<R(Args...)> {
protected:
    static constexpr Size bufSize = sizeof(void (*)()) * 8;

    struct alignas(std::max_align_t) Buffer {
        byte data[bufSize];
    };

    struct CallableBase {
        virtual R invoke(Args&&...) = 0;
        virtual CallableBase* clone(Buffer& buffer) const = 0;
        virtual CallableBase* move(Buffer& buffer) = 0;
        virtual void destroy(bool inBuffer) = 0;
        virtual ~CallableBase() = default;
    };

    template <typename F>
    struct CallableImpl : CallableBase {
        F func;

        template <typename U>
        CallableImpl(U&& f) : func(std::forward<U>(f)) {}

        R invoke(Args&&... args) override {
            return func(std::forward<Args>(args)...);
        }

        CallableBase* clone(Buffer& buffer) const override {
            if (sizeof(CallableImpl) <= bufSize) {
                return new (&buffer) CallableImpl(func);
            } else {
                return new CallableImpl(func);
            }
        }

        CallableBase* move(Buffer& buffer) override {
            if (sizeof(CallableImpl) <= bufSize) {
                return new (&buffer) CallableImpl(std::move(func));
            } else {
                return new CallableImpl(std::move(func));
            }
        }

        void destroy(bool inBuffer) noexcept(std::is_nothrow_destructible_v<F>) override {
            if (inBuffer) {
                this->~CallableImpl();
            } else {
                delete this;
            }
        }
    };

    CallableBase* callable = nullptr;

    bool inBuffer = false;
    Buffer buffer;

public:
    VaFunc() noexcept = default;
    VaFunc(std::nullptr_t) noexcept : VaFunc() {}

    template <typename F, typename = std::enable_if_t< !std::is_same_v<std::decay_t<F>, VaFunc> >>
    VaFunc(F&& f) {
        using Impl = CallableImpl<std::decay_t<F>>;
        if (sizeof(Impl) <= bufSize) {
            callable = new (static_cast<void*>(&buffer)) Impl(std::forward<F>(f));
            inBuffer = true;
        } else {
            callable = new Impl(std::forward<F>(f));
            inBuffer = false;
        }
    }

    VaFunc(const VaFunc& other) {
        if (other.callable) {
            callable = other.callable->clone(buffer);
            inBuffer = (callable == reinterpret_cast<CallableBase*>(&buffer));
        }
    }

    VaFunc(VaFunc&& other) noexcept {
        if (other.callable) {
            callable = other.callable->move(buffer);
            inBuffer = (callable == reinterpret_cast<CallableBase*>(&buffer));
            other.callable = nullptr;
        }
    }

    ~VaFunc() { reset(); }

    VaFunc& operator=(const VaFunc& other) {
        if (this != &other) {
            reset();
            if (other.callable) {
                callable = other.callable->clone(buffer);
                inBuffer = (callable == reinterpret_cast<CallableBase*>(&buffer));
            }
        }
        return *this;
    }

    VaFunc& operator=(VaFunc&& other) noexcept {
        if (this != &other) {
            reset();
            if (other.callable) {
                callable = other.callable->move(buffer);
                inBuffer = (callable == reinterpret_cast<CallableBase*>(&buffer));
                other.callable = nullptr;
            }
        }
        return *this;
    }

    void swap(VaFunc& other) noexcept {
        if (this == &other) return;

        VaFunc tmp = std::move(other);
        other = std::move(*this);
        *this = std::move(tmp);
    }

    void reset() {
        if (!callable) return;

        callable->destroy(inBuffer);
        callable = nullptr;
    }

    inline R call(Args... args) const {
        if (!callable) throw ValueError("call a null function");
        return callable->invoke(std::forward<Args>(args)...);
    }

    inline R operator()(Args... args) const {
        if (!callable) throw ValueError("call a null function");
        return callable->invoke(std::forward<Args>(args)...);
    }

  public operators:
    friend bool operator==(const VaFunc& lhs, std::nullptr_t rhs) noexcept {
        return lhs.callable == nullptr;
    }

    friend bool operator!=(const VaFunc& lhs, std::nullptr_t rhs) noexcept {
        return lhs.callable != nullptr;
    }

    inline explicit operator bool() const noexcept {
        return callable != nullptr;
    }
};

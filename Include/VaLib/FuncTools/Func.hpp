// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <VaLib/Types/BasicConcepts.hpp>
#include <VaLib/Types/Error.hpp>

#include <memory>
#include <type_traits>
#include <utility>

template <typename>
class VaFunc;

template <typename R, typename... Args>
class VaFunc<R(Args...)> {
  protected:
    static constexpr Size bufSize = sizeof(void*) * 8;
    using Buffer = std::aligned_storage_t<bufSize, alignof(std::max_align_t)>;

    struct CallableBase {
        virtual R invoke(Args&&...) = 0;
        virtual CallableBase* clone(Buffer& buffer) const = 0;
        virtual CallableBase* move(Buffer& buffer) = 0;
        virtual void destroy(bool inBuffer) noexcept = 0;
        virtual ~CallableBase() = default;
    };

    template <typename F>
    struct CallableImpl: CallableBase {
        F VaFunc;

        template <typename U>
        CallableImpl(U&& f) : VaFunc(std::forward<U>(f)) {}

        R invoke(Args&&... args) override { return VaFunc(std::forward<Args>(args)...); }

        CallableBase* clone(Buffer& buffer) const override {
            if (sizeof(CallableImpl) <= bufSize) {
                return new (&buffer) CallableImpl(VaFunc);
            } else {
                return new CallableImpl(VaFunc);
            }
        }

        CallableBase* move(Buffer& buffer) override {
            if (sizeof(CallableImpl) <= bufSize) {
                return new (&buffer) CallableImpl(std::move(VaFunc));
            } else {
                return new CallableImpl(std::move(VaFunc));
            }
        }

        void destroy(bool inBuffer) noexcept override {
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

    template <typename F, typename = std::enable_if_t<!std::is_same_v<std::decay_t<F>, VaFunc>>>
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

    void reset() {
        if (callable) {
            callable->destroy(inBuffer);
            callable = nullptr;
        }
    }

    R operator()(Args... args) const {
        if (!callable) throw ValueError();
        return callable->invoke(std::forward<Args>(args)...);
    }

    explicit operator bool() const noexcept { return callable != nullptr; }
};

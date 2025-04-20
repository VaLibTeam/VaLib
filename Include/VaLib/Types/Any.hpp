// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <VaLib/Types/BasicTypedef.hpp>

#include <VaLib/Types/Error.hpp>

#include <memory>
#include <type_traits>
#include <utility>

#include <new>
#include <stdexcept>
#include <type_traits>
#include <typeinfo>
#include <utility>

class VaAny {
  protected:
    using TypeID = const std::type_info;

    static constexpr std::size_t SBO_SIZE = 24;
    using Storage = std::aligned_storage_t<SBO_SIZE, alignof(std::max_align_t)>;

    struct VTable {
        void (*destroy)(void*);
        void (*copy)(const void*, void*);
        void (*move)(void*, void*);
        void* (*clone)(const void*);
    };

    const VTable* vtable = nullptr;
    TypeID* type = nullptr;
    bool onHeap = false;

    union {
        void* heapPtr;
        Storage sboBuf;
    };

    void* dataPtr() { return onHeap ? heapPtr : static_cast<void*>(&sboBuf); }
    const void* dataPtr() const { return onHeap ? heapPtr : static_cast<const void*>(&sboBuf); }

    template <typename T>
    static const VTable* makeVTable() {
        static const VTable vt = {
            // destroy
            [](void* ptr) { static_cast<T*>(ptr)->~T(); },

            // copy
            [](const void* src, void* dest) {
                if constexpr (std::is_copy_constructible_v<T>) {
                    new (dest) T(*static_cast<const T*>(src));
                } else {
                    throw std::runtime_error("Type is not copyable");
                }
            },

            // move
            [](void* src, void* dest) { new (dest) T(std::move(*static_cast<T*>(src))); },

            // clone (heap copy)
            [](const void* src) -> void* {
                if constexpr (std::is_copy_constructible_v<T>) {
                    void* mem = operator new(sizeof(T));
                    new (mem) T(*static_cast<const T*>(src));
                    return mem;
                } else {
                    throw std::runtime_error("Type is not copyable");
                }
            }};
        return &vt;
    }

  public:
    VaAny() = default;

    template <typename T>
    VaAny(T&& value) {
        emplace<std::decay_t<T>>(std::forward<T>(value));
    }

    VaAny(const VaAny& other) {
        if (other.vtable) {
            vtable = other.vtable;
            type = other.type;
            onHeap = other.onHeap;

            if (onHeap) {
                heapPtr = vtable->clone(other.dataPtr());
            } else {
                if (vtable->copy) {
                    vtable->copy(other.dataPtr(), &sboBuf);
                } else {
                    throw std::runtime_error("Type is not copyable");
                }
            }
        }
    }

    VaAny(VaAny&& other) noexcept {
        if (other.vtable) {
            vtable = other.vtable;
            type = other.type;
            onHeap = other.onHeap;

            if (onHeap) {
                heapPtr = other.heapPtr;
                other.heapPtr = nullptr;
            } else {
                vtable->move(other.dataPtr(), &sboBuf);
            }

            other.vtable = nullptr;
            other.type = nullptr;
        }
    }

    ~VaAny() { reset(); }

    VaAny& operator=(const VaAny& other) {
        if (this != &other) {
            reset();
            if (other.vtable) {
                vtable = other.vtable;
                type = other.type;
                onHeap = other.onHeap;

                if (onHeap) {
                    heapPtr = vtable->clone(other.dataPtr());
                } else {
                    if (vtable->copy) {
                        vtable->copy(other.dataPtr(), &sboBuf);
                    } else {
                        throw std::runtime_error("Type is not copyable");
                    }
                }
            }
        }
        return *this;
    }

    VaAny& operator=(VaAny&& other) noexcept {
        if (this != &other) {
            reset();
            if (other.vtable) {
                vtable = other.vtable;
                type = other.type;
                onHeap = other.onHeap;

                if (onHeap) {
                    heapPtr = other.heapPtr;
                    other.heapPtr = nullptr;
                } else {
                    vtable->move(other.dataPtr(), &sboBuf);
                }

                other.vtable = nullptr;
                other.type = nullptr;
            }
        }
        return *this;
    }

    template <typename T>
    void emplace(T&& value) {
        using U = std::decay_t<T>;
        const VTable* vt = makeVTable<U>();
        reset();

        type = &typeid(U);
        vtable = vt;

        if (sizeof(U) <= SBO_SIZE && alignof(U) <= alignof(Storage)) {
            onHeap = false;
            new (&sboBuf) U(std::forward<T>(value));
        } else {
            onHeap = true;
            heapPtr = operator new(sizeof(U));
            new (heapPtr) U(std::forward<T>(value));
        }
    }

    void reset() noexcept {
        if (vtable) {
            vtable->destroy(dataPtr());
            if (onHeap) {
                operator delete(heapPtr);
            }
            vtable = nullptr;
            type = nullptr;
        }
    }

    void swap(VaAny& other) noexcept {
        if (this == &other) return;
        VaAny temp = std::move(*this);
        *this = std::move(other);
        other = std::move(temp);
    }

    template <typename T>
    T& get() {
        if (!type || *type != typeid(T)) throw InvalidAnyCastError();
        return *static_cast<T*>(dataPtr());
    }

    template <typename T>
    const T& get() const {
        if (!type || *type != typeid(T)) throw InvalidAnyCastError();
        return *static_cast<const T*>(dataPtr());
    }

    bool hasValue() const noexcept { return vtable != nullptr; }

    template <typename T>
    bool isType() const {
        return type && *type == typeid(T);
    }

    TypeID& currentType() const { return type ? *type : typeid(void); }

    void* unsafePtr() noexcept { return dataPtr(); }
    const void* unsafePtr() const noexcept { return dataPtr(); }
};
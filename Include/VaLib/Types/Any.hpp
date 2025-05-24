// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <VaLib/Types/BasicTypedef.hpp>
#include <VaLib/Types/Error.hpp>

#include <typeinfo>
#include <type_traits>
#include <stdexcept>
#include <new>
#include <memory>

class VaAny {
  protected:
    using TypeID = const std::type_info;

    static constexpr Size SBO_SIZE = 24;
    struct alignas(MaxAlignType) AlignedBuffer {
        byte data[SBO_SIZE];
    };

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
        AlignedBuffer sboBuf;
    };

    void* dataPtr() { return onHeap ? heapPtr : static_cast<void*>(&sboBuf); }
    const void* dataPtr() const { return onHeap ? heapPtr : static_cast<const void*>(&sboBuf); }

    template <typename T>
    static const VTable* makeVTable() {
        static const VTable vt = {// destroy
            [](void* ptr) { static_cast<T*>(ptr)->~T(); },

            // copy
            [](const void* src, void* dest) {
                #if __cplusplus >= CPP17
                    if constexpr (tt::IsCopyConstructible<T>) {
                        new (dest) T(*static_cast<const T*>(src));
                    } else {
                        throw TypeError("Type is not copyable");
                    }
                #else
                    if (tt::IsCopyConstructible<T>) {
                        new (dest) T(*static_cast<const T*>(src));
                    } else {
                        throw TypeError("Type is not copyable");
                    }
                #endif
            },

            // move
            [](void* src, void* dest) { new (dest) T(std::move(*static_cast<T*>(src))); },

            // clone (heap copy)
            [](const void* src) -> void* {
                #if __cplusplus >= CPP17
                    if constexpr (tt::IsCopyConstructible<T>) {
                        void* mem = operator new(sizeof(T));
                        new (mem) T(*static_cast<const T*>(src));
                        return mem;
                    } else {
                        throw TypeError("Type is not copyable");
                    }
                #else
                    if (tt::IsCopyConstructible<T>) {
                        void* mem = operator new(sizeof(T));
                        new (mem) T(*static_cast<const T*>(src));
                        return mem;
                    } else {
                        throw TypeError("Type is not copyable");
                    }
                #endif
            }};
        return &vt;
    }

  public:
    /**
     * @brief Default constructor for VaAny.
     */
    VaAny() = default;

    /**
     * @brief Constructs a VaAny object with the given value.
     * @tparam T The type of the value to store.
     * @param value The value to store in the VaAny object.
     *
     * @note This constructor uses perfect forwarding to store the value.
     */
    template <typename T>
    VaAny(T&& value) {
        emplace<std::decay_t<T>>(std::forward<T>(value));
    }

    /**
     * @brief Copy constructor for VaAny.
     *        Creates a new VaAny object as a copy of the given object.
     *        If the value is stored on the heap, it will be cloned.
     *        Otherwise, it will be copied into the small buffer optimization (SBO) storage.
     * @param other The VaAny object to copy from.
     */
    VaAny(const VaAny& other) {
        if (other.vtable) {
            vtable = other.vtable;
            type = other.type;
            onHeap = other.onHeap;

            if (onHeap) {
                heapPtr = vtable->clone(other.dataPtr());
            } else {
                vtable->copy(other.dataPtr(), &sboBuf);
            }
        }
    }

    /**
     * @brief Move constructor for VaAny.
     *        Creates a new VaAny object by transferring ownership of the value from
     *        the given object. If the value is stored on the heap, the pointer is
     *        moved. Otherwise, the value is moved into the SBO storage.
     * @param other The VaAny object to move from.
     */
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

    /**
     * @brief Destructor for VaAny.
     *        Cleans up any resources held by the object, including destroying
     *        the stored value and deallocating memory if the value is stored on the heap.
     */
    ~VaAny() { reset(); }

    /**
     * @brief Copy assignment operator for VaAny.
     *        Assigns the value from another VaAny object to this one.
     *        If the value is stored on the heap, it will be cloned.
     *        Otherwise, it will be copied into the small buffer optimization (SBO) storage.
     * @param other The VaAny object to copy from.
     * @return A reference to this VaAny object.
     */
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
                    vtable->copy(other.dataPtr(), &sboBuf);
                }
            }
        }
        return *this;
    }

    /**
     * @brief Move assignment operator for VaAny.
     *        Transfers ownership of the value from another VaAny object to this one.
     *        If the value is stored on the heap, the pointer is moved.
     *        Otherwise, the value is moved into the SBO storage.
     * @param other The VaAny object to move from.
     * @return A reference to this VaAny object.
     */
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

    /**
     * @brief Constructs a new value in the VaAny object, replacing any existing value.
     * @tparam T The type of the value to construct.
     * @param value The value to construct in the VaAny object.
     *
     * @details This method uses perfect forwarding to construct the value in-place.
     *          If the size and alignment of the type allow, the value is stored in
     *          the small buffer optimization (SBO) storage. Otherwise, it is allocated
     *          on the heap. Any previously stored value is destroyed before the new
     *          value is constructed.
     */
    template <typename T>
    void emplace(T&& value) {
        using U = std::decay_t<T>;
        const VTable* vt = makeVTable<U>();
        reset();

        type = &typeid(U);
        vtable = vt;

        if (sizeof(U) <= SBO_SIZE && alignof(U) <= alignof(AlignedBuffer)) {
            onHeap = false;
            new (&sboBuf) U(std::forward<T>(value));
        } else {
            onHeap = true;
            heapPtr = operator new(sizeof(U));
            new (heapPtr) U(std::forward<T>(value));
        }
    }

    /**
     * @brief Resets the VaAny object, destroying any stored value.
     * @details If a value is stored in the VaAny object, this method destroys it
     *          and deallocates any memory if the value was stored on the heap.
     *          After calling this method, the VaAny object will be empty.
     */
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

    /**
     * @brief Swaps the contents of this VaAny object with another.
     * @param other The VaAny object to swap with.
     *
     * @details This method exchanges the contents of the two VaAny objects.
     *          If the objects are the same, no operation is performed.
     */
    void swap(VaAny& other) noexcept {
        if (this == &other) return;
        VaAny temp = std::move(*this);
        *this = std::move(other);
        other = std::move(temp);
    }

    /**
     * @brief Retrieves the stored value as the specified type.
     * @tparam T The type to cast the stored value to.
     * @return A reference to the stored value of type T.
     *
     * @throws InvalidAnyCastError If the stored value is not of type T.
     * @details This method casts the stored value to the specified type T.
     *          If the stored value's type does not match T, an exception is thrown.
     */
    template <typename T>
    T& get() {
        if (!type || *type != typeid(T)) throw InvalidAnyCastError();
        return *static_cast<T*>(dataPtr());
    }

    /**
     * @brief Retrieves the stored value as the specified type (const version).
     * @tparam T The type to cast the stored value to.
     * @return A const reference to the stored value of type T.
     *
     * @throws InvalidAnyCastError If the stored value is not of type T.
     * @details This method casts the stored value to the specified type T.
     *          If the stored value's type does not match T, an exception is thrown.
     */
    template <typename T>
    const T& get() const {
        if (!type || *type != typeid(T)) throw InvalidAnyCastError();
        return *static_cast<const T*>(dataPtr());
    }

    /**
     * @brief Checks if the VaAny object contains a value.
     * @return True if the object contains a value, false otherwise.
     *
     * @details This method returns true if the VaAny object currently holds a value,
     *          and false if it is empty.
     */
    bool hasValue() const noexcept { return vtable != nullptr; }

    /**
     * @brief Checks if the stored value is of the specified type.
     * @tparam T The type to check against the stored value.
     * @return True if the stored value is of type T, false otherwise.
     *
     * @details This method compares the type of the stored value with the specified type T.
     *          If the stored value's type matches T, it returns true. Otherwise, it returns false.
     */
    template <typename T>
    bool isType() const {
        return type && *type == typeid(T);
    }

    /**
     * @brief Retrieves the type information of the currently stored value.
     * @return A reference to the std::type_info object representing the type of the stored value.
     *
     * @details If the VaAny object is empty, this method returns the type information for void.
     */
    TypeID& currentType() const { return type ? *type : typeid(void); }

    /**
     * @brief Provides a raw pointer to the stored value.
     * @return A void pointer to the stored value.
     *
     * @details This method returns a raw pointer to the stored value, allowing unsafe access.
     *          Use this method with caution, as it bypasses type safety.
     */
    void* unsafePtr() noexcept { return dataPtr(); }

    /**
     * @brief Provides a raw pointer to the stored value (const version).
     * @return A const void pointer to the stored value.
     *
     * @details This method returns a raw pointer to the stored value, allowing unsafe access.
     *          Use this method with caution, as it bypasses type safety.
     */
    const void* unsafePtr() const noexcept { return dataPtr(); }
};

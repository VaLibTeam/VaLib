// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <VaLib/Types/BasicTypedef.hpp>
#include <VaLib/Meta/BasicDefine.hpp>

#include <VaLib/Mem/__SmartPtrControlBlock.hpp>
#include <utility>
#include <memory>

template <typename T>
class VaWeakPtr;

/**
 * @brief A shared pointer for managing single objects with reference counting.
 * @tparam T The type of the object being managed.
 */
template <typename T>
class VaSharedPtr {
  public:
    using ControlBlock = va::detail::ControlBlock<T>; ///< Alias for the control block type.

  protected:
    ControlBlock* ctrl; ///< Pointer to the control block managing the shared object.

    /**
     * @brief Releases ownership of the managed object and updates reference counts.
     * @note If the shared count reaches zero, the managed object is deleted.
     * @note If both shared and weak counts reach zero, the control block is deleted.
     */
    void release() noexcept {
        if (!ctrl) return;

        if (ctrl->sharedCount.fetch_sub(1, std::memory_order_acq_rel) == 1) {
            if (ctrl->weakCount.load(std::memory_order_acquire) == 0) {
                delete ctrl;
            }
        }

        ctrl = nullptr;
    }

  protected friends:
    friend class VaWeakPtr<T>; ///< Grants VaWeakPtr access to protected members.

  public:
    /**
     * @brief Default constructor. Initializes with a null pointer.
     */
    VaSharedPtr() : ctrl(nullptr) {}

    /**
     * @brief Constructor that takes ownership of a raw pointer.
     * @param ptr A raw pointer to the object.
     * @warning The provided pointer must point to a dynamically allocated object (i.e., from the heap),
     *          not a stack-allocated object or a reference (e.g., &x). Using an invalid pointer will
     *          result in undefined behavior.
     */
    VaSharedPtr(T*&& ptr) {
        ctrl = new ControlBlock(ptr);
        ptr = nullptr;
    }

    /**
     * @brief Copy constructor. Increments the shared count of the control block.
     * @param other The VaSharedPtr to copy from.
     */
    VaSharedPtr(const VaSharedPtr& other) {
        ctrl = other.ctrl;
        if (ctrl) ctrl->sharedCount.fetch_add(1, std::memory_order_acq_rel);
    }

    /**
     * @brief Move constructor. Transfers ownership from another VaSharedPtr.
     * @param other The VaSharedPtr to move from.
     */
    VaSharedPtr(VaSharedPtr&& other) noexcept {
        ctrl = other.ctrl;
        other.ctrl = nullptr;
    }

    /**
     * @brief Destructor. Releases ownership of the managed object.
     */
    ~VaSharedPtr() { release(); }

    /**
     * @brief Creates a new VaSharedPtr by copying the provided value.
     * @param val The value to copy.
     * @return A new VaSharedPtr managing a copy of the value.
     */
    static VaSharedPtr New(const T& val) {
        return VaSharedPtr(new T(val));
    }

    /**
     * @brief Creates a new VaSharedPtr by moving the provided value.
     * @param val The value to move.
     * @return A new VaSharedPtr managing the moved value.
     */
    static VaSharedPtr New(T&& val) {
        return VaSharedPtr(new T(std::move(val)));
    }

    /**
     * @brief Creates a VaSharedPtr from a raw pointer.
     * @param ptr A raw pointer to the object.
     * @return A new VaSharedPtr managing the raw pointer.
     * @warning The provided pointer must point to a dynamically allocated object (i.e., from the heap),
     *          not a stack-allocated object or a reference (e.g., &x). Using an invalid pointer will
     *          result in undefined behavior.
     */
    static VaSharedPtr From(T* ptr) {
        return VaSharedPtr(std::move(ptr));
    }

    /**
     * @brief Copy assignment operator. Increments the shared count of the control block.
     * @param other The VaSharedPtr to copy from.
     * @return A reference to this VaSharedPtr.
     */
    VaSharedPtr& operator=(const VaSharedPtr& other) {
        if (this != &other) {
            release();
            ctrl = other.ctrl;
            if (ctrl) ctrl->sharedCount.fetch_add(1, std::memory_order_acq_rel);
        }
        return *this;
    }

    /**
     * @brief Move assignment operator. Transfers ownership from another VaSharedPtr.
     * @param other The VaSharedPtr to move from.
     * @return A reference to this VaSharedPtr.
     */
    VaSharedPtr& operator=(VaSharedPtr&& other) noexcept {
        if (this != &other) {
            release();
            ctrl = other.ctrl;
            other.ctrl = nullptr;
        }
        return *this;
    }

    /**
     * @brief Checks if the pointer is not null.
     * @return True if the pointer is not null, false otherwise.
     */
    inline explicit operator bool() const noexcept { return ctrl && ctrl->ptr; }

    /**
     * @brief Checks if the pointer is null.
     * @return True if the pointer is null, false otherwise.
     */
    inline bool isNull() const noexcept { return ctrl == nullptr || ctrl->ptr == nullptr; }

    /**
     * @brief Logical negation operator for null check.
     * @return True if the pointer is null, false otherwise.
     */
    inline bool operator!() const noexcept { return isNull(); }

    /**
     * @brief Equality operator to check if the managed pointer is null.
     * @param _ A nullptr value.
     * @return True if the managed pointer is null, false otherwise.
     */
    inline bool operator==(std::nullptr_t) { return isNull(); }

    /**
     * @brief Resets the managed object, releasing ownership.
     */
    inline void reset() noexcept { release(); }

    /**
     * @brief Swaps the contents of this VaSharedPtr with another.
     * @param other The VaSharedPtr to swap with.
     */
    inline void swap(VaSharedPtr& other) noexcept { std::swap(ctrl, other.ctrl); }

    /**
     * @brief Gets the raw pointer to the managed object.
     * @return The raw pointer.
     */
    inline T* get() const noexcept { return ctrl ? ctrl->ptr : nullptr; }

    /**
     * @brief Gets the raw pointer to the managed object (const version).
     * @return The raw pointer.
     */
    inline const T* cget() const noexcept { return ctrl ? ctrl->ptr : nullptr; }

    /**
     * @brief Dereferences the pointer to access the managed object.
     * @return A reference to the managed object.
     */
    inline T& operator*() const noexcept { return *(ctrl->ptr); }

    /**
     * @brief Accesses the managed object through the pointer.
     * @return The raw pointer to the managed object.
     */
    inline T* operator->() const noexcept { return ctrl->ptr; }

    /**
     * @brief Gets the number of shared references to the managed object.
     * @return The shared reference count.
     */
    inline Size useCount() const noexcept { return ctrl ? ctrl->sharedCount.load() : 0; }

    /**
     * @brief Checks if this is the only shared reference to the managed object.
     * @return True if this is the only shared reference, false otherwise.
     */
    inline bool isUnique() const noexcept { return useCount() == 1; }
};

/**
 * @brief A shared pointer for managing single objects with reference counting.
 * @tparam T The type of the object being managed.
 */
template <typename T>
class VaSharedPtr<T[]> {
  public:
    using ControlBlock = va::detail::ControlBlock<T[]>; ///< Alias for the control block type.

  protected:
    ControlBlock* ctrl; ///< Pointer to the control block managing the shared array.

    /**
     * @brief Releases ownership of the managed array and updates reference counts.
     * @note If the shared count reaches zero, the managed array is deleted.
     * @note If both shared and weak counts reach zero, the control block is deleted.
     */
    void release() noexcept {
        if (!ctrl) return;

        if (--ctrl->sharedCount == 0) {
            if (ctrl->weakCount == 0) {
                delete ctrl;
            }
        }

        ctrl = nullptr;
    }

  protected friends:
    friend class VaWeakPtr<T[]>; ///< Grants VaWeakPtr access to protected members.

  public:
    /// @brief Default constructor. Initializes with a null pointer.
    VaSharedPtr() : ctrl(nullptr) {}

    /**
     * @brief Constructor that takes ownership of a raw pointer to an array.
     * @param ptr A raw pointer to the array.
     * @warning The provided pointer must point to a dynamically allocated object (i.e., from the heap),
     *          not a stack-allocated object or a reference (e.g., &x). Using an invalid pointer will
     *          result in undefined behavior.
     */
    VaSharedPtr(T*&& ptr) {
        ctrl = new ControlBlock(ptr);
        ptr = nullptr;
    }

    /**
     * @brief Copy constructor. Increments the shared count of the control block.
     * @param other The VaSharedPtr to copy from.
     */
    VaSharedPtr(const VaSharedPtr& other) {
        ctrl = other.ctrl;
        if (ctrl) ctrl->sharedCount.fetch_add(1, std::memory_order_acq_rel);
    }

    /**
     * @brief Move constructor. Transfers ownership from another VaSharedPtr.
     * @param other The VaSharedPtr to move from.
     */
    VaSharedPtr(VaSharedPtr&& other) noexcept {
        ctrl = other.ctrl;
        other.ctrl = nullptr;
    }

    /**
     * @brief Destructor. Releases ownership of the managed array.
     */
    ~VaSharedPtr() { release(); }

    /**
     * @brief Creates a new VaUniquePtr by copying values from an initializer_list.
     * @param ilist The initializer_list of values to initialize the array with.
     * @return A new VaUniquePtr managing the initialized array.
     */
    static VaSharedPtr New(std::initializer_list<T> init) {
        T* ptr = new T[init.size()];
        std::copy(init.begin(), init.end(), ptr);
        return VaUniquePtr(ptr);
    }

    /**
     * @brief Creates a new VaUniquePtr managing an array of n default-initialized elements.
     * @param n The number of elements in the array.
     * @return A new VaUniquePtr managing the array.
     */
    static VaSharedPtr<T[]> New(Size n) {
        return VaSharedPtr<T[]>(new T[n]());
    }

    /**
     * @brief Creates a VaSharedPtr from a raw pointer to an array.
     * @param ptr A raw pointer to the array.
     * @return A new VaSharedPtr managing the raw pointer.
     * @warning The provided pointer must point to a dynamically allocated object (i.e., from the heap),
     *          not a stack-allocated object or a reference (e.g., &x). Using an invalid pointer will
     *          result in undefined behavior.
     */
    static VaSharedPtr From(T* ptr) {
        return VaSharedPtr(std::move(ptr));
    }

    /**
     * @brief Copy assignment operator. Increments the shared count of the control block.
     * @param other The VaSharedPtr to copy from.
     * @return A reference to this VaSharedPtr.
     */
    VaSharedPtr& operator=(const VaSharedPtr& other) {
        if (this != &other) {
            release();
            ctrl = other.ctrl;
            if (ctrl) ctrl->sharedCount.fetch_add(1, std::memory_order_acq_rel);
        }
        return *this;
    }

    /**
     * @brief Move assignment operator. Transfers ownership from another VaSharedPtr.
     * @param other The VaSharedPtr to move from.
     * @return A reference to this VaSharedPtr.
     */
    VaSharedPtr& operator=(VaSharedPtr&& other) noexcept {
        if (this != &other) {
            release();
            ctrl = other.ctrl;
            other.ctrl = nullptr;
        }
        return *this;
    }

    /**
     * @brief Checks if the pointer is not null.
     * @return True if the pointer is not null, false otherwise.
     */
    inline explicit operator bool() const noexcept { return ctrl && ctrl->ptr; }

    /**
     * @brief Checks if the pointer is null.
     * @return True if the pointer is null, false otherwise.
     */
    inline bool isNull() const noexcept { return ctrl == nullptr || ctrl->ptr == nullptr; }

    /**
     * @brief Logical negation operator for null check.
     * @return True if the pointer is null, false otherwise.
     */
    inline bool operator!() const noexcept { return isNull(); }

    /**
     * @brief Equality operator to check if the managed pointer is null.
     * @param _ A nullptr value.
     * @return True if the managed pointer is null, false otherwise.
     */
    inline bool operator==(std::nullptr_t) { return isNull(); }

    /**
     * @brief Resets the managed array, releasing ownership.
     */
    inline void reset() noexcept { release(); }

    /**
     * @brief Swaps the contents of this VaSharedPtr with another.
     * @param other The VaSharedPtr to swap with.
     */
    inline void swap(VaSharedPtr& other) noexcept { std::swap(ctrl, other.ctrl); }

    /**
     * @brief Gets the raw pointer to the managed array.
     * @return The raw pointer.
     */
    inline T* get() const noexcept { return ctrl ? ctrl->ptr : nullptr; }

    /**
     * @brief Gets the raw pointer to the managed array (const version).
     * @return The raw pointer.
     */
    inline const T* const cget() const noexcept { return ctrl ? ctrl->ptr : nullptr; }

    /**
     * @brief Dereferences the pointer to access the first element of the array.
     * @return A reference to the first element of the array.
     */
    inline T& operator*() const noexcept { return *(ctrl->ptr); }

    /**
     * @brief Accesses an element of the array by index.
     * @param index The index of the element.
     * @return A reference to the element at the specified index.
     */
    inline T& operator[](Size index) const noexcept { return ctrl->ptr[index]; }

    /**
     * @brief Gets the number of shared references to the managed array.
     * @return The shared reference count.
     */
    inline Size useCount() const noexcept { return ctrl ? ctrl->sharedCount.load() : 0; }

    /**
     * @brief Checks if this is the only shared reference to the managed array.
     * @return True if this is the only shared reference, false otherwise.
     */
    inline bool isUnique() const noexcept { return useCount() == 1; }
};

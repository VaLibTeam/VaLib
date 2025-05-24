// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <VaLib/Types/BasicTypedef.hpp>

#include <VaLib/Mem/SharedPtr.hpp>
#include <VaLib/Mem/__SmartPtrControlBlock.hpp>

/**
 * @brief A weak pointer for managing non-owning references to a shared object.
 * @tparam T The type of the object being referenced.
 */
template <typename T>
class VaWeakPtr {
  public:
    using ControlBlock = va::detail::ControlBlock<T>; ///< Alias for the control block type.

  protected:
    ControlBlock* ctrl; ///< Pointer to the control block managing the shared object.

    /**
     * @brief Releases the weak reference to the control block.
     * @note If both shared and weak counts reach zero, the control block is deleted.
     */
    void release() {
        if (!ctrl) return;
        if (--ctrl->weakCount == 0 && ctrl->sharedCount == 0) {
            delete ctrl;
        }

        ctrl = nullptr;
    }

    /**
     * @brief Promotes the weak pointer to a shared pointer if the object is still valid.
     * @return A VaSharedPtr managing the shared object, or an empty VaSharedPtr if expired.
     */
    VaSharedPtr<T> promote() const {
        VaSharedPtr<T> sp;
        if (ctrl && ctrl->sharedCount > 0) {
            sp.ctrl = ctrl;
            ctrl->sharedCount.fetch_add(1, std::memory_order_acq_rel);
        }

        return sp;
    }

  protected friends:
    friend class VaSharedPtr<T>; ///< Grants VaSharedPtr access to protected members.

  public:
    /**
     * @brief Default constructor. Initializes with a null pointer.
     */
    VaWeakPtr() : ctrl(nullptr) {}

    /**
     * @brief Constructs a VaWeakPtr from a VaSharedPtr.
     * @param shared The VaSharedPtr to create a weak reference to.
     */
    VaWeakPtr(const VaSharedPtr<T>& shared) {
        ctrl = shared.ctrl;
        if (ctrl) ctrl->weakCount++;
    }

    /**
     * @brief Copy constructor. Increments the weak count of the control block.
     * @param other The VaWeakPtr to copy from.
     */
    VaWeakPtr(const VaWeakPtr& other) {
        ctrl = other.ctrl;
        if (ctrl) ctrl->weakCount++;
    }

    /**
     * @brief Move constructor. Transfers ownership from another VaWeakPtr.
     * @param other The VaWeakPtr to move from.
     */
    VaWeakPtr(VaWeakPtr&& other) noexcept {
        ctrl = other.ctrl;
        other.ctrl = nullptr;
    }

    /**
     * @brief Destructor. Releases the weak reference to the control block.
     */
    ~VaWeakPtr() { release(); }

    /**
     * @brief Copy assignment operator. Increments the weak count of the control block.
     * @param other The VaWeakPtr to copy from.
     * @return A reference to this VaWeakPtr.
     */
    VaWeakPtr& operator=(const VaWeakPtr& other) {
        if (this != &other) {
            release();
            ctrl = other.ctrl;
            if (ctrl) ctrl->weakCount++;
        }
        return *this;
    }

    /**
     * @brief Move assignment operator. Transfers ownership from another VaWeakPtr.
     * @param other The VaWeakPtr to move from.
     * @return A reference to this VaWeakPtr.
     */
    VaWeakPtr& operator=(VaWeakPtr&& other) noexcept {
        if (this != &other) {
            release();
            ctrl = other.ctrl;
            other.ctrl = nullptr;
        }
        return *this;
    }

    /**
     * @brief Checks if the referenced object has been destroyed.
     * @return True if the object is no longer valid, false otherwise.
     */
    bool isExpired() const noexcept { return !ctrl || ctrl->sharedCount == 0; }

    /**
     * @brief Attempts to lock the weak pointer and obtain a shared pointer.
     * @return A VaSharedPtr managing the shared object, or an empty VaSharedPtr if expired.
     */
    VaSharedPtr<T> lock() const {
        if (isExpired()) return VaSharedPtr<T>();
        return promote();
    }
};

/**
 * @brief A weak pointer for managing non-owning references to a shared array.
 * @tparam T The type of the objects in the array being referenced.
 */
template <typename T>
class VaWeakPtr<T[]> {
  public:
    using ControlBlock = va::detail::ControlBlock<T[]>; ///< Alias for the control block type.

  protected:
    ControlBlock* ctrl; ///< Pointer to the control block managing the shared array.

    /**
     * @brief Releases the weak reference to the control block.
     * @note If both shared and weak counts reach zero, the control block is deleted.
     */
    void release() {
        if (!ctrl) return;
        if (--ctrl->weakCount == 0 && ctrl->sharedCount == 0) {
            delete ctrl;
        }

        ctrl = nullptr;
    }

    /**
     * @brief Promotes the weak pointer to a shared pointer if the array is still valid.
     * @return A VaSharedPtr managing the shared array, or an empty VaSharedPtr if expired.
     */
    VaSharedPtr<T[]> promote() const {
        VaSharedPtr<T[]> sp;
        if (ctrl && ctrl->sharedCount > 0) {
            sp.ctrl = ctrl;
            ctrl->sharedCount.fetch_add(1, std::memory_order_acq_rel);
        }

        return sp;
    }

  protected friends:
    friend class VaSharedPtr<T[]>; ///< Grants VaSharedPtr access to protected members.

  public:
    /// @brief Default constructor. Initializes with a null pointer.
    VaWeakPtr() : ctrl(nullptr) {}

    /**
     * @brief Constructs a VaWeakPtr from a VaSharedPtr managing an array.
     * @param shared The VaSharedPtr to create a weak reference to.
     */
    VaWeakPtr(const VaSharedPtr<T[]>& shared) {
        ctrl = shared.ctrl;
        if (ctrl) ctrl->weakCount++;
    }

    /**
     * @brief Copy constructor. Increments the weak count of the control block.
     * @param other The VaWeakPtr to copy from.
     */
    VaWeakPtr(const VaWeakPtr& other) {
        ctrl = other.ctrl;
        if (ctrl) ctrl->weakCount++;
    }

    /**
     * @brief Move constructor. Transfers ownership from another VaWeakPtr.
     * @param other The VaWeakPtr to move from.
     */
    VaWeakPtr(VaWeakPtr&& other) noexcept {
        ctrl = other.ctrl;
        other.ctrl = nullptr;
    }

    /**
     * @brief Destructor. Releases the weak reference to the control block.
     */
    ~VaWeakPtr() { release(); }

    /**
     * @brief Copy assignment operator. Increments the weak count of the control block.
     * @param other The VaWeakPtr to copy from.
     * @return A reference to this VaWeakPtr.
     */
    VaWeakPtr& operator=(const VaWeakPtr& other) {
        if (this != &other) {
            release();
            ctrl = other.ctrl;
            if (ctrl) ctrl->weakCount++;
        }
        return *this;
    }

    /**
     * @brief Move assignment operator. Transfers ownership from another VaWeakPtr.
     * @param other The VaWeakPtr to move from.
     * @return A reference to this VaWeakPtr.
     */
    VaWeakPtr& operator=(VaWeakPtr&& other) noexcept {
        if (this != &other) {
            release();
            ctrl = other.ctrl;
            other.ctrl = nullptr;
        }
        return *this;
    }

    /**
     * @brief Checks if the referenced array has been destroyed.
     * @return True if the array is no longer valid, false otherwise.
     */
    bool isExpired() const noexcept { return !ctrl || ctrl->sharedCount == 0; }

    /**
     * @brief Attempts to lock the weak pointer and obtain a shared pointer to the array.
     * @return A VaSharedPtr managing the shared array, or an empty VaSharedPtr if expired.
     */
    VaSharedPtr<T[]> lock() const {
        if (isExpired()) return VaSharedPtr<T[]>();
        return promote();
    }
};

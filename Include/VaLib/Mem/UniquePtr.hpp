// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <VaLib/Types/BasicTypedef.hpp>
#include <memory>

/**
 * @brief A unique pointer for managing single objects.
 * @tparam T The type of the object being managed.
 */
template <typename T>
class VaUniquePtr {
  protected:
    T* data; /// @brief Raw pointer to the managed object.

  public:
    VaUniquePtr() : data(nullptr) {}                /// @brief Default constructor. Initializes with a null pointer.
    VaUniquePtr(std::nullptr_t) : data(nullptr) {}  /// @brief Constructor that initializes with a null pointer.

    /**
     * @brief Constructor that takes ownership of a raw pointer.
     * @param ptr A raw pointer to the object.
     * @warning The provided pointer must point to a dynamically allocated object (i.e., from the heap),
     *          not a stack-allocated object or a reference (e.g., &x). Using an invalid pointer will
     *          result in undefined behavior.
     */
    VaUniquePtr(T*&& ptr) : data(ptr) { ptr = nullptr; }

    /**
     * @brief Deleted copy constructor to prevent copying.
     */
    VaUniquePtr(const VaUniquePtr&) = delete;

    /**
     * @brief Move constructor. Transfers ownership from another VaUniquePtr.
     * @param other The VaUniquePtr to move from.
     */
    VaUniquePtr(VaUniquePtr&& other) noexcept : data(other.data) { other.data = nullptr; }

    /**
     * @brief Destructor. Deletes the managed object.
     */
    ~VaUniquePtr() { delete data; }

    /**
     * @brief Creates a new VaUniquePtr by copying the provided value.
     * @param val The value to copy.
     * @return A new VaUniquePtr managing a copy of the value.
     */
    static VaUniquePtr New(const T& val) {
        return VaUniquePtr(new T(val));
    }

    /**
     * @brief Creates a new VaUniquePtr by moving the provided value.
     * @param val The value to move.
     * @return A new VaUniquePtr managing the moved value.
     */
    static VaUniquePtr New(T&& val) {
        return VaUniquePtr(new T(std::move(val)));
    }

    /**
     * @brief Creates a VaUniquePtr from a raw pointer.
     * @param ptr A raw pointer to the object.
     * @return A new VaUniquePtr managing the raw pointer.
     * @warning The provided pointer must point to a dynamically allocated object (i.e., from the heap),
     *          not a stack-allocated object or a reference (e.g., &x). Using an invalid pointer will
     *          result in undefined behavior.
     */
    static VaUniquePtr From(T* ptr) {
        return VaUniquePtr(std::move(ptr));
    }

    /**
     * @brief Deleted copy assignment operator to prevent copying.
     */
    VaUniquePtr& operator=(const VaUniquePtr&) = delete;

    /**
     * @brief Move assignment operator. Transfers ownership from another VaUniquePtr.
     * @param other The VaUniquePtr to move from.
     * @return A reference to this VaUniquePtr.
     */
    VaUniquePtr& operator=(VaUniquePtr&& other) noexcept {
        if (this != &other) {
            delete data;
            data = other.data;
            other.data = nullptr;
        }
        return *this;
    }

    /**
     * @brief Resets the managed object, optionally taking ownership of a new raw pointer.
     * @param newPtr A raw pointer to the new object (default is nullptr).
     */
    void reset(T*&& newPtr = nullptr) {
        delete data;
        data = newPtr;
    }

    /**
     * @brief Releases ownership of the managed object and returns the raw pointer.
     * @return The raw pointer to the managed object.
     */
    T* release() noexcept {
        T* temp = data;
        data = nullptr;
        return temp;
    }

    /**
     * @brief Checks if the pointer is not null.
     * @return True if the pointer is not null, false otherwise.
     */
    inline explicit operator bool() const noexcept { return data != nullptr; }

    /**
     * @brief Checks if the pointer is null.
     * @return True if the pointer is null, false otherwise.
     */
    inline bool isNull() const noexcept { return data == nullptr; }

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
     * @brief Gets the raw pointer to the managed object.
     * @return The raw pointer.
     */
    inline T* get() const noexcept { return data; }

    /**
     * @brief Gets the raw pointer to the managed object (const version).
     * @return The raw pointer.
     */
    inline const T& operator*() const noexcept { return *data; }
    inline T* operator->() const noexcept { return data; }
};

/**
 * @brief A unique pointer for managing arrays of objects.
 * @tparam T The type of the objects being managed.
 */
template <typename T>
class VaUniquePtr<T[]> {
  protected:
    T* data; /// @brief Raw pointer to the managed array.

  public:
    VaUniquePtr() : data(nullptr) {}                /// @brief Default constructor. Initializes with a null pointer.
    VaUniquePtr(std::nullptr_t) : data(nullptr) {}  /// @brief Constructor that initializes with a null pointer.

    /**
     * @brief Constructor that takes ownership of a raw pointer to an array.
     * @param ptr A raw pointer to the array.
     * @warning The provided pointer must point to a dynamically allocated object (i.e., from the heap),
     *          not a stack-allocated object or a reference (e.g., &x). Using an invalid pointer will
     *          result in undefined behavior.
     */
    VaUniquePtr(T*&& ptr) : data(ptr) { ptr = nullptr; }

    /**
     * @brief Deleted copy constructor to prevent copying.
     */
    VaUniquePtr(const VaUniquePtr&) = delete;

    /**
     * @brief Move constructor. Transfers ownership from another VaUniquePtr.
     * @param other The VaUniquePtr to move from.
     */
    VaUniquePtr(VaUniquePtr&& other) noexcept : data(other.data) { other.data = nullptr; }

    /**
     * @brief Destructor. Deletes the managed array.
     */
    ~VaUniquePtr() { delete[] data; }

    /**
     * @brief Creates a new VaUniquePtr by copying values from an initializer_list.
     * @param ilist The initializer_list of values to initialize the array with.
     * @return A new VaUniquePtr managing the initialized array.
     */
    static VaUniquePtr New(std::initializer_list<T> init) {
        T* ptr = new T[init.size()];
        std::copy(init.begin(), init.end(), ptr);
        return VaUniquePtr(ptr);
    }

    /**
     * @brief Creates a new VaUniquePtr managing an array of n default-initialized elements.
     * @param n The number of elements in the array.
     * @return A new VaUniquePtr managing the array.
     */
    static VaUniquePtr New(Size n) {
        return new T[n]();
    }

    /**
     * @brief Creates a VaUniquePtr from a raw pointer to an array.
     * @param ptr A raw pointer to the array.
     * @return A new VaUniquePtr managing the raw pointer.
     * @warning The provided pointer must point to a dynamically allocated object (i.e., from the heap),
     *          not a stack-allocated object or a reference (e.g., &x). Using an invalid pointer will
     *          result in undefined behavior.
     */
    static VaUniquePtr From(T* ptr) {
        return VaUniquePtr(std::move(ptr));
    }

    /**
     * @brief Deleted copy assignment operator to prevent copying.
     */
    VaUniquePtr& operator=(const VaUniquePtr&) = delete;

    /**
     * @brief Move assignment operator. Transfers ownership from another VaUniquePtr.
     * @param other The VaUniquePtr to move from.
     * @return A reference to this VaUniquePtr.
     */
    VaUniquePtr& operator=(VaUniquePtr&& other) noexcept {
        if (this != &other) {
            delete[] data;
            data = other.data;
            other.data = nullptr;
        }
        return *this;
    }

    /**
     * @brief Resets the managed array, optionally taking ownership of a new raw pointer.
     * @param newPtr A raw pointer to the new array (default is nullptr).
     */
    void reset(T*&& newPtr = nullptr) {
        delete[] data;
        data = newPtr;
    }

    /**
     * @brief Releases ownership of the managed array and returns the raw pointer.
     * @return The raw pointer to the managed array.
     */
    T* release() noexcept {
        T* temp = data;
        data = nullptr;
        return temp;
    }

    /**
     * @brief Checks if the pointer is not null.
     * @return True if the pointer is not null, false otherwise.
     */
    inline explicit operator bool() const noexcept { return data != nullptr; }

    /**
     * @brief Checks if the pointer is null.
     * @return True if the pointer is null, false otherwise.
     */
    inline bool isNull() const noexcept { return data == nullptr; }

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
     * @brief Gets the raw pointer to the managed array.
     * @return The raw pointer.
     */
    inline T* get() const noexcept { return data; }

    /**
     * @brief Gets the raw pointer to the managed array (const version).
     * @return The raw pointer.
     */
    inline const T* cget() const noexcept { return data; }

    /**
     * @brief Dereferences the pointer to access the first element of the array.
     * @return A reference to the first element of the array.
     */
    inline T& operator*() const noexcept { return *data; }

    /**
     * @brief Accesses an element of the array by index.
     * @param i The index of the element.
     * @return A reference to the element at the specified index.
     */
    inline T& operator[](Size i) const noexcept { return data[i]; }
};

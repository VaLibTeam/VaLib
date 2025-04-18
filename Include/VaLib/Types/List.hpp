// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <cstdlib>

#include <VaLib/Types/BasicConcepts.hpp>
#include <VaLib/Types/BasicTypedef.hpp>
#include <VaLib/Utils/BasicDefine.hpp>

#include <VaLib/Types/Error.hpp>

#include <initializer_list>
#include <type_traits>
#include <utility>

template <typename T>
class VaSlice;

template <typename T>
class VaList {
  protected:
    Size len; ///< Number of elements currently stored.
    Size cap; ///< Current capacity of the allocated buffer.
    T* data; ///< Pointer to the raw array of elements.

    /**
     * @brief Resizes the internal buffer to a new capacity.
     * @param newCap New capacity for the buffer.
     */
    void resize(Size newCap) {
        T* newData = static_cast<T*>(std::malloc(newCap * sizeof(T)));
        if (!newData) throw NullPointerError();

        if constexpr (std::is_trivially_copyable_v<T>) {
            std::memcpy(newData, data, len * sizeof(T));
        } else {
            for (Size i = 0; i < len; i++) {
                new (&newData[i]) T(std::move(data[i]));
                data[i].~T();
            }
        }

        std::free(data);

        data = newData;
        cap = newCap;
    }

    /**
     * @brief Doubles the capacity or sets it to an initial value if zero.
     */
    inline void expand() { resize(cap == 0 ? 4 : cap * 2); }

    /**
     * @brief Ensures there is enough capacity for a new element.
     */
    inline void update() {
        if (len >= cap) expand();
    }

    /**
     * @brief Destroys all elements if their type is not trivially destructible.
     */
    inline void deleteObjects() {
        if constexpr (!std::is_trivially_destructible_v<T>) {
            for (Size i = 0; i < len; i++) {
                data[i].~T();
            }
        }
    }

    friend class VaSlice<T>;

  public:
    /**
     * @brief Constructs an empty list.
     */
    VaList() : len(0), cap(0), data(nullptr) {}

    /**
     * @brief Constructs the list from an initializer list.
     * @param init List of elements to initialize the list with.
     */
    VaList(std::initializer_list<T> init) : len(init.size()), cap(init.size()) {
        data = static_cast<T*>(std::malloc(cap * sizeof(T)));
        Size i = 0;
        for (const T& val : init) {
            new (&data[i++]) T(val);
        }
    }

    /**
     * @brief Copy constructor.
     * @param other The list to copy from.
     */
    VaList(const VaList& other) : len(other.len), cap(other.cap) {
        data = static_cast<T*>(std::malloc(cap * sizeof(T)));
        if constexpr (std::is_trivially_copyable_v<T>) {
            std::memcpy(data, other.data, len * sizeof(T));
        } else {
            for (Size i = 0; i < len; i++) {
                new (&data[i]) T(other.data[i]); // Placement new + copy
            }
        }
    }

    /**
     * @brief Move constructor.
     * @param other The list to move from.
     */
    VaList(VaList&& other) noexcept : len(other.len), cap(other.cap), data(other.data) {
        other.data = nullptr;
        other.len = other.cap = 0;
    }

    /**
     * @brief Constructs the list from a variadic list of arguments.
     * @tparam Args Types of the arguments.
     * @param args Values to initialize the list with.
     */
    template <typename... Args,
              typename = std::enable_if_t<(std::is_constructible_v<T, Args> && ...)>>
    VaList(Args&&... args) : len(sizeof...(Args)), cap(sizeof...(Args)) {
        data = static_cast<T*>(std::malloc(cap * sizeof(T)));
        Size i = 0;
        ((new (&data[i++]) T(std::forward<Args>(args))), ...);
    }

    /**
     * @brief Destructor. Destroys all elements and frees memory.
     */
    ~VaList() {
        if (data) {
            deleteObjects();
            std::free(data);
        }
    }

    /**
     * @brief Copy assignment operator.
     * @param other The list to copy from.
     * @return Reference to this list.
     */
    VaList& operator=(const VaList& other) {
        if (this == &other) return *this;

        deleteObjects();
        std::free(data);

        len = other.len;
        cap = other.cap;
        data = static_cast<T*>(std::malloc(cap * sizeof(T)));
        if constexpr (std::is_trivially_copyable_v<T>) {
            std::memcpy(data, other.data, len * sizeof(T));
        } else {
            for (Size i = 0; i < len; i++) {
                new (&data[i]) T(other.data[i]);
            }
        }
        return *this;
    }

    /**
     * @brief Move assignment operator.
     * @param other The list to move from.
     * @return Reference to this list.
     */
    VaList& operator=(VaList&& other) noexcept {
        if (this == &other) return *this;
        std::free(data);

        this->len = other.len;
        this->cap = other.cap;
        this->data = other.data;

        other.data = nullptr;
        other.len = other.cap = 0;
        return *this;
    }

    /**
     * @brief Returns a new list that is the concatenation of this and another list.
     * @param other The list to concatenate.
     * @return New combined list.
     */
    VaList operator+(const VaList& other) const {
        VaList result;
        result.resize(len + other.len);
        result.len = len + other.len;
        for (Size i = 0; i < len; i++) {
            result.data[i] = data[i];
        }
        for (Size i = 0; i < other.len; i++) {
            result.data[len + i] = other.data[i];
        }
        return result;
    }

    /**
     * @brief Appends another list to this one.
     * @param other The list to append.
     * @return Reference to this list.
     */
    VaList& operator+=(const VaList& other) {
        resize(len + other.len);
        for (Size i = 0; i < other.len; i++) {
            data[len + i] = other.data[i];
        }
        len += other.len;
        return *this;
    }

    /**
     * @brief Compares two lists for equality.
     * @param other The list to compare with.
     * @return True if the lists are equal, false otherwise.
     */
    bool operator==(const VaList& other) const {
        if (len != other.len) return false;
        if constexpr (!va::HasEqualityOperatorV<T>) {
            return std::memcmp(data, other.data, len * sizeof(T)) == 0;
        } else {
            for (Size i = 0; i < len; i++) {
                if (data[i] != other.data[i]) return false;
            }
            return true;
        }
    }

    /**
     * @brief Compares two lists for inequality.
     * @param other The list to compare with.
     * @return True if the lists are not equal, false otherwise.
     */
    bool operator!=(const VaList& other) const { return !(*this == other); }

    /**
     * @brief Ensures the internal capacity is at least the specified amount.
     * @param minCap Minimum required capacity.
     */
    inline void reserve(Size minCap) {
        if (minCap > cap) resize(minCap);
    }

    /**
     * @brief Appends a copy of an element to the end of the list.
     * @param elm The element to append.
     */
    void append(const T& elm) {
        update();
        new (&data[len++]) T(elm);
    }

    /**
     * @brief Appends an element to the end of the list using move semantics.
     * @param elm The element to move.
     */
    void append(T&& elm) {
        update();
        new (&data[len++]) T(std::move(elm));
    }

    /**
     * @brief Inserts an element at the beginning of the list.
     * @param elm The element to prepend.
     */
    void prepend(const T& elm) { insert(0, elm); }

    /**
     * @brief Inserts a moved element at the beginning of the list.
     * @param elm The element to move.
     */
    void prepend(T&& elm) { insert(0, elm); }

    /**
     * @brief Appends all elements from another list to the end.
     * @param other The list to extend from.
     */
    void extend(const VaList& other) { *this += other; }

    /**
     * @brief Constructs an element in place at the end of the list.
     * @tparam Args Types of constructor arguments.
     * @param args Arguments to forward to the constructor.
     * @return Reference to the newly added element.
     */
    template <typename... Args>
    T& emplace(Args&&... args) {
        update();
        new (&data[len]) T(std::forward<Args>(args)...);
        return data[len++];
    }

    /**
     * @brief Accesses an element by index (unchecked).
     * @param index Index of the element.
     * @return Reference to the element.
     */
    T& operator[](Size index) { return data[index]; }

    /**
     * @brief Accesses an element by index (unchecked).
     * @param index Index of the element.
     * @return Const reference to the element.
     */
    const T& operator[](Size index) const { return data[index]; }

    /**
     * @brief Accesses an element by index with bounds checking.
     * @param index Index of the element.
     * @return Reference to the element.
     *
     * @throws IndexOutOfRangeError If index is out of bounds.
     */
    T& at(Size index) {
        if (index >= len) throw IndexOutOfRangeError(len, index);
        return data[index];
    }

    /**
     * @brief Accesses an element by index with bounds checking.
     * @param index Index of the element.
     * @return Const reference to the element.
     *
     * @throws IndexOutOfRangeError If index is out of bounds.
     */
    const T& at(Size index) const {
        if (index >= len) throw IndexOutOfRangeError(len, index);
        return data[index];
    }

    /**
     * @brief Inserts an element at the specified index.
     * @param index Position to insert the element at.
     * @param value The element to insert (moved).
     *
     * @throws IndexOutOfRangeError If index is out of bounds.
     */
    void insert(Size index, T value) {
        if (index > len) throw IndexOutOfRangeError(len, index);
        update();
        for (Size i = len; i > index; i--) {
            new (&data[i]) T(std::move(data[i - 1]));
            data[i - 1].~T();
        }
        new (&data[index]) T(std::move(value));
        len++;
    }

    /**
     * @brief Constructs an element in place at the specified index.
     * @tparam Args Types of constructor arguments.
     * @param index Position to insert the new element.
     * @param args Arguments to forward to the constructor.
     * @return Reference to the newly inserted element.
     *
     * @throws IndexOutOfRangeError If index is out of bounds.
     */
    template <typename... Args>
    T& insertEmplace(Size index, Args&&... args) {
        if (index > len) throw IndexOutOfRangeError(len, index);
        update();

        for (Size i = len; i > index; i--) {
            new (&data[i]) T(std::move(data[i - 1]));
            data[i - 1].~T();
        }

        new (&data[index]) T(std::forward<Args>(args)...);
        len++;
        return data[index];
    }

    /**
     * @brief Deletes the element at the specified index.
     * @param index Index of the element to delete.
     *
     * @throws IndexOutOfRangeError If index is out of bounds.
     */
    void del(Size index) {
        if (index >= len) throw IndexOutOfRangeError(len, index);
        data[index].~T();

        for (Size i = index; i < len - 1; i++) {
            new (&data[i]) T(std::move(data[i + 1]));
            data[i + 1].~T();
        }
        len--;
    }

    /**
     * @brief Shrinks the internal capacity to fit the current size.
     */
    inline void shrink() { resize(len + 1); }

    /**
     * @brief Creates a slice from the given start index to the end.
     * @param start Starting index (can be negative).
     * @return A new list containing the sliced elements.
     *
     * @throws IndexOutOfRangeError If start is out of bounds.
     */
    VaList sliceFrom(int32 start) const {
        if (start < 0) start += len; // handle negative indices
        if (start < 0 || start >= len) throw IndexOutOfRangeError(len, start);
        VaList result;
        result.reserve(len - start);
        for (Size i = start; i < len; i++) {
            result.append(data[i]);
        }
        return result;
    }

    /**
     * @brief Creates a slice from the beginning to the given end index.
     * @param end Ending index (can be negative).
     * @return A new list containing the sliced elements.
     *
     * @throws IndexOutOfRangeError If end is out of bounds.
     */
    VaList sliceTo(int32 end) const {
        if (end < 0) end += len; // handle negative indices
        if (end < 0 || end > len) throw IndexOutOfRangeError(len, end);
        VaList result;
        result.reserve(end);
        for (Size i = 0; i < end; i++) {
            result.append(data[i]);
        }
        return result;
    }

    /**
     * @brief Creates a slice between the specified start and end indices.
     * @param start Start index (can be negative).
     * @param end End index (can be negative).
     * @return A new list containing the sliced elements.
     *
     * @throws IndexOutOfRangeError If indices are invalid or out of bounds.
     */
    VaList slice(int32 start, int32 end) const {
        if (start < 0) start += len;
        if (end < 0) end += len;

        if (start < 0) start = 0;
        if (end > len) end = len;
        if (start > end) throw IndexOutOfRangeError(len, start > end ? start : end);

        VaList result;
        result.reserve(end - start);
        for (Size i = start; i < end; i++) {
            result.append(data[i]);
        }
        return result;
    }

    /**
     * @brief Removes and returns the last element of the list.
     * @return The removed element.
     *
     * @throws ValueError If the list is empty.
     */
    T pop() {
        if (len == 0) {
            throw ValueError("pop() on empty list");
        }

        T value = std::move(data[len - 1]);
        data[len - 1].~T();
        len--;
        return value;
    }

#ifdef VaLib_USE_CONCEPTS
    template <va::Addable A = T>
    A sum() const {
        if (len <= 0) return A{};
        A v = at(0);
        for (Size i = 1; i < len; i++) {
            v += at(i);
        }
        return v;
    }
#endif

    /**
     * @brief Joins all elements into a single VaString using the given separator.
     * @tparam U Ensures this method is only enabled if T is VaString.
     * @param sep Separator to insert between elements.
     * @return Concatenated string.
     */
    template <typename U = T>
    std::enable_if_t<std::is_same_v<U, VaString>, VaString> join(const VaString& sep = "") const {
        if (len == 0) return VaString();
        VaString result = data[0];
        for (Size i = 1; i < len; i++) {
            result += sep + data[i];
        }
        return result;
    }

    /**
     * @brief Returns a pointer to the internal data array.
     * @return Pointer to the data.
     */
    inline T* getData() { return data; }

    /**
     * @brief Returns a const pointer to the internal data array.
     * @return Const pointer to the data.
     */
    inline const T* getData() const { return data; }

    /**
     * @brief Checks if the list is empty.
     * @return True if the list has no elements, false otherwise.
     */
    inline bool isEmpty() const { return len <= 0; }

    /**
     * @brief Converts the list to a boolean.
     * @return True if the list is not empty.
     */
    inline explicit operator bool() const { return len > 0; }

    /**
     * @brief Clears the list, destroying all elements and releasing memory.
     */
    void clear() {
        deleteObjects();
        std::free(data);
        data = nullptr;
        len = cap = 0;
    }

    inline T* begin() { return data; }
    inline T* end() { return data + len; }
    inline const T* begin() const { return data; }
    inline const T* end() const { return data + len; }

    /**
     * @brief Returns the number of elements in the list.
     * @param list The list to query.
     * @return Size of the list.
     */
    friend inline Size len(const VaList& list) { return list.len; }

    /**
     * @brief Returns the capacity of the list.
     * @param list The list to query.
     * @return Capacity of the list.
     */
    friend inline Size cap(const VaList& list) { return list.cap; }
};

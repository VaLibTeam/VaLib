// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#ifdef VaLib_USE_CONCEPTS
#include <VaLib/Types/BasicConcepts.hpp>
#endif

#include <VaLib/Types/BasicTypedef.hpp>
#include <VaLib/Utils/BasicDefine.hpp>

#include <VaLib/Types/Error.hpp>
#include <VaLib/Types/Pair.hpp>

#include <algorithm>
#include <cstdlib>
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
    T* data;  ///< Pointer to the raw array of elements.

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

  protected friends:
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
        for (const T& val: init) {
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
     * @brief Constructs an element in place at the end of the list.
     * @tparam Args Types of constructor arguments.
     * @param args Arguments to forward to the constructor.
     * @return Reference to the newly added element.
     */
    template <typename... Args>
    T& appendEmplace(Args&&... args) {
        update();
        new (&data[len]) T(std::forward<Args>(args)...);
        return data[len++];
    }

    template <typename... Args>
    [[ deprecated("Use appendEmplace") ]]
    T& emplace(Args&&... args) {
        update();
        new (&data[len]) T(std::forward<Args>(args)...);
        return data[len++];
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
     * @brief Constructs an element in place at the beginning of the list.
     * @tparam Args Types of constructor arguments.
     * @param args Arguments to forward to the constructor.
     * @return Reference to the newly added element.
     */
    template <typename... Args>
    T& prependEmplace(Args&&... args) {
        return insertEmplace(0, std::forward<Args>(args)...);
    }

    /**
     * @brief Appends all elements from another list to the end.
     * @param other The list to extend from.
     */
    inline void extend(const VaList& other) { *this += other; }

    /**
     * @brief Inserts an element at the specified index.
     * @param index Position to insert the element at.
     * @param value The element to insert (moved).
     *
     * @note This operation has O(n) time complexity as it requires shifting all elements after
     *       the insertion point. For performance critical code, consider using append() instead
     *       if possible, or batch insert operations together (@ref insertList).
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
     * @brief Inserts all elements from another list at the specified index.
     * @param index Position to insert the elements at.
     * @param other The list to insert.
     *
     * @throws IndexOutOfRangeError If index is out of bounds.
     */
    void insertList(Size index, const VaList& other) {
        if (index > len) throw IndexOutOfRangeError(len, index);
        if (other.len == 0) return;

        Size newLen = len + other.len;
        if (newLen > cap) resize(newLen);

        for (Size i = len - 1; i >= index && i != (Size)-1; i--) {
            new (&data[i + other.len]) T(std::move(data[i]));
            data[i].~T();
        }

        for (Size i = 0; i < other.len; i++) {
            new (&data[index + i]) T(other.data[i]);
        }

        len = newLen;
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

    /**
     * @brief Removes and returns the element at the specified index.
     * @param index Index of the element to remove.
     * @return The removed element.
     *
     * @throws IndexOutOfRangeError If index is out of bounds.
     */
    T pop(Size index) {
        if (index >= len) throw IndexOutOfRangeError(len, index);
        T value = std::move(data[index]);
        data[index].~T();

        for (Size i = index; i < len - 1; i++) {
            new (&data[i]) T(std::move(data[i + 1]));
            data[i + 1].~T();
        }

        len--;
        return value;
    }

    /**
     * @brief Accesses an element by index (unchecked).
     * @param index Index of the element.
     * @return Reference to the element.
     */
    inline T& get(Size index) { return data[index]; }

    /**
     * @brief Accesses an element by index (unchecked).
     * @param index Index of the element.
     * @return Const reference to the element.
     */
    inline const T& get(Size index) const { return data[index]; }

    /**
     * @brief Accesses an element by index (unchecked).
     * @param index Index of the element.
     * @return Reference to the element.
     */
    inline T& operator[](Size index) { return data[index]; }

    /**
     * @brief Accesses an element by index (unchecked).
     * @param index Index of the element.
     * @return Const reference to the element.
     */
    inline const T& operator[](Size index) const { return data[index]; }

    /**
     * @brief Accesses an element by index with bounds checking.
     * @param index Index of the element.
     * @return Reference to the element.
     *
     * @throws IndexOutOfRangeError If index is out of bounds.
     */
    inline T& at(Size index) {
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
    inline const T& at(Size index) const {
        if (index >= len) throw IndexOutOfRangeError(len, index);
        return data[index];
    }

    /**
     * @brief Returns a reference to the first element in the list.
     * @return Reference to the first element.
     * @throws IndexError If the list is empty.
     */
    inline T& front() {
        if (len <= 0) throw IndexError("front() on empty list");
        return data[0];
    }

    /**
     * @brief Returns a const reference to the first element in the list.
     * @return Const reference to the first element.
     * @throws IndexError If the list is empty.
     */
    inline const T& front() const {
        if (len <= 0) throw IndexError("front() on empty list");
        return data[0];
    }

    /**
     * @brief Returns a reference to the last element in the list.
     * @return Reference to the last element.
     * @throws IndexError If the list is empty.
     */
    inline T& back() {
        if (len <= 0) throw IndexError("back() on empty list");
        return data[len - 1];
    }

    /**
     * @brief Returns a const reference to the last element in the list.
     * @return Const reference to the last element.
     * @throws IndexError If the list is empty.
     */
    inline const T& back() const {
        if (len <= 0) throw IndexError("back() on empty list");
        return data[len - 1];
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

#ifdef VaLib_USE_CONCEPTS
    template <va::Addable A = T>
    A sum() const {
        if (len <= 0) return A{};
        A v = at(0);
        for (Size i = 1; i < len; i++) {
            v += data[i];
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

    template <typename U = T>
    std::enable_if_t<std::is_invocable_r_v<bool, U>, bool> all() const {
        for (int i = 0; i < len; i++) {
            if (!static_cast<bool>(data[i])) {
                return false;
            }
        }

        return true;
    }

    template <typename U = T>
    std::enable_if_t<std::is_invocable_r_v<bool, U>, bool> any() const {
        for (int i = 0; i < len; i++) {
            if (static_cast<bool>(data[i])) {
                return true;
            }
        }
        return false;
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

  public operators:
    /**
     * @brief Returns a new list that is the concatenation of this and another list.
     * @param other The list to concatenate.
     * @return New combined list.
     */
    friend VaList operator+(const VaList& lhs, const VaList& rhs) {
        VaList result;
        result.resize(lhs.len + rhs.len);
        result.len = lhs.len + rhs.len;
        for (Size i = 0; i < lhs.len; i++) {
            result.data[i] = lhs.data[i];
        }
        for (Size i = 0; i < rhs.len; i++) {
            result.data[lhs.len + i] = rhs.data[i];
        }

        return result;
    }

    /**
     * @brief Appends another list to this one.
     * @param other The list to append.
     * @return Reference to this list.
     */
    friend VaList& operator+=(VaList& lhs, const VaList& rhs) {
        lhs.resize(lhs.len + rhs.len);
        for (Size i = 0; i < rhs.len; i++) {
            lhs.data[lhs.len + i] = rhs.data[i];
        }
        lhs.len += rhs.len;
        return lhs;
    }

    /**
     * @brief Compares two lists for equality.
     * @param other The list to compare with.
     * @return True if the lists are equal, false otherwise.
     */
    friend bool operator==(const VaList& lhs, const VaList& rhs) {
        if (lhs.len != rhs.len) return false;
        if constexpr (!va::HasEqualityOperatorV<T>) {
            return std::memcmp(lhs.data, rhs.data, lhs.len * sizeof(T)) == 0;
        } else {
            for (Size i = 0; i < lhs.len; i++) {
                if (lhs.data[i] != rhs.data[i]) return false;
            }
            return true;
        }
    }

    /**
     * @brief Compares two lists for inequality.
     * @param other The list to compare with.
     * @return True if the lists are not equal, false otherwise.
     */
    friend bool operator!=(const VaList& lhs, const VaList& rhs) { return !(lhs == rhs); }

  public friends:
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

  public iterators:
    using Iterator = T*;
    using ConstIterator = const T*;
    using ReverseIterator = std::reverse_iterator<Iterator>;
    using ConstReverseIterator = std::reverse_iterator<ConstIterator>;

    inline Iterator begin() { return data; }
    inline Iterator end() { return data + len; }

    inline ConstIterator begin() const { return data; }
    inline ConstIterator end() const { return data + len; }

    inline ConstIterator cbegin() const { return data; }
    inline ConstIterator cend() const { return data + len; }

    inline ReverseIterator rbegin() { return ReverseIterator(end()); }
    inline ReverseIterator rend() { return ReverseIterator(begin()); }

    inline ConstReverseIterator rbegin() const { return ConstReverseIterator(end()); }
    inline ConstReverseIterator rend() const { return ConstReverseIterator(begin()); }

    inline ConstReverseIterator crbegin() const { return ConstReverseIterator(cend()); }
    inline ConstReverseIterator crend() const { return ConstReverseIterator(cbegin()); }
};

namespace va {

/**
 * @brief Applies a transformation function to each element of the input list and returns a new list.
 * @tparam Old Input type.
 * @tparam New Output type.
 * @param mod Function that transforms elements from Old to New.
 * @param data Input list.
 * @return A new VaList containing the transformed elements.
 */
template <typename Old, typename New>
VaList<New> map(Function<Old, New> mod, const VaList<Old>& data) {
    VaList<New> result;
    result.reserve(len(data));

    for (Size i = 0; i < len(data); i++) {
        result.append(mod(data[i]));
    }

    return result;
}

/**
 * @brief Returns a new list containing only the elements that satisfy the predicate.
 * @tparam T Element type.
 * @param predicate Function that returns true for elements to keep.
 * @param data Input list.
 * @return A new VaList containing the filtered elements.
 */
template <typename T>
VaList<T> filter(Function<T, bool> predicate, const VaList<T>& data) {
    VaList<T> result;
    for (Size i = 0; i < len(data); i++) {
        if (predicate(data[i])) {
            result.append(data[i]);
        }
    }

    return result;
}

/**
 * @brief Reduces the list to a single value by applying a reducer function.
 * @tparam T Element type.
 * @tparam R Accumulator/result type.
 * @param reducer Function taking accumulator and element, returning new accumulator.
 * @param data Input list.
 * @param initial Initial value for the accumulator.
 * @return The final reduced value.
 */
template <typename T, typename R>
R reduce(Function<R, R, T> reducer, const VaList<T>& data, R initial) {
    R acc = initial;
    for (Size i = 0; i < len(data); i++) {
        acc = reducer(acc, data[i]);
    }
    return acc;
}

/**
 * @brief Returns a new list of (index, element) pairs.
 * @tparam T Element type.
 * @param data Input list.
 * @return A VaList of VaPair<Size, T>.
 */
template <typename T>
VaList<VaPair<Size, T>> enumerate(const VaList<T>& data) {
    VaList<VaPair<Size, T>> result;
    result.reserve(len(data));
    for (Size i = 0; i < len(data); i++) {
        result.append({i, data[i]});
    }

    return result;
}

/**
 * @brief Combines two lists element-wise into a list of pairs.
 * @tparam T1 Type of the first list.
 * @tparam T2 Type of the second list.
 * @param a First input list.
 * @param b Second input list.
 * @return A VaList of VaPair<T1, T2>.
 *
 * @note Truncates to the shorter list.
 */
template <typename T1, typename T2>
VaList<VaPair<T1, T2>> zip(const VaList<T1>& a, const VaList<T2>& b) {
    Size count = std::min(len(a), len(b));

    VaList<VaPair<T1, T2>> result;
    result.reserve(count);
    for (Size i = 0; i < count; i++) {
        result.emplace(a[i], b[i]);
    }

    return result;
}

/**
 * @brief Returns a reversed copy of the input list.
 * @tparam T Element type.
 * @param data Input list.
 * @return A new VaList with elements in reverse order.
 */
template <typename T>
VaList<T> reversed(const VaList<T>& data) {
    VaList<T> result;
    result.reserve(len(data));
    for (Size i = len(data); i-- > 0;) {
        result.append(data[i]);
    }
    return result;
}

} // namespace va

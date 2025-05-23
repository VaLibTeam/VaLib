// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <VaLib/Meta/BasicDefine.hpp>
#include <VaLib/Types/BasicTypedef.hpp>
#include <VaLib/Types/TypeTraits.hpp>

#include <VaLib/Types/Error.hpp>
#include <VaLib/Types/List.hpp>

/**
 * @class VaSlice A lightweight view into a contiguous sequence of elements.
 * @tparam T Type of elements in the slice
 *
 * @note VaSlice doesn't own the data it points to - it's just a view.
 * @warning The user must ensure the underlying data remains valid while VaSlice is used.
 */
template <typename T>
class VaSlice {
  protected:
    T* data;  ///< Pointer to the first element in the slice
    Size len; ///< Number of elements in the slice

  public:
    /**
     * @brief Construct from pointer and size
     * @param data Pointer to the first element
     * @param size Number of elements
     *
     * @note The constructor doesn't perform any allocation
     * @warning The caller must ensure the pointer remains valid for the slice's lifetime
     */
    VaSlice(T* data, Size size) : data(data), len(size) {}

    /**
     * @brief Construct from pointer range
     * @param begin Pointer to the first element
     * @param end Pointer to one past the last element
     *
     * @note The range is [begin, end) - end is not included
     * @warning Undefined behavior if end comes before begin
     */
    VaSlice(T* begin, T* end) : data(begin), len(end - begin) {}

    /**
     * @brief Construct from VaList
     * @param list VaList to create a view of
     *
     * @note This creates a view of the entire VaList
     */
    VaSlice(VaList<T>& list) : data(list.dataPtr()), len(list.getLength()) {}

    template <typename U = T, typename = tt::EnableIf< tt::IsSame<U, char> >>
    VaSlice(VaString& str) : data(str.dataPtr()), len(str.getLength()) {}

    /**
      * @brief Construct from any container with data() and size() methods
      * @tparam C Container type
      * @param container Container to create view of
      *
      * @note Works with std::vector, std::array, and similar STL containers
      */
    template <typename C>
    VaSlice(C& container) : data(container.data()), len(container.size()) {}

    /**
     * @brief Construct from any const container with data() and size() methods
     * @tparam C Container type
     * @param container Container to create view of
     *
     * @note Creates a const view of the container's data
     */
    template <typename C>
    VaSlice(const C& container) : data(container.data()), len(container.size()) {}

    /**
     * @brief Construct from C-style array
     * @tparam N Size of the array
     * @param arr Array to create view of
     *
     * @note The array size is automatically deduced
     */
    template <Size N>
    VaSlice(T (&arr)[N]) : data(arr), len(N) {}

    /// @brief Default copy constructor
    VaSlice(const VaSlice&) = default;

    /// @brief Default move constructor
    VaSlice(VaSlice&&) = default;

    /// @brief Default copy assignment
    VaSlice& operator=(const VaSlice&) = default;

    /// @brief Default move assignment
    VaSlice& operator=(VaSlice&&) = default;

    /**
     * @brief Access element without bounds checking
     * @param index Position of the element
     * @return Reference to the element
     *
     * @warning No bounds checking is performed - UB if index is invalid
     */
    inline T& get(Size index) { return data[index]; }

    /**
     * @brief Access const element without bounds checking
     * @param index Position of the element
     * @return Const reference to the element
     *
     * @warning No bounds checking is performed - UB if index is invalid
     */
    inline const T& get(Size index) const { return data[index]; }

    /**
     * @brief Access element without bounds checking
     * @param index Position of the element
     * @return Reference to the element
     *
     * @warning No bounds checking is performed - UB if index is invalid
     */
    inline T& operator[](Size index) { return get(index); }

    /**
     * @brief Access const element without bounds checking
     * @param index Position of the element
     * @return Const reference to the element
     *
     * @warning No bounds checking is performed - UB if index is invalid
     */
    inline const T& operator[](Size index) const { return get(index); }

    /**
     * @brief Access element with bounds checking
     * @param index Position of the element
     * @return Reference to the element
     *
     * @throws IndexOutOfRangeError if index is out of bounds
     * @note Safer but slower than operator[]
     */
    inline T& at(int32 index) {
        if (index < 0) index += len; // wrap negative indices
        if (index < 0 || static_cast<Size>(index) >= len) throw IndexOutOfRangeError(len, index);
        return get(index);
    }

    /**
     * @brief Access const element with bounds checking
     * @param index Position of the element
     * @return Const reference to the element
     *
     * @throws IndexOutOfRangeError if index is out of bounds
     * @note Safer but slower than operator[]
     */
    inline const T& at(int32 index) const {
        if (index < 0) index += len; // wrap negative indices
        if (index < 0 || index >= len) throw IndexOutOfRangeError(len, index);
        return get(index);
    }

    /**
     * @brief Set element at specified index
     * @param index Position of the element
     * @param value Value to set
     *
     * @throws IndexOutOfRangeError if index is out of bounds
     * @note Supports negative indices for wrapping
     */
    void set(int32 index, const T& value) {
        if (index < 0) index += len; // wrap negative indices
        if (index < 0 || index >= len) throw IndexOutOfRangeError(len, index);
        data[index] = value;
    }

    /**
     * @brief Set element at specified index (move version)
     * @param index Position of the element
     * @param value Value to set (moved)
     *
     * @throws IndexOutOfRangeError if index is out of bounds
     * @note Supports negative indices for wrapping
     */
    void set(int32 index, T&& value) {
        if (index < 0) index += len; // wrap negative indices
        if (index < 0 || index >= len) throw IndexOutOfRangeError(len, index);
        data[index] = std::move(value);
    }

    /**
     * @brief Get raw pointer to the data
     * @return Pointer to the first element
     */
    T* dataPtr() noexcept { return data; }

    /**
     * @brief Get const raw pointer to the data
     * @return Const pointer to the first element
     */
    const T* dataPtr() const noexcept { return data; }

    /**
     * @brief Get size in bytes
     * @return Total size of the slice in bytes
     *
     * @note Computed as len * sizeof(T)
     */
    Size sizeBytes() const noexcept { return len * sizeof(T); }

    /**
     * @brief Check if slice is empty
     * @return true if slice has no elements, false otherwise
     */
    bool isEmpty() const noexcept { return len == 0; }

    /**
     * @brief Access first element
     * @return Reference to first element

     * @throws ValueError if slice is empty
     * @note Equivalent to slice[0] when not empty
     */
    T& front() {
        if (isEmpty()) throw ValueError("front() called on empty slice");
        return data[0];
    }

    /**
     * @brief Access last element
     * @return Reference to last element
     *
     * @throws ValueError if slice is empty
     */
    T& back() {
        if (isEmpty()) throw ValueError("back() called on empty slice");
        return data[len - 1];
    }

    /**
     * @brief Access first element (const version)
     * @return Const reference to first element
     *
     * @throws ValueError if slice is empty
     */
    const T& front() const {
        if (isEmpty()) throw ValueError("front() called on empty slice");
        return data[0];
    }

    /**
     * @brief Access last element (const version)
     * @return Const reference to last element
     *
     * @throws ValueError if slice is empty
     */
    const T& back() const {
        if (isEmpty()) throw ValueError("back() called on empty slice");
        return data[len - 1];
    }

    /**
     * @brief Create subslice with specified offset and count
     * @param offset Starting position of subslice
     * @param count Number of elements in subslice
     * @return New VaSlice representing the subrange

     * @throws IndexOutOfRangeError if offset + count exceeds bounds
     * @note The subslice is a view of the same underlying data
     */
    VaSlice subslice(Size offset, Size count) {
        if (offset + count > len) {
            throw IndexOutOfRangeError("subslice(offset, count) out of range");
        }
        return VaSlice(data + offset, count);
    }

    /**
      * @brief Create subslice from offset to end
      * @param offset Starting position of subslice
      * @return New VaSlice from offset to end

      * @throws IndexOutOfRangeError if offset exceeds bounds
      */
    VaSlice subslice(Size offset) {
        if (offset > len) {
            throw IndexOutOfRangeError("subslice(offset) out of range");
        }
        return VaSlice(data + offset, len - offset);
    }

    /**
     * @brief Get length of slice (friend function)
     * @param slice Slice to measure
     * @return Number of elements in slice
     */
    friend inline Size len(const VaSlice<T>& slice) noexcept { return slice.len; }

  public iterators:
    using Iterator = T*;
    using ConstIterator = const T*;
    using ReverseIterator = std::reverse_iterator<Iterator>;
    using ConstReverseIterator = std::reverse_iterator<ConstIterator>;

    inline Iterator begin() noexcept { return data; }
    inline Iterator end() noexcept { return data + len; }

    inline ConstIterator begin() const noexcept { return data; }
    inline ConstIterator end() const noexcept { return data + len; }

    inline ConstIterator cbegin() const noexcept { return data; }
    inline ConstIterator cend() const noexcept { return data + len; }

    inline ReverseIterator rbegin() noexcept { return ReverseIterator(end()); }
    inline ReverseIterator rend() noexcept { return ReverseIterator(begin()); }

    inline ConstReverseIterator rbegin() const { return ConstReverseIterator(end()); }
    inline ConstReverseIterator rend() const { return ConstReverseIterator(begin()); }

    inline ConstReverseIterator crbegin() noexcept { return ConstReverseIterator(end()); }
    inline ConstReverseIterator crend() noexcept { return ConstReverseIterator(begin()); }
};

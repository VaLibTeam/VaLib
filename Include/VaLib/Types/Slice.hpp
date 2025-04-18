// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <VaLib/Types/BasicTypedef.hpp>
#include <VaLib/Types/Error.hpp>
#include <VaLib/Types/List.hpp>

#include <vector>

/**
 * @class VaSlice A lightweight view into a contiguous sequence of elements.
 * 
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

    friend class VaList<T>;

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
    VaSlice(VaList<T>& list) : data(list.data), len(list.len) {}

    /**
      * @brief Construct from any container with data() and size() methods
      * @tparam C Container type
      * @param container Container to create view of
      * 
      * @note Works with std::vector, std::array, and similar containers
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
     * @warning No bounds checking is performed - unsafe if index is invalid
     */
    T& operator[](Size index) { return data[index]; }

    /**
     * @brief Access const element without bounds checking
     * @param index Position of the element
     * @return Const reference to the element
     * 
     * @warning No bounds checking is performed - unsafe if index is invalid
     */
    const T& operator[](Size index) const { return data[index]; }

    /**
     * @brief Access element with bounds checking
     * @param index Position of the element
     * @return Reference to the element
     * @throw IndexOutOfRangeError if index is out of bounds
     * 
     * @note Safer but slower than operator[]
     */
    T& at(Size index) {
        if (index >= len) throw IndexOutOfRangeError(len, index);
        return data[index];
    }

    /**
     * @brief Access const element with bounds checking
     * 
     * @param index Position of the element
     * @return Const reference to the element
     * @throw IndexOutOfRangeError if index is out of bounds
     * 
     * @note Safer but slower than operator[]
     */
    const T& at(Size index) const {
        if (index >= len) throw IndexOutOfRangeError(len, index);
        return data[index];
    }

    /**
     * @brief Get raw pointer to the data
     * 
     * @return Pointer to the first element
     * 
     * @note Useful for interoperability with C-style APIs
     */
    T* getData() noexcept { return data; }

    /**
     * @brief Get const raw pointer to the data
     * 
     * @return Const pointer to the first element
     */
    const T* getData() const noexcept { return data; }

    /**
     * @brief Get size in bytes
     * 
     * @return Total size of the slice in bytes
     * 
     * @note Computed as len * sizeof(T)
     */
    Size sizeBytes() const noexcept { return len * sizeof(T); }

    /**
     * @brief Check if slice is empty
     * 
     * @return true if slice has no elements, false otherwise
     */
    bool isEmpty() const noexcept { return len == 0; }

    /**
     * @brief Access first element
     * 
     * @return Reference to first element
     * @throw ValueError if slice is empty
     * 
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
     * @throw ValueError if slice is empty
     * @note Equivalent to slice[len-1] when not empty
     */
    T& back() {
        if (isEmpty()) throw ValueError("back() called on empty slice");
        return data[len - 1];
    }

    /**
     * @brief Access first element (const version)
     * @return Const reference to first element
     * 
     * @throw ValueError if slice is empty
     */
    const T& front() const {
        if (isEmpty()) throw ValueError("front() called on empty slice");
        return data[0];
    }

    /**
     * @brief Access last element (const version)
     * @return Const reference to last element
     * 
     * @throw ValueError if slice is empty
     */
    const T& back() const {
        if (isEmpty()) throw ValueError("back() called on empty slice");
        return data[len - 1];
    }

    /**
     * @brief Get iterator to beginning
     * @return Iterator to first element
     */
    inline T* begin() noexcept { return data; }

    /**
     * @brief Get iterator to end
     * @return Iterator to one past the last element
     */
    inline T* end() noexcept { return data + len; }

    /**
     * @brief Get const iterator to beginning
     * @return Const iterator to first element
     */
    inline const T* begin() const noexcept { return data; }

    /**
     * @brief Get const iterator to end
     * @return Const iterator to one past the last element
     */
    inline const T* end() const noexcept { return data + len; }

    /**
     * @brief Get reverse iterator to beginning
     * @return Reverse iterator starting at the end
     */
    std::reverse_iterator<T*> rbegin() noexcept { return std::reverse_iterator<T*>(end()); }

    /**
     * @brief Get reverse iterator to end
     * @return Reverse iterator starting at one before the first element
     */
    std::reverse_iterator<T*> rend() noexcept { return std::reverse_iterator<T*>(begin()); }

    /**
     * @brief Create subslice with specified offset and count
     * 
     * @param offset Starting position of subslice
     * @param count Number of elements in subslice
     * @return New VaSlice representing the subrange
     * @throw IndexOutOfRangeError if offset + count exceeds bounds
     * 
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
      * 
      * @param offset Starting position of subslice
      * @return New VaSlice from offset to end
      * @throw IndexOutOfRangeError if offset exceeds bounds
      */
    VaSlice subslice(Size offset) {
        if (offset > len) {
            throw IndexOutOfRangeError("subslice(offset) out of range");
        }
        return VaSlice(data + offset, len - offset);
    }

    /**
     * @brief Get length of slice (friend function)
     * 
     * @param slice Slice to measure
     * @return Number of elements in slice
     */
    friend inline Size len(const VaSlice<T>& slice) noexcept { return slice.len; }
};
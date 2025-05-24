// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <VaLib/RawAccess/List.hpp>

#ifdef VaLib_USE_CONCEPTS
#include <VaLib/Types/BasicConcepts.hpp>
#endif

#include <VaLib/Types/BasicTypedef.hpp>
#include <VaLib/Meta/BasicDefine.hpp>
#include <VaLib/Types/TypeTraits.hpp>

#include <VaLib/Types/Error.hpp>
#include <VaLib/Types/Pair.hpp>
#include <VaLib/Types/Tuple.hpp>

#include <VaLib/FuncTools/Func.hpp>

#include <algorithm>
#include <cstdlib>
#include <initializer_list>
#include <utility>

template <typename T>
class alignas(VaListRawView<T>) VaList {
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

        #if __cplusplus >= CPP17
            if constexpr (tt::IsTriviallyCopyable<T>) {
                std::memcpy(newData, data, len * sizeof(T));
            } else {
                for (Size i = 0; i < len; i++) {
                    new (&newData[i]) T(std::move(data[i]));
                    data[i].~T();
                }
            }
        #else
            for (Size i = 0; i < len; i++) {
                new (&newData[i]) T(std::move(data[i]));
                data[i].~T();
            }
        #endif

        std::free(data);
        data = newData;
        cap = newCap;
    }

    #if __cplusplus >= CPP17
        template <typename Tuple, Size... Is>
        inline void prependAllImpl(Tuple&& tup, std::index_sequence<Is...>) {
            // reverse fold using index pack
            ((prepend(std::get<sizeof...(Is) - 1 - Is>(std::forward<Tuple>(tup)))), ...);
        }
    #endif

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
        #if __cplusplus >= CPP17
            if constexpr (!tt::IsTriviallyDestructible<T>) {
                for (Size i = 0; i < len; i++) {
                    data[i].~T();
                }
            }
        #else
            for (Size i = 0; i < len; i++) {
                data[i].~T();
            }
        #endif
    }

    inline void take(VaList&& other) {
        std::free(data);
        data = other.data;
        len = other.len;
        cap = other.cap;

        other.data = nullptr;
        other.len = other.cap = 0;
    }

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

        #if __cplusplus >= CPP17
            if constexpr (tt::IsTriviallyCopyable<T>) {
                std::memcpy(data, other.data, len * sizeof(T));
            } else {
                for (Size i = 0; i < len; i++) {
                    new (&data[i]) T(other.data[i]); // Placement new + copy
                }
            }
        #else
            for (Size i = 0; i < len; i++) {
                new (&data[i]) T(other.data[i]); // Placement new + copy
            }
        #endif
    }

    /**
     * @brief Move constructor.
     * @param other The list to move from.
     */
    VaList(VaList&& other) noexcept : len(other.len), cap(other.cap), data(other.data) {
        other.data = nullptr;
        other.len = other.cap = 0;
    }

    #if __cplusplus >= CPP17
        /**
         * @brief Constructs the list from a variadic list of arguments.
         * @tparam Args Types of the arguments.
         * @param args Values to initialize the list with.
         */
        template <
            typename... Args,
            typename = tt::EnableIf<(tt::IsConstructible<T, Args> && ...)>
        >
        VaList(Args&&... args) : len(sizeof...(Args)), cap(sizeof...(Args)) {
            data = static_cast<T*>(std::malloc(cap * sizeof(T)));
            Size i = 0;
            ((new (&data[i++]) T(std::forward<Args>(args))), ...);
        }

        /**
         * @brief Constructs the list from a variadic list of arguments.
         * @tparam Args Types of the arguments.
         * @param args Values to initialize the list with.
         */
        template <
            typename... Args,
            typename = tt::EnableIf<(tt::IsConstructible<T, Args> && ...)>
        >
        static VaList From(Args&&... args) {
            VaList list;
            list.len = list.cap = (sizeof...(Args));
            list.data = static_cast<T*>(std::malloc(list.cap * sizeof(T)));
            Size i = 0;
            ((new (&list.data[i++]) T(std::forward<Args>(args))), ...);

            return list;
        }
    #endif

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
     * @brief Creates a VaList filled with a specified number of copies of a given value.
     * @param count The number of elements to create in the list.
     * @param val The value to fill the list with.
     * @return A VaList containing `count` copies of `val`.
     */
    static VaList Filled(Size count, const T& val) {
        VaList list;
        list.len = list.cap = count;
        list.data = static_cast<T*>(std::malloc(sizeof(T) * count));
        for (Size i = 0; i < count; i++) {
            new (&list.data[i]) T(val);
        }

        return list;
    }

    /**
     * @brief Constructs a VaList from a raw pointer and manual length/capacity.
     *        This function *adopts* an existing raw buffer and treats it as a valid VaList.
     *        No memory is copied, so this is a zero-cost operation.
     *
     * **UNSAFE: You are fully responsible for ensuring all invariants are correct.**
     *
     * @param raw A pointer to a buffer of T elements.
     * @param len The number of initialized elements in the buffer.
     * @param cap The total capacity of the buffer (must be >= len).
     * @return A VaList that assumes ownership of the provided memory.
     *
     * @warning
     * - The buffer **must** have been allocated with the same allocator used by VaList.
     * - If the list grows past `cap`, it will reallocate internally.
     *   - In such case, the original memory will be **automatically freed**.
     *   - You must manually free the memory **only** if you replace the buffer manually
     *     using Unsafe API before reallocation or destruction of the list.
     * - You can detect whether the original buffer is still in use via:
     *   ```cpp
     *   if (list.dataPtr() == raw) {
     *      // buffer still valid
     *   }
     *   ```
     * - If you provide an invalid @ref cap (less than @ref len), the behavior is undefined.
     * - If you provide an invalid `len`, accessors like @ref at may throw an exception,
     *      but destruction will still behave correctly (unless cap is wrong).
     * - If the memory was not dynamically allocated, calling the destructor or deallocating
     *      the memory will result in undefined behavior.
     *
     * @note This is intended for expert users who need full control over allocation. For a safe alternative, use the constructor that copies the data.
     */
    // @{
    static VaList UnsafeTake(T* raw, Size len, Size cap) {
        VaList result;
        result.data = raw;
        result.len = len;
        result.cap = cap;
        return result;
    }

    static VaList UnsafeTake(T* raw, Size len) {
        return UnsafeTake(raw, len, len);
    }
    // @}

    /**
     * @brief Creates a VaList by taking ownership of a VaListRawView.
     * @param view The VaListRawView to take ownership from.
     * @return A VaList that adopts the memory of the provided view.
     *
     * @note For more details see @ref UnsafeTake
     * @see UnsafeTake
     */
    static VaList UnsafeTakeFrom(const VaListRawView<T>* view) {
        return UnsafeTake(view->data, view->len, view->cap);
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

        #if __cplusplus >= CPP17
            if constexpr (tt::IsTriviallyCopyable<T>) {
                std::memcpy(data, other.data, len * sizeof(T));
            } else {
                for (Size i = 0; i < len; i++) {
                    new (&data[i]) T(other.data[i]);
                }
            }
        #else
            for (Size i = 0; i < len; i++) {
                new (&data[i]) T(other.data[i]);
            }
        #endif

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
     * @brief Adds all elements from another container to the end of the list.
     * @param other The container whose elements will be appended.
     *
     * This method iterates over the provided container and appends each element
     * to the end of the current list. The list's capacity is adjusted as needed
     * to accommodate the new elements.
     */
    template <typename Iterable>
    void appendEach(const Iterable& other) {
        Size otherSize = std::distance(std::begin(other), std::end(other));
        if (otherSize == 0) return;

        reserve(len + otherSize);
        for (const auto& elm: other) {
            new (&data[len++]) T(elm);
        }
    }

    /**
     * @brief Adds all elements from another container to the end of the list using move semantics.
     * @param other The container whose elements will be appended.
     *
     * This method iterates over the provided container and appends each element
     * to the end of the current list using move semantics. The list's capacity
     * is adjusted as needed to accommodate the new elements.
     *
     * @note The container must support move semantics for its elements.
     */
    template <typename Iterable>
    void appendEach(Iterable&& other) {
        Size otherSize = std::distance(std::begin(other), std::end(other));
        if (otherSize == 0) return;

        reserve(len + otherSize);
        for (auto& elm: other) {
            new (&data[len++]) T(std::move(elm));
        }
    }

    /**
     * @brief Adds all elements from another VaList to the end of the list.
     * @param other The VaList whose elements will be appended.
     *
     * This method appends all elements from the provided VaList to the end of
     * the current list. The list's capacity is expanded if necessary.
     */
    void appendEach(const VaList& other) {
        if (other.len == 0) return;

        reserve(len + other.len);
        for (Size i = 0; i < other.len; i++) {
            new (&data[len++]) T(other.data[i]);
        }
    }

    void appendEach(VaList&& other) {
        if (other.len == 0) return;

        // if the current list is empty, take ownership of the other list's data directly.
        if (len == 0) {
            take(std::move(other));
            return;
        }

        reserve(len + other.len);
        for (Size i = 0; i < other.len; i++) {
            new (&data[len++]) T(std::move(other.data[i]));
            other.data[i].~T();
        }

        // free the other list's buffer.
        std::free(other.data);
        other.data = nullptr;
        other.len = other.cap = 0;
    }

    /**
     * @brief Adds all elements from another container to the beginning of the list.
     * @param other The container whose elements will be prepended.
     *
     * This method shifts the existing elements in the list to make room for the
     * new elements from the provided container, which are then added to the front.
     */
    template <typename Iterable>
    void prependEach(const Iterable& other) {
        Size otherSize = std::distance(std::begin(other), std::end(other));
        if (otherSize == 0) return;

        reserve(len + otherSize);
        for (Size i = len; i > 0; i--) {
            new (&data[i + otherSize - 1]) T(std::move(data[i - 1]));
            data[i - 1].~T();
        }

        Size i = 0;
        for (const auto& elm: other) {
            new (&data[i++]) T(elm);
        }
        len += otherSize;
    }

    /**
     * @brief Adds all elements from another container to the beginning of the list using move semantics.
     * @param other The container whose elements will be prepended.
     *
     * This method shifts the existing elements in the list to make room for the
     * new elements from the provided container, which are then added to the front
     * using move semantics.
     *
     * @note The container must support move semantics for its elements.
     */
    template <typename Iterable>
    void prependEach(Iterable&& other) {
        Size otherSize = std::distance(std::begin(other), std::end(other));
        if (otherSize == 0) return;

        reserve(len + otherSize);
        for (Size i = len; i > 0; i--) {
            new (&data[i + otherSize - 1]) T(std::move(data[i - 1]));
            data[i - 1].~T();
        }

        Size i = 0;
        for (auto& elm: other) {
            new (&data[i++]) T(std::move(elm));
        }
        len += otherSize;
    }

    /**
     * @brief Adds all elements from another VaList to the beginning of the list.
     * @param other The VaList whose elements will be prepended.
     *
     * This method shifts the existing elements in the list to make room for the
     * new elements from the provided VaList, which are then added to the front.
     */
    void prependEach(const VaList& other) {
        if (other.len == 0) return;

        reserve(len + other.len);
        for (Size i = len; i > 0; i--) {
            new (&data[i + other.len - 1]) T(std::move(data[i - 1]));
            data[i - 1].~T();
        }

        for (Size i = 0; i < other.len; i++) {
            new (&data[i]) T(other.data[i]);
        }
        len += other.len;
    }

    /**
     * @brief Adds all elements from another VaList to the beginning of the list using move semantics.
     *       Shifts the existing elements in the list to make room for the
     *       new elements from the provided VaList, which are then added to the front
     *       using move semantics. If the current list is empty, it directly takes
     *       ownership of the other list's data without additional allocations.
     * @param other The VaList whose elements will be prepended.
     *
     * @note This operation modifies both the current list and the provided list.
     *       After the operation, the `other` list will be empty and its memory
     *       will be released.
     *
     * @throws NullPointerError If memory allocation fails during resizing.
     */
    void prependEach(VaList&& other) {
        if (other.len == 0) return;

        // if the current list is empty, take ownership of the other list's data directly.
        if (len == 0) {
            take(std::move(other));
            return;
        }

        // if the other list is larger, allocate a new buffer to avoid excessive shifting.
        if (other.len > cap-len) {
            Size newCap = len + other.len;
            T* newData = static_cast<T*>(std::malloc(newCap * sizeof(T)));
            if (!newData) throw NullPointerError();

            // move the other list's elements into the new buffer.
            for (Size i = 0; i < other.len; i++) {
                new (&newData[i]) T(std::move(other.data[i]));
                other.data[i].~T();
            }

            // move the current list's elements into the new buffer after the other list's elements.
            for (Size i = 0; i < len; i++) {
                new (&newData[other.len + i]) T(std::move(data[i]));
                data[i].~T();
            }

            std::free(data);
            data = newData;
            len += other.len;
            cap = newCap;

            std::free(other.data);
            other.data = nullptr;
            other.len = other.cap = 0;
            return;
        }

        // default:
        reserve(len + other.len);
        for (Size i = len; i > 0; i--) {
            new (&data[i + other.len - 1]) T(std::move(data[i - 1]));
            data[i - 1].~T();
        }

        for (Size i = 0; i < other.len; i++) {
            new (&data[i]) T(std::move(other.data[i]));
            other.data[i].~T();
        }
        len += other.len;

        std::free(other.data);
        other.data = nullptr;
        other.len = other.cap = 0;
    }

    /**
     * @brief Inserts all elements from another container at a specified position.
     * @param index The position where the elements will be inserted.
     * @param other The container whose elements will be inserted.
     *
     * This method shifts the existing elements starting from the specified index
     * to make room for the new elements from the provided container.
     *
     * @throws IndexOutOfRangeError If the index is out of bounds.
     */
    template <typename Iterable>
    void insertEach(Size index, const Iterable& other) {
        if (index > len) throw IndexOutOfRangeError(len, index);

        Size otherSize = std::distance(std::begin(other), std::end(other));
        if (otherSize == 0) return;

        reserve(len + otherSize);
        for (Size i = len; i > index; i--) {
            new (&data[i + otherSize - 1]) T(std::move(data[i - 1]));
            data[i - 1].~T();
        }

        Size i = index;
        for (const auto& elm: other) {
            new (&data[i++]) T(elm);
        }
        len += otherSize;
    }

    /**
     * @brief Inserts all elements from another container at a specified position using move semantics.
     * @param index The position where the elements will be inserted.
     * @param other The container whose elements will be inserted.
     *
     * This method shifts the existing elements starting from the specified index
     * to make room for the new elements from the provided container, which are then
     * inserted at the specified position using move semantics.
     *
     * @note The container must support move semantics for its elements.
     *
     * @throws IndexOutOfRangeError If the index is out of bounds.
     */
    template <typename Iterable>
    void insertEach(Size index, Iterable&& other) {
        if (index > len) throw IndexOutOfRangeError(len, index);

        Size otherSize = std::distance(std::begin(other), std::end(other));
        if (otherSize == 0) return;

        reserve(len + otherSize);
        for (Size i = len; i > index; i--) {
            new (&data[i + otherSize - 1]) T(std::move(data[i - 1]));
            data[i - 1].~T();
        }

        Size i = index;
        for (auto& elm: other) {
            new (&data[i++]) T(std::move(elm));
        }
        len += otherSize;
    }

    /**
     * @brief Inserts all elements from another VaList at a specified position.
     * @param index The position where the elements will be inserted.
     * @param other The VaList whose elements will be inserted.
     *
     * This method shifts the existing elements starting from the specified index
     * to make room for the new elements from the provided VaList.
     *
     * @throws IndexOutOfRangeError If the index is out of bounds.
     */
    void insertEach(Size index, const VaList& other) {
        if (index > len) throw IndexOutOfRangeError(len, index);
        if (other.len == 0) return;

        const Size newLen = len + other.len;
        if (newLen > cap) resize(newLen);

        for (Size i = len; i > index; i--) {
            new (&data[i + other.len - 1]) T(std::move(data[i - 1]));
            data[i - 1].~T();
        }

        for (Size i = 0; i < other.len; i++) {
            new (&data[index + i]) T(other.data[i]);
        }
        len += other.len;
    }

    void insertEach(Size index, VaList&& other) {
        if (index > len) throw IndexOutOfRangeError(len, index);
        if (other.len == 0) return;

        // if the current list is empty, take ownership of the other list's data directly.
        if (len == 0) {
            take(std::move(other));
            return;
        }

        // if the insertion point is at the end, append directly.
        if (index == len) {
            appendEach(std::move(other));
            return;
        }

        // if the other list is larger, allocate a new buffer to avoid excessive shifting.
        if (other.len > cap - len) {
            Size newCap = len + other.len;
            T* newData = static_cast<T*>(std::malloc(newCap * sizeof(T)));
            if (!newData) throw NullPointerError();

            for (Size i = 0; i < index; i++) {
                new (&newData[i]) T(std::move(data[i]));
                data[i].~T();
            }

            // move the other list's elements into the new buffer.
            for (Size i = 0; i < other.len; i++) {
                new (&newData[index + i]) T(std::move(other.data[i]));
                other.data[i].~T();
            }

            // copy elements after the insertion point.
            for (Size i = index; i < len; i++) {
                new (&newData[other.len + i]) T(std::move(data[i]));
                data[i].~T();
            }

            std::free(data);
            data = newData;
            len += other.len;
            cap = newCap;

            std::free(other.data);
            other.data = nullptr;
            other.len = other.cap = 0;
            return;
        }

        // default: shift elements and insert.
        reserve(len + other.len);
        for (Size i = len; i > index; i--) {
            new (&data[i + other.len - 1]) T(std::move(data[i - 1]));
            data[i - 1].~T();
        }

        for (Size i = 0; i < other.len; i++) {
            new (&data[index + i]) T(std::move(other.data[i]));
            other.data[i].~T();
        }
        len += other.len;

        std::free(other.data);
        other.data = nullptr;
        other.len = other.cap = 0;
    }

    template <typename Iterable>
    void extend(const Iterable& other) {
        appendEach(other);
    }

    template <typename Iterable>
    void extend(Iterable&& other) {
        appendEach(std::forward<Iterable>(other));
    }

    void extend(const VaList& other) {
        appendEach(other);
    }

    void extend(VaList&& other) {
        appendEach(std::move(other));
    }

    #if __cplusplus >= CPP17
        /**
         * @brief Appends multiple elements to the end of the list.
         * @tparam Args Types of the elements to append.
         * @param args Elements to append to the list.
         *
         * @note This method uses variadic templates to accept multiple arguments
         *       and appends each of them to the list in order.
         */
        template <typename... Args>
        inline void appendAll(Args&&... args) {
            reserve(len + sizeof...(args));
            (append(std::forward<Args>(args)), ...);
        }

        /**
         * @brief Prepends multiple elements to the beginning of the list.
         * @tparam Args Types of the elements to prepend.
         * @param args Elements to prepend to the list.
         *
         * @note This method uses variadic templates to accept multiple arguments
         *       and prepends each of them to the list in reverse order.
         */
        template <typename... Args>
        inline void prependAll(Args&&... args) {
            reserve(len + sizeof...(args));
            prependAllImpl(VaTuple<Args...>(std::forward<Args>(args)...), std::index_sequence_for<Args...>{});
        }

        /**
         * @brief Inserts multiple elements at the specified index in the list.
         * @tparam Args Types of the elements to insert.
         * @param index Position to insert the elements at.
         * @param args Elements to insert into the list.
         *
         * @note This method uses variadic templates to accept multiple arguments
         *       and inserts each of them at the specified index in order.
         *
         * @throws IndexOutOfRangeError If the index is out of bounds.
         */
        template <typename... Args>
        inline void insertAll(Size index, Args&&... args) {
            reserve(len + sizeof...(args));
            insertEach(index, {args...});
        }
    #endif

    /**
     * @brief Deletes the element at the specified index.
     * @param index Index of the element to delete.
     *
     * @throws IndexOutOfRangeError If index is out of bounds.
     */
    void del(Size index) {
        if (index >= len) throw IndexOutOfRangeError(len, index);

        for (Size i = index; i < len - 1; i++) {
            data[i].~T();
            new (&data[i]) T(std::move(data[i + 1]));
        }

        data[len - 1].~T();
        len--;
    }

    /**
     * @brief Deletes a range of elements from the list.
     * @param start The starting index of the range (inclusive).
     * @param end The ending index of the range (exclusive).
     *
     * @throws IndexOutOfRangeError If start or end are out of bounds.
     * @throws ValueError If start is greater than end.
     */
    void delRange(Size start, Size end) {
        if (start > end) throw ValueError("delRange(): start index cannot be greater than end index");
        if (end > len) throw IndexOutOfRangeError(len, end);
        if (start >= len) throw IndexOutOfRangeError(len, start);

        Size rangeSize = end - start;
        for (Size i = start; i < len - rangeSize; i++) {
            data[i].~T();
            new (&data[i]) T(std::move(data[i + rangeSize]));
        }

        for (Size i = len - rangeSize; i < len; i++) {
            data[i].~T();
        }

        len -= rangeSize;
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
     * @brief Checks if the given index is within the valid range for direct access methods like get() or operator[].
     * @param index The index to check.
     * @return True if the index is within the valid range, false otherwise.
     *
     * @note This method does not handle negative indices. It only checks if the index is between 0 and len - 1.
     */
    inline bool isIndexValid(Size index) const {
        return index < len;
    }

    /**
     * @brief Checks if the given index is within the valid range for methods like at(), which support wrapping negative indices.
     * @param index The index to check (can be negative).
     * @return True if the index is within the valid range after wrapping, false otherwise.
     *
     * @note This method accounts for negative indices by wrapping them to the valid range [0, len - 1].
     */
    inline bool isIndexValidWrapped(int32 index) const {
        if (index < 0) index += len;
        return index >= 0 && static_cast<Size>(index) < len;
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
     * @param index Index of the element (can be negative).
     * @return Reference to the element.
     *
     * @throws IndexOutOfRangeError If index is out of bounds.
     */
    inline T& at(int32 index) {
        if (index < 0) index += len; // handle negative indices
        if (index < 0 || static_cast<Size>(index) >= len) throw IndexOutOfRangeError(len, index);
        return data[static_cast<Size>(index)];
    }

    /**
     * @brief Accesses an element by index with bounds checking.
     * @param index Index of the element (can be negative).
     * @return Const reference to the element.
     *
     * @throws IndexOutOfRangeError If index is out of bounds.
     */
    inline const T& at(int32 index) const {
        if (index < 0) index += len; // handle negative indices
        if (index < 0 || static_cast<Size>(index) >= len) throw IndexOutOfRangeError(len, index);
        return data[static_cast<Size>(index)];
    }

    /**
     * @brief Sets the value at the specified index.
     * @param index Index of the element to set (can be negative).
     * @param value The value to set.
     *
     * @throws IndexOutOfRangeError If index is out of bounds.
     */
    void set(int32 index, const T& value) {
        if (index < 0) index += len; // handle negative indices
        if (index < 0 || static_cast<Size>(index) >= len) throw IndexOutOfRangeError(len, index);
        data[static_cast<Size>(index)].~T();
        new (&data[static_cast<Size>(index)]) T(value);
    }

    /**
     * @brief Sets the value at the specified index using move semantics.
     * @param index Index of the element to set (can be negative).
     * @param value The value to set (moved).
     *
     * @throws IndexOutOfRangeError If index is out of bounds.
     */
    void set(int32 index, T&& value) {
        if (index < 0) index += len; // handle negative indices
        if (index < 0 || static_cast<Size>(index) >= len) throw IndexOutOfRangeError(len, index);
        data[static_cast<Size>(index)].~T();
        new (&data[static_cast<Size>(index)]) T(std::move(value));
    }

    /**
     * @brief Returns a reference to the first element in the list.
     * @return Reference to the first element.
     *
     * @throws ValueError If the list is empty.
     */
    inline T& front() {
        if (len <= 0) throw ValueError("front() on empty list");
        return data[0];
    }

    /**
     * @brief Returns a const reference to the first element in the list.
     * @return Const reference to the first element.
     *
     * @throws ValueError If the list is empty.
     */
    inline const T& front() const {
        if (len <= 0) throw ValueError("front() on empty list");
        return data[0];
    }

    /**
     * @brief Returns a reference to the last element in the list.
     * @return Reference to the last element.
     *
     * @throws ValueError If the list is empty.
     */
    inline T& back() {
        if (len <= 0) throw ValueError("back() on empty list");
        return data[len - 1];
    }

    /**
     * @brief Returns a const reference to the last element in the list.
     * @return Const reference to the last element.
     *
     * @throws ValueError If the list is empty.
     */
    inline const T& back() const {
        if (len <= 0) throw ValueError("back() on empty list");
        return data[len - 1];
    }

    /**
     * @brief Returns a reference to the first element in the list without bounds checking.
     * @return Reference to the first element.
     *
     * @note This method does not perform any size checks. The behavior is undefined if the list is empty.
     */
    inline T& frontUnchecked() noexcept {
        return data[0];
    }

    /**
     * @brief Returns a const reference to the first element in the list without bounds checking.
     * @return Const reference to the first element.
     *
     * @note This method does not perform any size checks. The behavior is undefined if the list is empty.
     */
    inline const T& frontUnchecked() const noexcept {
        return data[0];
    }

    /**
     * @brief Returns a reference to the last element in the list without bounds checking.
     * @return Reference to the last element.
     *
     * @note This method does not perform any size checks. The behavior is undefined if the list is empty.
     */
    inline T& backUnchecked() noexcept {
        return data[len - 1];
    }

    /**
     * @brief Returns a const reference to the last element in the list without bounds checking.
     * @return Const reference to the last element.
     *
     * @note This method does not perform any size checks. The behavior is undefined if the list is empty.
     */
    inline const T& backUnchecked() const noexcept {
        return data[len - 1];
    }

    /**
     * @brief Shrinks the internal capacity to fit the current size.
     */
    inline void shrink() { resize(len); }

    /**
     * @brief Fills the list with the specified value.
     * @param val The value to fill the list with.
     *
     * @note This method replaces all elements in the list with the given value. The size of the list remains unchanged.
     */
    inline void fill(const T& val) {
        for (Size i = 0; i < len; i++) {
            data[i] = val;
        }
    }

    /**
     * @brief Fills the list with the specified value from the given start index to the end index.
     * @param val The value to fill the list with.
     * @param start The starting index (inclusive).
     * @param end The ending index (exclusive).
     *
     * @throws IndexOutOfRangeError If start or end are out of bounds.
     * @throws ValueError If start is greater than end.
     *
     * @note This method replaces all elements in the specified range with the given value.
     */
    void fill(const T& val, Size start, Size end) {
        if (start > end) throw ValueError("fill(): start index cannot be greater than end index");
        if (end > len) throw IndexOutOfRangeError(len, end);
        if (start >= len) throw IndexOutOfRangeError(len, start);

        for (Size i = start; i < end; i++) {
            data[i] = val;
        }
    }

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
        result.reserve(len - static_cast<Size>(start));
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
        if (end < 0 || static_cast<Size>(end) > len) throw IndexOutOfRangeError(len, end);

        VaList result;
        result.reserve(static_cast<Size>(end));
        for (Size i = 0; i < static_cast<Size>(end); i++) {
            result.append(data[i]);
        }
        return result;
    }


    /**
     * @brief Creates a slice between the specified start and end indices with an optional step.
     * @param start Start index (can be negative).
     * @param end End index (can be negative).
     * @param step Step size for slicing (default is 1).
     * @return A new list containing the sliced elements.
     *
     * @throws IndexOutOfRangeError If indices are invalid or out of bounds.
     * @throws ValueError If step is zero.
     */
    VaList slice(int32 start, int32 end, int32 step = 1) const {
        if (step == 0) throw ValueError("slice(): step cannot be zero");

        if (start < 0) start += len;
        if (end < 0) end += len;

        if (start < 0) start = 0;
        if (static_cast<Size>(end) > len) end = static_cast<int32>(len);

        VaList result;

        if (step > 0) {
            if (start > end) return result; // empty result for positive step if start > end
            const Size ustart = static_cast<Size>(start);
            const Size uend = static_cast<Size>(end);
            result.reserve((uend - ustart + step - 1) / step); // Calculate required capacity
            for (Size i = ustart; i < uend; i += step) {
                result.append(data[i]);
            }
        } else {
            if (start < end) return result; // empty result for negative step if start < end
            const Size ustart = static_cast<Size>(start);
            const Size uend = static_cast<Size>(end);
            result.reserve((ustart - uend - step - 1) / -step);
            for (Size i = ustart; i > uend; i += step) {
                result.append(data[i]);
            }
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
    tt::EnableIf<tt::IsSame<U, VaString>, VaString> join(const VaString& sep = "") const {
        if (len == 0) return VaString();
        VaString result = data[0];
        for (Size i = 1; i < len; i++) {
            result += sep + data[i];
        }
        return result;
    }

    template <typename U = T>
    tt::EnableIf<tt::IsConvertible<bool, U>, bool> all() const {
        for (int i = 0; i < len; i++) {
            if (!static_cast<bool>(data[i])) {
                return false;
            }
        }

        return true;
    }

    template <typename U = T>
    tt::EnableIf<tt::IsConvertible<bool, U>, bool> any() const {
        for (int i = 0; i < len; i++) {
            if (static_cast<bool>(data[i])) {
                return true;
            }
        }
        return false;
    }

    /**
     * @brief Returns the number of elements currently stored in the list.
     * @return The current length of the list.
     */
    inline Size getLength() const noexcept {
        return this->len;
    }

    /**
     * @brief Returns the total capacity of the list's internal buffer.
     * @return The current capacity of the list.
     */
    inline Size getCapacity() const noexcept {
        return this->cap;
    }

    /**
     * @brief Returns a pointer to the internal data array.
     * @return Pointer to the data.
     */
    inline T* dataPtr() { return data; }

    /**
     * @brief Returns a const pointer to the internal data array.
     * @return Const pointer to the data.
     */
    inline const T* dataPtr() const { return data; }

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

    /**
     * @brief Alias for a raw view of the list's internal state.
     *        This type provides direct access to the list's internal fields (length, capacity, and pointer),
     *        and may be used for both read-only and mutable purposes depending on context.
     *
     * @note The `RawView` is a lightweight representation and does not own any memory.
     *       It is valid only as long as the original `VaList` object exists.
     * @note Modifications to the `VaList` object will be reflected in all associated `RawView` instances.
     * @warning Any invalidation of the `VaList` (e.g., destruction or reallocation) will render the `RawView` invalid.
     *
     * @see getRawView()
     * @see getUnsafeAccess()
     */
    using RawView = VaListRawView<T>;

    /**
     * @brief Returns a read-only view of the list's internal state.
     *        This method returns a `VaListRawView<T>` pointer to the current list object.
     *        The returned pointer can be used for safe inspection of the list's internal fields
     *        without the ability to modify them.
     * @return A const-qualified pointer to a `VaListRawView<T>` describing the list's state.
     *
     * @note This does not copy any data; it simply reinterprets the current object.
     * @note The returned view is valid only as long as the `VaList` object exists and remains unmodified.
     * @warning Although this view is read-only, changes to the `VaList` object (e.g., resizing or clearing)
     *          will affect the data visible through the view.
     * @see getUnsafeAccess() for writable access.
     */
    const RawView* getRawView() const {
        return reinterpret_cast<const RawView*>(this);
    }

    /**
     * @brief Grants unsafe, mutable access to the list's internals.
     *        This method returns a pointer to the current list reinterpreted as a `VaListRawView<T>`.
     *        The caller may directly read and modify the list’s length, capacity, and data pointer.
     * @return A pointer to a `VaListRawView<T>` representing the internal list state.
     *
     * @note This does not copy any data; it simply reinterprets the current object.
     * @note The returned view is valid only as long as the `VaList` object exists and remains unmodified.
     * @warning This is an unsafe interface. You are fully responsible for maintaining all invariants.
     *          Incorrect usage may lead to memory corruption or undefined behavior.
     * @warning Modifications to the `VaList` object through this view will directly affect the original list
     *          and any other views derived from it.
     * @see getRawView() for a safe, read-only alternative.
     */
    RawView* getUnsafeAccess() {
        return reinterpret_cast<RawView*>(this);
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
        if constexpr (!tt::HasEqualityOperator_v<T>) {
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

    /**
     * @brief Compares two lists for less-than.
     * @param other The list to compare with.
     * @return True if this list is lexicographically less than the other.
     */
    friend bool operator<(const VaList& lhs, const VaList& rhs) {
        Size minLen = std::min(lhs.len, rhs.len);
        for (Size i = 0; i < minLen; i++) {
            if (lhs.data[i] < rhs.data[i]) return true;
            if (lhs.data[i] > rhs.data[i]) return false;
        }
        return lhs.len < rhs.len;
    }

    /**
     * @brief Compares two lists for greater-than.
     * @param other The list to compare with.
     * @return True if this list is lexicographically greater than the other.
     */
    friend bool operator>(const VaList& lhs, const VaList& rhs) {
        return rhs < lhs;
    }

    /**
     * @brief Compares two lists for less-than-or-equal.
     * @param other The list to compare with.
     * @return True if this list is lexicographically less than or equal to the other.
     */
    friend bool operator<=(const VaList& lhs, const VaList& rhs) {
        return !(rhs < lhs);
    }

    /**
     * @brief Compares two lists for greater-than-or-equal.
     * @param other The list to compare with.
     * @return True if this list is lexicographically greater than or equal to the other.
     */
    friend bool operator>=(const VaList& lhs, const VaList& rhs) {
        return !(lhs < rhs);
    }

    template < typename Iterable, typename = tt::EnableIf<!tt::IsSame<tt::RemoveReference<tt::RemoveCV<Iterable>>, VaList>> >
    friend bool operator==(const VaList& lhs, const Iterable& rhs) {
        auto leftIt = lhs.begin();
        auto rightIt = std::begin(rhs);
        auto rhsEnd = std::end(rhs);

        while (leftIt != lhs.end() && rightIt != rhsEnd) {
            if (*leftIt != *rightIt) return false;
            ++leftIt;
            ++rightIt;
        }

        return leftIt == lhs.end() && rightIt == rhsEnd;
    }

    template < typename Iterable, typename = tt::EnableIf<!tt::IsSame<tt::RemoveReference<tt::RemoveCV<Iterable>>, VaList>> >
    friend bool operator!=(const VaList& lhs, const Iterable& rhs) {
        return !(lhs == rhs);
    }

    template < typename Iterable, typename = tt::EnableIf<!tt::IsSame<tt::RemoveReference<tt::RemoveCV<Iterable>>, VaList>> >
    friend bool operator<(const VaList& lhs, const Iterable& rhs) {
        auto leftIt = lhs.begin();
        auto rightIt = std::begin(rhs);
        auto rhsEnd = std::end(rhs);

        while (leftIt != lhs.end() && rightIt != rhsEnd) {
            if (*leftIt < *rightIt) return true;
            if (*leftIt > *rightIt) return false;
            ++leftIt;
            ++rightIt;
        }

        return leftIt == lhs.end() && rightIt != rhsEnd;
    }

    template < typename Iterable, typename = tt::EnableIf<!tt::IsSame<tt::RemoveReference<tt::RemoveCV<Iterable>>, VaList>> >
    friend bool operator>(const VaList& lhs, const Iterable& rhs) {
        return rhs < lhs;
    }

    template < typename Iterable, typename = tt::EnableIf<!tt::IsSame<tt::RemoveReference<tt::RemoveCV<Iterable>>, VaList>> >
    friend bool operator<=(const VaList& lhs, const Iterable& rhs) {
        return !(rhs < lhs);
    }

    template < typename Iterable, typename = tt::EnableIf<!tt::IsSame<tt::RemoveReference<tt::RemoveCV<Iterable>>, VaList>> >
    friend bool operator>=(const VaList& lhs, const Iterable& rhs) {
        return !(lhs < rhs);
    }

  public friends:
    /**
     * @brief Returns the number of elements in the list.
     * @param list The list to query.
     * @return Size of the list.
     */
    friend inline Size len(const VaList& list) noexcept { return list.len; }

    /**
     * @brief Returns the capacity of the list.
     * @param list The list to query.
     * @return Capacity of the list.
     */
    friend inline Size cap(const VaList& list) noexcept { return list.cap; }

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
// @{
template <typename Old, typename New>
VaList<New> map(VaFunc<Old(New)> mod, const VaList<Old>& data) {
    VaList<New> result;
    result.reserve(len(data));

    for (Size i = 0; i < len(data); i++) {
        result.append(mod(data[i]));
    }

    return result;
}

template <typename Old, typename New>
VaList<New> map(VaFunc<Old(const New&)> mod, const VaList<Old>& data) {
    VaList<New> result;
    result.reserve(len(data));

    for (Size i = 0; i < len(data); i++) {
        result.append(mod(data[i]));
    }

    return result;
}
// @}

/**
 * @brief Returns a new list containing only the elements that satisfy the predicate.
 * @tparam T Element type.
 * @param predicate Function that returns true for elements to keep.
 * @param data Input list.
 * @return A new VaList containing the filtered elements.
 */
// @{
template <typename T>
VaList<T> filter(VaFunc<bool(const T&)> predicate, const VaList<T>& data) {
    VaList<T> result;
    for (Size i = 0; i < len(data); i++) {
        if (predicate(data[i])) {
            result.append(data[i]);
        }
    }

    return result;
}

template <typename T>
VaList<T> filter(VaFunc<bool(T)> predicate, const VaList<T>& data) {
    VaList<T> result;
    for (Size i = 0; i < len(data); i++) {
        if (predicate(data[i])) {
            result.append(data[i]);
        }
    }

    return result;
}
// @}

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
R reduce(VaFunc<R(R, T)> reducer, const VaList<T>& data, R initial) {
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
        result.appendEmplace(i, data[i]);
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
        result.appendEmplace(a[i], b[i]);
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
    for (Size i = len(data); i > 0; i--) {
        result.append(data[i]);
    }
    return result;
}

} // namespace va

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
    Size len, cap;
    T* data;

    void resize(Size newCap) {
        T* newData = static_cast<T*>(std::malloc(newCap * sizeof(T)));

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

    inline void expand() { resize(cap == 0 ? 4 : cap * 2); }

    inline void update() {
        if (len >= cap) expand();
    }

    inline void deleteObjects() {
        if constexpr (!std::is_trivially_destructible_v<T>) {
            for (Size i = 0; i < len; i++) {
                data[i].~T();
            }
        }
    }

    friend class VaSlice<T>;

  public:
    VaList() : len(0), cap(0), data(nullptr) {}

    VaList(std::initializer_list<T> init) : len(init.size()), cap(init.size()) {
        data = static_cast<T*>(std::malloc(cap * sizeof(T)));
        Size i = 0;
        for (const T& val : init) {
            new (&data[i++]) T(val);
        }
    }

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

    VaList(VaList&& other) noexcept : len(other.len), cap(other.cap), data(other.data) {
        other.data = nullptr;
        other.len = other.cap = 0;
    }

    ~VaList() {
        if (data) {
            deleteObjects();
            std::free(data);
        }
    }

    template <typename... Args,
              typename = std::enable_if_t<(std::is_constructible_v<T, Args> && ...)>>
    VaList(Args&&... args) : len(sizeof...(Args)), cap(sizeof...(Args)) {
        data = static_cast<T*>(std::malloc(cap * sizeof(T)));
        Size i = 0;
        ((new (&data[i++]) T(std::forward<Args>(args))), ...);
    }

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

    VaList& operator=(VaList&& other) noexcept {
        if (this == &other) return *this;
        std::free(data);
        len = other.len;
        cap = other.cap;
        data = other.data;
        other.data = nullptr;
        other.len = other.cap = 0;
        return *this;
    }

    inline void reserve(Size minCap) {
        if (minCap > cap) resize(minCap);
    }

    void append(const T& elm) {
        update();
        new (&data[len++]) T(elm);
    }

    void append(T&& elm) {
        update();
        new (&data[len++]) T(std::move(elm));
    }

    void prepend(const T& elm) { insert(0, elm); }
    void prepend(T&& elm) { insert(0, elm); }

    void extend(const VaList& other) { *this += other; }

    template <typename... Args>
    T& emplace(Args&&... args) {
        update();
        new (&data[len]) T(std::forward<Args>(args)...);
        return data[len++];
    }

    T& operator[](Size index) { return data[index]; }
    const T& operator[](Size index) const { return data[index]; }

    T& at(Size index) {
        if (index >= len) throw IndexOutOfRangeError(len, index);
        return data[index];
    }

    const T& at(Size index) const {
        if (index >= len) throw IndexOutOfRangeError(len, index);
        return data[index];
    }

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

    void del(Size index) {
        if (index >= len) throw IndexOutOfRangeError(len, index);
        data[index].~T();
        for (Size i = index; i < len - 1; i++) {
            new (&data[i]) T(std::move(data[i + 1]));
            data[i + 1].~T();
        }
        len--;
    }

    inline void shrink() { resize(len + 1); }

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

    T pop() {
        if (len == 0) throw IndexOutOfRangeError(len, 0);
        T value = std::move(data[len - 1]);
        data[len - 1].~T();
        len--;
        return value;
    }

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

    VaList& operator+=(const VaList& other) {
        resize(len + other.len);
        for (Size i = 0; i < other.len; i++) {
            data[len + i] = other.data[i];
        }
        len += other.len;
        return *this;
    }

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

    bool operator!=(const VaList& other) const { return !(*this == other); }

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

    template <typename U = T>
    std::enable_if_t<std::is_same_v<U, VaString>, VaString> join(const VaString& sep = "") const {
        if (len == 0) return VaString();
        VaString result = data[0];
        for (Size i = 1; i < len; i++) {
            result += sep + data[i];
        }
        return result;
    }

    inline T* getData() { return data; }
    inline const T* getData() const { return data; }

    inline bool isEmpty() const { return len <= 0; }
    inline explicit operator bool() const { return len > 0; }

    void clear() {
        deleteObjects();
        std::free(data);
        data = nullptr;
        cap = 0;
        len = 0;
    }

    inline T* begin() { return data; }
    inline T* end() { return data + len; }
    const inline T* begin() const { return data; }
    const inline T* end() const { return data + len; }

    friend inline Size len(const VaList& list) { return list.len; }
    friend inline Size cap(const VaList& list) { return list.cap; }
};

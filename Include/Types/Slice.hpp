// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <Types/BasicTypedef.hpp>
#include <Types/Error.hpp>
#include <Types/List.hpp>

#include <vector>

template <typename T>
class VaSlice {
  protected:
    T* data;
    Size len;

    friend class VaList<T>;

  public:
    VaSlice(T* data, Size size) : data(data), len(size) {}
    VaSlice(T* begin, T* end) : data(begin), len(end - begin) {}

    VaSlice(VaList<T>& list) : data(list.data), len(list.len) {}

    template <typename C>
    VaSlice(C& container) : data(container.data()), len(container.size()) {}

    template <typename C>
    VaSlice(const C& container) : data(container.data()), len(container.size()) {}

    template <Size N>
    VaSlice(T (&arr)[N]) : data(arr), len(N) {}

    VaSlice(const VaSlice&) = default;
    VaSlice(VaSlice&&) = default;
    VaSlice& operator=(const VaSlice&) = default;
    VaSlice& operator=(VaSlice&&) = default;

    T& operator[](Size index) { return data[index]; }
    const T& operator[](Size index) const { return data[index]; }

    T& at(Size index) {
        if (index >= len) throw IndexOutOfTheRangeError(len, index);
        return data[index];
    }

    const T& at(Size index) const {
        if (index >= len) throw IndexOutOfTheRangeError(len, index);
        return data[index];
    }

    T* getData() noexcept { return data; }
    const T* getData() const noexcept { return data; }

    Size sizeBytes() const noexcept { return len * sizeof(T); }

    bool empty() const noexcept { return len == 0; }

    T& front() {
        if (empty()) throw ValueError("front() called on empty slice");
        return data[0];
    }

    T& back() {
        if (empty()) throw ValueError("back() called on empty slice");
        return data[len - 1];
    }

    const T& front() const {
        if (empty()) throw ValueError("front() called on empty slice");
        return data[0];
    }

    const T& back() const {
        if (empty()) throw ValueError("back() called on empty slice");
        return data[len - 1];
    }

    T* begin() noexcept { return data; }
    T* end() noexcept { return data + len; }

    const T* begin() const noexcept { return data; }
    const T* end() const noexcept { return data + len; }

    std::reverse_iterator<T*> rbegin() noexcept { return std::reverse_iterator<T*>(end()); }
    std::reverse_iterator<T*> rend() noexcept { return std::reverse_iterator<T*>(begin()); }

    VaSlice subslice(Size offset, Size count) {
        if (offset + count > len) {
            throw IndexOutOfTheRangeError("subslice(offset, count) out of range");
        }
        return VaSlice(data + offset, count);
    }

    VaSlice subslice(Size offset) {
        if (offset > len) {
            throw IndexOutOfTheRangeError("subslice(offset) out of range");
        }
        return VaSlice(data + offset, len - offset);
    }

    friend inline Size len(const VaSlice<T>& slice) noexcept { return slice.len; }
};

// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <cstdlib>

#include <Types/BasicTypedef.hpp>

#include <initializer_list>
#include <utility>

template <typename T>
class VaList {
  protected:
    Size len, cap;
    T* data;

    void resize(Size newCap) {
        T* newData = new T[newCap];
        for (Size i = 0; i < len; i++) {
            newData[i] = std::move(data[i]);
        }
        delete[] data;
        data = newData;
        cap = newCap;
    }

    inline void expand() { resize(cap == 0 ? 4 : cap * 2); }

    inline void update() {
        if (len >= cap) expand();
    }

  public:
    VaList() : len(0), cap(0), data(nullptr) {}

    VaList(std::initializer_list<T> init) : len(init.size()), cap(init.size()) {
        data = new T[cap];
        Size i = 0;
        for (const T& val : init) {
            data[i++] = val;
        }
    }

    VaList(const VaList& other) : len(other.len), cap(other.cap) {
        data = new T[cap];
        for (Size i = 0; i < len; i++) {
            data[i] = other.data[i];
        }
    }

    VaList(VaList&& other) noexcept : len(other.len), cap(other.cap), data(other.data) {
        other.data = nullptr;
        other.len = other.cap = 0;
    }

    ~VaList() { delete[] data; }

    template <typename... Args,
              typename = std::enable_if_t<(std::is_convertible_v<Args, T> && ...)>>
    VaList(Args&&... args) : len(sizeof...(Args)), cap(sizeof...(Args)) {
        data = new T[cap];
        Size i = 0;
        ((data[i++] = T(std::forward<Args>(args))), ...);
    }

    VaList& operator=(const VaList& other) {
        if (this == &other) return *this;
        delete[] data;
        len = other.len;
        cap = other.cap;
        data = new T[cap];
        for (Size i = 0; i < len; i++) {
            data[i] = other.data[i];
        }
        return *this;
    }

    VaList& operator=(VaList&& other) noexcept {
        if (this == &other) return *this;
        delete[] data;
        len = other.len;
        cap = other.cap;
        data = other.data;
        other.data = nullptr;
        other.len = other.cap = 0;
        return *this;
    }

    void append(const T& elm) {
        update();
        data[len++] = elm;
    }

    void append(T&& elm) {
        update();
        data[len++] = std::move(elm);
    }

    void extend(const VaList& other) { *this += other; }

    T& operator[](Size index) {
        // if (len < index) {
        //     throw
        // }
        return data[index];
    }

    const T& operator[](Size index) const { return data[index]; }

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
        for (Size i = 0; i < len; i++) {
            if (data[i] != other.data[i]) return false;
        }
        return true;
    }

    bool operator!=(const VaList& other) const { return !(*this == other); }

    // #ifdef VaLib_USE_CONCEPTS
    //     concept Addable = requires(T x, T y) {
    //         { x + y } -> std::same_as<T>;
    //     };

    //     template <Addable A = T>
    //     A sum() {
    //         TODO(_maqix_, "todo")
    //     }
    // #endif

    T* begin() { return data; }
    T* end() { return data + len; }
    const T* begin() const { return data; }
    const T* end() const { return data + len; }

    friend inline Size len(const VaList& list) { return list.len; }
    friend inline Size cap(const VaList& list) { return list.cap; }
};

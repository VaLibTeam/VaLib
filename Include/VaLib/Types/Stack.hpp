// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <VaLib/Types/BasicTypedef.hpp>
#include <VaLib/Types/Error.hpp>

#include <memory>
#include <stdexcept>
#include <vector>

namespace va::detail {

#ifdef VaLib_USE_CONCEPTS

template <typename T, typename Container>
class StackBase;

template <typename C, typename T>
concept VaContainer = requires(C t, T elm, T value, Size index) {
    { t.append(elm) } -> std::same_as<void>;
    { t.pop() } -> std::convertible_to<T>;

    { len(t) } -> std::convertible_to<Size>;
    { cap(t) } -> std::convertible_to<Size>;

    { t[index] } -> std::convertible_to<T&>;
};

template <typename C, typename T>
concept StdContainer = requires(C t, T elm, T value, Size index) {
    { t.push_back(elm) } -> std::same_as<void>;
    { t.pop_back() } -> std::convertible_to<void>;

    { t.size() } -> std::convertible_to<Size>;
    { t.capacity() } -> std::convertible_to<Size>;

    { t[index] } -> std::convertible_to<T&>;
};

template <typename T, typename Container>
    requires VaContainer<Container, T>
class StackBase<T, Container> {
  protected:
    Container container;

    Size size() const { return len(container); }
    Size capacity() const { return cap(container); }

  public:
    void push(const T& value) { container.append(value); }

    void pop() {
        if (isEmpty()) {
            throw IndexOutOfRangeError("Stack is empty");
        }

        container.pop();
    }

    T& top() {
        if (isEmpty()) {
            throw IndexOutOfRangeError("Stack is empty");
        }
        return container[len(container) - 1];
    }

    const T& top() const {
        if (isEmpty()) {
            throw IndexOutOfRangeError("Stack is empty");
        }
        return container[len(container) - 1];
    }

    inline bool isEmpty() const { return len(container) == 0; }
};

template <typename T, typename Container>
    requires StdContainer<Container, T>
class StackBase<T, Container> {
  protected:
    Container container;

    Size size() const { return container.size(); }
    Size capacity() const { return container.capacity(); }

  public:
    void push(const T& value) { container.push_back(value); }

    void pop() {
        if (isEmpty()) {
            throw IndexOutOfRangeError("Stack is empty");
        }

        container.pop_back();
    }

    T& top() {
        if (isEmpty()) {
            throw IndexOutOfRangeError("Stack is empty");
        }
        return container[container.size() - 1];
    }

    const T& top() const {
        if (isEmpty()) {
            throw IndexOutOfRangeError("Stack is empty");
        }
        return container[container.size() - 1];
    }

    inline bool isEmpty() const { return container.size() == 0; }
};

#else

template <typename T, typename Container>
class StackBase {
  protected:
    Container container;

    Size size() const { return len(container); }
    Size capacity() const { return cap(container); }

  public:
    void push(const T& value) { container.append(value); }

    void pop() {
        if (isEmpty()) {
            throw IndexOutOfRangeError("Stack is empty");
        }
        container.pop();
    }

    T& top() {
        if (isEmpty()) {
            throw IndexOutOfRangeError("Stack is empty");
        }
        return container[len(container) - 1];
    }

    const T& top() const {
        if (isEmpty()) {
            throw IndexOutOfRangeError("Stack is empty");
        }
        return container[len(container) - 1];
    }

    inline bool isEmpty() const { return len(container) == 0; }
};

#endif

template <typename T>
class StackBase<T, void> {
  protected:
    Size len, cap;
    T* data;

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

    Size size() const { return len; }
    Size capacity() const { return cap; }

    inline void deleteObjects() {
        if constexpr (!std::is_trivially_destructible_v<T>) {
            for (Size i = 0; i < len; i++) {
                data[i].~T();
            }
        }
    }

    inline void expand() { resize(cap == 0 ? 4 : cap * 2); }

    inline void update() {
        if (len >= cap) expand();
    }

  public:
    StackBase() : cap(0), len(0), data(nullptr) {}
    ~StackBase() {
        if (data) {
            deleteObjects();
            std::free(data);
        }
    }

    void push(const T& elm) {
        update();
        new (&data[len++]) T(elm);
    }

    void push(T&& elm) {
        update();
        new (&data[len++]) T(std::move(elm));
    }

    void pop() {
        if (len <= 0) {
            throw ValueError("pop() on empty stack");
        }

        data[len - 1].~T();
        len--;
    }

    T& top() {
        if (isEmpty()) {
            throw IndexOutOfRangeError("Stack is empty");
        }
        return data[len - 1];
    }

    const T& top() const {
        if (isEmpty()) {
            throw IndexOutOfRangeError("Stack is empty");
        }
        return data[len - 1];
    }

    bool isEmpty() const { return len == 0; }
};

}

template <typename T, typename Container = void>
class VaStack: public va::detail::StackBase<T, Container> {
  public:
    // using va::detail::StackBase<T, Container>::StackBase;
    // using va::detail::StackBase<T, Container>::push;
    // using va::detail::StackBase<T, Container>::pop;
    // using va::detail::StackBase<T, Container>::top;
    // using va::detail::StackBase<T, Container>::empty;
    // using va::detail::StackBase<T, Container>::size;

    friend inline Size len(const VaStack& stack) { return stack.size(); }
    friend inline Size cap(const VaStack& stack) { return stack.capacity(); }
};
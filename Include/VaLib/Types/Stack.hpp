// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <VaLib/Types/BasicTypedef.hpp>
#include <VaLib/Types/Error.hpp>

#include <memory>
#include <stdexcept>
#include <vector>

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
    { t.pop_back() } -> std::convertible_to<T>;

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

  public:
    void push(const T& value) { container.append(value); }

    void pop() {
        if (empty()) {
            throw IndexOutOfRangeError("Stack is empty");
        }
        container.pop();
    }

    T& top() {
        if (empty()) {
            throw IndexOutOfRangeError("Stack is empty");
        }
        return container[len(container) - 1];
    }

    const T& top() const {
        if (empty()) {
            throw IndexOutOfRangeError("Stack is empty");
        }
        return container[len(container) - 1];
    }

    inline bool empty() const { return len(container) == 0; }
};

template <typename T, typename Container>
    requires StdContainer<Container, T>
class StackBase<T, Container> {
  protected:
    Container container;

    Size size() const { return container.size(); }

  public:
    void push(const T& value) { container.push_back(value); }

    void pop() {
        if (empty()) {
            throw IndexOutOfRangeError("Stack is empty");
        }
        container.pop_back();
    }

    T& top() {
        if (empty()) {
            throw IndexOutOfRangeError("Stack is empty");
        }
        return container[container.size() - 1];
    }

    const T& top() const {
        if (empty()) {
            throw IndexOutOfRangeError("Stack is empty");
        }
        return container[container.size() - 1];
    }

    inline bool empty() const { return container.size() == 0; }
};

#else

template <typename T, typename Container>
class StackBase {
  protected:
    Container container;

    Size size() const { return len(container); }

  public:
    void push(const T& value) { container.append(value); }

    void pop() {
        if (empty()) {
            throw IndexOutOfRangeError("Stack is empty");
        }
        container.pop();
    }

    T& top() {
        if (empty()) {
            throw IndexOutOfRangeError("Stack is empty");
        }
        return container[len(container) - 1];
    }

    const T& top() const {
        if (empty()) {
            throw IndexOutOfRangeError("Stack is empty");
        }
        return container[len(container) - 1];
    }

    inline bool empty() const { return len(container) == 0; }
};

#endif

template <typename T>
class StackBase<T, void> {
  protected:
    Size capacity;
    Size stackSize;
    std::unique_ptr<T[]> data;

    Size size() const { return stackSize; }

    void resize() {
        Size newCapacity = capacity == 0 ? 1 : capacity * 2;
        std::unique_ptr<T[]> newData(new T[newCapacity]);

        for (Size i = 0; i < stackSize; ++i) {
            newData[i] = std::move(data[i]);
        }

        data = std::move(newData);
        capacity = newCapacity;
    }

  public:
    StackBase() : capacity(0), stackSize(0), data(nullptr) {}

    void push(const T& value) {
        if (stackSize >= capacity) {
            resize();
        }
        data[stackSize++] = value;
    }

    void pop() {
        if (empty()) {
            throw std::out_of_range("Stack is empty");
        }
        --stackSize;
    }

    T& top() {
        if (empty()) {
            throw std::out_of_range("Stack is empty");
        }
        return data[stackSize - 1];
    }

    const T& top() const {
        if (empty()) {
            throw std::out_of_range("Stack is empty");
        }
        return data[stackSize - 1];
    }

    bool empty() const { return stackSize == 0; }
};

template <typename T, typename Container = void>
class Stack: protected StackBase<T, Container> {
  public:
    using StackBase<T, Container>::StackBase;
    using StackBase<T, Container>::push;
    using StackBase<T, Container>::pop;
    using StackBase<T, Container>::top;
    using StackBase<T, Container>::empty;
    using StackBase<T, Container>::size;

    friend inline Size len(const Stack& stack) { return stack.size(); }
};
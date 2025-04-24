// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <VaLib/Utils/BasicDefine.hpp>
#include <VaLib/Types/BasicTypedef.hpp>
#include <VaLib/Types/Error.hpp>
#include <VaLib/Types/TypeTraits.hpp>

#include <initializer_list>
#include <type_traits>

template <typename T>
struct VaLinkedListNode {
    T value;
    VaLinkedListNode<T>* next;
    VaLinkedListNode<T>* prev;

    VaLinkedListNode() : next(nullptr), prev(nullptr) {}
    explicit VaLinkedListNode(const T& val) : value(val), next(nullptr), prev(nullptr) {}
};

template <typename T>
class VaLinkedList {
  public:
    using Node = VaLinkedListNode<T>;

  protected:
    Node* head;
    Node* tail;
    Size len;

    Node* freeListHead;
    Size freeListSize;

    void unlinkFromOrder(Node* node) noexcept {
        if (node->prev) {
            node->prev->next = node->next;
        } else {
            head = node->next;
        }

        if (node->next) {
            node->next->prev = node->prev;
        } else {
            tail = node->prev;
        }

        node->next = nullptr;
        node->prev = nullptr;
    }

    void linkToEnd(Node* node) noexcept {
        node->prev = tail;
        node->next = nullptr;

        if (tail) {
            tail->next = node;
        } else {
            head = node;
        }

        tail = node;
    }

    void linkToFront(Node* node) noexcept {
        node->next = head;
        node->prev = nullptr;

        if (head) {
            head->prev = node;
        } else {
            tail = node;
        }

        head = node;
    }

    void insertBefore(Node* target, Node* node) noexcept {
        node->prev = target->prev;
        node->next = target;

        if (target->prev) {
            target->prev->next = node;
        } else {
            head = node;
        }

        target->prev = node;
    }

    Node* getNode(const T& value) {
        if (freeListHead) {
            Node* node = freeListHead;
            freeListHead = freeListHead->next;
            freeListSize--;

            new (&node->value) T(value);
            node->next = nullptr;
            node->prev = nullptr;

            return node;
        }

        return new Node(value);
    }

    Node* getNode(T&& value) {
        if (freeListHead) {
            Node* node = freeListHead;
            freeListHead = freeListHead->next;
            freeListSize--;

            new (&node->value) T(std::move(value));
            node->next = nullptr;
            node->prev = nullptr;

            return node;
        }

        return new Node(std::move(value));
    }

    void returnNode(Node* node) noexcept {
        node->value.~T();
        node->next = freeListHead;
        node->prev = nullptr; // just in case
        freeListHead = node;

        freeListSize++;
    }

    void addNodes(Size count) noexcept {
        for (Size i = 0; i < count; ++i) {
            Node* node = new Node();
            node->next = freeListHead;
            freeListHead = node;
        }

        freeListSize += count;
    }

    Node* nodeAt(Size index) const {
        if (index < len / 2) {
            Node* current = head;
            for (Size i = 0; i < index; ++i) current = current->next;
            return current;
        } else {
            Node* current = tail;
            for (Size i = len - 1; i > index; --i) current = current->prev;
            return current;
        }
    }

  public:
    VaLinkedList() : head(nullptr), tail(nullptr), len(0), freeListHead(nullptr), freeListSize(0) {}
    VaLinkedList(Size initCap) : VaLinkedList() { addNodes(initCap); }

    VaLinkedList(const VaLinkedList& other) : VaLinkedList() {
        Node* current = other.head;
        while (current) {
            append(current->value);
            current = current->next;
        }
    }

    VaLinkedList(std::initializer_list<T> init) : VaLinkedList() {
        for (const auto& value: init) {
            append(value);
        }
    }

    VaLinkedList(VaLinkedList&& other) noexcept
        : head(other.head), tail(other.tail), len(other.len), freeListHead(other.freeListHead),
          freeListSize(other.freeListSize) {
        other.head = nullptr;
        other.tail = nullptr;
        other.len = 0;
        other.freeListHead = nullptr;
        other.freeListSize = 0;
    }

    ~VaLinkedList() {
        clear(true);
        while (freeListHead) {
            Node* next = freeListHead->next;
            delete freeListHead;
            freeListHead = next;
        }
    }

    VaLinkedList& operator=(const VaLinkedList& other) {
        if (this != &other) {
            clear();
            Node* current = other.head;
            while (current) {
                append(current->value);
                current = current->next;
            }
        }
        return *this;
    }

    VaLinkedList& operator=(VaLinkedList&& other) noexcept {
        if (this != &other) {
            clear();
            while (freeListHead) {
                Node* next = freeListHead->next;
                delete freeListHead;
                freeListHead = next;
            }

            head = other.head;
            tail = other.tail;
            len = other.len;
            freeListHead = other.freeListHead;
            freeListSize = other.freeListSize;

            other.head = nullptr;
            other.tail = nullptr;
            other.len = 0;
            other.freeListHead = nullptr;
            other.freeListSize = 0;
        }

        return *this;
    }

    bool operator==(const VaLinkedList& other) const {
        if (this == &other) return true;
        if (len != other.len) return false;

        Node* a = head;
        Node* b = other.head;

        while (a && b) {
            if constexpr (va::HasInequalityOperatorV<T>) {
                if (a->value != b->value) return false;
            } else if constexpr (va::HasEqualityOperatorV<T>) {
                if (!(a->value == b->value)) return false;
            } else if constexpr (std::is_trivially_copyable_v<T>) {
                if (std::memcmp(&a->value, &b->value, sizeof(T)) != 0) return false;
            } else {
                COMPILE_ERROR("Type T must have an equality operator or be trivially copyable");
            }

            a = a->next;
            b = b->next;
        }

        return a == nullptr && b == nullptr;
    }

    bool operator!=(const VaLinkedList& other) const { return !(*this == other); }

    bool operator<(const VaLinkedList& other) const {
        if (this == &other) return false;

        Node* a = head;
        Node* b = other.head;

        if constexpr (!va::HasLessThanV<T>) {
            // No less than operator, but for consistency return false
            return false;
        } else {
            while (a && b) {
                if (a->value < b->value) return true;
                if (b->value < a->value) return false;

                a = a->next;
                b = b->next;
            }

            return a == nullptr && b != nullptr;
        }
    }

    bool operator>(const VaLinkedList& other) const { return other < *this; }
    bool operator<=(const VaLinkedList& other) const { return !(other < *this); }
    bool operator>=(const VaLinkedList& other) const { return !(*this < other); }

    // Element access
    inline T& get(Size index) {
        Node* current = head;
        for (Size i = 0; i < index; ++i) {
            current = current->next;
        }

        return current->value;
    }

    inline const T& get(Size index) const {
        Node* current = head;
        for (Size i = 0; i < index; ++i) {
            current = current->next;
        }

        return current->value;
    }

    T& operator[](Size index) {
        return this->get(index);
    }

    const T& operator[](Size index) const {
        return this->get(index);
    }

    T& at(Size index) {
        if (index >= len) throw IndexOutOfRangeError(len, index);
        return this->get(index);
    }

    const T& at(Size index) const {
        if (index >= len) throw IndexOutOfRangeError(len, index);
        return this->get(index);
    }

    void append(const T& value) {
        Node* node = getNode(value);
        linkToEnd(node);
        len++;
    }

    void append(T&& value) {
        Node* node = getNode(std::move(value));
        linkToEnd(node);
        len++;
    }

    void prepend(const T& value) {
        Node* node = getNode(value);
        linkToFront(node);
        len++;
    }

    void prepend(T&& value) {
        Node* node = getNode(std::move(value));
        linkToFront(node);
        len++;
    }

    void insert(Size index, const T& value) {
        if (index > len) throw IndexOutOfRangeError(len, index);
        if (index == 0) return prepend(value);
        if (index == len) return append(value);

        Node* current = nodeAt(index);
        Node* node = getNode(value);
        insertBefore(current, node);
        len++;
    }

    void insert(Size index, T&& value) {
        if (index > len) throw IndexOutOfRangeError(len, index);
        if (index == 0) return prepend(std::move(value));
        if (index == len) return append(std::move(value));

        Node* current = nodeAt(index);
        Node* node = getNode(std::move(value));
        insertBefore(current, node);
        len++;
    }

    void del(Size index) {
        if (index >= len) throw IndexOutOfRangeError(len, index);

        Node* target = nodeAt(index);
        unlinkFromOrder(target);
        returnNode(target);
        len--;
    }

    T pop() {
        if (len == 0) throw ValueError("pop() on empty list");

        Node* target = tail;
        T value = target->value;

        unlinkFromOrder(target);
        returnNode(target);
        len--;

        return value;
    }

    T shift() {
        if (len == 0) throw ValueError("shift() on empty list");

        Node* target = head;
        T value = target->value;

        unlinkFromOrder(target);
        returnNode(target);
        len--;

        return value;
    }

    void reserve(Size minCap) noexcept {
        Size totalCurrent = len + freeListSize;
        if (minCap > totalCurrent) {
            addNodes(minCap - totalCurrent);
        }
    }

    void shrink() noexcept {
        while (freeListHead) {
            Node* next = freeListHead->next;
            delete freeListHead;
            freeListHead = next;
        }
        freeListSize = 0;
    }

    void clear(bool destroyNodes = false) noexcept {
        Node* current = head;
        while (current) {
            Node* next = current->next;
            unlinkFromOrder(current);

            if (destroyNodes) {
                delete current;
            } else {
                returnNode(current);
            }

            current = next;
        }

        head = tail = nullptr;
        len = 0;
    }

  public friends:
    friend inline Size len(const VaLinkedList& lst) noexcept { return lst.len; }
    friend inline Size cap(const VaLinkedList& lst) noexcept { return lst.len + lst.freeListSize; }

  public iterators:
    class Iterator {
        Node* current;

      public:
        explicit Iterator(Node* node) : current(node) {}

        T& operator*() const { return current->value; }
        T* operator->() const { return &current->value; }

        Iterator& operator++() {
            current = current->next;
            return *this;
        }

        Iterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        Iterator& operator--() {
            current = current->prev;
            return *this;
        }

        Iterator operator--(int) {
            Iterator tmp = *this;
            --(*this);
            return tmp;
        }

        bool operator==(const Iterator& other) const { return current == other.current; }
        bool operator!=(const Iterator& other) const { return current != other.current; }
    };

    class ConstIterator {
        Node* current;

      public:
        explicit ConstIterator(Node* node) : current(node) {}

        const T& operator*() const { return current->value; }
        const T* operator->() const { return &current->value; }

        ConstIterator& operator++() {
            current = current->next;
            return *this;
        }

        ConstIterator operator++(int) {
            ConstIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        ConstIterator& operator--() {
            current = current->prev;
            return *this;
        }

        ConstIterator operator--(int) {
            ConstIterator tmp = *this;
            --(*this);
            return tmp;
        }

        bool operator==(const ConstIterator& other) const { return current == other.current; }
        bool operator!=(const ConstIterator& other) const { return current != other.current; }
    };

    using ReverseIterator = std::reverse_iterator<Iterator>;
    using ConstReverseIterator = std::reverse_iterator<ConstIterator>;

    inline Iterator begin() { return Iterator(head); }
    inline Iterator end() { return Iterator(nullptr); }

    inline ConstIterator begin() const { return ConstIterator(head); }
    inline ConstIterator end() const { return ConstIterator(nullptr); }

    inline ConstIterator cbegin() const { return ConstIterator(head); }
    inline ConstIterator cend() const { return ConstIterator(nullptr); }

    inline ReverseIterator rbegin() { return ReverseIterator(end()); }
    inline ReverseIterator rend() { return ReverseIterator(begin()); }

    inline ConstReverseIterator rbegin() const { return ConstReverseIterator(end()); }
    inline ConstReverseIterator rend() const { return ConstReverseIterator(begin()); }

    inline ConstReverseIterator crbegin() const { return ConstReverseIterator(cend()); }
    inline ConstReverseIterator crend() const { return ConstReverseIterator(cbegin()); }
};

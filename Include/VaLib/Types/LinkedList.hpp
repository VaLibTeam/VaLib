// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <VaLib/Meta/BasicDefine.hpp>
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
    Node* head; /// @brief Pointer to the first node in the list
    Node* tail; /// @brief Pointer to the last node in the list
    Size len;   /// @brief Current number of elements in the list

    Node* freeListHead; /// @brief Current number of elements in the list
    Size freeListSize;  /// @brief Number of nodes currently stored in the free list

    /**
     * @brief Unlinks a node from the list without deallocating it.
     * @param node Pointer to the node to be unlinked.
     */
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

    /**
     * @brief Appends a node to the end of the list.
     * @param node Pointer to the node to be linked at the end.
     */
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

    /**
     * @brief Inserts a node at the front of the list.
     * @param node Pointer to the node to be linked at the front.
     */
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

    /**
     * @brief Inserts a node before a given target node in the list.
     * @param target The node before which the new node will be inserted.
     * @param node The node to insert.
     */
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

    /**
     * @brief Retrieves a node with a copy of the given value. Reuses from the free list if possible.
     * @param value The value to initialize the node with.
     * @return Pointer to the allocated or reused node.
     */
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

    /**
     * @brief Retrieves a node by moving the given value. Reuses from the free list if possible.
     * @param value The value to move into the node.
     * @return Pointer to the allocated or reused node.
     */
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

    /**
     * @brief Returns a node to the free list after destroying its value.
     * @param node Pointer to the node to be returned.
     */
    void returnNode(Node* node) noexcept {
        node->value.~T();
        node->next = freeListHead;
        node->prev = nullptr; // just in case
        freeListHead = node;

        freeListSize++;
    }

    /**
     * @brief Preallocates a number of nodes and adds them to the free list.
     * @param count Number of nodes to add.
     */
    void addNodes(Size count) noexcept {
        for (Size i = 0; i < count; ++i) {
            Node* node = new Node();
            node->next = freeListHead;
            freeListHead = node;
        }

        freeListSize += count;
    }

    /**
     * @brief Returns a pointer to the node at a specified index. Traverses from head or tail depending on proximity.
     * @param index Index of the node to retrieve.
     * @return Pointer to the node at the specified index.
     */
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
    /**
     * @brief Default constructor. Initializes an empty list with no reserved capacity.
     */
    VaLinkedList() : head(nullptr), tail(nullptr), len(0), freeListHead(nullptr), freeListSize(0) {}

    /**
     * @brief Constructs an empty list and preallocates a given number of nodes.
     * @param initCap Number of nodes to preallocate in the internal free list.
     */
    VaLinkedList(Size initCap) : VaLinkedList() { addNodes(initCap); }

    /**
     * @brief Copy constructor. Creates a deep copy of another list, duplicating all elements.
     * @param other The list to copy from.
     */
    VaLinkedList(const VaLinkedList& other) : VaLinkedList() {
        Node* current = other.head;
        while (current) {
            append(current->value);
            current = current->next;
        }
    }

    /**
     * @brief Constructs a list from an initializer list.
     * @param init List of elements to initialize the list with.
     */
    VaLinkedList(std::initializer_list<T> init) : VaLinkedList() {
        for (const auto& value: init) {
            append(value);
        }
    }

    /**
     * @brief Move constructor. Transfers ownership of resources from another list.
     * @param other The list to move from.
     */
    VaLinkedList(VaLinkedList&& other) noexcept : len(other.len), freeListHead(other.freeListHead), freeListSize(other.freeListSize) {
        head = other.head;
        tail = other.tail;

        other.head = nullptr;
        other.tail = nullptr;
        other.len = 0;
        other.freeListHead = nullptr;
        other.freeListSize = 0;
    }

    /**
     * @brief Destructor. Destroys all elements and releases memory used by the free list.
     */
    ~VaLinkedList() {
        clear(true);
        while (freeListHead) {
            Node* next = freeListHead->next;
            delete freeListHead;
            freeListHead = next;
        }
    }

    /**
     * @brief Copy assignment operator. Replaces contents with a deep copy of another list.
     * @param other The list to copy from.
     * @return Reference to this list.
     */
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

    /**
     * @brief Move assignment operator. Transfers ownership of contents from another list.
     * @param other The list to move from.
     * @return Reference to this list.
     */
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

    /**
     * @brief Returns a reference to the element at the given index.
     * @param index Position of the element to access.
     * @return Reference to the element at the specified index.
     *
     * @warning This is a slow O(n/2) operation. Prefer other access patterns when possible.
     */
    inline T& get(Size index) {
        return nodeAt(index)->value;
    }

    /**
     * @brief Returns a const reference to the element at the given index.
     * @param index Position of the element to access.
     * @return Const reference to the element at the specified index.
     *
     * @warning This is a slow O(n/2) operation. Prefer other access patterns when possible.
     */
    inline const T& get(Size index) const {
        return nodeAt(index)->value;
    }

    /**
     * @brief Provides unchecked access to the element at the given index.
     * @param index Position of the element to access.
     * @return Reference to the element at the specified index.
     *
     * @warning This is a slow O(n/2) operation. Use only when bounds are guaranteed.
     */
    T& operator[](Size index) {
        return this->get(index);
    }

    /**
     * @brief Provides unchecked const access to the element at the given index.
     * @param index Position of the element to access.
     * @return Const reference to the element at the specified index.
     *
     * @warning This is a slow O(n/2) operation. Use only when bounds are guaranteed.
     */
    const T& operator[](Size index) const {
        return this->get(index);
    }

    /**
     * @brief Returns a reference to the element at the given index with bounds checking.
     * @param index Position of the element to access.
     * @return Reference to the element at the specified index.
     *
     * @throw IndexOutOfRangeError if index is out of bounds.
     * @warning This is a slow O(n/2) operation. Use with care in performance-critical code.
     */
    T& at(Size index) {
        if (index >= len) throw IndexOutOfRangeError(len, index);
        return this->get(index);
    }

    /**
     * @brief Returns a const reference to the element at the given index with bounds checking.
     * @param index Position of the element to access.
     * @return Const reference to the element at the specified index.
     *
     * @throw IndexOutOfRangeError if index is out of bounds.
     * @warning This is a slow O(n/2) operation. Use with care in performance-critical code.
     */
    const T& at(Size index) const {
        if (index >= len) throw IndexOutOfRangeError(len, index);
        return this->get(index);
    }

    /**
     * @brief Appends a new element to the end of the list.
     * @param value The value to append.
     */
    void append(const T& value) {
        Node* node = getNode(value);
        linkToEnd(node);
        len++;
    }

    /**
     * @brief Appends a new element to the end of the list (move version).
     * @param value The value to append.
     */
    void append(T&& value) {
        Node* node = getNode(std::move(value));
        linkToEnd(node);
        len++;
    }

    /**
     * @brief Prepends a new element to the beginning of the list.
     * @param value The value to prepend.
     */
    void prepend(const T& value) {
        Node* node = getNode(value);
        linkToFront(node);
        len++;
    }

    /**
     * @brief Prepends a new element to the beginning of the list (move version).
     * @param value The value to prepend.
     */
    void prepend(T&& value) {
        Node* node = getNode(std::move(value));
        linkToFront(node);
        len++;
    }

    /**
     * @brief Inserts a new element at the specified index.
     * @param index The position at which to insert the element.
     * @param value The value to insert.
     *
     * @throw IndexOutOfRangeError If the index is greater than the current list length.
     */
    void insert(Size index, const T& value) {
        if (index > len) throw IndexOutOfRangeError(len, index);
        if (index == 0) return prepend(value);
        if (index == len) return append(value);

        Node* current = nodeAt(index);
        Node* node = getNode(value);
        insertBefore(current, node);
        len++;
    }

    /**
     * @brief Inserts a new element at the specified index (move version).
     * @param index The position at which to insert the element.
     * @param value The value to insert.
     *
     * @throw IndexOutOfRangeError If the index is greater than the current list length.
     */
    void insert(Size index, T&& value) {
        if (index > len) throw IndexOutOfRangeError(len, index);
        if (index == 0) return prepend(std::move(value));
        if (index == len) return append(std::move(value));

        Node* current = nodeAt(index);
        Node* node = getNode(std::move(value));
        insertBefore(current, node);
        len++;
    }

    /**
     * @brief Deletes the element at the specified index.
     * @param index The position of the element to delete.
     *
     * @throw IndexOutOfRangeError If the index is out of bounds.
     */
    void del(Size index) {
        if (index >= len) throw IndexOutOfRangeError(len, index);

        Node* target = nodeAt(index);
        unlinkFromOrder(target);
        returnNode(target);
        len--;
    }

    /**
     * @brief Removes and returns the last element in the list.
     *
     * @return The value of the removed element.
     * @throw ValueError If the list is empty.
     */
    T pop() {
        if (len == 0) throw ValueError("pop() on empty list");

        Node* target = tail;
        T value = target->value;

        unlinkFromOrder(target);
        returnNode(target);
        len--;

        return value;
    }

    /**
     * @brief Deletes and returns the element at the specified index.
     * @param index The position of the element to delete.
     *
     * @throw IndexOutOfRangeError If the index is out of bounds.
     */
    T pop(Size index) {
        if (index >= len) throw IndexOutOfRangeError(len, index);

        Node* target = nodeAt(index);
        T value = target->value;

        unlinkFromOrder(target);
        returnNode(target);
        len--;

        return value;
    }

    /**
     * @brief Removes and returns the first element in the list.
     *
     * @return The value of the removed element.
     * @throw ValueError If the list is empty.
     */
    T shift() {
        if (len == 0) throw ValueError("shift() on empty list");

        Node* target = head;
        T value = target->value;

        unlinkFromOrder(target);
        returnNode(target);
        len--;

        return value;
    }

    /**
     * @brief Ensures the internal storage can accommodate at least the given number of elements.
     * @param minCap The minimum total capacity (used + free nodes) to ensure.
     *
     * @note Nodes are preallocated and added to the free list if necessary.
     */
    void reserve(Size minCap) noexcept {
        Size totalCurrent = len + freeListSize;
        if (minCap > totalCurrent) {
            addNodes(minCap - totalCurrent);
        }
    }

    /**
     * @brief Frees all unused nodes currently in the free list.
     *
     * @note This reduces memory usage by deallocating unused preallocated nodes.
     */
    void shrink() noexcept {
        while (freeListHead) {
            Node* next = freeListHead->next;
            delete freeListHead;
            freeListHead = next;
        }
        freeListSize = 0;
    }

    /**
     * @brief Removes all elements from the list.
     * @param destroyNodes If true, all nodes are deleted instead of returned to the free list.
     *
     * @note If destroyNodes is false, nodes are preserved for reuse.
     */
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

  public:
    /**
     * @brief Compares two lists for equality element by element.
     * @param lhs The first list to compare.
     * @param rhs The second list to compare.
     * @return True if both lists are equal in size and content.
     *
     * @note Supports types with !=, ==, or that are trivially copyable.
     */
    friend bool operator==(const VaLinkedList& lhs, const VaLinkedList& rhs) {
        if (&lhs == &rhs) return true;
        if (lhs.len != rhs.len) return false;

        Node* a = lhs.head;
        Node* b = rhs.head;

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

    /**
     * @brief Checks whether two lists are not equal.
     * @param lhs The first list to compare.
     * @param rhs The second list to compare.
     * @return True if the lists differ in size or content.
     */
    friend bool operator!=(const VaLinkedList& lhs, const VaLinkedList& rhs) { return !(lhs == rhs); }

    /**
     * @brief Compares two lists lexicographically using the less-than operator.
     * @param lhs The first list.
     * @param rhs The second list.
     * @return True if lhs is lexicographically less than rhs.
     *
     * @warning Does nothing meaningful if T does not support operator< — returns false.
     */
    friend bool operator<(const VaLinkedList& lhs, const VaLinkedList& rhs) {
        if (&lhs == &rhs) return false;

        Node* a = lhs.head;
        Node* b = rhs.head;

        if constexpr (!va::HasLessThanV<T>) {
            // no less than operator, but for consistency return false
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

    /**
     * @brief Compares two lists to determine if the first is greater than the second.
     * @param lhs The first list.
     * @param rhs The second list.
     * @return true if lhs is greater than rhs.
     */
    friend bool operator>(const VaLinkedList& lhs, const VaLinkedList& rhs) { return rhs < lhs; }

    /**
     * @brief Checks if one list is less than or equal to another.
     * @param lhs The first list.
     * @param rhs The second list.
     * @return True if lhs is less than or equal to rhs.
     */
    friend bool operator<=(const VaLinkedList& lhs, const VaLinkedList& rhs) { return !(rhs < lhs); }

    /**
     * @brief Checks if one list is greater than or equal to another.
     * @param lhs The first list.
     * @param rhs The second list.
     * @return True if lhs is greater than or equal to rhs.
     */
    friend bool operator>=(const VaLinkedList& lhs, const VaLinkedList& rhs) { return !(lhs < rhs); }

  public :
    friend inline Size len(const VaLinkedList& lst) noexcept { return lst.len; }
    friend inline Size cap(const VaLinkedList& lst) noexcept { return lst.len + lst.freeListSize; }

  public:
    /**
     * @brief Bidirectional iterator for traversing and modifying the list.
     *
     * Provides access to non-const elements and supports both forward and backward iteration.
     */
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

    /**
     * @brief Provides read-only bidirectional access to the list elements.
     *
     * Supports forward and backward traversal without modifying elements.
     */
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

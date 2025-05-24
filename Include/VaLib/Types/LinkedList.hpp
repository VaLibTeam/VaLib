// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <VaLib/RawAccess/LinkedList.hpp>

#include <VaLib/Meta/BasicDefine.hpp>
#include <VaLib/Types/BasicTypedef.hpp>
#include <VaLib/Types/Error.hpp>
#include <VaLib/Types/TypeTraits.hpp>

#include <initializer_list>

template <typename T>
struct VaLinkedListNode {
    T value;
    VaLinkedListNode<T>* next;
    VaLinkedListNode<T>* prev;

    VaLinkedListNode() : next(nullptr), prev(nullptr) {}
    explicit VaLinkedListNode(const T& val) : value(val), next(nullptr), prev(nullptr) {}
};

template <typename T>
class alignas(VaLinkedListRawView<T>) VaLinkedList {
  public:
    using Node = VaLinkedListNode<T>;

  protected:
    Node* head; ///< Pointer to the first node in the list
    Node* tail; ///< Pointer to the last node in the list
    Size len;   ///< Current number of elements in the list

    Node* freeListHead; ///< Current number of elements in the list
    Size freeListSize;  ///< Number of nodes currently stored in the free list

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

    #if __cplusplus >= CPP17
        template <typename Tuple, Size... Is>
        inline void prependAllImpl(Tuple&& tup, std::index_sequence<Is...>) {
            // reverse fold using index pack
            ((prepend(std::get<sizeof...(Is) - 1 - Is>(std::forward<Tuple>(tup)))), ...);
        }
    #endif

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
    [[ nodiscard("O(n) access") ]]
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
    [[ nodiscard("O(n) access") ]]
    inline const T& get(Size index) const {
        return nodeAt(index)->value;
    }

    /**
     * @brief Provides unchecked access to the element at the given index.
     * @param index Position of the element to access.
     * @return Reference to the element at the specified index.
     *
     * @warning This operation is O(n). For frequent indexed access, consider a contiguous list.
     */
    [[ nodiscard("O(n) access") ]]
    T& operator[](Size index) {
        return this->get(index);
    }

    /**
     * @brief Provides unchecked const access to the element at the given index.
     * @param index Position of the element to access.
     * @return Const reference to the element at the specified index.
     *
     * @warning This operation is O(n). For frequent indexed access, consider a contiguous list.
     */
    [[ nodiscard("O(n) access") ]]
    const T& operator[](Size index) const {
        return this->get(index);
    }

    /**
     * @brief Returns a reference to the element at the given index with bounds checking.
     *        Supports negative indexing, where -1 refers to the last element, -2 to the second last, and so on.
     * @param index Position of the element to access. Negative values count from the end of the list.
     * @return Reference to the element at the specified index.
     *
     * @throws IndexOutOfRangeError if index is out of bounds.
     * @warning This is a slow O(n/2) operation. Use with care in performance-critical code.
     */
    [[ nodiscard("O(n) access") ]]
    T& at(int32 index) {
        if (index < 0) index += len;
        if (index < 0 || static_cast<Size>(index) >= len) throw IndexOutOfRangeError(len, index);
        return this->get(index);
    }

    /**
     * @brief Returns a const reference to the element at the given index with bounds checking.
     *        Supports negative indexing, where -1 refers to the last element, -2 to the second last, and so on.
     * @param index Position of the element to access. Negative values count from the end of the list.
     * @return Const reference to the element at the specified index.
     *
     * @throws IndexOutOfRangeError if index is out of bounds.
     * @warning This is a slow O(n/2) operation. Use with care in performance-critical code.
     */
    [[ nodiscard("O(n) access") ]]
    const T& at(int32 index) const {
        if (index < 0) index += len;
        if (index < 0 || index >= len) throw IndexOutOfRangeError(len, index);
        return this->get(index);
    }

    /**
     * @brief Sets the value at the specified index to the given value.
     *        Supports negative indexing, where -1 refers to the last element, -2 to the second last, and so on.
     * @param index Position of the element to set. Negative values count from the end of the list.
     * @param value The value to set at the specified index.
     *
     * @throws IndexOutOfRangeError if index is out of bounds.
     * @warning This is a slow O(n/2) operation. Use with care in performance-critical code.
     */
    void set(int32 index, const T& value) {
        if (index < 0) index += len;
        if (index < 0 || static_cast<Size>(index) >= len) throw IndexOutOfRangeError(len, index);
        nodeAt(index)->value = value;
    }

    /**
     * @brief Sets the value at the specified index to the given value (move version).
     *        Supports negative indexing, where -1 refers to the last element, -2 to the second last, and so on.
     * @param index Position of the element to set. Negative values count from the end of the list.
     * @param value The value to move into the specified index.
     *
     * @throws IndexOutOfRangeError if index is out of bounds.
     * @warning This is a slow O(n/2) operation. Use with care in performance-critical code.
     */
    void set(int32 index, T&& value) {
        if (index < 0) index += len;
        if (index < 0 || static_cast<Size>(index) >= len) throw IndexOutOfRangeError(len, index);
        nodeAt(index)->value = std::move(value);
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
     * @throws IndexOutOfRangeError If the index is greater than the current list length.
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
     * @throws IndexOutOfRangeError If the index is greater than the current list length.
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

    template <typename... Args>
    void appendEmplace(Args&&... args) {
        Node* node = getNode();
        new (&node->value) T(std::forward<Args>(args)...);
        linkToEnd(node);
        len++;
    }

    template <typename... Args>
    void prependEmplace(Args&&... args) {
        Node* node = getNode();
        new (&node->value) T(std::forward<Args>(args)...);
        linkToFront(node);
        len++;
    }

    template <typename... Args>
    void insertEmplace(Size index, Args&&... args) {
        if (index > len) throw IndexOutOfRangeError(len, index);
        if (index == 0) return prependEmplace(std::forward<Args>(args)...);
        if (index == len) return appendEmplace(std::forward<Args>(args)...);

        Node* current = nodeAt(index);
        Node* node = getNode();
        new (&node->value) T(std::forward<Args>(args)...);

        insertBefore(current, node);
        len++;
    }

    /**
     * @brief Appends each element from an iterable container to the end of the list.
     * @tparam Iterable A container type that supports iteration (e.g., std::vector, std::list).
     * @param iterable The container whose elements will be appended.
     */
    template <typename Iterable>
    void appendEach(const Iterable& iterable) {
        for (auto it = std::begin(iterable); it != std::end(iterable); ++it) {
            append(*it);
        }
    }

    /**
     * @brief Appends each element from an iterable container to the end of the list (move version).
     * @tparam Iterable A container type that supports iteration (e.g., std::vector, std::list).
     * @param iterable The container whose elements will be moved and appended.
     */
    template <typename Iterable, typename = tt::EnableIf<!tt::IsSame<tt::RemoveCVRef<Iterable>, VaLinkedList<T>>> >
    void appendEach(Iterable&& iterable) {
        for (auto it = std::begin(iterable); it != std::end(iterable); ++it) {
            append(std::move(*it));
        }
    }

    /**
     * @brief Appends all elements from another linked list to the end of this list.
     * @param other The linked list whose elements will be appended.
     */
    void appendEach(const VaLinkedList& other) {
        for (Node* n = other.head; n; n = n->next) {
            append(n->value);
        }
    }

    /**
     * @brief Appends all elements from another linked list to the end of this list (move version).
     * @param other The linked list whose elements will be moved and appended.
     */
    void appendEach(VaLinkedList&& other) {
        if (other.len == 0 || this == &other) return;
        if (len == 0) {
            head = other.head;
            tail = other.tail;
            len = other.len;
        } else {
            tail->next = other.head;
            other.head->prev = tail;
            tail = other.tail;
            len += other.len;
        }
        other.head = other.tail = nullptr;
        other.len = 0;
    }

    template <typename Iterable>
    void extend(const Iterable& iterable) {
        appendEach(iterable);
    }

    template <typename Iterable, typename = tt::EnableIf<!tt::IsSame<tt::RemoveCVRef<Iterable>, VaLinkedList<T>>> >
    void extend(Iterable&& iterable) {
        appendEach(std::forward<Iterable>(iterable));
    }

    /**
     * @brief Prepends each element from an iterable container to the beginning of the list.
     * @tparam Iterable A container type that supports reverse iteration (e.g., std::vector, std::list).
     * @param iterable The container whose elements will be prepended.
     */
    template <typename Iterable>
    void prependEach(const Iterable& iterable) {
        for (auto it = std::rbegin(iterable); it != std::rend(iterable); ++it) {
            prepend(*it);
        }
    }

    /**
     * @brief Prepends each element from an iterable container to the beginning of the list (move version).
     * @tparam Iterable A container type that supports reverse iteration (e.g., std::vector, std::list).
     * @param iterable The container whose elements will be moved and prepended.
     */
    template <typename Iterable, typename = tt::EnableIf<!tt::IsSame<tt::RemoveCVRef<Iterable>, VaLinkedList<T>>> >
    void prependEach(Iterable&& iterable) {
        for (auto it = std::rbegin(iterable); it != std::rend(iterable); ++it) {
            prepend(std::move(*it));
        }
    }

    /**
     * @brief Prepends all elements from another linked list to the beginning of this list.
     * @param other The linked list whose elements will be prepended.
     */
    void prependEach(const VaLinkedList& other) {
        for (Node* n = other.tail; n; n = n->prev) {
            prepend(n->value);
        }
    }

    /**
     * @brief Prepends all elements from another linked list to the beginning of this list (move version).
     * @param other The linked list whose elements will be moved and prepended.
     */
    void prependEach(VaLinkedList&& other) {
        if (other.len == 0 || this == &other) return;
        if (len == 0) {
            head = other.head;
            tail = other.tail;
            len = other.len;
        } else {
            other.tail->next = head;
            head->prev = other.tail;
            head = other.head;
            len += other.len;
        }
        other.head = other.tail = nullptr;
        other.len = 0;
    }

    /**
     * @brief Inserts each element from an iterable container at the specified position in the list.
     * @tparam Iterable A container type that supports reverse iteration (e.g., std::vector, std::list).
     * @param pos The position at which to insert the elements.
     * @param iterable The container whose elements will be inserted.
     *
     * @throws IndexOutOfRangeError If the position is out of bounds.
     */
    template <typename Iterable>
    void insertEach(Size pos, const Iterable& iterable) {
        if (pos > len) throw IndexOutOfRangeError(len, pos);
        for (auto it = std::rbegin(iterable); it != std::rend(iterable); ++it) {
            insert(pos, *it);
        }
    }

    /**
     * @brief Inserts each element from an iterable container at the specified position in the list (move version).
     * @tparam Iterable A container type that supports reverse iteration (e.g., std::vector, std::list).
     * @param pos The position at which to insert the elements.
     * @param iterable The container whose elements will be moved and inserted.
     *
     * @throws IndexOutOfRangeError If the position is out of bounds.
     */
    template <typename Iterable, typename = tt::EnableIf<!tt::IsSame<tt::RemoveCVRef<Iterable>, VaLinkedList<T>>> >
    void insertEach(Size pos, Iterable&& iterable) {
        if (pos > len) throw IndexOutOfRangeError(len, pos);
        for (auto it = std::rbegin(iterable); it != std::rend(iterable); ++it) {
            insert(pos, std::move(*it));
        }
    }

    /**
     * @brief Inserts all elements from another linked list at the specified position in this list.
     * @param pos The position at which to insert the elements.
     * @param other The linked list whose elements will be inserted.
     *
     * @throws IndexOutOfRangeError If the position is out of bounds.
     */
    void insertEach(Size pos, const VaLinkedList& other) {
        if (pos > len) throw IndexOutOfRangeError(len, pos);
        for (Node* n = other.tail; n; n = n->prev) {
            insert(pos, n->value);
        }
    }

    /**
     * @brief Inserts all elements from another linked list at the specified position in this list (move version).
     * @param pos The position at which to insert the elements.
     * @param other The linked list whose elements will be moved and inserted.
     *
     * @throws IndexOutOfRangeError If the position is out of bounds.
     */
    void insertEach(Size pos, VaLinkedList&& other) {
        if (other.len == 0 || this == &other) return;
        if (pos > len) throw IndexOutOfRangeError(len, pos);
        if (pos == len) {
            appendEach(std::move(other));
            return;
        }
        if (pos == 0) {
            prependEach(std::move(other));
            return;
        }
        Node* atNode = nodeAt(pos);
        Node* before = atNode->prev;

        before->next = other.head;
        other.head->prev = before;
        other.tail->next = atNode;
        atNode->prev = other.tail;

        len += other.len;
        other.head = other.tail = nullptr;
        other.len = 0;
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
     * @param index The position of the element to delete.
     *
     * @throws IndexOutOfRangeError If the index is out of bounds.
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
     * @throws ValueError If the list is empty.
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
     * @throws IndexOutOfRangeError If the index is out of bounds.
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
     * @return The value of the removed element.
     *
     * @throws ValueError If the list is empty.
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
     * @brief Returns a reference to the first element in the list.
     * @return Reference to the first element.
     *
     * @throws ValueError If the list is empty.
     */
    T& front() {
        if (len == 0) throw ValueError("front() on empty list");
        return head->value;
    }

    /**
     * @brief Returns a const reference to the first element in the list.
     * @return Const reference to the first element.
     *
     * @throws ValueError If the list is empty.
     */
    const T& front() const {
        if (len == 0) throw ValueError("front() on empty list");
        return head->value;
    }

    /**
     * @brief Returns a reference to the last element in the list.
     * @return Reference to the last element.
     *
     * @throws ValueError If the list is empty.
     */
    T& back() {
        if (len == 0) throw ValueError("back() on empty list");
        return tail->value;
    }

    /**
     * @brief Returns a const reference to the last element in the list.
     *
     * @return Const reference to the last element.
     * @throws ValueError If the list is empty.
     */
    const T& back() const {
        if (len == 0) throw ValueError("back() on empty list");
        return tail->value;
    }

    /**
     * @brief Returns a reference to the first element in the list without bounds checking.
     * @return Reference to the first element.
     *
     * @note This method does not perform any size checks. The behavior is undefined if the list is empty.
     */
    T& frontUnchecked() noexcept {
        return head->value;
    }

    /**
     * @brief Returns a const reference to the first element in the list without bounds checking.
     * @return Const reference to the first element.
     *
     * @note This method does not perform any size checks. The behavior is undefined if the list is empty.
     */
    const T& frontUnchecked() const noexcept {
        return head->value;
    }

    /**
     * @brief Returns a reference to the last element in the list without bounds checking.
     * @return Reference to the last element.
     *
     * @note This method does not perform any size checks. The behavior is undefined if the list is empty.
     */
    T& backUnchecked() noexcept {
        return tail->value;
    }

    /**
     * @brief Returns a const reference to the last element in the list without bounds checking.
     * @return Const reference to the last element.
     *
     * @note This method does not perform any size checks. The behavior is undefined if the list is empty.
     */
    const T& backUnchecked() const noexcept {
        return tail->value;
    }

    /**
     * @brief Ensures the internal storage can accommodate at least the given number of elements.
     * @param minCap The minimum total capacity (used + free nodes) to ensure.
     *
     * @note Nodes are preallocated and added to the free list if necessary.
     * @warning This is a linked list with a free list system. Using reserve() before performing many element addition operations does not improve performance, as linked lists do not benefit from preallocation in the same way as contiguous containers like array lists.
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

    bool isEmpty() {
        return this->len == 0;
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

    /**
     * @brief Returns the number of elements currently stored in the list.
     * @return The current length of the list.
     */
    inline Size getLength() const noexcept {
        return this->len;
    }

    /**
     * @brief Returns the total capacity of the linked list.
     *        This method calculates the total capacity of the linked list,
     *        which is the sum of the number of elements currently stored in the list (`len`)
     *        and the number of unused nodes available in the free list (`freeListSize`).
     * @return The total capacity of the linked list.
     */
    inline Size getCapacity() const noexcept {
        return this->len + this->freeListSize;
    }

    /**
     * @brief Alias for a raw layout-compatible view of the linked list internals.
     *        This struct can be used for safe inspection or unsafe manipulation.
     */
    using RawView = VaLinkedListRawView<T>;

    /**
     * @brief Returns a read-only view of the linked list internals.
     *        Reinterprets this list as a const-qualified raw view struct.
     * @return A const pointer to `VaLinkedListRawView<T>`.
     *
     * @note This method is layout-dependent and assumes standard layout ordering.
     * @see getUnsafeAccess() for mutating access.
     */
    const RawView* getRawView() const {
        return reinterpret_cast<const RawView*>(this);
    }

    /**
     * @brief Returns a mutable raw view of the linked list internals.
     *        Reinterprets this list as a `VaLinkedListRawView<T>*`.
     *        Allows full mutation of head/tail/length/freelist pointers.
     *
     * @return A pointer to `VaLinkedListRawView<T>` for direct access.
     *
     * @warning This is an unsafe API. You are responsible for keeping the structure valid.
     * @see getRawView() for safe read-only access.
     */
    RawView* getUnsafeAccess() {
        return reinterpret_cast<RawView*>(this);
    }

  public:
    /**
     * @brief Compares two lists for equality element by element.
     * @param lhs The first list to compare.
     * @param rhs The second list to compare.
     * @return True if both lists are equal in size and content.
     *
     * @note Supports types with ==, or that are trivially copyable.
     */
    friend bool operator==(const VaLinkedList& lhs, const VaLinkedList& rhs) {
        if (&lhs == &rhs) return true;
        if (lhs.len != rhs.len) return false;

        Node* a = lhs.head;
        Node* b = rhs.head;

        while (a && b) {
            #if __cplusplus >= CPP17
                if constexpr (tt::HasEqualityOperator_v<T>) {
                    if (!(a->value == b->value)) return false;
                } else if constexpr (tt::IsTriviallyCopyable<T>) {
                    if (std::memcmp(&a->value, &b->value, sizeof(T)) != 0) return false;
                } else {
                    static_assert(sizeof(T) == -1, "Cannon compare list with non-comparable elements");
                }
            #else
                if (!(a->value == b->value)) return false;
            #endif

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

        static_assert(tt::HasLessThan<T>::value, "Cannot compare lists with non-comparable elements");
        while (a && b) {
            if (a->value < b->value) return true;
            if (b->value < a->value) return false;

            a = a->next;
            b = b->next;
        }

        return a == nullptr && b != nullptr;
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

    template <typename Iterable, typename = tt::EnableIf<!tt::IsSame<tt::RemoveCVRef<Iterable>, VaLinkedList<T>>>>
    friend bool operator==(const VaLinkedList& lhs, const Iterable& rhs) {
        auto it = std::begin(rhs);
        Node* current = lhs.head;

        while (current && it != std::end(rhs)) {
            if (!(current->value == *it)) return false;
            current = current->next;
            ++it;
        }

        return current == nullptr && it == std::end(rhs);
    }

    template <typename Iterable, typename = tt::EnableIf<!tt::IsSame<tt::RemoveCVRef<Iterable>, VaLinkedList<T>>>>
    friend bool operator!=(const VaLinkedList& lhs, const Iterable& rhs) {
        return !(lhs == rhs);
    }

    template <typename Iterable, typename = tt::EnableIf<!tt::IsSame<tt::RemoveCVRef<Iterable>, VaLinkedList<T>>>>
    friend bool operator<(const VaLinkedList& lhs, const Iterable& rhs) {
        auto it = std::begin(rhs);
        Node* current = lhs.head;

        while (current && it != std::end(rhs)) {
            if (current->value < *it) return true;
            if (*it < current->value) return false;
            current = current->next;
            ++it;
        }

        return current == nullptr && it != std::end(rhs);
    }

    template <typename Iterable, typename = tt::EnableIf<!tt::IsSame<tt::RemoveCVRef<Iterable>, VaLinkedList<T>>>>
    friend bool operator>(const VaLinkedList& lhs, const Iterable& rhs) {
        return rhs < lhs;
    }

    template <typename Iterable, typename = tt::EnableIf<!tt::IsSame<tt::RemoveCVRef<Iterable>, VaLinkedList<T>>>>
    friend bool operator<=(const VaLinkedList& lhs, const Iterable& rhs) {
        return !(rhs < lhs);
    }

    template <typename Iterable, typename = tt::EnableIf<!tt::IsSame<tt::RemoveCVRef<Iterable>, VaLinkedList<T>>>>
    friend bool operator>=(const VaLinkedList& lhs, const Iterable& rhs) {
        return !(lhs < rhs);
    }

  public friends:
    /**
     * @brief Returns the number of elements in the linked list.
     * @param lst The linked list to query.
     * @return The number of elements in the list.
     */
    friend inline Size len(const VaLinkedList& lst) noexcept { return lst.len; }

    /**
     * @brief Returns the total capacity of the linked list, including free nodes.
     * @param lst The linked list to query.
     * @return The total capacity of the list.
     */
    friend inline Size cap(const VaLinkedList& lst) noexcept { return lst.len + lst.freeListSize; }

  public iterators:
    /**
     * @brief Bidirectional iterator for traversing and modifying the list.
     *        Provides access to non-const elements and supports both forward and backward iteration.
     */
    class Iterator {
      protected:
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
     *        Supports forward and backward traversal without modifying elements.
     */
    class ConstIterator {
      protected:
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

// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <VaLib/Types/BasicTypedef.hpp>
#include <VaLib/Meta/BasicDefine.hpp>
#include <VaLib/Types/Error.hpp>

#include <algorithm>
#include <utility>

template <typename T, Size ChunkSize>
struct VaLinkedChunkedListNode {
    T data[ChunkSize];
    Size count = 0;

    VaLinkedChunkedListNode* next = nullptr;
    VaLinkedChunkedListNode* prev = nullptr;

    inline void pushBack(const T& val) noexcept {
        new (&data[count++]) T(val);
    }
    inline void pushBack(T&& val) noexcept {
        new (&data[count++]) T(std::move(val));
    }

    inline void pushFront(const T& val) {
        for (Size i = count; i > 0; --i) {
            new (&data[i]) T(std::move(data[i - 1]));
            data[i - 1].~T();
        }
        new (&data[0]) T(val);
        ++count;
    }
    inline void pushFront(T&& val) {
        for (Size i = count; i > 0; --i) {
            new (&data[i]) T(std::move(data[i - 1]));
            data[i - 1].~T();
        }
        new (&data[0]) T(std::move(val));
        ++count;
    }

    inline bool isFull() const {
        return count == ChunkSize;
    }
    inline bool isEmpty() const {
        return count == 0;
    }

    VaLinkedChunkedListNode() = default;
    ~VaLinkedChunkedListNode() {
        for (size_t i = 0; i < count; ++i) {
            data[i].~T();
        }
    }
};

template <typename T, Size ChunkSize = 16>
class VaLinkedChunkedList {
  public:
    using Chunk = VaLinkedChunkedListNode<T, ChunkSize>;

  protected:
    Chunk* head;
    Chunk* tail;
    Size len;

    inline Chunk* getChunk() {
        return new Chunk;
    }

    inline void returnChunk(Chunk* chunk) {
        delete chunk;
    }

    Chunk* chunkContainsIndex(Size index, Size* offset = nullptr) {
        if (!head) return nullptr;
        if (index < len / 2) {
            Chunk* node = head;
            while (index >= node->count) {
                index -= node->count;
                node = node->next;
            }
            if (offset) *offset = index;
            return node;
        } else {
            Size remaining = len - index;
            Chunk* node = tail;
            while (remaining > node->count) {
                remaining -= node->count;
                node = node->prev;
            }
            if (offset) *offset = node->count - remaining;
            return node;
        }
    }

    void shiftRightInChunk(Chunk* chunk, Size offset) {
        for (Size i = chunk->count; i > offset; --i) {
            new (&chunk->data[i]) T(std::move(chunk->data[i - 1]));
            chunk->data[i - 1].~T();
        }
    }

    Chunk* splitChunk(Chunk* chunk) {
        Chunk* newChunk = getChunk();
        new (&newChunk->data[0]) T(std::move(chunk->data[ChunkSize - 1]));
        chunk->data[ChunkSize - 1].~T();
        newChunk->count = 1;

        newChunk->next = chunk->next;
        newChunk->prev = chunk;
        if (chunk->next) {
            chunk->next->prev = newChunk;
        } else {
            tail = newChunk;
        }
        chunk->next = newChunk;
        --chunk->count;
        return newChunk;
    }

    void unlinkFromOrder(Chunk* chunk) {
        if (chunk->prev) {
            chunk->prev->next = chunk->next;
        } else {
            head = chunk->next;
        }

        if (chunk->next) {
            chunk->next->prev = chunk->prev;
        } else {
            tail = chunk->prev;
        }

        chunk->next = nullptr;
        chunk->prev = nullptr;
    }

    T removeFromChunk(Chunk* chunk, Size offset) {
        T removedValue = std::move(chunk->data[offset]);
        for (Size i = offset; i < chunk->count - 1; ++i) {
            chunk->data[i].~T();
            new (&chunk->data[i]) T(std::move(chunk->data[i + 1]));
        }
        chunk->data[chunk->count - 1].~T();
        --chunk->count;

        if (chunk->isEmpty()) {
            unlinkFromOrder(chunk);
            returnChunk(chunk);
        }

        return removedValue;
    }

    void insertValueInChunk(Chunk* chunk, Size offset, const T& value) {
        shiftRightInChunk(chunk, offset);
        new (&chunk->data[offset]) T(value);
        chunk->count++;
    }

    void insertValueInChunk(Chunk* chunk, Size offset, T&& value) {
        shiftRightInChunk(chunk, offset);
        new (&chunk->data[offset]) T(std::move(value));
        chunk->count++;
    }

  public:
    VaLinkedChunkedList() : head(nullptr), tail(nullptr), len(0) {}
    VaLinkedChunkedList(std::initializer_list<T> init) : VaLinkedChunkedList() {
        for (const auto& item: init) {
            append(item);
        }
    }
    VaLinkedChunkedList(const VaLinkedChunkedList& other) : VaLinkedChunkedList() {
        Chunk* current = other.head;
        while (current) {
            for (Size i = 0; i < current->count; ++i) {
                append(current->data[i]);
            }
            current = current->next;
        }
    }

    VaLinkedChunkedList(VaLinkedChunkedList&& other) noexcept : head(other.head), tail(other.tail), len(other.len) {
        other.head = nullptr;
        other.tail = nullptr;
        other.len = 0;
    }

    VaLinkedChunkedList& operator=(const VaLinkedChunkedList& other) {
        if (this != &other) {
            clear();
            Chunk* current = other.head;
            while (current) {
                for (Size i = 0; i < current->count; ++i) {
                    append(current->data[i]);
                }
                current = current->next;
            }
        }
        return *this;
    }

    VaLinkedChunkedList& operator=(VaLinkedChunkedList&& other) noexcept {
        if (this != &other) {
            clear();
            head = other.head;
            tail = other.tail;
            len = other.len;
            other.head = nullptr;
            other.tail = nullptr;
            other.len = 0;
        }
        return *this;
    }

    ~VaLinkedChunkedList() {
        clear();
    }

    void append(const T& value) {
        if (!tail || tail->isFull()) {
            Chunk* newChunk = getChunk();
            if (tail) {
                tail->next = newChunk;
                newChunk->prev = tail;
            } else {
                head = newChunk;
            }
            tail = newChunk;
        }
        tail->pushBack(value);
        ++len;
    }

    void append(T&& value) {
        if (!tail || tail->isFull()) {
            Chunk* newChunk = getChunk();
            if (tail) {
                tail->next = newChunk;
                newChunk->prev = tail;
            } else {
                head = newChunk;
            }
            tail = newChunk;
        }
        tail->pushBack(std::move(value));
        ++len;
    }

    void prepend(const T& value) {
        if (!head || head->isFull()) {
            Chunk* newChunk = getChunk();
            newChunk->next = head;
            if (head) {
                head->prev = newChunk;
            } else {
                tail = newChunk;
            }
            head = newChunk;
        }
        head->pushFront(value);
        ++len;
    }

    void prepend(T&& value) {
        if (!head || head->isFull()) {
            Chunk* newChunk = getChunk();
            newChunk->next = head;
            if (head) {
                head->prev = newChunk;
            } else {
                tail = newChunk;
            }
            head = newChunk;
        }
        head->pushFront(std::move(value));
        ++len;
    }

    void insert(Size index, const T& value) {
        if (index == len) return append(value);

        Size offset = 0;
        Chunk* node = chunkContainsIndex(index, &offset);

        if (!node) return prepend(value);

        if (node->isFull()) {
            splitChunk(node);
        }

        insertValueInChunk(node, offset, value);
        ++len;
    }

    void insert(Size index, T&& value) {
        if (index == len) {
            append(std::move(value));
            return;
        }

        Size offset = 0;
        Chunk* node = chunkContainsIndex(index, &offset);

        if (!node) {
            prepend(std::move(value));
            return;
        }

        if (node->isFull()) {
            splitChunk(node);
        }

        insertValueInChunk(node, offset, std::move(value));
        ++len;
    }

    template <typename... Args>
    void appendEmplace(Args&&... args) {
        if (!tail || tail->isFull()) {
            Chunk* newChunk = getChunk();
            if (tail) {
                tail->next = newChunk;
                newChunk->prev = tail;
            } else {
                head = newChunk;
            }
            tail = newChunk;
        }
        new (&tail->data[tail->count++]) T(std::forward<Args>(args)...);
        ++len;
    }

    template <typename... Args>
    void prependEmplace(Args&&... args) {
        if (!head || head->isFull()) {
            Chunk* newChunk = getChunk();
            newChunk->next = head;
            if (head) {
                head->prev = newChunk;
            } else {
                tail = newChunk;
            }
            head = newChunk;
        }
        for (Size i = head->count; i > 0; --i) {
            new (&head->data[i]) T(std::move(head->data[i - 1]));
            head->data[i - 1].~T();
        }
        new (&head->data[0]) T(std::forward<Args>(args)...);
        ++head->count;
        ++len;
    }

    template <typename... Args>
    void insertEmplace(Size index, Args&&... args) {
        if (index == len) {
            appendEmplace(std::forward<Args>(args)...);
            return;
        }

        Size offset = 0;
        Chunk* node = chunkContainsIndex(index, &offset);

        if (!node) {
            prependEmplace(std::forward<Args>(args)...);
            return;
        }

        if (node->isFull()) {
            splitChunk(node);
        }

        shiftRightInChunk(node, offset);
        new (&node->data[offset]) T(std::forward<Args>(args)...);
        ++node->count;
        ++len;
    }

    void appendEach(const VaLinkedChunkedList& other) {
        if (other.len == 0) return;

        Chunk* current = other.head;
        while (current) {
            for (Size i = 0; i < current->count; ++i) {
                append(current->data[i]);
            }
            current = current->next;
        }
    }

    void appendEach(VaLinkedChunkedList&& other) {
        if (other.len == 0) return;

        if (!tail) {
            head = other.head;
            tail = other.tail;
            len = other.len;
        } else {
            tail->next = other.head;
            if (other.head) {
                other.head->prev = tail;
            }
            tail = other.tail;
            len += other.len;
        }

        other.head = nullptr;
        other.tail = nullptr;
        other.len = 0;
    }

    template <typename Iterable>
    void appendEach(const Iterable& iterable) {
        for (const auto& item : iterable) {
            append(item);
        }
    }

    template <typename Iterable, typename = tt::EnableIf<!tt::IsSame<tt::RemoveCVRef<Iterable>, VaLinkedChunkedList<T>>> >
    void appendEach(Iterable&& iterable) {
        for (auto& item: iterable) {
            append(std::move(item));
        }
    }

    void prependEach(const VaLinkedChunkedList& other) {
        if (other.len == 0) return;

        Chunk* current = other.tail;
        while (current) {
            for (Size i = current->count; i > 0; --i) {
                prepend(current->data[i - 1]);
            }
            current = current->prev;
        }
    }

    void prependEach(VaLinkedChunkedList&& other) {
        if (other.len == 0) return;

        if (!head) {
            head = other.head;
            tail = other.tail;
            len = other.len;
        } else {
            other.tail->next = head;
            if (head) {
                head->prev = other.tail;
            }
            head = other.head;
            len += other.len;
        }

        other.head = nullptr;
        other.tail = nullptr;
        other.len = 0;
    }

    template <typename Iterable>
    void prependEach(const Iterable& iterable) {
        for (auto it = iterable.rbegin(); it != iterable.rend(); ++it) {
            prepend(*it);
        }
    }

    template <typename Iterable, typename = tt::EnableIf<!tt::IsSame<tt::RemoveCVRef<Iterable>, VaLinkedChunkedList<T>>>>
    void prependEach(Iterable&& iterable) {
        for (auto it = iterable.rbegin(); it != iterable.rend(); ++it) {
            prepend(std::move(*it));
        }
    }

    void insertEach(Size index, const VaLinkedChunkedList& other) {
        if (other.len == 0) return;

        if (index == 0) {
            prependEach(other);
            return;
        }

        if (index >= len) {
            appendEach(other);
            return;
        }

        Size offset = 0;
        chunkContainsIndex(index, &offset);

        Chunk* current = other.head;
        while (current) {
            for (Size i = 0; i < current->count; ++i) {
                insert(index++, current->data[i]);
            }
            current = current->next;
        }
    }

    void insertEach(Size index, VaLinkedChunkedList&& other) {
        if (other.len == 0) return;

        if (index == 0) {
            prependEach(std::move(other));
            return;
        }

        if (index >= len) {
            appendEach(std::move(other));
            return;
        }

        Size offset = 0;
        Chunk* node = chunkContainsIndex(index, &offset);

        while (other.head) {
            Chunk* next = other.head->next;

            if (!node) {
                prependEach(std::move(other));
                return;
            }

            if (node->isFull()) {
                node = splitChunk(node);
            }

            for (Size i = 0; i < other.head->count; ++i) {
                insertValueInChunk(node, offset++, std::move(other.head->data[i]));
                ++len;
            }

            returnChunk(other.head);
            other.head = next;
        }

        other.tail = nullptr;
        other.len = 0;
    }

    template <typename Iterable>
    void insertEach(Size index, const Iterable& iterable) {
        if (index == 0) {
            prependEach(iterable);
            return;
        }

        if (index >= len) {
            appendEach(iterable);
            return;
        }

        for (const auto& item: iterable) {
            insert(index++, item);
        }
    }

    template <typename Iterable, typename = tt::EnableIf<!tt::IsSame<tt::RemoveCVRef<Iterable>, VaLinkedChunkedList<T>>>>
    void insertEach(Size index, Iterable&& iterable) {
        if (index == 0) {
            prependEach(std::move(iterable));
            return;
        }

        if (index >= len) {
            appendEach(std::move(iterable));
            return;
        }

        for (auto& item: iterable) {
            insert(index++, std::move(item));
        }
    }

    T& get(Size index) {
        Size offset = 0;
        Chunk* node = chunkContainsIndex(index, &offset);
        return node->data[offset];
    }

    const T& get(Size index) const {
        Size offset = 0;
        Chunk* node = chunkContainsIndex(index, &offset);
        return node->data[offset];
    }

    T& operator[](Size index) {
        Size offset = 0;
        Chunk* node = chunkContainsIndex(index, &offset);
        return node->data[offset];
    }

    const T& operator[](Size index) const {
        Size offset = 0;
        Chunk* node = chunkContainsIndex(index, &offset);
        return node->data[offset];
    }

    T& at(int32 index) {
        if (index < 0) index += static_cast<int32>(len);
        if (index < 0 || static_cast<Size>(index) >= len) {
            throw IndexOutOfRangeError(len, index);
        }

        Size offset = 0;
        Chunk* node = chunkContainsIndex(static_cast<Size>(index), &offset);
        return node->data[offset];
    }

    const T& at(int32 index) const {
        if (index < 0) index += static_cast<int32>(len);
        if (index < 0 || static_cast<Size>(index) >= len) {
            throw IndexOutOfRangeError(len, index);
        }

        Size offset = 0;
        Chunk* node = chunkContainsIndex(static_cast<Size>(index), &offset);
        return node->data[offset];
    }

    void clear() noexcept {
        while (head) {
            Chunk* next = head->next;
            returnChunk(head);
            head = next;
        }
        tail = nullptr;
        len = 0;
    }

    T& front() {
        if (!head || head->isEmpty()) {
            throw ValueError("front() on empty list");
        }
        return head->data[0];
    }

    const T& front() const {
        if (!head || head->isEmpty()) {
            throw ValueError("front() on empty list");
        }
        return head->data[0];
    }

    T& back() {
        if (!tail || tail->isEmpty()) {
            throw ValueError("back() on empty list");
        }
        return tail->data[tail->count - 1];
    }

    const T& back() const {
        if (!tail || tail->isEmpty()) {
            throw ValueError("back() on empty list");
        }
        return tail->data[tail->count - 1];
    }

    T& frontUnchecked() noexcept {
        return head->data[0];
    }

    const T& frontUnchecked() const noexcept {
        return head->data[0];
    }

    T& backUnchecked() noexcept {
        return tail->data[tail->count - 1];
    }

    const T& backUnchecked() const noexcept {
        return tail->data[tail->count - 1];
    }

    void del(Size index) {
        if (index >= len) {
            throw IndexOutOfRangeError(len, index);
        }

        Size offset = 0;
        Chunk* node = chunkContainsIndex(index, &offset);

        if (!node) {
            throw ValueError("Invalid index for deletion");
        }

        removeFromChunk(node, offset);
        --len;
    }

    T pop() {
        if (!tail || tail->isEmpty()) {
            throw ValueError("pop() on empty list");
        }

        T value = removeFromChunk(tail, tail->count - 1);
        --len;
        return value;
    }

    T shift() {
        if (!head || head->isEmpty()) {
            throw ValueError("shift() on empty list");
        }

        T value = removeFromChunk(head, 0);
        --len;
        return value;
    }

    /**
     * @brief Returns the number of elements currently stored in the list.
     * @return The current length of the list.
     */
    inline Size getLength() const noexcept {
        return this->len;
    }

  public operators:
    // ---- SAME CHUNK SIZE COMPARE ---- //
    bool areChunksEqual(const VaLinkedChunkedList& other) const {
        if (this->len != other.len) return false;

        Chunk* leftChunk = this->head;
        Chunk* rightChunk = other.head;

        while (leftChunk && rightChunk) {
            if (leftChunk->count != rightChunk->count) return false;

            for (Size i = 0; i < leftChunk->count; ++i) {
                if (!(leftChunk->data[i] == rightChunk->data[i])) return false;
            }

            leftChunk = leftChunk->next;
            rightChunk = rightChunk->next;
        }

        return leftChunk == nullptr && rightChunk == nullptr;
    }

    bool areChunksOrderedSame(const VaLinkedChunkedList& other) const {
        if (this->len != other.len) return false;

        Chunk* leftChunk = this->head;
        Chunk* rightChunk = other.head;

        while (leftChunk && rightChunk) {
            Size minCount = std::min(leftChunk->count, rightChunk->count);

            for (Size i = 0; i < minCount; ++i) {
                if (leftChunk->data[i] < rightChunk->data[i]) return true;
                if (rightChunk->data[i] < leftChunk->data[i]) return false;
            }

            leftChunk = leftChunk->next;
            rightChunk = rightChunk->next;
        }

        return this->len < other.len;
    }

    // ---- DIFFERENT CHUNK SIZE COMPARE ---- //
    template <Size OtherChunkSize>
    friend bool operator==(const VaLinkedChunkedList& lhs, const VaLinkedChunkedList<T, OtherChunkSize>& rhs) {
        if (lhs.len != rhs.len) return false;

        auto leftIt = lhs.begin();
        auto rightIt = rhs.begin();

        while (leftIt != lhs.end() && rightIt != rhs.end()) {
            if (!(*leftIt == *rightIt)) return false;
            ++leftIt;
            ++rightIt;
        }

        return leftIt == lhs.end() && rightIt == rhs.end();
    }

    template <Size OtherChunkSize>
    friend bool operator!=(const VaLinkedChunkedList& lhs, const VaLinkedChunkedList<T, OtherChunkSize>& rhs) {
        return !(lhs == rhs);
    }

    template <Size OtherChunkSize>
    friend bool operator<(const VaLinkedChunkedList& lhs, const VaLinkedChunkedList<T, OtherChunkSize>& rhs) {
        auto leftIt = lhs.begin();
        auto rightIt = rhs.begin();

        while (leftIt != lhs.end() && rightIt != rhs.end()) {
            if (*leftIt < *rightIt) return true;
            if (*rightIt < *leftIt) return false;
            ++leftIt;
            ++rightIt;
        }

        return lhs.len < rhs.len;
    }

    template <Size OtherChunkSize>
    friend bool operator<=(const VaLinkedChunkedList& lhs, const VaLinkedChunkedList<T, OtherChunkSize>& rhs) {
        return !(rhs < lhs);
    }

    template <Size OtherChunkSize>
    friend bool operator>(const VaLinkedChunkedList& lhs, const VaLinkedChunkedList<T, OtherChunkSize>& rhs) {
        return rhs < lhs;
    }

    template <Size OtherChunkSize>
    friend bool operator>=(const VaLinkedChunkedList& lhs, const VaLinkedChunkedList<T, OtherChunkSize>& rhs) {
        return !(lhs < rhs);
    }

    template <typename Iterable, typename = tt::EnableIf<!tt::IsSame<tt::RemoveCVRef<Iterable>, VaLinkedChunkedList<T>>>>
    friend bool operator==(const VaLinkedChunkedList& lhs, const Iterable& rhs) {
        auto leftIt = lhs.begin();
        auto rightIt = std::begin(rhs);

        while (leftIt != lhs.end() && rightIt != std::end(rhs)) {
            if (!(*leftIt == *rightIt)) return false;
            ++leftIt;
            ++rightIt;
        }

        return leftIt == lhs.end() && rightIt == std::end(rhs);
    }

    template <typename Iterable, typename = tt::EnableIf<!tt::IsSame<tt::RemoveCVRef<Iterable>, VaLinkedChunkedList>>>
    friend bool operator!=(const VaLinkedChunkedList& lhs, const Iterable& rhs) {
        return !(lhs == rhs);
    }

    template <typename Iterable, typename = tt::EnableIf<!tt::IsSame<tt::RemoveCVRef<Iterable>, VaLinkedChunkedList>>>
    friend bool operator<(const VaLinkedChunkedList& lhs, const Iterable& rhs) {
        auto leftIt = lhs.begin();
        auto rightIt = std::begin(rhs);

        while (leftIt != lhs.end() && rightIt != std::end(rhs)) {
            if (*leftIt < *rightIt) return true;
            if (*rightIt < *leftIt) return false;
            ++leftIt;
            ++rightIt;
        }

        return lhs.len < static_cast<Size>(std::distance(std::begin(rhs), std::end(rhs)));
    }

    template <typename Iterable, typename = tt::EnableIf<!tt::IsSame<tt::RemoveCVRef<Iterable>, VaLinkedChunkedList<T>>>>
    friend bool operator<=(const VaLinkedChunkedList& lhs, const Iterable& rhs) {
        return !(rhs < lhs);
    }

    template <typename Iterable, typename = tt::EnableIf<!tt::IsSame<tt::RemoveCVRef<Iterable>, VaLinkedChunkedList<T>>>>
    friend bool operator>(const VaLinkedChunkedList& lhs, const Iterable& rhs) {
        return rhs < lhs;
    }

    template <typename Iterable, typename = tt::EnableIf<!tt::IsSame<tt::RemoveCVRef<Iterable>, VaLinkedChunkedList<T>>>>
    friend bool operator>=(const VaLinkedChunkedList& lhs, const Iterable& rhs) {
        return !(lhs < rhs);
    }

  public friends:
    friend inline Size len(const VaLinkedChunkedList& list) { return list.len; }

  public iterators:
    class Iterator {
      public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type        = T;
        using difference_type   = std::ptrdiff_t;
        using pointer           = T*;
        using reference         = T&;

      protected:
        Chunk* currentChunk;
        Size index;

      public:
        Iterator(Chunk* chunk, Size index) : currentChunk(chunk), index(index) {}

        T& operator*() {
            return currentChunk->data[index];
        }

        T* operator->() {
            return &currentChunk->data[index];
        }

        Iterator& operator++() {
            if (++index >= currentChunk->count) {
                currentChunk = currentChunk->next;
                index = 0;
            }
            return *this;
        }

        Iterator operator++(int) {
            Iterator temp = *this;
            ++(*this);
            return temp;
        }

        Iterator& operator--() {
            if (index == 0) {
                currentChunk = currentChunk->prev;
                index = currentChunk ? currentChunk->count - 1 : 0;
            } else {
                --index;
            }
            return *this;
        }

        Iterator operator--(int) {
            Iterator temp = *this;
            --(*this);
            return temp;
        }

        bool operator==(const Iterator& other) const {
            return currentChunk == other.currentChunk && index == other.index;
        }

        bool operator!=(const Iterator& other) const {
            return !(*this == other);
        }
    };

    class ConstIterator {
      public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type        = T;
        using difference_type   = std::ptrdiff_t;
        using pointer           = const T*;
        using reference         = const T&;

      protected:
        const Chunk* currentChunk;
        Size index;

      public:
        ConstIterator(const Chunk* chunk, Size index) : currentChunk(chunk), index(index) {}

        const T& operator*() const {
            return currentChunk->data[index];
        }

        const T* operator->() const {
            return &currentChunk->data[index];
        }

        ConstIterator& operator++() {
            if (++index >= currentChunk->count) {
                currentChunk = currentChunk->next;
                index = 0;
            }
            return *this;
        }

        ConstIterator operator++(int) {
            ConstIterator temp = *this;
            ++(*this);
            return temp;
        }

        ConstIterator& operator--() {
            if (index == 0) {
                currentChunk = currentChunk->prev;
                index = currentChunk ? currentChunk->count - 1 : 0;
            } else {
                --index;
            }
            return *this;
        }

        ConstIterator operator--(int) {
            ConstIterator temp = *this;
            --(*this);
            return temp;
        }

        bool operator==(const ConstIterator& other) const {
            return currentChunk == other.currentChunk && index == other.index;
        }

        bool operator!=(const ConstIterator& other) const {
            return !(*this == other);
        }
    };

    using ReverseIterator = std::reverse_iterator<Iterator>;
    using ConstReverseIterator = std::reverse_iterator<ConstIterator>;

    inline Iterator begin() { return Iterator(head, 0); }
    inline Iterator end() { return Iterator(nullptr, 0); }

    inline ConstIterator begin() const { return ConstIterator(head, 0); }
    inline ConstIterator end() const { return ConstIterator(nullptr, 0); }

    inline ConstIterator cbegin() const { return ConstIterator(head, 0); }
    inline ConstIterator cend() const { return ConstIterator(nullptr, 0); }

    inline ReverseIterator rbegin() { return ReverseIterator(end()); }
    inline ReverseIterator rend() { return ReverseIterator(begin()); }

    inline ConstReverseIterator rbegin() const { return ConstReverseIterator(end()); }
    inline ConstReverseIterator rend() const { return ConstReverseIterator(begin()); }

    inline ConstReverseIterator crbegin() const { return ConstReverseIterator(cend()); }
    inline ConstReverseIterator crend() const { return ConstReverseIterator(cbegin()); }
};

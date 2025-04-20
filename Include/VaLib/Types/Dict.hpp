// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <VaLib/Types/BasicTypedef.hpp>
#include <VaLib/Utils/BasicDefine.hpp>

#include <VaLib/Types/Error.hpp>
#include <VaLib/Types/Pair.hpp>

template <typename K>
struct DictHash {
    Size operator()(const K& key) const { return std::hash<K>()(key); }
};

template <typename K, typename V>
struct DictEntry {
    K key;
    V value;

    DictEntry<K, V>* next;      ///< Next in hash bucket
    DictEntry<K, V>* prevOrder; ///< Previous in insertion order
    DictEntry<K, V>* nextOrder; ///< Next in insertion order

    DictEntry(const K& k, const V& v) : next(nullptr), prevOrder(nullptr), nextOrder(nullptr) {
        key = k;
        value = v;
    }
};

template <typename K, typename V, typename Hash = DictHash<K>>
class VaDict {
  protected:
    using Entry = DictEntry<K, V>;

    Size cap;  ///< Capacity of the hash table
    Size size; ///< Current number of entries stored in the hash table.

    Entry**
        buckets; ///< Array of pointers to hash buckets (each bucket is a linked list of entries).

    Entry* head;
    Entry* tail;

    Hash hashFunc; ///< Hash function used to compute bucket indices from keys.

    Size computeIndex(const K& key) const { return hashFunc(key) % cap; }

    void resize(Size newCap) {
        Entry** newBuckets = new Entry*[newCap]();
        for (Size i = 0; i < newCap; ++i) {
            newBuckets[i] = nullptr;
        }

        Entry* current = head;
        while (current != nullptr) {
            Size index = hashFunc(current->key) % newCap;
            Entry* entry = current;
            entry->next = newBuckets[index];
            newBuckets[index] = entry;
            current = current->nextOrder;
        }

        delete[] buckets;
        buckets = newBuckets;
        cap = newCap;
    }

    void insertBefore(Entry* next, Entry* newEntry) {
        Entry* prev = next ? next->prevOrder : tail;

        newEntry->nextOrder = next;
        newEntry->prevOrder = prev;

        if (prev) {
            prev->nextOrder = newEntry;
        } else {
            head = newEntry;
        }

        if (next) {
            next->prevOrder = newEntry;
        } else {
            tail = newEntry;
        }
    }

    void appendEntry(Entry* newEntry) {
        newEntry->prevOrder = tail;
        newEntry->nextOrder = nullptr;

        if (tail) {
            tail->nextOrder = newEntry;
        } else {
            head = newEntry;
        }
        tail = newEntry;
    }

    void unlinkEntry(Entry* entry) {
        if (entry->prevOrder) {
            entry->prevOrder->nextOrder = entry->nextOrder;
        } else {
            head = entry->nextOrder;
        }

        if (entry->nextOrder) {
            entry->nextOrder->prevOrder = entry->prevOrder;
        } else {
            tail = entry->prevOrder;
        }

        entry->nextOrder = entry->prevOrder = nullptr;
    }

    void unlinkFromOrder(Entry* entry) {
        if (entry->prevOrder) {
            entry->prevOrder->nextOrder = entry->nextOrder;
        } else {
            head = entry->nextOrder;
        }

        if (entry->nextOrder) {
            entry->nextOrder->prevOrder = entry->prevOrder;
        } else {
            tail = entry->prevOrder;
        }
    }

  public:
    /**
     * @brief Struct representing a mutable key-value pair reference.
     * @note Used for accessing entries in pairAtIndex().
     */
    struct PairRef {
        const K& key;
        V& value;
    };

    /**
     * @brief Struct representing a const key-value pair reference.
     * @note Used for accessing entries in pairAtIndex() for const VaDict.
     */
    struct ConstPairRef {
        const K& key;
        const V& value;
    };

  public:
    /** 
     * @brief Constructs an empty dictionary with a given initial capacity.
     * @param initialCap Initial number of hash buckets (default is 16).
     */
    VaDict(Size initialCap = 32) : head(nullptr), tail(nullptr), size(0) {
        cap = initialCap;

        buckets = new Entry*[cap]();
        for (Size i = 0; i < cap; ++i) {
            buckets[i] = nullptr;
        }
    }

    /** 
     * @brief Copy constructor. Creates a deep copy of another dictionary.
     * @param other The dictionary to copy from.
     */
    VaDict(const VaDict& other)
        : cap(other.cap), size(0), head(nullptr), tail(nullptr), hashFunc(other.hashFunc) {
        buckets = new Entry*[cap]();
        for (Size i = 0; i < cap; ++i) {
            buckets[i] = nullptr;
        }

        Entry* current = other.head;
        while (current != nullptr) {
            put(current->key, current->value);
            current = current->nextOrder;
        }
    }

    /** 
     * @brief Move constructor. Transfers ownership of internal data from another dictionary.
     * @param other The dictionary to move from.
     */
    VaDict(VaDict&& other) noexcept
        : cap(other.cap), size(other.size), buckets(other.buckets), head(other.head),
          tail(other.tail), hashFunc(std::move(other.hashFunc)) {
        other.buckets = nullptr;
        other.head = other.tail = nullptr;
        other.size = 0;
        other.cap = 0;
    }

    /** 
     * @brief Constructs a dictionary from an initializer list of key-value pairs.
     * @param init Initializer list of key-value pairs.
     */
    VaDict(std::initializer_list<VaPair<const K&, const V&>> init) : VaDict() {
        for (const auto& [k, v] : init) {
            put(k, v);
        }
    }

    /** 
     * @brief Destructor. Frees all allocated memory and clears the dictionary.
     */
    ~VaDict() {
        Entry* current = head;

        while (current != nullptr) {
            Entry* next = current->nextOrder;
            delete current;
            current = next;
        }

        delete[] buckets;
    }

    /** 
     * @brief Copy assignment operator. Replaces the contents with a copy of another dictionary.
     * @param other The dictionary to copy from.
     * @return Reference to this dictionary.
     */
    VaDict& operator=(const VaDict& other) {
        if (this == &other) return *this;

        this->clear();
        delete[] buckets;

        cap = other.cap;
        size = 0;
        head = tail = nullptr;
        hashFunc = other.hashFunc;

        buckets = new Entry*[cap]();
        for (Size i = 0; i < cap; ++i) {
            buckets[i] = nullptr;
        }

        Entry* current = other.head;
        while (current != nullptr) {
            put(current->key, current->value);
            current = current->nextOrder;
        }

        return *this;
    }

    /** 
     * @brief Move assignment operator. Transfers the contents of another dictionary.
     * @param other The dictionary to move from.
     * @return Reference to this dictionary.
     */
    VaDict& operator=(VaDict&& other) noexcept {
        if (this == &other) return *this;

        clear();
        delete[] buckets;

        this->cap = other.cap;
        this->size = other.size;
        this->buckets = other.buckets;
        this->head = other.head;
        this->tail = other.tail;
        this->hashFunc = std::move(other.hashFunc);

        other.buckets = nullptr;
        other.head = other.tail = nullptr;
        other.size = 0;
        other.cap = 0;

        return *this;
    }

    /**
    * @brief Lexicographically compares two dictionaries.
    *
    * Elements are compared in insertion order. If keys differ, the key comparison determines the result.
    *
    * @note Requires both @ref K and @ref V to be comparable via <.
    * @warning Behavior is undefined if the types do not implement @a operator<.
    *
    * @param other The other dictionary to compare with.
    * @return @a true if this dictionary is lexicographically less than the other.
    */
    bool operator<(const VaDict<K, V>& other) const {
        Entry* a = head;
        Entry* b = other.head;

        while (a != nullptr && b != nullptr) {
            if (a->key < b->key) return true;
            if (b->key < a->key) return false;

            if (a->value < b->value) return true;
            if (b->value < a->value) return false;

            a = a->nextOrder;
            b = b->nextOrder;
        }

        return b != nullptr;
    }

    /**
     * @brief Lexicographically compares two dictionaries in the opposite direction.
     * @param other The other dictionary to compare with.
     * @return `true` if this dictionary is lexicographically greater than the other.
     * 
     * @note Equivalent to `!(this < other) && !(this == other)`.
     * @warning Requires both `K` and `V` to implement `operator<` or `operator>`.
     */
    bool operator>(const VaDict<K, V>& other) const { return other < *this; }

    /**
     * @brief Checks if this dictionary is less than or equal to the other.
     * @param other The other dictionary to compare with.
     * @return `true` if this dictionary is less than or equal to the other.
     */
    bool operator<=(const VaDict<K, V>& other) const { return !(other < *this); }

    /**
     * @brief Checks if this dictionary is greater than or equal to the other.
     * @param other The other dictionary to compare with.
     * @return `true` if this dictionary is greater than or equal to the other.
     */
    bool operator>=(const VaDict<K, V>& other) const { return !(*this < other); }

    /** 
     * @brief Checks if this dictionary is equal to another, ignoring insertion order.
     * @param other The dictionary to compare with.
     * @return true if all key-value pairs are equal, false otherwise.
     */
    bool operator==(const VaDict<K, V>& other) const {
        if (size != other.size) return false;
        if (&other == this) return true;

        for (const Entry* e = head; e != nullptr; e = e->nextOrder) {
            V otherValue;
            if (!other.get(e->key, otherValue)) {
                return false;
            }

#if __cplusplus >= CPP20
            if constexpr (requires { e->value == otherValue; }) {
                if (!(e->value == otherValue)) return false;
            } else {
                if (std::memcmp(&e->value, &otherValue, sizeof(V)) != 0) return false;
            }
#else
            if (!(e->value == otherValue)) return false;
#endif
        }

        return true;
    }

    /** 
     * @brief Checks if this dictionary is not equal to another.
     * @param other The dictionary to compare with.
     * @return true if the dictionaries differ, false otherwise.
     */
    bool operator!=(const VaDict<K, V>& other) const { return !(*this == other); }

    /**
     * @brief Compares two dictionaries for equality, preserving insertion order.
     * @param other Dictionary to compare with.
     * @return true if all key-value pairs and their insertion order match.
     * 
     * @note Slower than unordered comparison; useful when order matters.
     */
    bool equalsOrdered(const VaDict<K, V>& other) const {
        if (size != other.size) return false;
        if (&other == this) return true;

        const Entry* a = head;
        const Entry* b = other.head;

        while (a && b) {
            if (!(a->key == b->key)) return false;

#if __cplusplus >= CPP20
            if constexpr (requires { a->value == b->value; }) {
                if (!(a->value == b->value)) return false;
            } else {
                if (std::memcmp(&a->value, &b->value, sizeof(V)) != 0) return false;
            }
#else
            if (!(a->value == b->value)) return false;
#endif

            a = a->nextOrder;
            b = b->nextOrder;
        }

        return true;
    }

    /**
     * @brief Ensures capacity is at least the specified amount.
     * @param minCap The minimum number of buckets to reserve.
     * 
     * @note Triggers rehashing if current capacity is smaller.
     */
    void reserve(Size minCap) {
        if (minCap > cap) resize(minCap);
    }

    /**
     * @brief Inserts a key-value pair at a specific position in insertion order.
     * @param index Position to insert at (0-based).
     * @param key The key to insert.
     * @param value The value to associate with the key.
     * 
     * @throws IndexOutOfRangeError if index > size.
     * @note If the key already exists, it is first removed.
     * @note If index == size, the pair is appended to the end.
     * @warning This is an O(n) operation due to the need to preserve insertion order.
     */
    void insert(Size index, const K& key, const V& value) {
        if (index > size) throw IndexOutOfRangeError(size, index);

        if (contains(key)) {
            remove(key);
            if (index > size) index = size;
        }

        if (size >= cap * 0.75) resize(cap * 2);

        Size bucketIndex = computeIndex(key);
        Entry* newEntry = new Entry(key, value);

        newEntry->next = buckets[bucketIndex];
        buckets[bucketIndex] = newEntry;

        if (index == size) {
            appendEntry(newEntry);
        } else {
            Entry* target = head;
            for (Size i = 0; i < index; ++i) {
                target = target->nextOrder;
            }
            insertBefore(target, newEntry);
        }

        size++;
    }

    void put(const K& key, const V& value) {
        if (size >= cap * 0.75) resize(cap * 2);

        Size index = computeIndex(key);
        Entry* entry = buckets[index];

        while (entry) {
            if (entry->key == key) {
                entry->value = value;
                return;
            }

            entry = entry->next;
        }

        Entry* newEntry = new Entry(key, value);
        newEntry->next = buckets[index];
        buckets[index] = newEntry;

        if (tail == nullptr) {
            head = tail = newEntry;
        } else {
            tail->nextOrder = newEntry;
            newEntry->prevOrder = tail;
            tail = newEntry;
        }

        size++;
    }

    /**
     * @brief Retrieves the value for a given key, if it exists.
     * @param key The key to search for.
     * @param value Output parameter for the value, if found.
     * @return true if key exists, false otherwise.
     * 
     * @note Does not throw on missing key.
     */
    bool get(const K& key, V& value) const {
        Size index = computeIndex(key);
        Entry* entry = buckets[index];

        while (entry != nullptr) {
            if (entry->key == key) {
                value = entry->value;
                return true;
            }
            entry = entry->next;
        }
        return false;
    }

    /**
     * @brief Removes all key-value pairs from the dictionary.
     * 
     * @note Leaves the capacity unchanged.
     * @note After clearing, size is 0 and iteration is reset.
     */
    void clear() {
        Entry* current = head;
        while (current != nullptr) {
            Entry* next = current->nextOrder;
            delete current;
            current = next;
        }

        for (Size i = 0; i < cap; i++) {
            buckets[i] = nullptr;
        }

        head = tail = nullptr;
        size = 0;
    }

    /**
     * @brief Removes a key-value pair from the dictionary.
     * @param key The key to remove.
     * 
     * @note Does nothing if key is not found.
     * @note Preserves order of remaining elements.
     */
    void remove(const K& key) {
        Size index = computeIndex(key);

        Entry* entry = buckets[index];
        Entry* prev = nullptr;

        while (entry != nullptr) {
            if (entry->key == key) {
                if (prev) {
                    prev->next = entry->next;
                } else {
                    buckets[index] = entry->next;
                }

                unlinkFromOrder(entry);

                delete entry;
                size--;

                return;
            }

            prev = entry;
            entry = entry->next;
        }
    }

    /**
     * @brief Checks if the dictionary contains the given key.
     * @param key The key to check.
     * @return true if key is present, false otherwise.
     */
    bool contains(const K& key) const {
        Size index = computeIndex(key);
        Entry* entry = buckets[index];
        while (entry != nullptr) {
            if (entry->key == key) return true;
            entry = entry->next;
        }
        return false;
    }

    /**
     * @brief Returns a reference to the value for the given key, inserting a default if missing.
     * @param key The key to retrieve or insert.
     * @return Reference to the existing or newly inserted value.
     * 
     * @note Inserts default-constructed value if key is not present.
     * @note May cause rehash if load factor exceeds threshold.
     */
    V& operator[](const K& key) {
        Size index = computeIndex(key);
        Entry* entry = buckets[index];

        while (entry != nullptr) {
            if (entry->key == key) {
                return entry->value;
            }
            entry = entry->next;
        }

        // Create and link new entry directly
        Entry* newEntry = new Entry(key, V());
        newEntry->next = buckets[index];
        buckets[index] = newEntry;

        if (tail == nullptr) {
            head = tail = newEntry;
        } else {
            tail->nextOrder = newEntry;
            newEntry->prevOrder = tail;
            tail = newEntry;
        }

        size++;
        return newEntry->value;
    }

    // --- at
    /**
     * @brief Returns a reference to the value for the given key.
     * @param key The key to search for.
     * @return Reference to the value associated with the key.
     * 
     * @throws KeyNotFoundError if the key does not exist.
     */
    V& at(const K& key) {
        Size index = computeIndex(key);
        Entry* entry = buckets[index];

        while (entry != nullptr) {
            if (entry->key == key) {
                return entry->value;
            }
            entry = entry->next;
        }

        throw KeyNotFoundError();
    }

    /**
     * @brief Returns a const reference to the value for the given key.
     * @param key The key to search for.
     * @return Const reference to the value associated with the key.
     * 
     * @throws KeyNotFoundError if the key is not present.
     * @note Read-only variant of the non-const overload.
     */
    const V& at(const K& key) const {
        Size index = computeIndex(key);
        Entry* entry = buckets[index];

        while (entry != nullptr) {
            if (entry->key == key) {
                return entry->value;
            }
            entry = entry->next;
        }

        throw KeyNotFoundError();
    }

    // --- atIndex
    /**
     * @brief Returns a reference to the value at a given insertion index.
     * @param index Index in insertion order (0-based).
     * @return Reference to the value at the specified index.
     * 
     * @throws IndexOutOfRangeError if index >= size.
     * @note This is a linear-time operation.
     */
    V& atIndex(Size index) {
        if (index >= size) throw IndexOutOfRangeError(size, index);

        Entry* current = head;
        for (Size i = 0; i < index; ++i) {
            current = current->nextOrder;
        }
        return current->value;
    }

    /**
     * @brief Returns a const reference to the value at a given insertion index.
     * @param index Index in insertion order (0-based).
     * @return Const reference to the value at the specified index.
     * 
     * @throws IndexOutOfRangeError if index >= size
     */
    const V& atIndex(Size index) const {
        if (index >= size) throw IndexOutOfRangeError(size, index);

        Entry* current = head;
        for (Size i = 0; i < index; ++i) {
            current = current->nextOrder;
        }
        return current->value;
    }

    /**
     * @brief Returns a mutable key-value pair reference at a given insertion index.
     * @param index Index of the entry to access.
     * @return A PairRef containing references to the key and value.
     * 
     * @throws IndexOutOfRangeError if index is out of bounds.
     */
    PairRef pairAtIndex(Size index) {
        if (index >= size) throw IndexOutOfRangeError(size, index);

        Entry* current = head;
        for (Size i = 0; i < index; ++i) {
            current = current->nextOrder;
        }
        return PairRef{current->key, current->value};
    }

    /**
     * @brief Returns a const key-value pair reference at a given insertion index.
     * @param index Index of the entry to access.
     * @return A ConstPairRef with const references to key and value.
     * 
     * @throws IndexOutOfRangeError if index is invalid.
     * @note Use when no modification is needed.
     */
    ConstPairRef pairAtIndex(Size index) const {
        if (index >= size) throw IndexOutOfRangeError(size, index);

        Entry* current = head;
        for (Size i = 0; i < index; ++i) {
            current = current->nextOrder;
        }
        return ConstPairRef{current->key, current->value};
    }

    // --- iterator
    class Iterator {
      protected:
        Entry* ptr;

      public:
        Iterator(Entry* e) : ptr(e) {}

        bool operator!=(const Iterator& other) const { return ptr != other.ptr; }
        void operator++() {
            if (ptr) ptr = ptr->nextOrder;
        }

        VaPair<const K&, V&> operator*() { return VaPair<const K&, V&>{ptr->key, ptr->value}; }

        VaPair<const K&, const V&> operator*() const {
            return VaPair<const K&, const V&>{ptr->key, ptr->value};
        }
    };

    // --- const iterator
    class ConstIterator {
      protected:
        const Entry* ptr;

      public:
        ConstIterator(const Entry* e) : ptr(e) {}

        bool operator!=(const ConstIterator& other) const { return ptr != other.ptr; }
        void operator++() {
            if (ptr) ptr = ptr->nextOrder;
        }

        VaPair<const K&, const V&> operator*() const {
            return VaPair<const K&, const V&>{ptr->key, ptr->value};
        }
    };

    Iterator begin() { return Iterator(head); }
    Iterator end() { return Iterator(nullptr); }

    ConstIterator begin() const { return ConstIterator(head); }
    ConstIterator end() const { return ConstIterator(nullptr); }

    Entry* next(const Entry* current) const { return current ? current->nextOrder : nullptr; }

    friend inline Size size(const VaDict& dict) { return dict.size; }
    friend inline Size cap(const VaDict& dict) { return dict.cap; }
};
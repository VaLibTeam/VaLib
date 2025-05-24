// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <VaLib/RawAccess/Dict.hpp>

#include <VaLib/Types/BasicTypedef.hpp>
#include <VaLib/Meta/BasicDefine.hpp>

#include <VaLib/Utils/Hash.hpp>
#include <VaLib/Types/Error.hpp>
#include <VaLib/Types/Pair.hpp>
#include <VaLib/Types/List.hpp>

#include <VaLib/FuncTools/Func.hpp>

template <typename K, typename V>
struct VaDictEntry {
    K key;
    V value;

    VaDictEntry<K, V>* next;      ///< Next in hash bucket
    VaDictEntry<K, V>* prevOrder; ///< Previous in insertion order
    VaDictEntry<K, V>* nextOrder; ///< Next in insertion order

    VaDictEntry(const K& k, const V& v) : next(nullptr), prevOrder(nullptr), nextOrder(nullptr) {
        key = k;
        value = v;
    }
};

/**
 * @class VaDict A hash-based dictionary (map) that maintains insertion order.
 * This class implements a dictionary (key-value store) using a hash table with separate chaining.
 *
 * @tparam K The key type (must support equality comparison and hashing).
 * @tparam V The value type.
 * @tparam Hash The hash function type (defaults to `VaHash<K>`).
 *
 * @note it preserves the order in which elements were inserted.
 */
template <typename K, typename V, typename Hash = VaHash<K>>
class alignas(VaDictRawView<K, V, Hash>) VaDict {
  protected:
    using Entry = VaDictEntry<K, V>;

    Size cap;  ///< Capacity of the hash table
    Size size; ///< Current number of entries stored in the hash table.

    Entry** buckets; ///< Array of pointers to hash buckets (each bucket is a linked list of entries).
    Entry* head;     ///< Pointer to first node
    Entry* tail;     ///< Pointer to last node

    Hash hashFunc; ///< Hash function used to compute bucket indices from keys.

    /**
     * @brief Computes the index for a given key in the hash table.
     * @param key The key to compute the index for.
     * @return The index in the hash table.
     */
    Size computeIndex(const K& key) const { return hashFunc(key) % cap; }

    /**
     * @brief Resizes the hash table to a new capacity.
     * @param newCap The new capacity for the hash table.
     *
     * @note Rehashes all existing entries to fit into the new bucket array.
     */
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

    /**
     * @brief Inserts a new entry before a specified entry in insertion order.
     * @param next The entry to insert before.
     * @param newEntry The new entry to insert.
     */
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

    /**
     * @brief Appends a new entry to the end of the insertion order.
     * @param newEntry The new entry to append.
     */
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

    /**
     * @brief Removes an entry from a specific bucket in the hash table.
     * @param index The index of the bucket.
     * @param key The key of the entry to remove.
     * @return Pointer to the removed entry, or nullptr if not found.
     */
    Entry* removeFromBucket(Size index, const K& key) {
        Entry* entry = buckets[index];
        Entry* prev = nullptr;

        while (entry) {
            if (entry->key == key) {
                if (prev) {
                    prev->next = entry->next;
                } else {
                    buckets[index] = entry->next;
                }
                return entry;
            }

            prev = entry;
            entry = entry->next;
        }

        return nullptr;
    }

    /**
     * @brief Unlinks an entry from both the hash table and insertion order.
     * @param entry The entry to unlink.
     */
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

    /**
     * @brief Unlinks an entry from the insertion order only.
     * @param entry The entry to unlink.
     */
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

    /**
     * @brief Ensures the dictionary has enough capacity to accommodate new entries.
     *        Resizes the hash table if the load factor exceeds 0.75.
     */
    void ensureCapacity() {
        if (size >= cap * 0.75) {
            resize(cap * 2);
        }
    }

    /**
     * @brief Finds an entry in the dictionary by its key.
     * @param key The key to search for.
     * @return Pointer to the entry if found, or nullptr if not found.
     */
    Entry* findEntry(const K& key) {
        Size index = computeIndex(key);
        Entry* entry = buckets[index];

        while (entry) {
            if (entry->key == key) {
                return entry;
            }
            entry = entry->next;
        }

        return nullptr;
    }

    /**
     * @brief Creates a new entry and links it to the hash table.
     * @param key The key for the new entry.
     * @param value The value for the new entry.
     * @return Pointer to the newly created entry.
     */
    inline Entry* getEntry(const K& key, const V& value) {
        Size index = computeIndex(key);
        Entry* newEntry = new Entry(key, value);
        newEntry->next = buckets[index];
        buckets[index] = newEntry;
        size++;
        return newEntry;
    }

    inline void returnEntry(Entry* e) {
        delete e;
    }

    /**
     * @brief Appends a new entry to the back of the insertion order.
     * @param newEntry The new entry to append.
     */
    void pushEntry(Entry* newEntry) {
        if (tail == nullptr) {
            head = tail = newEntry;
        } else {
            tail->nextOrder = newEntry;
            newEntry->prevOrder = tail;
            tail = newEntry;
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
    VaDict(VaDict&& other) noexcept :
        cap(other.cap), size(other.size), buckets(other.buckets),
        head(other.head), tail(other.tail), hashFunc(std::move(other.hashFunc))
    {
        other.buckets = nullptr;
        other.head = other.tail = nullptr;
        other.size = 0;
        other.cap = 0;
    }

    /**
     * @brief Constructs a dictionary from an initializer list of key-value pairs.
     * @param init Initializer list of key-value pairs.
     */
    template <
        typename U1 = K, typename U2 = V,
        typename = tt::EnableIf< !tt::IsReference<U2> >
    >
    VaDict(std::initializer_list<VaPair<U1, U2>> init) : VaDict() {
        resize(init.size());

        #if __cplusplus >= CPP17
            for (auto& [k, v]: init) {
                put(k, v);
            }
        #else
            for (const auto& pair: init) {
                put(pair.first, pair.second);
            }
        #endif
    }

    /**
     * @brief Constructs a dictionary from an initializer list of key-value pairs.
     * @param init Initializer list of key-value pairs.
     */
    template <
        typename U1 = K, typename U2 = V,
        typename = tt::EnableIf< !tt::IsReference<U2> >
    >
    VaDict(std::initializer_list<VaPair<const U1, const U2>> init) : VaDict() {
        resize(init.size());

        #if __cplusplus >= CPP17
            for (const auto& [k, v]: init) {
                put(k, v);
            }
        #else
            for (const auto& pair: init) {
                put(pair.first, pair.second);
            }
        #endif
    }

    /**
     * @brief Constructs a dictionary from an initializer list of key-value pairs.
     * @param init Initializer list of key-value pairs.
     */
    template <
        typename U1 = K, typename U2 = V,
        typename = tt::EnableIf< !std::is_reference<U2>::value >
    >
    VaDict(VaList<VaPair<U1, U2>> init) : VaDict() {
        resize(init.size());

        #if __cplusplus >= CPP17
            for (auto& [k, v]: init) {
                put(k, v);
            }
        #else
            for (const auto& pair: init) {
                put(pair.first, pair.second);
            }
        #endif
    }

    /**
     * @brief Constructs a dictionary from an initializer list of key-value pairs.
     * @param init Initializer list of key-value pairs.
     */
    template <
        typename U1 = K, typename U2 = V,
        typename = tt::EnableIf< !std::is_reference<U2>::value >
    >
    VaDict(VaList<VaPair<const U1, const U2>> init) : VaDict() {
        resize(init.size());
        for (const auto& [k, v]: init) {
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
            returnEntry(current);
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

    Size hash() const {
        Size hash = 0;
        Entry* current = head;
        while (current != nullptr) {
            hash ^= VaHash<K>{}(current->key) ^ (VaHash<V>{}(current->value) << 1);
            current = current->nextOrder;
        }
        return hash;
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
            del(key);
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

    /**
     * @brief Inserts or updates a key-value pair in the dictionary.
     * @param key The key to insert or update.
     * @param value The value to associate with the key.
     *
     * @note If the key already exists, its associated value is updated.
     * @note If the key does not exist, a new entry is created and added to the dictionary.
     * @note May trigger a resize if the load factor exceeds 0.75.
     *
     * @warning This operation invalidates iterators if resizing occurs.
     */
    // @{
    void putAtBack(const K& key, const V& value) {
        this->ensureCapacity();
        Entry* entry = findEntry(key);

        if (entry) {
            entry->value = value;
        } else {
            Entry* newEntry = getEntry(key, value);
            pushEntry(newEntry);
        }
    }
    inline void put(const K& key, const V& value) { putAtFront(key, value); }
    // @}

    /**
     * @brief Inserts or updates a key-value pair at the back of the dictionary.
     * @param pair A pair containing the key and value to insert or update.
     *
     * @note If the key already exists, its associated value is updated.
     * @note If the key does not exist, a new entry is created and added to the back of the dictionary.
     * @note May trigger a resize if the load factor exceeds 0.75.
     *
     * @warning This operation invalidates iterators if resizing occurs.
     */
    // @{
    void putAtBack(ConstPairRef pair) {
        this->putAtBack(pair.key, pair.value);
    }
    void putAtBack(VaPair<K, V> pair) {
        this->putAtBack(pair.first, pair.second);
    }
    // @}

    /**
     * @brief Inserts or updates a key-value pair at the front of the dictionary.
     * @param key The key to insert or update.
     * @param value The value to associate with the key.
     *
     * @note If the key already exists, its associated value is updated, and the key-value pair
     *       is moved to the front of the insertion order.
     * @note If the key does not exist, a new entry is created and added to the front of the dictionary.
     * @note May trigger a resize if the load factor exceeds 0.75.
     *
     * @warning This operation invalidates iterators if resizing occurs.
     */
    void putAtFront(const K& key, const V& value) {
        this->ensureCapacity();
        Entry* entry = findEntry(key);

        if (entry) {
            unlinkFromOrder(entry);
            entry->value = value;
            insertBefore(head, entry);
        } else {
            Entry* newEntry = getEntry(key, value);
            insertBefore(head, newEntry);
        }
    }

    /**
     * @brief Inserts or updates a key-value pair at the front of the dictionary.
     * @param pair A PairRef containing references to the key and value to insert or update.
     *
     * @note If the key already exists, its associated value is updated, and the key-value pair
     *       is moved to the front of the insertion order.
     * @note If the key does not exist, a new entry is created and added to the front of the dictionary.
     * @note May trigger a resize if the load factor exceeds 0.75.
     *
     * @warning This operation invalidates iterators if resizing occurs.
     */
    // @{
    void putAtFront(ConstPairRef pair) {
        this->putAtFront(pair.key, pair.value);
    }
    void putAtFront(VaPair<K, V> pair) {
        this->putAtFront(pair.first, pair.second);
    }
    // @}

    /**
     * @brief Removes a key-value pair from the dictionary.
     * @param key The key to remove.
     *
     * @note Does nothing if key is not found.
     * @note Preserves order of remaining elements.
     */
    void del(const K& key) {
        Size index = computeIndex(key);
        Entry* removed = removeFromBucket(index, key);
        if (!removed) return;

        unlinkFromOrder(removed);
        returnEntry(removed);
        size--;
    }

    /**
     * @brief Removes an entry from the dictionary at a specific insertion index.
     * @param index The 0-based index of the entry to remove.
     *
     * @throws IndexOutOfRangeError if the index is greater than or equal to the size of the dictionary.
     * @note This operation preserves the insertion order of the remaining elements.
     * @note The operation is O(n) due to the need to traverse the insertion order list.
     */
    void delIndex(Size index) {
        if (index >= size) throw IndexOutOfRangeError(size, index);

        Entry* current = head;
        for (Size i = 0; i < index; ++i) {
            current = current->nextOrder;
        }

        Size bucketIndex = computeIndex(current->key);
        Entry* removed = removeFromBucket(bucketIndex, current->key);
        if (!removed) return;

        unlinkFromOrder(removed);
        returnEntry(removed);
        size--;
    }

    /**
     * @brief Removes all key-value pairs from the dictionary.
     *
     * @param freeEntries If true, deallocates the memory used for the hash table buckets.
     *
     * @note If freeEntries is false, the capacity remains unchanged, and the hash table can be reused.
     * @note After clearing, size is 0 and iteration is reset.
     */
    void clear(bool freeEntries = false) {
        Entry* current = head;
        while (current != nullptr) {
            Entry* next = current->nextOrder;
            returnEntry(current);
            current = next;
        }

        for (Size i = 0; i < cap; i++) {
            buckets[i] = nullptr;
        }

        head = tail = nullptr;
        size = 0;

        if (freeEntries) {
            delete[] buckets;
            buckets = nullptr;
            cap = 0;
        }
    }

    /**
     * @brief Removes a key-value pair from the dictionary.
     * @param key The key to remove.
     *
     * @note Does nothing if key is not found.
     * @note Preserves order of remaining elements.
     */
    [[ deprecated("use `del` instated") ]]
    inline void remove(const K& key) {
        return del(key);
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
     * @brief Updates the value associated with a given key.
     * @param key The key to update.
     * @param value The new value to associate with the key.
     *
     * @note If the key does not exist, it will be inserted into the dictionary.
     * @note Preserves the insertion order of the key if it already exists.
     */
    void set(const K& key, const V& value) {
        Entry* entry = findEntry(key);

        if (entry) {
            entry->value = value;
        } else {
            put(key, value);
        }
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
        Entry* entry = findEntry(key);

        if (entry) {
            return entry->value;
        }

        ensureCapacity();
        Entry* newEntry = getEntry(key, V());
        pushEntry(newEntry);
        return newEntry->value;
    }

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

    /**
     * @brief Returns a reference to the value at a given insertion index.
     * @param index Index in insertion order (0-based).
     * @return Reference to the value at the specified index.
     *
     * @throws IndexOutOfRangeError if index >= size.
     * @note This is a linear-time operation.
     */
    // @{
    V& valueAtIndex(Size index) {
        if (index >= size) throw IndexOutOfRangeError(size, index);

        Entry* current = head;
        for (Size i = 0; i < index; ++i) {
            current = current->nextOrder;
        }
        return current->value;
    }
    V& atIndex(Size index) { return valueAtIndex(index); }
    // @}

    /**
     * @brief Returns a const reference to the value at a given insertion index.
     * @param index Index in insertion order (0-based).
     * @return Const reference to the value at the specified index.
     *
     * @throws IndexOutOfRangeError if index >= size
     */
    // @{
    const V& valueAtIndex(Size index) const {
        if (index >= size) throw IndexOutOfRangeError(size, index);

        Entry* current = head;
        for (Size i = 0; i < index; ++i) {
            current = current->nextOrder;
        }
        return current->value;
    }
    const V& atIndex(Size index) const { return valueAtIndex(index); }
    // @}

    /**
     * @brief Returns a reference to the key at a given insertion index.
     * @param index Index in insertion order (0-based).
     * @return Reference to the key at the specified index.
     *
     * @throws IndexOutOfRangeError if index >= size.
     * @note This is a linear-time operation.
     */
    const K& keyAtIndex(Size index) const {
        if (index >= size) throw IndexOutOfRangeError(size, index);

        Entry* current = head;
        for (Size i = 0; i < index; ++i) {
            current = current->nextOrder;
        }
        return current->key;
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

    /**
     * @brief Retrieves the value at the front of the dictionary.
     * @return Reference to the value at the front.
     * @throws IndexOutOfRangeError if the dictionary is empty.
     */
    // @{
    V& valueAtFront() {
        if (isEmpty()) throw IndexOutOfRangeError(size, 0);
        return head->value;
    }
    V& front() { return valueAtFront(); }
    // @}

    /**
     * @brief Retrieves the value at the front of the dictionary (const version).
     * @return Const reference to the value at the front.
     * @throws IndexOutOfRangeError if the dictionary is empty.
     */
    // @{
    const V& valueAtFront() const {
        if (isEmpty()) throw IndexOutOfRangeError(size, 0);
        return head->value;
    }
    const V& front() const { return valueAtFront(); }
    // @}

    /**
     * @brief Retrieves the key at the front of the dictionary.
     * @return Const reference to the key at the front.
     * @throws IndexOutOfRangeError if the dictionary is empty.
     */
    const K& keyAtFront() const {
        if (isEmpty()) throw IndexOutOfRangeError(size, 0);
        return head->key;
    }

    /**
     * @brief Retrieves the key-value pair at the front of the dictionary.
     * @return A PairRef containing references to the key and value at the front.
     * @throws IndexOutOfRangeError if the dictionary is empty.
     */
    PairRef pairAtFront() {
        if (isEmpty()) throw IndexOutOfRangeError(size, 0);
        return PairRef{head->key, head->value};
    }

    /**
     * @brief Retrieves the key-value pair at the front of the dictionary (const version).
     * @return A ConstPairRef containing const references to the key and value at the front.
     * @throws IndexOutOfRangeError if the dictionary is empty.
     */
    ConstPairRef pairAtFront() const {
        if (isEmpty()) throw IndexOutOfRangeError(size, 0);
        return ConstPairRef{head->key, head->value};
    }

    /**
     * @brief Retrieves the value at the back of the dictionary.
     * @return Reference to the value at the back.
     * @throws IndexOutOfRangeError if the dictionary is empty.
     */
    // @{
    V& valueAtBack() {
        if (isEmpty()) throw IndexOutOfRangeError(size, size - 1);
        return tail->value;
    }
    V& back() { return valueAtBack(); }
    // @}

    /**
     * @brief Retrieves the value at the back of the dictionary (const version).
     * @return Const reference to the value at the back.
     * @throws IndexOutOfRangeError if the dictionary is empty.
     */
    // @{
    const V& valueAtBack() const {
        if (isEmpty()) throw IndexOutOfRangeError(size, size - 1);
        return tail->value;
    }
    const V& back() const { return valueAtBack(); }
    // @}

    /**
     * @brief Retrieves the key at the back of the dictionary.
     * @return Const reference to the key at the back.
     * @throws IndexOutOfRangeError if the dictionary is empty.
     */
    const K& keyAtBack() const {
        if (isEmpty()) throw IndexOutOfRangeError(size, size - 1);
        return tail->key;
    }

    /**
     * @brief Retrieves the key-value pair at the back of the dictionary.
     * @return A PairRef containing references to the key and value at the back.
     * @throws IndexOutOfRangeError if the dictionary is empty.
     */
    PairRef pairAtBack() {
        if (isEmpty()) throw IndexOutOfRangeError(size, size - 1);
        return PairRef{tail->key, tail->value};
    }

    /**
     * @brief Retrieves the key-value pair at the back of the dictionary (const version).
     * @return A ConstPairRef containing const references to the key and value at the back.
     * @throws IndexOutOfRangeError if the dictionary is empty.
     */
    ConstPairRef pairAtBack() const {
        if (isEmpty()) throw IndexOutOfRangeError(size, size - 1);
        return ConstPairRef{tail->key, tail->value};
    }

    /**
     * @brief Checks if the dictionary is empty.
     * @return true if the dictionary is empty, false otherwise.
     */
    inline bool isEmpty() const noexcept { return size == 0; }

    /**
     * @brief Retrieves the current number of elements in the dictionary.
     * @return The size of the dictionary.
     */
    inline Size getSize() const noexcept { return size; }

    /**
     * @brief Retrieves the current capacity of the dictionary.
     * @return The capacity of the dictionary.
     */
    inline Size getCapacity() const noexcept { return cap; }

    /**
     * @brief Alias for a raw view of the dictionary's internal state.
     *        This struct provides a layout-compatible snapshot of the hash map's internals,
     *        usable for both inspection and unsafe mutation.
     *
     * @note The `RawView` type is layout-compatible with `VaDict` and provides direct access
     *       to the internal fields such as buckets, size, and capacity. This is useful for
     *       advanced use cases like serialization or debugging.
     */
    using RawView = VaDictRawView<K, V, Hash>;

    /**
     * @brief Returns a safe, read-only view of the dictionary internals.
     *        This method returns a pointer to the current dictionary object,
     *        reinterpreted as a const-qualified raw view. It allows inspection
     *        of the internal state without enabling mutation.
     *
     * @return A const pointer to a `VaDictRawView<K, V, Hash>`.
     *
     * @note The returned view is valid only as long as the owning dictionary (`VaDict`) exists.
     *       Accessing the view after the dictionary is destroyed results in undefined behavior.
     * @note This cast is valid because the layout of `VaDict` begins with fields that are
     *       identical to those in `VaDictRawView`.
     * @warning This method does not perform any synchronization. If the dictionary is modified
     *          concurrently, the view may become inconsistent.
     * @see getUnsafeAccess() for mutable access.
     */
    const RawView* getRawView() const {
        return reinterpret_cast<const RawView*>(this);
    }

    /**
     * @brief Grants unsafe mutable access to the dictionary internals.
     *        This method returns a raw pointer to this object reinterpreted
     *        as a `VaDictRawView<K, V, Hash>`. All members (buckets, size, hash) can be modified.
     *
     * @return A mutable pointer to a `VaDictRawView<K, V, Hash>`.
     *
     * @note The returned view is valid only as long as the owning dictionary (`VaDict`) exists.
     *       Accessing the view after the dictionary is destroyed results in undefined behavior.
     * @warning This is an unsafe interface. The caller is responsible for maintaining
     *          all invariants of the dictionary. Incorrect use may result in memory corruption,
     *          undefined behavior, or crashes.
     * @warning This method does not perform any synchronization. Concurrent modifications
     *          to the dictionary while using the view may lead to race conditions or corruption.
     * @see getRawView() for a read-only alternative.
     */
    RawView* getUnsafeAccess() {
        return reinterpret_cast<RawView*>(this);
    }

  public operators:
    /**
    * @brief Lexicographically compares two dictionaries.
    *        Elements are compared in insertion order.
    *        If keys differ, the key comparison determines the result.
    * @param other The other dictionary to compare with.
    * @return @a true if this dictionary is lexicographically less than the other.
    *
    * @note Requires both @ref K and @ref V to be comparable via <.
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
     * @warning Requires both `K` and `V` to implement `operator<`
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
                if constexpr (tt::IsTriviallyCopyable<V>) {
                    if (std::memcmp(&e->value, &otherValue, sizeof(V)) != 0) return false;
                } else {
                    if (!(e->value == otherValue)) return false;
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
                if constexpr (tt::IsTriviallyCopyable<V>) {
                    if (std::memcmp(&a->value, &b->value, sizeof(V)) != 0) return false;
                } else {
                    if (!(a->value == b->value)) return false;
                }
            #else
                if (!(a->value == b->value)) return false;
            #endif

            a = a->nextOrder;
            b = b->nextOrder;
        }

        return true;
    }

  public friends:
    friend inline Size len(const VaDict& dict) { return dict.size; }
    friend inline Size cap(const VaDict& dict) { return dict.cap; }

  public iterators:
    class Iterator {
      public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = Entry;
        using difference_type = std::ptrdiff_t;
        using reference = PairRef;
        using pointer = void;

      protected:
        Entry* ptr;

      public:
        explicit Iterator(Entry* e) : ptr(e) {}

        bool operator!=(const Iterator& other) const { return ptr != other.ptr; }
        void operator++() {
            if (ptr) ptr = ptr->nextOrder;
        }

        PairRef operator*() { return {ptr->key, ptr->value}; }
        ConstPairRef operator*() const { return {ptr->key, ptr->value}; }
    };

    class ConstIterator {
      public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = Entry;
        using difference_type = std::ptrdiff_t;
        using reference = PairRef;
        using pointer = void;

      protected:
        const Entry* ptr;

      public:
        explicit ConstIterator(const Entry* e) : ptr(e) {}

        bool operator!=(const ConstIterator& other) const { return ptr != other.ptr; }
        void operator++() {
            if (ptr) ptr = ptr->nextOrder;
        }

        ConstPairRef operator*() const { return {ptr->key, ptr->value}; }
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

    Entry* next(const Entry* current) const { return current ? current->nextOrder : nullptr; }
};

namespace va {

/**
 * @brief Swaps the keys and values of a dictionary.
 *
 * @tparam K The key type of the input dictionary.
 * @tparam V The value type of the input dictionary.
 * @tparam NewHash The hash function type for the resulting dictionary (defaults to `VaHash<K>`).
 * @tparam OldHash The hash function type for the input dictionary (defaults to `VaHash<K>`).
 *
 * @param dict The input dictionary whose keys and values will be swapped.
 *
 * @return A new dictionary where the keys are the values of the input dictionary,
 *         and the values are the keys of the input dictionary.
 *
 * @note The resulting dictionary uses the `NewHash` hash function.
 * @note If the input dictionary contains duplicate values, only one of the corresponding keys
 *       will be preserved in the resulting dictionary.
 */
template <typename K, typename V, typename NewHash = VaHash<K>, typename OldHash = VaHash<K>>
VaDict<V, K, NewHash> swapKeyValue(const VaDict<K, V, OldHash>& dict) {
    VaDict<V, K, NewHash> result;
    result.reserve(dict.getSize());
    for (const auto& pair: dict) {
        result.put(pair.value, pair.key);
    }
    return result;
}

/**
 * @brief Transforms the values of a dictionary using a provided function.
 *
 * @tparam K The key type of the dictionary.
 * @tparam OldV The original value type of the dictionary.
 * @tparam NewV The new value type after transformation.
 * @tparam NewHash The hash function type for the resulting dictionary (defaults to `VaHash<K>`).
 * @tparam OldHash The hash function type for the input dictionary (defaults to `VaHash<K>`).
 *
 * @param mod A function that takes a value of type `OldV` (const reference) and returns a transformed value of type `NewV`.
 * @param dict The input dictionary whose values will be transformed.
 *
 * @return A new dictionary with the same keys as the input dictionary and transformed values.
 *
 * @note The order of elements in the resulting dictionary matches the insertion order of the input dictionary.
 * @note The resulting dictionary uses the `NewHash` hash function.
 */
// @{
template <typename K, typename OldV, typename NewV, typename NewHash = VaHash<K>, typename OldHash = VaHash<K>>
VaDict<K, NewV, NewHash> mapValues(VaFunc<NewV(const OldV&)> mod, const VaDict<K, OldV, OldHash>& dict) {
    VaDict<K, NewV, NewHash> result;
    result.reserve(dict.getSize());
    for (const auto& pair: dict) {
        result.put(pair.key, mod(pair.value));
    }
    return result;
}

template <typename K, typename OldV, typename NewV, typename NewHash = VaHash<K>, typename OldHash = VaHash<K>>
VaDict<K, NewV, NewHash> mapValues(VaFunc<NewV(OldV)> mod, const VaDict<K, OldV, OldHash>& dict) {
    VaDict<K, NewV, NewHash> result;
    result.reserve(dict.getSize());
    for (const auto& pair: dict) {
        result.put(pair.key, mod(pair.value));
    }
    return result;
}
//@}

/**
 * @brief Transforms the keys of a dictionary using a provided function.
 *
 * @tparam V The value type of the dictionary.
 * @tparam OldK The original key type of the dictionary.
 * @tparam NewK The new key type after transformation.
 * @tparam NewHash The hash function type for the resulting dictionary (defaults to `VaHash<NewK>`).
 * @tparam OldHash The hash function type for the input dictionary (defaults to `VaHash<OldK>`).
 *
 * @param mod A function that takes a key of type `OldK` (const reference) and returns a transformed key of type `NewK`.
 * @param dict The input dictionary whose keys will be transformed.
 *
 * @return A new dictionary with transformed keys and the same values as the input dictionary.
 *
 * @note The order of elements in the resulting dictionary matches the insertion order of the input dictionary.
 * @note The resulting dictionary uses the `NewHash` hash function.
 */
//@{
template <typename V, typename OldK, typename NewK, typename NewHash = VaHash<NewK>, typename OldHash = VaHash<OldK>>
VaDict<NewK, V, NewHash> mapKeys(VaFunc<NewK(const OldK&)> mod, const VaDict<OldK, V, OldHash>& dict) {
    VaDict<NewK, V, NewHash> result;
    result.reserve(dict.getSize());
    for (const auto& pair: dict) {
        result.put(mod(pair.key), pair.value);
    }
    return result;
}

template <typename V, typename OldK, typename NewK, typename NewHash = VaHash<NewK>, typename OldHash = VaHash<OldK>>
VaDict<NewK, V, NewHash> mapKeys(VaFunc<NewK(OldK)> mod, const VaDict<OldK, V, OldHash>& dict) {
    VaDict<NewK, V, NewHash> result;
    result.reserve(dict.getSize());
    for (const auto& pair: dict) {
        result.put(mod(pair.key), pair.value);
    }
    return result;
}
//@}

/**
 * @brief Filters a dictionary by its keys based on a predicate function.
 *
 * @tparam K The key type.
 * @tparam V The value type.
 * @tparam NewHash The hash function type for the resulting dictionary (defaults to `VaHash<K>`).
 * @tparam OldHash The hash function type for the input dictionary (defaults to `VaHash<K>`).
 *
 * @param predicate A function that takes a key (const reference) and returns a boolean.
 *                  Only keys for which the predicate returns true will be included in the result.
 * @param dict The input dictionary to filter.
 *
 * @return A new dictionary containing only the key-value pairs where the keys satisfy the predicate.
 *
 * @note The resulting dictionary uses the `NewHash` hash function.
 * @note The order of elements in the resulting dictionary matches the insertion order of the input dictionary.
 */
//@{
template <typename K, typename V, typename NewHash = VaHash<K>, typename OldHash = VaHash<K>>
VaDict<K, V, NewHash> filterByKey(VaFunc<bool(const K&)> predicate, VaDict<K, V, OldHash> dict) {
    VaDict<K, V, NewHash> result;
    result.reserve(dict.getSize());
    for (const auto& pair : dict) {
        if (predicate(pair.key)) {
            result.put(pair.key, pair.value);
        }
    }
    return result;
}

template <typename K, typename V, typename NewHash = VaHash<K>, typename OldHash = VaHash<K>>
VaDict<K, V, NewHash> filterByKey(VaFunc<bool(K)> predicate, VaDict<K, V, OldHash> dict) {
    VaDict<K, V, NewHash> result;
    result.reserve(dict.getSize());
    for (const auto& pair : dict) {
        if (predicate(pair.key)) {
            result.put(pair.key, pair.value);
        }
    }
    return result;
}
//@}

/**
 * @brief Filters a dictionary by its values based on a predicate function.
 *
 * @tparam K The key type.
 * @tparam V The value type.
 * @tparam NewHash The hash function type for the resulting dictionary (defaults to `VaHash<K>`).
 * @tparam OldHash The hash function type for the input dictionary (defaults to `VaHash<K>`).
 *
 * @param predicate A function that takes a value (const reference) and returns a boolean.
 *                  Only key-value pairs where the value satisfies the predicate will be included in the result.
 * @param dict The input dictionary to filter.
 *
 * @return A new dictionary containing only the key-value pairs where the values satisfy the predicate.
 *
 * @note The resulting dictionary uses the `NewHash` hash function.
 * @note The order of elements in the resulting dictionary matches the insertion order of the input dictionary.
 */
//@{
template <typename K, typename V, typename NewHash = VaHash<K>, typename OldHash = VaHash<K>>
VaDict<K, V, NewHash> filterByValue(VaFunc<bool(const V&)> predicate, VaDict<K, V, OldHash> dict) {
    VaDict<K, V, NewHash> result;
    result.reserve(dict.getSize());
    for (const auto& pair : dict) {
        if (predicate(pair.value)) {
            result.put(pair.key, pair.value);
        }
    }
    return result;
}

template <typename K, typename V, typename NewHash = VaHash<K>, typename OldHash = VaHash<K>>
VaDict<K, V, NewHash> filterByValue(VaFunc<bool(V)> predicate, VaDict<K, V, OldHash> dict) {
    VaDict<K, V, NewHash> result;
    result.reserve(dict.getSize());
    for (const auto& pair : dict) {
        if (predicate(pair.value)) {
            result.put(pair.key, pair.value);
        }
    }
    return result;
}
//@}

} // namespace va

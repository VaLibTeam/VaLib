// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <VaLib/Types/BasicTypedef.hpp>
#include <VaLib/Meta/BasicDefine.hpp>

template <typename K, typename V, typename Hash>
class VaDict;

template <typename K, typename V>
struct VaDictEntry;

/**
 * @brief Raw view into the internal state of a dictionary (hash map).
 *        This struct provides a layout-compatible snapshot of a hash map’s internal state:
 *        bucket array, element count, insertion order, and hashing logic.
 *        It is intended for both safe inspection and unsafe manipulation,
 *        depending on how it is obtained.
 *
 * @tparam K Key type.
 * @tparam V Value type.
 * @tparam Hash Hash function type.
 *
 * @note This is a non-owning, layout-compatible view. It assumes that the referenced
 *       dictionary stays alive during its usage.
 *
 * @warning If used for mutation, the caller is fully responsible for preserving internal invariants.
 *          Misuse may result in memory corruption or undefined behavior.
 *
 * @see VaDict<K, V, Hash>::getRawView()
 * @see VaDict<K, V, Hash>::getUnsafeAccess()
 */
template <typename K, typename V, typename Hash>
struct VaDictRawView {
    using Entry = VaDictEntry<K, V>;

    Size cap;       ///< Capacity of the hash table
    Size size;      ///< Current number of entries stored in the hash table

    Entry** buckets; ///< Array of pointers to hash buckets
    Entry* head;     ///< Pointer to the first element (in insertion order)
    Entry* tail;     ///< Pointer to the last element (in insertion order)

    Hash hashFunc;  ///< Hash function used to compute bucket indices from keys
};

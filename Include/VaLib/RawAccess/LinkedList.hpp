// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <VaLib/Types/BasicTypedef.hpp>

template <typename T>
struct VaLinkedListNode;

template <typename T>
class VaLinkedList;

/**
 * @brief Raw view into the internal state of a linked list.
 *        This struct provides layout-compatible access to the internal pointers
 *        and metadata of a linked list: head/tail, length, and freelist.
 *
 * @tparam T The type of elements in the list.
 *
 * @note This struct is non-owning and assumes the original list remains valid.
 * @note Intended for both safe read-only access and unsafe direct mutation,
 *       depending on context in which it is used.
 *
 * @warning If used for mutation (via unsafe access), the caller is responsible
 *          for preserving all list invariants. Misuse can lead to memory corruption.
 *
 * @see VaLinkedList<T>::getRawView()
 * @see VaLinkedList<T>::getUnsafeAccess()
 */
template <typename T>
struct VaLinkedListRawView {
    using Node = VaLinkedListNode<T>;

    Node* head;         ///< Pointer to the first node in the list
    Node* tail;         ///< Pointer to the last node in the list
    Size len;           ///< Current number of elements in the list

    Node* freeListHead; ///< Pointer to the first node in the freelist
    Size freeListSize;  ///< Number of nodes currently stored in the freelist
};

// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <VaLib/Types/BasicTypedef.hpp>
#include <VaLib/Meta/BasicDefine.hpp>

template <typename T>
class VaList;

/**
 * @brief Raw view into the internal state of a dynamic list.
 *        This struct provides a simple, layout-compatible view of a list's buffer,
 *        size, and capacity. It is intended for both safe read-only inspection and
 *        unsafe mutable access, depending on how it is obtained.
 *
 * @tparam T The type of elements in the list.
 *
 * @note This is a non-owning, trivially copyable view. It holds no guarantees about
 *       memory safety or lifetime. Use responsibly.
 *
 * @note The layout of this struct is guaranteed to match the beginning of `VaList<T>`,
 *       which makes reinterpret_casts between them valid.
 *
 * @warning If used for mutation, the caller is fully responsible for upholding invariants.
 *          Incorrect use may cause memory corruption or undefined behavior.
 *
 * @see VaList<T>::getRawView()
 * @see VaList<T>::getUnsafeAccess()
 */
template <typename T>
struct VaListRawView {
    Size len;  ///< Number of elements currently stored.
    Size cap;  ///< Current capacity of the allocated buffer.
    T* data;   ///< Pointer to the raw array of elements.
};

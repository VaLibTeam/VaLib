// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <VaLib/Types/BasicTypedef.hpp>
#include <atomic>

namespace va::detail {

template <typename T>
struct ControlBlock {
    T* ptr; ///< Pointer to the managed object

    std::atomic<Size> sharedCount; ///< Atomic counter for the number of shared references
    std::atomic<Size> weakCount;   ///< Atomic counter for the number of weak references

    /// @brief Initializes the control block with a pointer to the managed object
    ControlBlock(T* p) : ptr(p), sharedCount(1), weakCount(0) {}

    /// @brief Deletes the managed object when the control block is destroyed.
    ~ControlBlock() { delete ptr; }
};

template <typename T>
struct ControlBlock<T[]> {
    T* ptr; ///< Pointer to the managed array

    std::atomic<Size> sharedCount; ///< Atomic counter for the number of shared references
    std::atomic<Size> weakCount;   ///< Atomic counter for the number of weak references

    /// @brief Initializes the control block with a pointer to the managed array
    ControlBlock(T* p) : ptr(p), sharedCount(1), weakCount(0) {}

    /// @brief Deletes the managed array when the control block is destroyed.
    ~ControlBlock() { delete[] ptr; }
};

} // namespace va::detail

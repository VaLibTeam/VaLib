// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <VaLib/Types/BasicConcepts.hpp>

#include <stddef.h>
#include <stdint.h>

#include <memory>
#include <string>

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef float float32;
typedef double float64;

typedef int32 rune;

typedef void FuncType();
typedef void (*FuncPtr)();

template <typename R, typename... Args>
using Function = R (*)(Args...);

typedef uint8 Byte;
typedef uint16 Word;
typedef uint32 DWord;
typedef uint64 QWord;

typedef time_t Time;

#ifdef VaLib_USE_INT128
typedef __int128_t int128;
typedef __uint128_t uint128;
#endif

// types from C++ stdlib
template <typename T>
using UniquePtr = std::unique_ptr<T>;

template <typename T>
using SharedPtr = std::shared_ptr<T>;

template <typename T>
using WeakPtr = std::weak_ptr<T>;
// ---

typedef size_t Size;
namespace va {

template <typename T, typename = void>
struct HasEqualityOperator: std::false_type {};

template <typename T>
struct HasEqualityOperator<T, std::void_t<decltype(std::declval<T>() == std::declval<T>())>>
    : std::true_type {};

template <typename T>
inline constexpr bool HasEqualityOperatorV = HasEqualityOperator<T>::value;

} // namespace va
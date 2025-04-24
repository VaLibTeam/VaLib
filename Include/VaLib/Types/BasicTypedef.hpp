// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#ifdef VaLib_USE_CONCEPTS
#include <VaLib/Types/BasicConcepts.hpp>
#endif

#include <VaLib/Types/TypeTraits.hpp>

#include <stddef.h>
#include <stdint.h>

#include <ctime>

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
typedef unsigned int uint;

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

typedef size_t Size;

// nothing. 🙏
struct NoneType {};
constexpr NoneType nil{};

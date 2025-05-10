// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <type_traits>
#ifdef VaLib_USE_CONCEPTS
#include <VaLib/Types/BasicConcepts.hpp>
#endif

#include <VaLib/Types/TypeTraits.hpp>

#include <stddef.h>
#include <stdint.h>

#include <ctime>

#ifndef VaLib_NOT_DEFINE_UINTS
    typedef uint8_t uint8;
    typedef uint16_t uint16;
    typedef uint32_t uint32;
    typedef uint64_t uint64;

    #ifdef VaLib_USE_INT128
        typedef __uint128_t uint128;
    #endif

    typedef unsigned int uint;
#endif

#ifndef VaLib_NOT_DEFINE_INTS
    typedef int8_t int8;
    typedef int16_t int16;
    typedef int32_t int32;
    typedef int64_t int64;

    #ifdef VaLib_USE_INT128
        typedef __int128_t int128;
    #endif
#endif

#ifndef VaLib_NOT_DEFINE_FLOATS
    typedef float float32;
    typedef double float64;
#endif

typedef int32_t rune;
typedef unsigned char byte;

#ifndef VaLib_NOT_DEFINE_FUNCTIONS
    typedef void FuncType();
    typedef void (*FuncPtr)();

    /**
    * @brief Alias for a function pointer type.
    * @tparam R The return type of the function.
    * @tparam Args The parameter types of the function.
    *
    * @note This alias is specifically for raw function pointers.
    *       If you need to store lambdas, functors, or other callable objects, consider using @ref VaFunc from `FuncTools/Func.hpp` instead.
    */
    template <typename R, typename... Args>
    using Function = R (*)(Args...);
#endif

#ifndef VaLib_NOT_DEFINE_BYTE_WORDS
    typedef uint8_t Byte;
    typedef uint16_t Word;
    typedef uint32_t DWord;
    typedef uint64_t QWord;
#endif

#ifndef VaLib_NOT_DEFINE_TIME
    typedef time_t Time;
#endif

#ifndef VaLib_NOT_DEFINE_TRUE_FALSE_TYPES
    using TrueType = std::true_type;
    using FalseType = std::false_type;
#endif

#ifndef VaLib_NOT_DEFINE_SIZE
    typedef size_t Size;
#endif

#ifndef VaLib_NOT_DEFINE_NONETYPE_NIL
    // nothing.
    struct NoneType {};
    constexpr NoneType nil{};
#endif

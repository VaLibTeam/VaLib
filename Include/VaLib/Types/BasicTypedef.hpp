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
#include <time.h>

#ifndef VaLib_NOT_DEFINE_INTS
    typedef int8_t int8;    /// @brief 8-bit signed integer
    typedef int16_t int16;  /// @brief 16-bit signed integer
    typedef int32_t int32;  /// @brief 32-bit signed integer
    typedef int64_t int64;  /// @brief 64-bit signed integer

    #ifdef VaLib_USE_INT128
        typedef __int128_t int128; /// @brief 128-bit signed integer
    #endif
#endif

#ifndef VaLib_NOT_DEFINE_UINTS
    typedef uint8_t uint8;    /// @brief 8-bit unsigned integer
    typedef uint16_t uint16;  /// @brief 16-bit unsigned integer
    typedef uint32_t uint32;  /// @brief 32-bit unsigned integer
    typedef uint64_t uint64;  /// @brief 64-bit unsigned integer

    #ifdef VaLib_USE_INT128
        typedef __uint128_t uint128; /// @brief 128-bit unsigned integer
    #endif

    typedef unsigned int uint;
#endif

#ifndef VaLib_NOT_DEFINE_FLOATS
    #if __cplusplus >= CPP20 && defined(__STDCPP_FLOAT32_T__) && defined(__STDCPP_FLOAT64_T__)
        #include <stdfloat>
        typedef std::float32_t float32;
        typedef std::float64_t float64;
    #else
        typedef float float32;   // likely 32-bit, but not guaranteed
        typedef double float64;  // likely 64-bit, but not guaranteed
    #endif

    typedef long double floatExt;
#endif

#ifndef VaLib_NOT_DEFINE_RUNE
    typedef int32_t rune;
#endif

#ifndef VaLib_NOT_DEFINE_BYTE
    typedef unsigned char byte;
#endif

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

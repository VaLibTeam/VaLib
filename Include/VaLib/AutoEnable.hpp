// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <VaLib/Utils/BasicDefine.hpp>

#if __cplusplus >= CPP20
#define VaLib_USE_CONCEPTS
#endif

#ifdef __SIZEOF_INT128__
#define VaLib_USE_INT128
#endif

/*
if any of the following macros are defined,
You must provide the corresponding symbols manually.
NOTE: Your versions must have the same API and behavior as the orginal ones for everything to work correctly

-- EXAMPLE --
```cpp
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

#define VaLib_NOT_DEFINE_INTS // From now on VaLib will use your int8, int16, etc. Just remember to put this before including other VaLib headers
```
*/
// #define VaLib_NOT_DEFINE_INTS              // int8, int16, int32, int64
// #define VaLib_NOT_DEFINE_UINTS             // uint8, uint16, uint32, uint64
// #define VaLib_NOT_DEFINE_FLOATS            // float32, float64
// #define VaLib_NOT_DEFINE_BYTE              // byte
// #define VaLib_NOT_DEFINE_FUNCS             // Function, FuncType, FuncPtr
// #define VaLib_NOT_DEFINE_TIME              // Time
// #define VaLib_NOT_DEFINE_TRUE_FALSE_TYPES  // TrueType, FalseType
// #define VaLib_NOT_DEFINE_NONETYPE_NIL      // NoneType, nil
// #define VaLib_NOT_DEFINE_SIZE              // Size
// #define VaLib_NOT_DEFINE_BYTE_WORDS        // Byte, Word, DWord, QWord

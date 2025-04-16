// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <VaLib/Utils/BasicDefine.hpp>

#if __cplusplus >= CPP20
#define VaLib_USE_CONCEPTS
#endif

#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
#define VaLib_USE_INT128
#endif
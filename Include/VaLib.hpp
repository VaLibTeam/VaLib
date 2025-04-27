// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <VaLib/AutoEnable.hpp>

#include <VaLib/Types.hpp>
#include <VaLib/Utils.hpp>
#include <VaLib/FuncTools.hpp>

#define VaLib_VERSION_MAJOR 1
#define VaLib_VERSION_MINOR 8
#define VaLib_VERSION_PATCH 0
#define VaLib_VERSION stringify(VaLib_VERSION_MAJOR.VaLib_VERSION_MINOR.VaLib_VERSION_PATCH)

struct VaVersion {
    int major;
    int minor;
    int patch;
};

constexpr VaVersion VaLibVersion{
    VaLib_VERSION_MAJOR, VaLib_VERSION_MINOR, VaLib_VERSION_PATCH,
};

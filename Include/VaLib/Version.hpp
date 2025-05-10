// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <VaLib/Utils/BasicDefine.hpp>

#include <VaLib/Types/String.hpp>
#include <VaLib/Types/ImmutableString.hpp>

#include <compare>
#include <string>

#define VaLib_VERSION_MAJOR 1
#define VaLib_VERSION_MINOR 9
#define VaLib_VERSION_PATCH 0
#define VaLib_VERSION ( stringify_expanded(VaLib_VERSION_MAJOR) "." stringify_expanded(VaLib_VERSION_MINOR) "." stringify_expanded(VaLib_VERSION_PATCH) )

class VaVersion {
public:
    int major, minor, patch;

    explicit constexpr VaVersion() : major(0), minor(0), patch(0) {}
    explicit constexpr VaVersion(int major, int minor = 0, int patch = 0) : major(major), minor(minor), patch(patch) {}


    VaString toString()                   { return VaLib_VERSION; }
    VaImmutableString toImmutableString() { return VaLib_VERSION; }
    std::string toStdString()             { return VaLib_VERSION; }

public operators:
    friend inline constexpr bool operator==(const VaVersion& lhs, const VaVersion& rhs) {
        return lhs.major == rhs.major && lhs.minor == rhs.minor && lhs.patch == rhs.patch;
    }

    friend inline constexpr bool operator!=(const VaVersion& lhs, const VaVersion& rhs) {
        return !(lhs == rhs);
    }

    friend inline constexpr bool operator<(const VaVersion& lhs, const VaVersion& rhs) {
        return (lhs.major < rhs.major) ||
               (lhs.major == rhs.major && lhs.minor < rhs.minor) ||
               (lhs.major == rhs.major && lhs.minor == rhs.minor && lhs.patch < rhs.patch);
    }

    friend inline constexpr bool operator>(const VaVersion& lhs, const VaVersion& rhs) {
        return rhs < lhs;
    }

    friend inline constexpr bool operator<=(const VaVersion& lhs, const VaVersion& rhs) {
        return !(rhs < lhs);
    }

    friend inline constexpr bool operator>=(const VaVersion& lhs, const VaVersion& rhs) {
        return !(lhs < rhs);
    }

    #if __cplusplus >= CPP20
        friend inline constexpr auto operator<=>(const VaVersion& lhs, const VaVersion& rhs) = default;
    #endif
};

constexpr VaVersion VaLibVersion{
    VaLib_VERSION_MAJOR, VaLib_VERSION_MINOR, VaLib_VERSION_PATCH,
};

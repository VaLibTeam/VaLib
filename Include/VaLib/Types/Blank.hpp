// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

namespace va::detail {

class Blank {
public:
    constexpr Blank() noexcept = default;
    constexpr Blank(const Blank&) noexcept = default;
    constexpr Blank(Blank&&) noexcept = default;
    constexpr Blank& operator=(const Blank&) noexcept = default;
    constexpr Blank& operator=(Blank&&) noexcept = default;

    template <typename T>
    constexpr Blank& operator=(T&&) noexcept { return *this; }

    template <typename T>
    constexpr const Blank& operator=(T&&) const noexcept { return *this; }

    explicit operator bool() const noexcept { return false; }
};

}

constexpr va::detail::Blank _;

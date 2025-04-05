// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <Types/BasicTypedef.hpp>

#include <ostream>
#include <string>

#include <Types/ImmutableString.hpp>
#include <Types/String.hpp>

#ifdef VaLib_USE_CONCEPTS
#include <concepts>

template <typename T>
concept StdStringer = requires(T t) {
    { t.toStdString() } -> std::same_as<std::string>;
};

template <typename T>
concept Stringer = requires(T t) {
    { t.toString() } -> std::same_as<VaString>;
};

template <typename T>
concept ImmutableStringer = requires(T t) {
    { t.toImmutableString } -> std::same_as<VaImmutableString>;
};

template <StdStringer T>
std::ostream& operator<<(std::ostream& os, T v) {
    os << v.toStdString();
    return os;
}

template <Stringer T>
std::ostream& operator<<(std::ostream& os, T v) {
    os << v.toString();
    return os;
}

template <ImmutableStringer T>
std::ostream& operator<<(std::ostream& os, T v) {
    os << v.toImmutableString().toStdString();
}

#endif
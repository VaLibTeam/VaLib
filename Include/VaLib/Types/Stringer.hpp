// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <VaLib/Types/ImmutableString.hpp>
#include <VaLib/Types/String.hpp>

#include <string>

template <typename T>
concept StdStringer = requires(T t) {
    { t.toStdString() } -> std::convertible_to<std::string>;
};

template <typename T>
concept VaStringer = requires(T t) {
    { t.toString() } -> std::convertible_to<VaString>;
};

template <typename T>
concept ImmutableStringer = requires(T t) {
    { t.toImmutableString() } -> std::same_as<VaImmutableString>;
};
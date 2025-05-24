// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <VaLib/Types/BasicTypedef.hpp>
#include <VaLib/Meta/BasicDefine.hpp>

// TypeAt
template <Size Index, typename... T>
class VaTemplateTypeAt;

template <typename First, typename... Rest>
class VaTemplateTypeAt<0, First, Rest...> {
  public:
    using Type = First;
};

template <Size Index, typename First, typename... Rest>
class VaTemplateTypeAt<Index, First, Rest...> {
  public:
    static_assert(Index < sizeof...(Rest) + 1, "Index out of bounds");
    using Type = typename VaTemplateTypeAt<Index - 1, Rest...>::Type;
};

template <Size Index, typename... Ts>
using VaTemplateTypeAt_t = typename VaTemplateTypeAt<Index, Ts...>::Type;

#if __cplusplus >= CPP17

    // ValueAt
    template <Size Index, auto... T>
    class VaTemplateValueAt;

    template <auto First, auto... Rest>
    class VaTemplateValueAt<0, First, Rest...> {
      public:
        static constexpr auto value = First;
    };

    template <Size Index, auto First, auto... Rest>
    class VaTemplateValueAt<Index, First, Rest...> {
      public:
        static_assert(Index < sizeof...(Rest) + 1, "Index out of bounds");
        static constexpr auto value = VaTemplateValueAt<Index - 1, Rest...>::value;
    };

    template <Size Index, auto... Vs>
    constexpr auto VaTemplateValueAt_v = VaTemplateValueAt<Index, Vs...>::value;

#endif

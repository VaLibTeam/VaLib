// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <VaLib/Meta/BasicDefine.hpp>

#include <VaLib/Types/BasicTypedef.hpp>
#include <VaLib/Meta/BasicDefine.hpp>
#include <VaLib/Meta/TemplateAt.hpp>

#include <utility>

template <typename... Ts>
class VaTemplateTypeTuple {
  protected:
    template <Size Offset, typename Seq>
    struct OffsetSequence;

    template <Size Offset, Size... Is>
    struct OffsetSequence<Offset, std::index_sequence<Is...>> {
        using Type = std::index_sequence<(Is + Offset)...>;
    };

    template <Size Index, typename Tp, typename BeforeSeq, typename AfterSeq, typename... Tps>
    struct SplitAndInsertImpl;

    template <Size Index, typename Tp, Size... I1, Size... I2, typename... Tps>
    struct SplitAndInsertImpl<Index, Tp, std::index_sequence<I1...>, std::index_sequence<I2...>, Tps...> {
        using type = VaTemplateTypeTuple<
        typename VaTemplateTypeAt<I1, Tps...>::Type...,
        Tp,
        typename VaTemplateTypeAt<Index + I2, Tps...>::Type...
        >;
    };

    template <Size Index, typename Tp, typename... Tps>
    using InsertHelper = typename VaTemplateTypeTuple<Tps...>::template SplitAndInsertImpl<
        Index,
        Tp,
        std::make_index_sequence<Index>,
        typename OffsetSequence<Index, std::make_index_sequence<sizeof...(Tps) - Index>>::Type,
        Tps...
    >::Type;

  public:
    static constexpr Size len = sizeof...(Ts);
    static constexpr Size size = sizeof...(Ts);

    template <typename Tp>
    static constexpr bool contains = ((tt::IsSame<Tp, Ts>) || ...);

    template <Size Index>
    using TypeAt = typename VaTemplateTypeAt<Index, Ts...>::Type;

    using Front = typename VaTemplateTypeAt<len - 1, Ts...>::Type;
    using Back =  typename VaTemplateTypeAt<0, Ts...>::Type;

    template <typename Tp>
    using Append = VaTemplateTypeTuple<Ts..., Tp>;

    template <typename Tp>
    using Prepend = VaTemplateTypeTuple<Tp, Ts...>;

    template <Size Index, typename Tp>
    using Insert = InsertHelper<Index, Tp, Ts...>;
};

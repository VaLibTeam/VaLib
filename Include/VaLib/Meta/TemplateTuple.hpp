// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <VaLib/Meta/BasicDefine.hpp>
#include <VaLib/Meta/TemplateAt.hpp>
#include <VaLib/Meta/TemplateTypeTuple.hpp>
#include <VaLib/Meta/ValueType.hpp>

#include <VaLib/Types/BasicTypedef.hpp>
#include <VaLib/Types/Tuple.hpp>

#include <utility>

#if __cplusplus >= CPP17
    template <auto... Vs>
    class VaTemplateTuple {
      protected:
        template <Size Index, auto Val, typename BeforeSeq, typename AfterSeq, auto... Vls>
        struct SplitAndInsertImpl;

        template <Size Index, auto Val, Size... I1, Size... I2, auto... Vls>
        struct SplitAndInsertImpl<Index, Val, std::index_sequence<I1...>, std::index_sequence<I2...>, Vls...> {
            using Type = VaTemplateTuple<
                (VaTemplateValueAt<I1, Vls...>::value) ...,         // elements before Index
                Val,                                                // inserted value
                (VaTemplateValueAt<Index + I2, Vls...>::value) ...  // elements after Index
            >;
        };

        template <Size Index, auto Val, auto... Vls>
        using InsertHelper = typename SplitAndInsertImpl<
            Index,
            Val,
            std::make_index_sequence<Index>,
            std::make_index_sequence<sizeof...(Vls) - Index>,
            Vs...
        >::Type;

      public:
        static constexpr Size len = sizeof...(Vs);
        static constexpr Size size = sizeof...(Vs);

        template <auto V>
        static constexpr bool contains = ((Vs == V) || ...);

        template <Size Index>
        using TypeAt = typename VaTemplateTypeAt<Index, decltype(Vs)...>::Type;

        template <Size Index>
        static constexpr auto valueAt = VaTemplateValueAt<Index, Vs...>::value;

        static constexpr auto front = VaTemplateValueAt<len - 1, Vs...>::value;
        static constexpr auto back = VaTemplateValueAt<0, Vs...>::value;

        template <auto Val>
        using Append = VaTemplateTuple<Vs..., Val>;

        template <auto Val>
        using Prepend = VaTemplateTuple<Val, Vs...>;

        template <Size Index, auto Val>
        using Insert = InsertHelper<Index, Val, Vs...>;

        using AsTuple = VaTuple<typename VaValueType<Vs>::Type...>;
        using AsTemplateTypeTuple = VaTemplateTypeTuple<typename VaValueType<Vs>::Type...>;
    };
#endif

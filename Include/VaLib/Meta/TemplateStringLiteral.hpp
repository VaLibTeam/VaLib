// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <VaLib/Types/BasicTypedef.hpp>
#include <VaLib/Meta/BasicDefine.hpp>
#include <VaLib/Meta/TemplateAt.hpp>

#include <VaLib/Types/Array.hpp>

#if __cplusplus >= CPP17
    template <char... Cs>
    class VaTemplateStringLiteral {
      public:
        static constexpr Size len  = sizeof...(Cs);
        static constexpr Size size = sizeof...(Cs);

        template <Size Index>
        static constexpr auto charAt = VaTemplateValueAt<Index, Cs...>::value;

        static constexpr VaArray<char, len> asArray() {
            return { Cs... };
        }
        static constexpr VaArray<char, len> asNullTerminatedArray() {
            return { Cs..., '\0' };
        }
    };
#endif

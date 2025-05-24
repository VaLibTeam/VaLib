// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam

#include <cstdio>
#include <iostream>
#include <lib/testing.hpp>

#include <VaLib/Meta/TemplateTuple.hpp>
#include <VaLib/Meta/TemplateTypeTuple.hpp>
#include <VaLib/Meta/TemplateStringLiteral.hpp>
#include <VaLib/Meta/ConstexprStrings.hpp>

class MyCls {
  public:
    constexpr MyCls() = default;
    constexpr ~MyCls() = default;

    static constexpr const char* getHello() {
        return "Hello";
    }
};

using Hello = VaTemplateStringLiteral<'H', 'e', 'l', 'l', 'o'>;

using TpTuple1 = VaTemplateTypeTuple<int, char, MyCls>;

constexpr const char* hello = TpTuple1::TypeAt<2>::getHello();
static_assert(va::constexprStrEq(hello, "Hello"), "VaTemplateTypeTuple::TypeAt failed");

using Integer = TpTuple1::TypeAt<0>;
static_assert(tt::IsSame<Integer, int>, "VaTemplateTypeTuple::TypeAt failed");

using ValTuple1 = VaTemplateTuple< 10, 20, Hello{}, 30, MyCls{} >;
static_assert(tt::IsSame<tt::RemoveCV<decltype(ValTuple1::valueAt<2>)>, Hello>, "fail");

using TpTuple2 = TpTuple1::Append<decltype(ValTuple1::valueAt<2>)>;
static_assert(tt::IsSame<tt::RemoveCV<TpTuple2::TypeAt<TpTuple2::len-1>>, Hello>, "Append");

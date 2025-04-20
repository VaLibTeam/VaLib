// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam

#include <lib/testing.hpp>

#include <VaLib/Utils/BasicDefine.hpp>

#include <VaLib/Types/Any.hpp>
#include <VaLib/Types/List.hpp>
#include <VaLib/Types/String.hpp>

#include <any>

bool testAny(testing::Test& t) {
    VaAny any = int(123);
    if (any.get<int>() != 123) {
        return t.fail("unexpected result");
    }

    any = VaString("Hello");
    if (any.get<VaString>() != "Hello") {
        return t.fail("unexpected result");
    }

    expect({
        any.get<float32>();
        return t.fail("expected an exception");
    })

        return t.success();
}

int main() { testing::run(testAny); }
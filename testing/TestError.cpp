// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam

#include <lib/testing.hpp>

#include <VaLib/Types/Error.hpp>
#include <VaLib/Types/String.hpp>

bool testError(testing::Test& t) {
    VaResult<int> r = new ValueError("Kaboom");

    expect({
        r.throwErr();
        return t.fail("expected exception");
    })
    expect({
        r.unwrap();
        return t.fail("expected exception");
    })

    r = 123;
    if (r.unwrap() != 123) {
        return t.fail("unexpected result");
    }

    expect({
        r.unwrapErr();
        r.throwErr();

        return t.fail("expected exception");
    })

    r = new IndexError("Kaboom again");
    try {
        r.throwErr();

        return t.fail("expected exception");
    } catch (IndexError& err) {
    }

    return t.success();
}

int main() { return testing::run(testError); }

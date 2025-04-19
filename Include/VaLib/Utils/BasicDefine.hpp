// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <iostream>

#include <VaLib/Types/BasicConcepts.hpp>
#include <VaLib/Types/BasicTypedef.hpp>

#define CPP98 199711L
#define CPP03 199711L
#define CPP11 201103L
#define CPP14 201402L
#define CPP17 201703L
#define CPP20 202002L
#define CPP23 202302L

#define expect(code)                                                                                    \
    try {                                                                                               \
        code                                                                                            \
    } catch (...) {                                                                                     \
    }

#define stringify(expr) #expr
#define dbg(expr) VaString(stringify(expr)) + ": " + std::to_string(expr)

#define TODO(username, info)
TODO(test, "test!");
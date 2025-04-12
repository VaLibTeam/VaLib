// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <iostream>

#include <Types/BasicConcepts.hpp>
#include <Types/BasicTypedef.hpp>

#define cpp_98 199711L
#define cpp_03 199711L
#define cpp_11 201103L
#define cpp_14 201402L
#define cpp_17 201703L
#define cpp_20 202002L
#define cpp_23 202302L

#define stringify(expr) #expr
#define dbg(expr) VaString(stringify(expr)) + ": " + std::to_string(expr)

#define TODO(username, info)
TODO(test, "test!");
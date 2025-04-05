// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam

#include <lib/benchmark.hpp>

#include <VaLib.hpp>
#include <Types/ImmutableString.hpp>

Time benchmarkImmutableString(benchmarking::Benchmark& b) {
    b.start();

    VaImmutableString str;

    for (int i = 0; i < 10000000; i++) {
        str += "Hello, world!";
    }
    benchmarking::escape(str);

    return b.done();
}

int main() {
    benchmarking::run(benchmarkImmutableString, 30);
}
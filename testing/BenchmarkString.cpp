// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam

#include <lib/benchmark.hpp>

#include <VaLib.hpp>
#include <Types/String.hpp>

Time benchmarkString(benchmarking::Benchmark& b) {
    b.start();

    std::string str;
    str.reserve(10000000 * (13 * sizeof(char)));

    for (int i = 0; i < 10000000; i++) {
        str.append("Hello, world!", 13);
    }
    benchmarking::escape(str);

    return b.done();
}

int main() {
    benchmarking::run(benchmarkString, 30);
}
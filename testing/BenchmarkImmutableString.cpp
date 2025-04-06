// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam

#include <lib/benchmark.hpp>

#include <Types/ImmutableString.hpp>
#include <VaLib.hpp>

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
    std::cout << "sizeof(std::string): " << sizeof(std::string) << ", sizeof(VaString): " << sizeof(VaString) << "\n"; 
    // return benchmarking::run(benchmarkImmutableString, 30);
    std::cout << "BenchmarkImmutableString: skipping. ImmutableString is not designed for "
                 "demanding operations, benchmarking it doesn't make sense.";
    return 0;
}
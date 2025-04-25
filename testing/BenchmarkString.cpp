// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam

#define VaLib_USE_CONCEPTS
#include <lib/benchmarking.hpp>

#include <VaLib.hpp>
#include <VaLib/Types/String.hpp>

constexpr Size appendBenchmarkLimit = 1000000;

template <typename T>
Time benchmarkStringAppend(benchmarking::Benchmark& b) {
    b.start();

    T str;
    str.reserve(appendBenchmarkLimit * (13 * sizeof(char)));

    for (Size i = 0; i < appendBenchmarkLimit; i++) {
        str.append("Hello, world!", 13);
    }
    benchmarking::escape(str);

    return b.done();
}

Time benchmarkRawStringAppend(benchmarking::Benchmark& b) {
    b.start();

    char* raw = static_cast<char*>(std::malloc(appendBenchmarkLimit * (13 * sizeof(char))));
    Size len = 0;
    for (Size i = 0; i < appendBenchmarkLimit; ++i) {
        std::memcpy(raw + len, "Hello, world!", 13);
        len += 13;
    }
    benchmarking::escape(raw);

    return b.done();
}

Time benchmarkVaStringModify(benchmarking::Benchmark& b) {
    b.start();

    VaString str(100000, ' ');
    for (Size i = 0; i < cap(str); i++) {
        str[i] = i > 80000 ? 'c' : i > 40000 ? 'b' : i > 10000 ? 'a' : 'd';
    }

    return b.done();
}

Time benchmarkStdStringModify(benchmarking::Benchmark& b) {
    b.start();

    std::string str(100000, ' ');
    for (Size i = 0; i < str.length(); i++) {
        str[i] = i > 80000 ? 'c' : i > 40000 ? 'b' : i > 10000 ? 'a' : 'd';
    }

    return b.done();
}

int main() {
    auto bg = benchmarking::BenchmarkGroup("String append benchmark", 100);

    bg.add("VaString", benchmarkStringAppend<VaString>);
    bg.add("std::string", benchmarkStringAppend<std::string>);
    bg.add("Raw string (char*)", benchmarkRawStringAppend);
    bg.run();

    bg = benchmarking::BenchmarkGroup("String modify benchmark", 100);

    bg.add("VaString", benchmarkVaStringModify);
    bg.add("std::string", benchmarkStdStringModify);
    return bg.run();
}

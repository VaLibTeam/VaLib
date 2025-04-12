// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam

#include <lib/benchmark.hpp>

#include <VaLib/Types/List.hpp>
#include <VaLib/Types/String.hpp>

#include <vector>

Time benchmarkVaList(benchmarking::Benchmark& b) {
    b.start();

    VaList<int> list;
    for (int i = 0; i < 10'000'000; i++) {
        list.append(123);
    }

    // std::cout << "len(list): " << len(list) << ", cap(list): " << cap(list) << "\n";
    // std::cout << "list[4000]: " << list[4000] << ", list[100000]: " << list[100000] << ", list[9'000'000]: " << list[9'100'100] << "\n";

    return b.done();
}

Time benchmarkStdVector(benchmarking::Benchmark& b) {
    b.start();

    std::vector<int> list;
    for (int i = 0; i < 10'000'000; i++) {
        list.push_back(123);
    }

    // std::cout << "list.size(): " << list.size() << ", list.capacity(): " << list.capacity() << "\n";
    // std::cout << "list[4000]: " << list[4000] << ", list[100000]: " << list[100000] << ", list[9'000'000]: " << list[9'100'100] << "\n";

    return b.done();
}

Time benchmarkVaList2(benchmarking::Benchmark& b) {
    b.start();

    VaList<std::string> list("Hello", ", ", "world", "!");
    for (int i = 0; i < 1'000'000; i++) {
        list.append("Goodbye, mars?");
    }

    return b.done();
}

Time benchmarkStdVector2(benchmarking::Benchmark& b) {
    b.start();

    std::vector<std::string> list = {"Hello", ", ", "world", "!"};
    for (int i = 0; i < 1'000'000; i++) {
        list.push_back("Goodbye, mars?");
    }

    return b.done();
}

int main() {
    auto bg = benchmarking::BenchmarkGroup("List append test", 30);

    bg.add("VaList", benchmarkVaList);
    bg.add("std::vector", benchmarkStdVector);
    bg.run();

    bg = benchmarking::BenchmarkGroup("List append test (on class)", 30);

    bg.add("std::vector", benchmarkStdVector2);
    bg.add("VaList", benchmarkVaList2);
    bg.run();
}
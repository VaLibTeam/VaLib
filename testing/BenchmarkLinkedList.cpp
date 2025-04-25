// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam

#include <VaLib/Types/LinkedList.hpp>
#include <VaLib/Utils.hpp>

#include <cstdarg>
#include <list>

#include <lib/benchmarking.hpp>

Time benchmarkVaLinkedList(benchmarking::Benchmark& b) {
    b.start();

    VaLinkedList<int> list;
    for (int i = 0; i < 1'000'000; i++) {
        list.append(i);
    }
    for (int i = 0; i < 1'000'000; i++) {
        list.prepend(i);
    }
    for (int i = 0; i < 10'000; i++) {
        list.insert(i, i);
    }

    return b.done();
}

Time benchmarkStdList(benchmarking::Benchmark& b) {
    b.start();

    std::list<int> list;
    for (int i = 0; i < 1'000'000; i++) {
        list.push_back(i);
    }
    for (int i = 0; i < 1'000'000; i++) {
        list.push_front(i);
    }
    for (int i = 0; i < 10'000; i++) {
        auto it = list.begin();
        std::advance(it, i);
        list.insert(it, i);
    }

    return b.done();
}

Time benchmarkReuseVaLinkedList(benchmarking::Benchmark& b) {
    b.start();

    VaLinkedList<int> list;
    for (int i = 0; i < 1'000'000; ++i) {
        list.append(i);
    }
    for (int i = 0; i < 500'000; ++i) {
        list.pop();
    }
    for (int i = 0; i < 500'000; ++i) {
        list.shift();
    }
    for (int i = 0; i < 1'000'000; ++i) {
        list.prepend(i);
    }

    return b.done();
}

Time benchmarkReuseStdList(benchmarking::Benchmark& b) {
    b.start();

    std::list<int> list;
    for (int i = 0; i < 1'000'000; ++i) {
        list.push_back(i);
    }
    for (int i = 0; i < 500'000; ++i) {
        list.pop_back();
    }
    for (int i = 0; i < 500'000; ++i) {
        list.pop_front();
    }
    for (int i = 0; i < 1'000'000; ++i) {
        list.push_front(i);
    }

    return b.done();
}


int main() {
    auto bg = benchmarking::BenchmarkGroup("Linked list append-prepend-insert benchmark", 50);

    bg.add("VaLinkedList", benchmarkVaLinkedList);
    bg.add("std::list", benchmarkStdList);
    bg.run();

    bg = benchmarking::BenchmarkGroup("Linked list pop-shift and reuse benchmark", 70);

    bg.add("VaLinkedList", benchmarkReuseVaLinkedList);
    bg.add("std::list", benchmarkReuseStdList);
    bg.run();
}

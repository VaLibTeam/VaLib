// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <iostream>

#include <VaLib.hpp>
#include <Types/String.hpp>

#include <chrono>

namespace benchmarking {

class Benchmark {
  protected:
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    VaString msg;

    friend int run(Function<Time, Benchmark&> func, int repeat);

  public:
    void start() {
        startTime = std::chrono::high_resolution_clock::now();
    }

    Time done() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        return duration.count();
    }

    inline int fail(const VaString m) {
        msg = m;
        return -1;
    }
};

int run(Function<Time, Benchmark&> func, int repeat = 1) {
    Benchmark b;
    Time total = 0;
    for (int i = 0; i < repeat; i++) {
        Time result = func(b);
        if (result < 0) {
            std::cerr << "\033[31;1m" << "[ FAIL ]: " << "\033[0m" << b.msg << "\n";
            return 1;
        }
        total += result;
    }

    Time avg = total / repeat;
    std::cout << "\033[32;1m" << "[ SUCCESS ]: " << "\033[0m"
              << "benchmark completed successfully. Avg time: "
              << avg << " µs over " << repeat << " runs. " << b.msg << "\n";
    return 0;
}

template <typename T>
inline void escape(T&& value) {
    asm volatile("" : : "g"(value) : "memory");
}

}
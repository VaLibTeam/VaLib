// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <iostream>

#include <VaLib/Types/String.hpp>
#include <VaLib.hpp>

#include <chrono>

namespace benchmarking {

class Benchmark {
  protected:
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;

  public:
    VaString msg;
    void start() { startTime = std::chrono::high_resolution_clock::now(); }

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
              << "benchmark completed successfully. Avg time: " << avg << " µs over " << repeat
              << " runs. " << b.msg << "\n";
    return 0;
}

class BenchmarkGroup {
    struct Entry {
        VaString name;
        Function<Time, Benchmark&> func;
        Time result;
    };

    VaString groupName;
    int repeatCount;
    std::vector<Entry> entries;

  public:
    BenchmarkGroup(const VaString& name, int repeat = 1) : groupName(name), repeatCount(repeat) {}

    void add(const VaString& name, Function<Time, Benchmark&> f) {
        entries.push_back({name, f, 0});
    }

    int run() {
        std::cout << "\033[36;1m[ BENCHMARK GROUP ]:\033[0m " << groupName << " (" << repeatCount
                  << "x each)\n";

        for (auto& entry : entries) {
            Benchmark b;
            Time total = 0;
            for (int i = 0; i < repeatCount; ++i) {
                Time t = entry.func(b);
                if (t < 0) {
                    std::cerr << "\033[31;1m[ FAIL ]:\033[0m " << entry.name << ": " << b.msg
                              << "\n";
                    entry.result = -1;
                    break;
                }
                total += t;
            }
            if (entry.result != -1) entry.result = total / repeatCount;
        }

        std::sort(entries.begin(), entries.end(),
                  [](const Entry& a, const Entry& b) { return a.result < b.result; });

        showResults();
        return 0;
    }
    void showResults() const {
        std::cout << "\n\033[34;1m[ RESULTS ]:\033[0m\n";

        for (Size i = 0; i < entries.size(); ++i) {
            const auto& current = entries[i];
            if (current.result < 0) continue;

            std::string color;
            if (i == 0) {
                color = "\033[32m";
            } else if (i == entries.size() - 1) {
                color = "\033[31m";
            } else {
                // Gradient from yellow to orange
                int gradient = 33 + (i * 2);
                color = "\033[" + std::to_string(gradient) + "m";
            }

            std::cout << "  \033[1m" << color << current.name << "\033[0m - " << current.result
                      << " µs";

            if (i == 0) {
                std::cout << " (the fastest";
                for (Size j = 1; j < entries.size(); ++j) {
                    const auto& slower = entries[j];
                    if (slower.result < 0) continue;
                    float timesFaster = (float)slower.result / (float)current.result;
                    std::cout << ", faster than " << slower.name << " " << std::fixed
                              << std::setprecision(2) << timesFaster << "x";
                }
                std::cout << ")\n";
            } else {
                const auto& fastest = entries[0];
                float timesSlower = (float)current.result / (float)fastest.result;
                std::cout << " (slower than " << fastest.name << " " << std::fixed
                          << std::setprecision(2) << timesSlower << "x";

                bool found = false;
                for (Size j = i + 1; j < entries.size(); ++j) {
                    const auto& evenSlower = entries[j];
                    if (evenSlower.result < 0) continue;
                    float timesFaster = (float)evenSlower.result / (float)current.result;
                    std::cout << ", faster than " << evenSlower.name << " " << std::fixed
                              << std::setprecision(2) << timesFaster << "x";
                    found = true;
                }
                std::cout << ")\n";
            }
        }
    }
};

template <typename T>
inline void escape(T&& value) {
    asm volatile("" : : "g"(value) : "memory");
}

} // namespace benchmarking
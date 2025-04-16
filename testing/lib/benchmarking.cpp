// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam

#include <lib/benchmarking.hpp>

#include <iostream>

#include <VaLib/Types.hpp>
#include <VaLib/Utils.hpp>

#include <chrono>

namespace benchmarking {

Time Benchmark::done() {
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    return duration.count();
}

int run(Function<Time, Benchmark&> func, int repeat) {
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

int BenchmarkGroup::run() {
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

void BenchmarkGroup::showResults() const {
    std::cout << "\n\033[34;1m[ RESULTS ]:\033[0m\n";

    for (Size i = 0; i < len(entries); i++) {
        const auto& current = entries[i];
        if (current.result < 0) continue;

        VaString color;
        if (i == 0) {
            color = "\033[32m";
        } else if (i == len(entries) - 1) {
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
            for (Size j = 1; j < len(entries); j++) {
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
            for (Size j = i + 1; j < len(entries); j++) {
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

} // namespace benchmarking
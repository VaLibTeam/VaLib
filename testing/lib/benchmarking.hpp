// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <VaLib/AutoEnable.hpp>

#include <VaLib/FuncTools/Func.hpp>
#include <VaLib/Types/String.hpp>
#include <VaLib/Types/List.hpp>

#include <chrono>

namespace benchmarking {

class Benchmark {
  protected:
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;

  public:
    VaString msg;
    inline void start() { startTime = std::chrono::high_resolution_clock::now(); }

    Time done();

    inline int fail(const VaString m) {
        msg = m;
        return 1;
    }
};

int run(VaFunc<Time(Benchmark&)> func, int repeat = 1);

class BenchmarkGroup {
    struct Entry {
        VaString name;
        VaFunc<Time(Benchmark&)> func;
        Time result;
    };

    VaString groupName;
    int repeatCount;
    VaList<Entry> entries;

  public:
    inline BenchmarkGroup(const VaString& name, int repeat = 1)
        : groupName(name), repeatCount(repeat) {}

    inline void add(const VaString& name, VaFunc<Time(Benchmark&)> f) {
        entries.append({name, f, 0});
    }

    int run();
    void showResults() const;

    void exportToMarkdown(const VaString& filename) const;
};

template <typename T>
inline void escape(T&& value) {
    asm volatile("" : : "g"(value) : "memory");
}

constexpr int TODO_EXIT = 127;
inline int todo() {
    return TODO_EXIT;
}

} // namespace benchmarking

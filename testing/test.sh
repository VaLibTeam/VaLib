#!/bin/bash
# VaLib - Vast Library
# Licensed under GNU GPL v3 License. See LICENSE file.
# (C) 2025 VaLibTeam

cd "$(dirname "$0")" || exit 1
source "../scripts/utils.sh" || exit 1

CXX="${CXX:-g++}"
CXXFLAGS="${CXXFLAGS:-"-std=c++20 -O3 -fPIC"}"

includePath=(
    "../Include"
    "."
)

SuccessExit=0
CompilationErrorExit=1
LinkingErrorExit=2
ErrorExit=3

OUTDIR="build"
if [[ "$OUTDIR" = "" ]]; then
    OUTDIR="build"
fi

mkdir -p "$OUTDIR"

Test() {
    testFile="$1"
    obj="$OUTDIR/$testFile.o"
    out="$OUTDIR/$testFile"

    echo -e "\033[34;1m" "Testing $testFile..." "\033[0m"

    "$CXX" $CXXFLAGS "$testFile.cpp" "${includePath[@]/#/-I}" -c -o "$obj" || ShowError $CompilationErrorExit "The \"$testFile\" test failed to compile."
    "$CXX" $CXXFLAGS libva.o "$obj" -o "$out" || ShowError $LinkingErrorExit "The \"$testFile\" test failed to link to executable."

    "$out"
    exitCode="$?"

    if [[ $exitCode -ne 0 ]]; then
        echo -e "\033[31;1m" "$testFile: fail." "\033[0m\033[31mExit code: $exitCode" "\033[0m"
        return 1
    else
        echo -e  "\033[32;1m" "$testFile: pass." "\033[0m"
        return 0
    fi
}

Benchmark() {
    benchFile="$1"
    obj="$OUTDIR/$benchFile.o"
    out="$OUTDIR/$benchFile"

    echo -e "\033[35;1m" "Benchmarking $benchFile..." "\033[0m"

    "$CXX" $CXXFLAGS "$benchFile.cpp" "${includePath[@]/#/-I}" -c -o "$obj" || ShowError $CompilationErrorExit "Benchmark \"$benchFile\" failed to compile."
    "$CXX" $CXXFLAGS libva.o "$obj" -o "$out" || ShowError $LinkingErrorExit "Benchmark \"$benchFile\" failed to link."

    ShowInfo "start"
    "$out"
    exitCode="$?"

    if [[ $exitCode -ne 0 ]]; then
        echo -e "\033[31;1m" "$benchFile: fail." "\033[0m\033[31mExit code: $exitCode" "\033[0m"
        return 1
    else
        echo -e  "\033[32;1m" "$benchFile: success." "\033[0m"
        return 0
    fi
}

tests=()
benchmarks=()

for arg in "$@"; do
    case "$arg" in
    -Test*)
        tests+=("${arg##-}");;
    -Benchmark*)
        benchmarks+=("${arg##-}");;
    esac
done

totalTests="${#tests[@]}"
totalBenchmarks="${#benchmarks[@]}"

if [[ $totalTests -le 0 && $totalBenchmarks -le 0 ]]; then

    allTests=()
    while IFS= read -r file; do
        allTests+=("$(basename "$file" .cpp)")
    done < <(find . -iname "Test*.cpp")

    allBenchmarks=()
    while IFS= read -r file; do
        allBenchmarks+=("$(basename "$file" .cpp)")
    done < <(find . -iname "Benchmark*.cpp")

    tests=("${allTests[@]}")
    benchmarks=("${allBenchmarks[@]}")
    totalTests="${#tests[@]}"
    totalBenchmarks="${#benchmarks[@]}"
fi

if [[ $totalTests -gt 0 ]]; then
    passed=0
    failed=0

    ShowInfo "Running tests..."
    for test in "${tests[@]}"; do
        if Test "$test"; then
            ((passed++))
        else
            ((failed++))
        fi
    done

    if [[ $passed -eq $totalTests ]]; then
        ShowSuccess "All tests have passed!"
    elif [[ $failed -eq $totalTests ]]; then
        ShowWarn "All tests failed"
    else
        ShowWarn "Some tests failed (failed $failed/$totalTests)"
    fi

    echo
fi

if [[ $totalBenchmarks -gt 0 ]]; then
    passed=0
    failed=0

    ShowInfo "Running benchmarks..."
    for bench in "${benchmarks[@]}"; do
        if Benchmark "$bench"; then
            ((passed++))
        else
            ((failed++))
        fi
    done

    if [[ $passed -eq $totalBenchmarks ]]; then
        ShowSuccess "all benchmarks have been run successfully!"
    elif [[ $failed -eq $totalBenchmarks ]]; then
        ShowWarn "All benchmarks failed"
    else
        ShowWarn "Some benchmarks failed (failed $failed/$totalBenchmarks)"
    fi
fi

exit $SuccessExit
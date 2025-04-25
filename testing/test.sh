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
EnvironmentErrorExit=3
ErrorExit=4

OUTDIR="build"
if [[ -z "$OUTDIR" ]]; then
    OUTDIR="build"
fi

buildMode="-L./"

mkdir -p "$OUTDIR"
verbose=false

tests=()
benchmarks=()

Clean() {
    if [[ -d "$OUTDIR" ]]; then
        echo "Cleaning directory: $OUTDIR"
        rm -rf "$OUTDIR"
    fi

    mkdir -p "$OUTDIR"
    return $SuccessExit
}

Help() {
    echo "Usage: script [options] [targets]"
    echo
    echo "Options:"
    echo "  --echo, --verbose, -v       Show verbose output"
    echo "  --clean                     Clean build directory"
    echo "  --cxx=<compiler>            Set C++ compiler path"
    echo "  --cxxflags=<flags>          Set C++ compilation flags"
    echo "  --outdir=<directory>        Set output directory"
    echo "  --help                      Show this help message"
    echo
    echo "Targets:"
    echo "  -Test{NAME}                 Run specific test"
    echo "  -Benchmark{NAME}            Run specific benchmark"
    echo "  -TestAll                    Run all available tests"
    echo "  -BenchmarkAll               Run all available benchmarks"
    echo
    echo "If no targets specified, all tests and benchmarks will be run."

    return $SuccessExit
}

ParseFlags() {
    for arg in "$@"; do
        case "$arg" in
        -TestAll)
            tests=("ALL") ;;
        -BenchmarkAll)
            benchmarks=("ALL") ;;

        -Test*)
            tests+=("${arg##-}") ;;
        -Benchmark*)
            benchmarks+=("${arg##-}") ;;

        --echo | --verbose | -v)
            verbose=true ;;
        --clean)
            Clean
            exit $?
            ;;
        --help | -h)
            Help
            exit $?
            ;;

        --cxx=*)
            CXX="${arg#*=}" ;;
        --cxxflags=*)
            CXXFLAGS="${arg#*=}" ;;

        --outdir=*)
            OUTDIR="${arg#*=}" ;;
        esac
    done
}

RunTarget() {
    target="$1"
    if [[ $verbose = true ]]; then
        "$target"
    else
        "$target" > /dev/null
    fi
    return $?
}

CheckEnvironment() {
    if ! command -v "$CXX" >/dev/null 2>&1; then
        ShowError $EnvironmentErrorExit "The C++ compiler needed to compile the tests was not found. try specifying the correct path in the --cxx=<compiler> flag or in the CXX environment variable"
    fi
}

CheckLibraries() {
    if command -v "ldconfig" > /dev/null 2>&1; then
        if ldconfig -p | grep -q "libvalib.so"; then
            buildMode="-lvalib"
            return
        fi
    fi

    if [[ -r "./libvalib.so" || -r "build/libvalib.so" ]]; then
        buildMode="-L./ -L./build/ -lvalib"
        export LD_LIBRARY_PATH="./build:$LD_LIBRARY_PATH"
    elif [[ -r "./libvalib.a" || -r "build/libvalib.a" ]]; then
        buildMode="-L./ -L./build/ -lvalib"
    elif [[ -r "./libvalib.o" ]]; then
        buildMode="./libvalib.o"
    elif [[ -r "build/libvalib.o" ]]; then
        buildMode="build/libvalib.o"
    else
        ../build.sh --target=shared --output="libvalib" || ShowError $ErrorExit "failed to compile valib."
        mv ../libvalib.so build/libvalib.so
        buildMode="-L./ -L./build/ -lvalib"
        export LD_LIBRARY_PATH="./build:$LD_LIBRARY_PATH"
    fi
}

CompileTest() {
    local testFile="$1"
    local obj="$OUTDIR/$testFile.o"
    local out="$OUTDIR/$testFile"

    # Check if object file needs to be recompiled
    if [[ ! -f "$obj" ]] || [[ "$testFile.cpp" -nt "$obj" ]]; then
        "$CXX" $CXXFLAGS "$testFile.cpp" "${includePath[@]/#/-I}" -c -o "$obj" || ShowError $CompilationErrorExit "Test \"$testFile\" failed to compile."
    fi

    if [[ ! -f "$out" ]] || [[ "$obj" -nt "$out" ]] || [[ "build/testing.o" -nt "$out" ]]; then
        "$CXX" $CXXFLAGS $buildMode "$obj" "build/testing.o" -o "$out" || ShowError $LinkingErrorExit "Test \"$testFile\" failed to link."
    fi
}

Test() {
    local testFile="$1"

    local obj="$OUTDIR/$testFile.o"
    local out="$OUTDIR/$testFile"

    echo -e "\033[34;1m" "Testing $testFile..." "\033[0m"
    CompileTest "$testFile"

    RunTarget "$out"

    exitCode="$?"
    if [[ $exitCode -eq 127 ]]; then
        echo -e "\033[31;1m" "$testFile: TODO." "\033[0m\033[30m TODO: Implement test $testFile: $testFile.cpp" "\033[0m"
        return 1
    elif [[ $exitCode -ne 0 ]]; then
        echo -e "\033[31;1m" "$testFile: fail." "\033[0m\033[31mExit code: $exitCode" "\033[0m"
        return 1
    else
        echo -e  "\033[32;1m" "$testFile: pass." "\033[0m"
        return 0
    fi
}

CompileBenchmark() {
    local benchFile="$1"
    local obj="$OUTDIR/$benchFile.o"
    local out="$OUTDIR/$benchFile"

    # Check if object file needs to be recompiled
    if [[ ! -f "$obj" ]] || [[ "$benchFile.cpp" -nt "$obj" ]]; then
        "$CXX" $CXXFLAGS "$benchFile.cpp" "${includePath[@]/#/-I}" -c -o "$obj" || ShowError $CompilationErrorExit "Benchmark \"$benchFile\" failed to compile."
    fi

    if [[ ! -f "$out" ]] || [[ "$obj" -nt "$out" ]] || [[ "build/benchmarking.o" -nt "$out" ]]; then
        "$CXX" $CXXFLAGS $buildMode "$obj" "build/benchmarking.o" -o "$out" || ShowError $LinkingErrorExit "Benchmark \"$benchFile\" failed to link."
    fi
}

Benchmark() {
    benchFile="$1"
    obj="$OUTDIR/$benchFile.o"
    out="$OUTDIR/$benchFile"

    echo -e "\033[35;1m" "Benchmarking $benchFile..." "\033[0m"
    CompileBenchmark "$benchFile"

    ShowInfo "start"
    RunTarget "$out"

    exitCode="$?"
    if [[ $exitCode -eq 127 ]]; then
        echo -e "\033[33;1m" "$benchFile: TODO." "\033[0m\033[33m TODO: Implement benchmark $benchFile: $benchFile.cpp" "\033[0m"
        return 1
    elif [[ $exitCode -ne 0 ]]; then
        echo -e "\033[31;1m" "$benchFile: fail." "\033[0m\033[31mExit code: $exitCode" "\033[0m"
        return 1
    else
        echo -e  "\033[32;1m" "$benchFile: success." "\033[0m"
        return 0
    fi
}

CompileLib() {
    if [[ ! -f "build/benchmarking.o" ]] || [[ "lib/benchmarking.cpp" -nt "build/benchmarking.o" ]]; then
        "$CXX" $CXXFLAGS "lib/benchmarking.cpp" "${includePath[@]/#/-I}" -c -o "build/benchmarking.o" || ShowError $CompilationErrorExit "Failed to compile benchmark.cpp."
    fi

    if [[ ! -f "build/testing.o" ]] || [[ "lib/testing.cpp" -nt "build/testing.o" ]]; then
        "$CXX" $CXXFLAGS "lib/testing.cpp" "${includePath[@]/#/-I}" -c -o "build/testing.o" || ShowError $CompilationErrorExit "Failed to compile testing.cpp."
    fi
}

Main() {
    ParseFlags "$@"

    CheckEnvironment
    CheckLibraries

    CompileLib

    totalTests="${#tests[@]}"
    totalBenchmarks="${#benchmarks[@]}"

    if [[ "${tests[*]}" == "ALL" || ($totalBenchmarks -le 0 && $totalTests -le 0) ]]; then
        tests=()
        while IFS= read -r file; do
            [[ "$(dirname "$file")" == "." ]] && tests+=("$(basename "$file" .cpp)")
        done < <(find . -maxdepth 1 -iname "Test*.cpp")
    fi

    if [[ "${benchmarks[*]}" == "ALL" || ($totalBenchmarks -le 0 && $totalTests -le 0) ]]; then
        benchmarks=()
        while IFS= read -r file; do
            [[ "$(dirname "$file")" == "." ]] && benchmarks+=("$(basename "$file" .cpp)")
        done < <(find . -maxdepth 1 -iname "Benchmark*.cpp")
    fi

    totalTests="${#tests[@]}"
    totalBenchmarks="${#benchmarks[@]}"

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

    return $SuccessExit
}

Main "$@"
exit $?

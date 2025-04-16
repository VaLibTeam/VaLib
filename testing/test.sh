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
if [[ "$OUTDIR" = "" ]]; then
    OUTDIR="build"
fi

buildMode="-L./"

mkdir -p "$OUTDIR"
verbose=false

tests=()
benchmarks=()

Clean() {
    [[ -n "$OUTDIR" ]] && exit $SuccessExit

    rm "$OUTDIR/*"
}

for arg in "$@"; do
    case "$arg" in
    -Test*)
        tests+=("${arg##-}") ;;
    -Benchmark*)
        benchmarks+=("${arg##-}") ;;
    
    --echo | --verbose | -v)
        verbose=true ;;
    --clean)
        Clean ;;

    --cxx=*)
	    CXX="${arg#*=}" ;;
	--cxxflags=*)
	    CXXFLAGS="${arg#*=}" ;;

    --outdir=*)
        OUTDIR="${arg#*=}" ;;
    esac
done

totalTests="${#tests[@]}"
totalBenchmarks="${#benchmarks[@]}"

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

Test() {
    testFile="$1"
    obj="$OUTDIR/$testFile.o"
    out="$OUTDIR/$testFile"

    echo -e "\033[34;1m" "Testing $testFile..." "\033[0m"

    "$CXX" $CXXFLAGS "$testFile.cpp" "${includePath[@]/#/-I}" -c -o "$obj" > /dev/null || ShowError $CompilationErrorExit "The \"$testFile\" test failed to compile."
    "$CXX" $CXXFLAGS $buildMode "$obj" -o "$out" || ShowError $LinkingErrorExit "The \"$testFile\" test failed to link to executable."

    RunTarget "$out"
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
    "$CXX" $CXXFLAGS $buildMode "$obj" "build/benchmarking.o" -o "$out" || ShowError $LinkingErrorExit "Benchmark \"$benchFile\" failed to link."

    ShowInfo "start"

    RunTarget "$out"    
    exitCode="$?"

    if [[ $exitCode -ne 0 ]]; then
        echo -e "\033[31;1m" "$benchFile: fail." "\033[0m\033[31mExit code: $exitCode" "\033[0m"
        return 1
    else
        echo -e  "\033[32;1m" "$benchFile: success." "\033[0m"
        return 0
    fi
}

CompileLib() {
    "$CXX" $CXXFLAGS "lib/benchmarking.cpp" "${includePath[@]/#/-I}" -c -o "build/benchmarking.o" || ShowError $CompilationErrorExit "Failed to compile benchmark.cpp."
    # "$CXX" $CXXFLAGS "lib/testing.cpp" "${includePath[@]/#/-I}" -c -o "build/testing.o" || ShowError $CompilationErrorExit "Failed to compile testing.cpp."
}

CheckEnvironment
CheckLibraries

CompileLib

if [[ $totalTests -le 0 && $totalBenchmarks -le 0 ]]; then
    allTests=()
    while IFS= read -r file; do
        [[ "$(dirname "$file")" == "." ]] && allTests+=("$(basename "$file" .cpp)")
    done < <(find . -maxdepth 1 -iname "Test*.cpp")

    allBenchmarks=()
    while IFS= read -r file; do
        [[ "$(dirname "$file")" == "." ]] && allBenchmarks+=("$(basename "$file" .cpp)")
    done < <(find . -maxdepth 1 -iname "Benchmark*.cpp")

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

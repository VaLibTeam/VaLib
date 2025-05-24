#!/bin/bash
# VaLib - Vast Library
# Licensed under GNU GPL v3 License. See LICENSE file.
# (C) 2025 VaLibTeam

cd "$(dirname "$0")" || exit 1
source "../scripts/utils.sh" || exit 1
source "../scripts/get-cxx-flags.sh" || exit 1

CXX="${CXX:-g++}"

includePath=(
    "../Include"
    "."
)

SuccessExit=0
CompilationErrorExit=1
LinkingErrorExit=2
EnvironmentErrorExit=3
InvalidFlagExit=4
ErrorExit=5

LIBSDIR="${LIBSDIR:-libs}"
OUTDIR="${OUTDIR:-build}"

buildMode="-L./"

mkdir -p "$OUTDIR"
verbose=false

tests=()
benchmarks=()
metaTests=()

Clean() {
    if [[ -d "$OUTDIR" ]]; then
        ShowProgress "Cleaning directory: $OUTDIR"
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
    echo "  -MetaTest{NAME}              Run specific meta test (static_assert compile-time test)"
    echo
    echo "  -TestAll                    Run all available tests"
    echo "  -BenchmarkAll               Run all available benchmarks"
    echo "  -MetaTestAll                 Run all available meta tests"
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
        -MetaTestAll)
            metaTests+=("ALL") ;;

        -Test*)
            tests+=("${arg##-}") ;;
        -Benchmark*)
            benchmarks+=("${arg##-}") ;;
        -MetaTest*)
            metaTests+=("${arg##-}") ;;

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
            read -ra CXXFLAGS <<< "${arg#*=}" ;;

        --outdir=*)
            OUTDIR="${arg#*=}" ;;

        *)
            ShowError $InvalidFlagExit "Invalid flag: $arg"
            ;;
        esac
    done
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
        buildMode="-L./ -L./$LIBSDIR/ -lvalib"
        export LD_LIBRARY_PATH="./:$LIBSDIR:$LD_LIBRARY_PATH"
    elif [[ -r "./libvalib.a" || -r "$LIBSDIR/libvalib.a" ]]; then
        buildMode="-L./ -L./$LIBSDIR/ -lvalib"
    elif [[ -r "./libvalib.o" ]]; then
        buildMode="./libvalib.o"
    elif [[ -r "$LIBSDIR/libvalib.o" ]]; then
        buildMode="$LIBSDIR/libvalib.o"
    else
        ../build.sh --target=shared --output="libvalib" || ShowError $ErrorExit "failed to compile valib."
        mv ../libvalib.so build/libvalib.so
        buildMode="-L./ -L./$LIBSDIR/ -lvalib"
        export LD_LIBRARY_PATH="./:$LIBSDIR:$LD_LIBRARY_PATH"
    fi
}

RunTarget() {
    local target="$1"
    chmod +x "$target"

    if [[ $verbose = true ]]; then
        "$target"
    else
        "$target" > /dev/null
    fi
    return $?
}

NeedsCompile() {
    local target="$1"
    local dependency="$2"
    [[ ! -f "$target" ]] || [[ "$dependency" -nt "$target" ]]
    return $?
}

NameObj() {
    echo "$OUTDIR/$1.o"
}

NameOut() {
    echo "$OUTDIR/$1"
    # echo "$OUTDIR/$1.elf"
    # echo "$OUTDIR/$1.out"
}

CompileTest() {
    local testFile="$1"
    local obj="$(NameObj "$testFile")"
    local out="$(NameOut "$testFile")"

    # Check if object file needs to be recompiled
    if NeedsCompile "$obj" "$testFile.cpp"; then
        "$CXX" "${CXXFLAGS[@]}" "$testFile.cpp" "${includePath[@]/#/-I}" -c -o "$obj" || ShowError $CompilationErrorExit "Test \"$testFile\" failed to compile."
    fi

    if NeedsCompile "$out" "$obj" || [[ "build/testing.o" -nt "$out" ]]; then
        "$CXX" "${CXXFLAGS[@]}" $buildMode "$obj" "build/testing.o" -o "$out" || ShowError $LinkingErrorExit "Test \"$testFile\" failed to link."
    fi
}

Test() {
    local testFile="$1"
    local obj="$(NameObj "$testFile")"
    local out="$(NameOut "$testFile")"

    echo -e "\033[34;1m" "Testing $testFile..." "\033[0m"
    CompileTest "$testFile"

    RunTarget "$out"
    local exitCode="$?"
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
    local obj="$(NameObj "$benchFile")"
    local out="$(NameOut "$benchFile")"

    if NeedsCompile "$out" "$obj"; then
        "$CXX" "${CXXFLAGS[@]}" "$benchFile.cpp" "${includePath[@]/#/-I}" -c -o "$obj" || ShowError $CompilationErrorExit "Benchmark \"$benchFile\" failed to compile."
    fi

    if NeedsCompile "$out" "$obj" || [[ "build/benchmarking.o" -nt "$out" ]]; then
        "$CXX" "${CXXFLAGS[@]}" $buildMode "$obj" "build/benchmarking.o" -o "$out" || ShowError $LinkingErrorExit "Benchmark \"$benchFile\" failed to link."
    fi
}

Benchmark() {
    benchFile="$1"
    obj="$(NameObj "$benchFile")"
    out="$(NameOut "$benchFile")"

    echo -e "\033[35;1m" "Benchmarking $benchFile..." "\033[0m"
    CompileBenchmark "$benchFile"

    ShowInfo "start"

    RunTarget "$out"
    local exitCode="$?"
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

CompileMetaTest() {
    local metaFile="$1"
    local obj="$(NameObj "$metaFile")"

    "$CXX" "${CXXFLAGS[@]}" "$metaFile.cpp" "${includePath[@]/#/-I}" -c -o "$obj" 2> "$OUTDIR/${metaFile}-compile.log"
    return $?
}

MetaTest() {
    local metaFile="$1"
    echo -e "\033[36;1m" "MetaTesting $metaFile..." "\033[0m"

    CompileMetaTest "$metaFile"
    local exitCode="$?"
    if [[ $exitCode -eq 0 ]]; then
        rm -f "$out"
        rm -f "$OUTDIR/${metaFile}-compile.log"

        echo -e  "\033[36;1m" "$metaFile: pass." "\033[0m"
        return 0
    else
        echo -e "\033[31;1m" "$metaFile: fail." "\033[0m\033[31mCompile error. See $OUTDIR/${metaFile}-compile.log\033[0m"
        return 1
    fi
}

CompileLib() {
    if [[ ! -f "build/benchmarking.o" ]] || [[ "lib/benchmarking.cpp" -nt "build/benchmarking.o" ]]; then
        "$CXX" "${CXXFLAGS[@]}" "lib/benchmarking.cpp" "${includePath[@]/#/-I}" -c -o "build/benchmarking.o" || ShowError $CompilationErrorExit "Failed to compile benchmark.cpp."
    fi

    if [[ ! -f "build/testing.o" ]] || [[ "lib/testing.cpp" -nt "build/testing.o" ]]; then
        "$CXX" "${CXXFLAGS[@]}" "lib/testing.cpp" "${includePath[@]/#/-I}" -c -o "build/testing.o" || ShowError $CompilationErrorExit "Failed to compile testing.cpp."
    fi
}

PrintAll() {
    local targets=("$@")

    local obj=""
    local out=""

    if [[ $verbose = true ]]; then
        for tg in "${targets[@]}"; do
            obj="$(NameObj "$tg")"
            out="$(NameOut "$tg")"
            if NeedsCompile "$obj" "$tg.cpp" && NeedsCompile "$out" "$obj"; then
                echo -en "${tg} "
            elif NeedsCompile "$obj" "$tg.cpp"; then
                echo -en "${BOLD}${YELLOW}${tg}${RESET} "
            else
                echo -en "${BOLD}${tg}${RESET} "
            fi
        done
        echo
    fi
}

Main() {
    ParseFlags "$@"

    CheckEnvironment
    CheckLibraries

    CompileLib

    totalTests="${#tests[@]}"
    totalBenchmarks="${#benchmarks[@]}"
    totalMetaTests="${#metaTests[@]}"

    if [[ "${tests[*]}" == "ALL" || ($totalBenchmarks -le 0 && $totalTests -le 0) ]]; then
        tests=()
        while IFS= read -r file; do
            if [[ "$(dirname "$file")" == "." ]]; then
                tests+=("$(basename "$file" .cpp)")
            fi
        done < <(find . -maxdepth 1 -iname "Test*.cpp")
    fi

    if [[ "${benchmarks[*]}" == "ALL" || ($totalBenchmarks -le 0 && $totalTests -le 0) ]]; then
        benchmarks=()
        while IFS= read -r file; do
            if [[ "$(dirname "$file")" == "." ]]; then
                benchmarks+=("$(basename "$file" .cpp)")
            fi
        done < <(find . -maxdepth 1 -iname "Benchmark*.cpp")
    fi


    if [[ "${metaTests[*]}" == "ALL" || (${#metaTests[@]} -le 0 && $totalTests -le 0 && $totalBenchmarks -le 0) ]]; then
        metaTests=()
        while IFS= read -r file; do
            if [[ "$(dirname "$file")" == "." ]]; then
                metaTests+=("$(basename "$file" .cpp)")
            fi
        done < <(find . -maxdepth 1 -iname "MetaTest*.cpp")
    fi

    totalTests="${#tests[@]}"
    totalBenchmarks="${#benchmarks[@]}"
    totalMetaTests="${#metaTests[@]}"

    if [[ $totalTests -gt 0 ]]; then
        passed=0
        failed=0

        ShowInfo "Running tests..."
        PrintAll "${tests[@]}"

        for test in "${tests[@]}"; do
            if Test "$test"; then
                (( passed++ ))
            else
                (( failed++ ))
            fi
        done

        if [[ $passed -eq $totalTests ]]; then
            ShowSuccess "All tests have passed!"
        elif [[ $failed -eq $totalTests ]]; then
            ShowWarn "All tests failed"
        else
            ShowWarn "Some tests failed (failed $failed/$totalTests)"
        fi
    fi

    if [[ $totalMetaTests -gt 0 ]]; then
        passed=0
        failed=0

        [[ $totalTests -gt 0 ]] && echo

        ShowInfo "Running meta tests (static_assert compile-time)..."
        for meta in "${metaTests[@]}"; do
            if MetaTest "$meta"; then
                (( passed++ ))
            else
                (( failed++ ))
            fi
        done

        if [[ $passed -eq $totalMetaTests ]]; then
            ShowSuccess "All meta tests have passed!"
        elif [[ $failed -eq $totalMetaTests ]]; then
            ShowWarn "All meta tests failed"
        else
            ShowWarn "Some meta tests failed (failed $failed/$totalMetaTests)"
        fi
    fi

    if [[ $totalBenchmarks -gt 0 ]]; then
        passed=0
        failed=0

        [[ $totalMetaTests -gt 0 ]] && echo

        ShowInfo "Running benchmarks..."
        for bench in "${benchmarks[@]}"; do
            if Benchmark "$bench"; then
                (( passed++ ))
            else
                (( failed++ ))
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

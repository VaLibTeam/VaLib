#!/bin/bash
# VaLib - Vast Library
# Licensed under GNU GPL v3 License. See LICENSE file.
# (C) 2025 VaLibTeam

cd "$(dirname "$0")" || exit 1
source "scripts/utils.sh" || exit 1
set -e || exit 1

SRC="src"
BUILD="build"
OUTPUT="libva"

CXX="${CXX:-g++}"
CXXFLAGS="${CXXFLAGS:-"-Wall -Wextra -std=c++20 -O2 -fPIC"}"

AR="ar"
ARFLAGS=""

SuccessExit=0
CompilationErrorExit=1
LinkingErrorExit=2
ArgsErrorExit=3
ErrorExit=4

includePath=(
    "Include"
)

target=""
for arg in "$@"; do
    case $arg in
    --CXX=*)
	    CXX="${arg#*=}" ;;
	--CXXFLAGS=*)
	    CXXFLAGS="${arg#*=}" ;;
        
    --AR=*)
        AR="${arg#*=}" ;;
    --ARFLAGS=*)
            ARFLAGS="${arg#*=}" ;;

    --BUILD-DIR=*)
        BUILD="${arg#*=}" ;;
    --OUTPUT=*)
        OUTPUT="${arg#*=}" ;;
    
    --target=*)
        target=$(echo "$arg" | sed 's/--target=//') ;;
    *)
        ShowError $ArgsErrorExit "Invalid argument: $arg" ;;
    esac
done

if [ -z "$target" ]; then
    ShowError $ErrorExit "Usage: $0 --target={static,shared,object}"
fi

mkdir -p "$BUILD"

cppFiles=( $(find "$SRC" -name "*.cpp") )
objFiles=()
toCompile=()

for file in "${cppFiles[@]}"; do
    obj="$BUILD/$(basename "$file" .cpp).o"
    objFiles+=("$obj")

    if [ ! -f "$obj" ] || [ "$file" -nt "$obj" ]; then
        toCompile+=("$file")
    fi
done

total=${#toCompile[@]}
compiled=0

for file in "${toCompile[@]}"; do
    obj="$BUILD/$(basename "$file" .cpp).o"
    $CXX $CXXFLAGS "${includePath[@]/#/-I}" -c "$file" -o "$obj" || ShowError $CompilationErrorExit "Failed to compile $file"
    percent=$(((compiled + 1) * 100 / total))
    ShowInfo "Progress: $percent% ($compiled/$total)"
done

case $target in
    static)
        ShowInfo "Creating static library (.a)..."
        $AR rcs "$BUILD/$OUTPUT.a" "${objFiles[@]}" || ShowError $LinkingErrorExit "Failed to link."
        ;;
    shared)
        ShowInfo "Creating shared library (.so)..."
        $CXX -shared -o "$BUILD/$OUTPUT.so" "${objFiles[@]}" || ShowError $LinkingErrorExit "Failed to link."
        ;;
    object)
        ShowInfo "Merging object files into one .o..."
        ld -r "${objFiles[@]}" -o "$BUILD/$OUTPUT.o" || ShowError $LinkingErrorExit "Failed to link."
        ;;
    *)
        ShowError $ArgsErrorExit "Not correct target: $target"
        ;;
esac

ShowSuccess "Done!"
exit $SuccessExit
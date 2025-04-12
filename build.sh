#!/bin/bash
# VaLib - Vast Library
# Licensed under GNU GPL v3 License. See LICENSE file.
# (C) 2025 VaLibTeam

cd "$(dirname "$0")" || exit 1
source "scripts/utils.sh" || exit 1
set -e || exit 1
shopt -s nullglob || exit 1

SRC="src"
BUILD="build"
OUTPUT="libvalib"
OUTDIR="out"

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

shouldInstall=false
target=""
for arg in "$@"; do
    case $arg in
    --cxx=*)
	    CXX="${arg#*=}" ;;
	--cxxflags=*)
	    CXXFLAGS="${arg#*=}" ;;
        
    --ar=*)
        AR="${arg#*=}" ;;
    --arflags=*)
            ARFLAGS="${arg#*=}" ;;

    --build-dir=*)
        BUILD="${arg#*=}" ;;
    --output=*)
        OUTPUT="${arg#*=}" ;;
    --out-dir=*)
        OUTDIR="${arg#*=}" ;;
    
    --target=*)
        target=$(echo "$arg" | sed 's/--target=//') ;;

    --install)
        shouldInstall=true ;;

    *)
        ShowError $ArgsErrorExit "Invalid argument: $arg" ;;
    esac
done

mkdir -p "$OUTDIR"

if [ -z "$target" ]; then
    ShowError $ErrorExit "Usage: $0 --target={static,shared,object,all}"
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

BuildStatic() {
    ShowInfo "Creating static library (.a)..."
    $AR rcs "$OUTDIR/$OUTPUT.a" "${objFiles[@]}" || ShowError $LinkingErrorExit "Failed to link."
}

BuildShared() {
    ShowInfo "Creating shared library (.so)..."
    $CXX -shared -o "$OUTDIR/$OUTPUT.so" "${objFiles[@]}" || ShowError $LinkingErrorExit "Failed to link."
}

BuildObject() {
    ShowInfo "Merging object files into one .o..."
    ld -r "${objFiles[@]}" -o "$OUTDIR/$OUTPUT.o" || ShowError $LinkingErrorExit "Failed to link."
}

InstallStatic() {
    ShowInfo "Installing static library..."
    install -d "/usr/local/lib"
    install -m 644 "$OUTDIR/$OUTPUT.a" "/usr/local/lib/" || ShowError $ErrorExit "Failed to install static library."
}

InstallShared() {
    ShowInfo "Installing shared library..."
    install -d "/usr/local/lib"
    install -m 755 "$OUTDIR/$OUTPUT.so" "/usr/local/lib/" || ShowError $ErrorExit "Failed to install shared library."
}

InstallHeaders() {
    SysIncludeDir="/usr/local/include"

    ShowInfo "Installing headers..."
    for dir in "${includePath[@]}"; do
        if [[ -d "$dir" ]]; then
            find "$dir" -type d | while read -r subdir; do
                if [[ "$subdir" == "$dir" ]]; then
                    relPath=""
                else
                    relPath="${subdir#"$dir"/}"
                fi

                targetDir="$SysIncludeDir/$relPath"

                install -d "$targetDir"

                headers=("$subdir"/*.{hpp,tpp,h})
                if (( ${#headers[@]} )); then
                    install -m 644 "${headers[@]}" "$targetDir/" || ShowError $ErrorExit "Failed to install headers from $subdir."
                fi
            done
        fi
    done
}

case $target in
    all)
        BuildStatic
        BuildShared
        BuildObject
        ;;
    static)
        BuildStatic
        ;;
    shared)
        BuildShared
        ;;
    object)
        BuildObject
        ;;
    *)
        ShowError $ArgsErrorExit "Not correct target: $target"
        ;;
esac

if [[ $shouldInstall = true ]]; then
    if [[ $EUID -ne 0 ]]; then
        ShowError $ErrorExit "To make the installation option work run the script as root."
    fi

    InstallHeaders

    case $target in
    all)
        InstallStatic
        InstallShared
        ;;
    static)
        InstallStatic
        ;;
    shared)
        InstallShared
        ;;
    esac

    ldconfig
fi

ShowSuccess "Done!"
exit $SuccessExit
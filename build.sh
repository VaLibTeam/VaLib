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

installLibs=false
installHeaders=false

uninstallLibs=false
uninstallHeaders=false

Help() {
    echo "Usage: $0 [options...]"
    echo ""
    echo "Options:"
    echo "  --cxx=<compiler>             Specify the C++ compiler to use (default: g++)"
    echo "  --cxxflags=<flags>           Specify the compiler flags (default: -Wall -Wextra -std=c++20 -O2 -fPIC)"
    echo "  --ar=<archiver>              Specify the archiver to use (default: ar)"
    echo "  --arflags=<flags>            Specify the archiver flags"
    echo
    echo "  --build-dir=<dir>            Specify the build directory (default: build)"
    echo "  --output=<name>              Specify the output library name (default: libvalib)"
    echo "  --out-dir=<dir>              Specify the output directory (default: out)"
    echo "  --target=<type>              Specify the build target: static, shared, object, or all"
    echo
    echo "  --install-headers            Install the headers"
    echo "  --install-libs               Install the built library"
    echo "  --install-all/--install      Install the built library and headers"
    echo
    echo "  --uninstall-headers          Uninstall the headers"
    echo "  --uninstall-libs             Uninstall the libraries"
    echo "  --uninstall-all/--uninstall  Uninstall the libraries and headers"
    echo
    echo "  --help                       Show this help message"
}

Clean() {
    if [[ -n "$BUILD" ]]; then
        # ShowInfo "I won't make the same mistake as steam 😎"   # Uncomment me please
        exit $SuccessExit
    fi

    rm -r "$BUILD/*"
    rm -r "$OUTDIR/*"
}

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

    --install-libs)
        installLibs=true
        ;;
    --install-headers)
        installHeaders=true
        ;;
    --install | --install-all)
        installLibs=true
        installHeaders=true
        ;;
    
    --uninstall-libs)
        uninstallLibs=true
        ;;
    --uninstall-headers)
        uninstallHeaders=true
        ;;
    --uninstall | --uninstall-all)
        uninstallHeaders=true
        uninstallLibs=true
        ;;
    
    --clean)
        Clean
        exit $SuccessExit
        ;;

    --help)
        Help
        exit $SuccessExit
        ;;

    *)
        ShowError $ArgsErrorExit "Invalid argument: $arg" ;;
    esac
done

if [[ ($uninstallHeaders = true && $installHeaders = true) || ($installLibs = true && $uninstallLibs = true) ]]; then
    # ShowError $ArgsErrorExit "Conflicting options: Cannot install and uninstall simultaneously."
    ShowError $ArgsErrorExit "what the fuck are you doing mate"
fi

mkdir -p "$OUTDIR"
mkdir -p "$BUILD"

if [[ "$target" != "" ]]; then
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
fi

BuildStatic() {
    ShowInfo "Creating static library (.a)..."
    $AR rcs $ARFLAGS "$OUTDIR/$OUTPUT.a" "${objFiles[@]}" || ShowError $LinkingErrorExit "Failed to link."
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
    install -d "/usr/local/lib"
    install -m 644 "$OUTDIR/$OUTPUT.a" "/usr/local/lib/" || ShowError $ErrorExit "Failed to install static library."
}

InstallShared() {
    install -d "/usr/local/lib"
    install -m 755 "$OUTDIR/$OUTPUT.so" "/usr/local/lib/" || ShowError $ErrorExit "Failed to install shared library."
}

InstallHeaders() {
    SysIncludeDir="/usr/local/include"
    for dir in "${includePath[@]}"; do
        if [[ -d "$dir" ]]; then
            find "$dir" -type d | while read -r subdir; do

                if [[ "$subdir" == "$dir" ]]; then relPath=""
                else relPath="${subdir#"$dir"/}"; fi

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

UninstallStatic() {
    ShowInfo "Uninstalling static library..."
    rm -f "/usr/local/lib/$OUTPUT.a" || ShowError $ErrorExit "Failed to uninstall static library."
}

UninstallShared() {
    ShowInfo "Uninstalling shared library..."
    rm -f "/usr/local/lib/$OUTPUT.so" || ShowError $ErrorExit "Failed to uninstall shared library."
}

UninstallHeaders() {
    SysIncludeDir="/usr/local/include"

    for dir in "${includePath[@]}"; do
        if [[ -d "$dir" ]]; then
            find "$dir" -type d | while read -r subdir; do
                if [[ "$subdir" == "$dir" ]]; then relPath=""
                else relPath="${subdir#"$dir"/}"; fi

                targetDir="$SysIncludeDir/$relPath"

                headers=("$subdir"/*.{hpp,tpp,h})
                if (( ${#headers[@]} )); then
                    for header in "${headers[@]}"; do
                        rm -f "$targetDir/$(basename "$header")" || ShowError $ErrorExit "Failed to uninstall header: $header"
                    done
                fi

                # Remove empty directories
                rmdir --ignore-fail-on-non-empty "$targetDir" || true
            done
        fi
    done
}

if [[ -n "$target" ]]; then
    case "$target" in
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
fi

if [[ $installHeaders = true ]]; then
    if [[ $EUID -ne 0 ]]; then
        ShowError $ErrorExit "To make the installation option work run the script as root."
    fi

    ShowInfo "Installing headers..."
    InstallHeaders
fi

if [[ $installLibs = true ]]; then
    if [[ -z "$target" ]]; then
        ShowInfo "--install-libs: No target specified. nothing to do."
    else
        if [[ $EUID -ne 0 ]]; then
            ShowError $ErrorExit "To make the installation option work run the script as root."
        fi

        case $target in
        all)
            ShowInfo "Installing static library..."
            InstallStatic

            ShowInfo "Installing shared library..."
            InstallShared
            ;;
        static)
            ShowInfo "Installing static library..."
            InstallStatic
            ;;
        shared)
            ShowInfo "Installing shared library..."
            InstallShared
            ;;
        esac

        ldconfig
    fi
fi

if [[ $uninstallHeaders = true ]]; then
    if [[ $EUID -ne 0 ]]; then
        ShowError $ErrorExit "To make the uninstallation option work run the script as root."
    fi

    ShowInfo "Uninstalling headers..."
    UninstallHeaders
fi

if [[ $uninstallLibs = true ]]; then
    if [[ $EUID -ne 0 ]]; then
        ShowError $ErrorExit "To make the uninstallation option work run the script as root."
    fi

    ShowInfo "Uninstalling libraries..."
    UninstallStatic
    UninstallShared
    ldconfig
fi

ShowSuccess "Done!"
exit $SuccessExit
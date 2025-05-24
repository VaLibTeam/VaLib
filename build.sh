#!/bin/bash
# VaLib - Vast Library
# Licensed under GNU GPL v3 License. See LICENSE file.
# (C) 2025 VaLibTeam

cd "$(dirname "$0")" || exit 1
source "scripts/utils.sh" || exit 1
source "scripts/get-cxx-flags.sh" || exit 1
set -e || exit 1
shopt -s nullglob || exit 1

SRC="src"
BUILD="build"
OUTPUT="libvalib"
OUTDIR="out"

CXX="${CXX:-g++}"
# CXXFLAGS=( -03 )

AR="ar"
ARFLAGS=()

TAR="tar"
TARFLAGS=()

srcArchiveName="VaLib-Source-$VER"
develArchiveName="VaLib-Devel-$VER"
objArchiveName="VaLib-ObjFiles-$VER"
fullArchiveName="VaLib-Full-$VER"

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

interactiveMode=true

toCompile=()
targets=()

Help() {
    Sep() { echo; }

    PrintOption() {
        printf "  ${BOLD}%-30s${RESET} %s\n" "$1" "$2"
    }
    PrintTarget() {
        printf "  ${BOLD}%-30s${RESET} %s\n" "$1" "$2"
    }
    PrintSection() {
        Sep
        printf "\n${BOLD}%s:${RESET}\n" "$1"
    }

    echo -en "Usage: $0 [options...]"

    # --- Options ---
    PrintSection "Options"
    PrintOption "--default"                    "Use default options"
    PrintOption "--default-devel"              "Use default options for development"
    PrintOption "--non-interactive"            "Disable interactive mode"
    PrintOption "--interactive"                "Enable interactive mode"
    PrintOption "--no-colors"                  "Disable color output"
    PrintOption "--colors"                     "Enable color output"
    Sep
    PrintOption "--cxx=<compiler>"              "Specify the C++ compiler to use (default: g++)"
    PrintOption "--cxxflags=<flags>"            "Specify the compiler flags (default: -Wall -Wextra -std=c++20 -O2 -fPIC)"
    PrintOption "--ar=<archiver>"               "Specify the archiver to use (default: ar)"
    PrintOption "--arflags=<flags>"             "Specify the archiver flags"
    Sep
    PrintOption "--build-dir=<dir>"             "Specify the build directory (default: build)"
    PrintOption "--output=<name>"               "Specify the output library name (default: libvalib)"
    PrintOption "--out-dir=<dir>"               "Specify the output directory (default: out)"
    Sep
    PrintOption "--compile-sources/--compile"   "Compile source files to object files (.o)"
    Sep
    PrintOption "--build-static"                "Build static library (.a)"
    PrintOption "--build-shared"                "Build shared library (.so)"
    PrintOption "--build-object"                "Build merged object file (.o)"
    PrintOption "--build-all"                   "Build all targets"
    PrintOption "--build-src-archive"           "Build source archive (.tar.xz)"
    PrintOption "--build-devel-archive"         "Build development archive with headers (.tar.xz)"
    PrintOption "--build-obj-archive"           "Build object files archive (.tar.xz)"
    PrintOption "--build-full-archive"          "Build full archive with headers, object files, and source code (.tar.xz)"
    Sep
    PrintOption "--build-target=<target>"       "Specify the target to build"
    PrintOption "--target=<target>"             "Specify the target to build [ deprecated ]"
    Sep
    PrintOption "--install-headers"             "Install the headers"
    PrintOption "--install-libs"                "Install the built library"
    PrintOption "--install-all/--install"       "Install the built library and headers"
    Sep
    PrintOption "--uninstall-headers"           "Uninstall the headers"
    PrintOption "--uninstall-libs"              "Uninstall the libraries"
    PrintOption "--uninstall-all/--uninstall"   "Uninstall the libraries and headers"
    Sep
    PrintOption "--clean"                       "Clean the build directory"
    PrintOption "--help/-h"                     "Show this help message"

    # --- Targets ---
    PrintSection "Targets"
    PrintTarget "all"           "Build all targets (static, shared, and object)"
    PrintTarget "static"        "Build the static library (.a)"
    PrintTarget "shared"        "Build the shared library (.so)"
    PrintTarget "object"        "Build a single merged object file (.o)"
    PrintTarget "src-archive"   "Build the archive (tar.gz) containing the source code"
    PrintTarget "libs-archive"  "Build the archive (tar.gz) containing the static and shared libraries"
}

Clean() {
    [[ -n "$BUILD" && -d "$BUILD" && "$(ls -A "$BUILD")"    ]] && rm -r "${BUILD:?}"/*
    [[ -n "$OUTDIR" && -d "$OUTDIR" && "$(ls -A "$OUTDIR")" ]] && rm -r "${OUTDIR:?}"/*
}

CheckArgs() {
    if [[ ($uninstallHeaders = true && $installHeaders = true) || ($installLibs = true && $uninstallLibs = true) ]]; then
        # ShowError $ArgsErrorExit "Conflicting options: Cannot install and uninstall simultaneously."
        ShowError $ArgsErrorExit "What the fuck are you doing mate"
    fi

    toCheckCommands=("$CXX" "$AR")
    for cmd in "${toCheckCommands[@]}"; do
        if ! command -v "$cmd" > /dev/null 2>&1; then
            ShowError "$NotExit" "Tool '$cmd' is not installed."

            if CanInstall "$cmd" && $interactiveMode; then
                read -n 1 -r -p "$(ShowYn 'Do you want to install it?')" ans
                echo

                case "$ans" in
                [Yy])
                    if ! Install "$cmd"; then
                        ShowWarn "Failed to install $cmd."
                    fi
                    ;;
                [Nn])
                    pass
                    ;;
                *)
                    ShowError $ErrorExit "Invalid option."
                    ;;
                esac
            else
                exit "$ArgsErrorExit"
            fi
        fi
    done

    toCheckDirs=("$SRC")
    for dir in "${toCheckDirs[@]}"; do
        if [[ ! -d "$dir" ]]; then
            ShowError "$ArgsErrorExit" "Directory '$dir' does not exist."
        fi
    done
}

ParseFlags() {
    for arg in "$@"; do
        case $arg in
        --default)
            if [[ -t 0 ]]; then
                interactiveMode=true
                colors=true
            fi

            targets=("static" "shared")

            if [[ "$EUID" -eq 0 ]]; then
                installLibs=true
            fi

            ShowTip "Using default settings: build static and shared libraries, install libs, no headers"
            ;;
        --default-devel)
            if [[ -t 0 ]]; then
                interactiveMode=true
                colors=true
            fi

            targets=("static" "shared")

            if [[ "$EUID" -eq 0 ]]; then
                installLibs=true
                installHeaders=true
            fi

            ShowTip "Using default settings for development: build static and shared libraries, install libs and headers"
            ;;
        --interactive)
            interactiveMode=true ;;
        --non-interactive)
            interactiveMode=false ;;
        --colors)
            colors=true ;;
        --no-colors)
            colors=false ;;

        --cxx=*)
            CXX="${arg#*=}" ;;
        --cxxflags=*)
            read -ra CXXFLAGS <<< "${arg#*=}" ;;

        --ar=*)
            AR="${arg#*=}" ;;
        --arflags=*)
            read -ra ARFLAGS <<< "${arg#*=}" ;;

        --tar=*)
            TAR="${arg#*=}" ;;
        --tar-flags=*)
            read -ra TARFLAGS <<< "${arg#*=}" ;;

        --build-dir=*)
            BUILD="${arg#*=}" ;;
        --output=*)
            OUTPUT="${arg#*=}" ;;
        --out-dir=*)
            OUTDIR="${arg#*=}" ;;
        --src-archive-name=*)
            srcArchiveName="${arg#*=}" ;;
        --devel-archive-name=*)
            develArchiveName="${arg#*=}" ;;
        --full-archive-name=*)
            fullArchiveName="${arg#*=}" ;;

        --compile | --compile-sources)
            targets+=("compile") ;;

        --build-static)
            targets+=("static") ;;
        --build-shared)
            targets+=("shared") ;;
        --build-libs)
            targets+=("shared" "static") ;;
        --build-object)
            targets+=("object") ;;

        --build-src-archive)
            targets+=("src-archive") ;;
        --build-devel-archive)
            targets+=("devel-archive") ;;
        --build-objects-archive)
            targets+=("objects-archive") ;;
        --build-full-archive)
            targets+=("full-archive") ;;

        --build-all | --target=all | --build-target=all)
            all=("static" "shared" "object" "src-archive" "devel-archive" "objects-archive" "full-archive")
            targets+=( "${all[@]}" ) ;;

        --target=* | --build-target=*)
            tg="${arg#*=}"
            if [[ -z "$tg" ]]; then
                ShowError $ArgsErrorExit "Invalid target: $tg"
            fi

            targets+=("$tg") ;;

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

        --help | -h)
            Help
            exit $SuccessExit
            ;;

        *)
            ShowError $ArgsErrorExit "Invalid argument: $arg" ;;
        esac
    done
}

CompileSources() {
    mapfile -t cppFiles < <(find "$SRC" -name "*.cpp")
    objFiles=()

    for file in "${cppFiles[@]}"; do
        obj="$BUILD/$(basename "$file" .cpp).o"
        objFiles+=("$obj")

        if [[ ! -f "$obj" || "$file" -nt "$obj" ]]; then
            toCompile+=("$file")
        fi
    done

    total=${#toCompile[@]}
    if [[ $total -eq 0 ]]; then
        ShowInfo "Nothing to compile"
        return
    fi

    ShowInfo "Compiling $total sources..."
    compiled=0
    for file in "${toCompile[@]}"; do
        obj="$BUILD/$(basename "$file" .cpp).o"
        "$CXX" "${CXXFLAGS[@]}" "${includePath[@]/#/-I}" -c "$file" -o "$obj" || ShowError $CompilationErrorExit "Failed to compile $file"
        percent=$(( (compiled++ + 1) * 100 / total ))
        ShowProgress "Compiled: ${percent}% ($compiled/$total)"
    done
}

BuildStatic() {
    "$AR" rcs "${ARFLAGS[@]}" "$OUTDIR/$OUTPUT.a" "${objFiles[@]}" || ShowError $LinkingErrorExit "Failed to link."
}

BuildShared() {
    "$CXX" "${CXXFLAGS[@]}" -shared -o "$OUTDIR/$OUTPUT.so" "${objFiles[@]}" || ShowError $LinkingErrorExit "Failed to link."
}

BuildObject() {
    ld -r "${objFiles[@]}" -o "$OUTDIR/$OUTPUT.o" || ShowError $LinkingErrorExit "Failed to link."
}

BuildSrcArchive() {
    "$TAR" -cJf "${TARFLAGS[@]}" "$OUTDIR/$srcArchiveName.tar.xz" "$SRC/" || ShowError $ErrorExit "Failed to create source archive."
}

BuildDevelArchive() {
    "$TAR" -cJf "${TARFLAGS[@]}" "$OUTDIR/$develArchiveName.tar.xz" "${includePath[@]}" || ShowError $ErrorExit "Failed to create development archive."
}

BuildObjectArchive() {
    "$TAR" -cJf "${TARFLAGS[@]}" "$OUTDIR/$objArchiveName.tar.xz" "$BUILD/" || ShowError $ErrorExit "Failed to create object archive."
}

BuildFullArchive() {
    "$TAR" -cJf "${TARFLAGS[@]}" "$OUTDIR/$fullArchiveName.tar.xz" "${includePath[@]}" "$BUILD/" "$SRC/" "$OUTDIR/" || ShowError $ErrorExit "Failed to create full archive."
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
    sysIncludeDir="/usr/local/include"
    for dir in "${includePath[@]}"; do
        if [[ -d "$dir" ]]; then
            find "$dir" -type d | while read -r subdir; do

                if [[ "$subdir" == "$dir" ]]; then relPath=""
                else relPath="${subdir#"$dir"/}"
                fi

                targetDir="$sysIncludeDir/$relPath"
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
    rm -f "/usr/local/lib/$OUTPUT.a" || ShowError $ErrorExit "Failed to uninstall static library."
}

UninstallShared() {
    rm -f "/usr/local/lib/$OUTPUT.so" || ShowError $ErrorExit "Failed to uninstall shared library."
}

UninstallHeaders() {
    sysIncludeDir="/usr/local/include"

    for dir in "${includePath[@]}"; do
        if [[ -d "$dir" ]]; then
            find "$dir" -type d | while read -r subdir; do
                if [[ "$subdir" == "$dir" ]]; then relPath=""
                else relPath="${subdir#"$dir"/}"; fi

                local targetDir="$sysIncludeDir/$relPath"

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

Main() {
    ParseFlags "$@"
    CheckArgs

    mkdir -p "$OUTDIR"
    mkdir -p "$BUILD"

    if [[ ${#targets[@]} -gt 0 ]] || (printf "%s\n" "${targets[@]}" | grep -q "^compile$"); then
        CompileSources

        fullArchiveRequested=false

        for target in "${targets[@]}"; do
            case "$target" in
            static)
                ShowInfo "Creating static library (.a)..."
                BuildStatic
                ;;
            shared)
                ShowInfo "Creating shared library (.so)..."
                BuildShared
                ;;
            object)
                ShowInfo "Merging object files into one .o..."
                BuildObject
                ;;
            src-archive)
                ShowInfo "Creating source archive (.tar.xz)..."
                BuildSrcArchive
                ;;
            devel-archive)
                ShowInfo "Creating development archive (.tar.xz)..."
                BuildDevelArchive
                ;;
            objects-archive)
                ShowInfo "Creating objects archive (.tar.xz)..."
                BuildObjectArchive
                ;;
            full-archive)
                fullArchiveRequested=true
                ;;
            compile | compile-sources)
                ;;
            *)
                ShowError $ArgsErrorExit "Not correct target: $target"
                ;;
            esac
        done

        if [[ $fullArchiveRequested = true ]]; then
            ShowInfo "Creating full archive (.tar.xz)..."
            BuildFullArchive
        fi
    fi

    if [[ $installHeaders = true ]]; then
        if [[ $EUID -ne 0 ]]; then
            ShowError $ErrorExit "To make the installation option work run the script as root."
        fi

        ShowInfo "Installing headers..."
        InstallHeaders
    fi

    if [[ $installLibs = true ]]; then
        if [[ ${#targets[@]} -eq 0 ]]; then
            ShowWarn "--install-libs: No target specified. nothing to do."
        else
            if [[ $EUID -ne 0 ]]; then
                ShowError $ErrorExit "To make the installation option work run the script as root."
            fi

            for target in "${targets[@]}"; do
                case "$target" in
                static)
                    ShowInfo "Installing static library..."
                    InstallStatic
                    ;;
                shared)
                    ShowInfo "Installing shared library..."
                    InstallShared
                    ;;
                esac
            done

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

    if [[ ${#targets[@]} -gt 0 || $uninstallHeaders = true || $uninstallLibs = true  || $installLibs = true || $installHeaders = true ]]; then
        ShowSuccess "Done!"
    else
        ShowOk "Nothing to do."
        ShowTip "Run the script with the -h option for usage information."
    fi
    return $SuccessExit
}

Main "$@"
exit $?

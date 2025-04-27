#!/bin/env bash
# VaLib - Vast Library
# Licensed under GNU GPL v3 License. See LICENSE file.
# (C) 2025 VaLibTeam

cd "$(dirname "$0")" || exit 1
source "./utils.sh" || exit 1
set -e || exit 1
shopt -s nullglob || exit 1

SuccessExit=0
ArgsErrorExit=1
ErrorExit=2

AllInstalledExit=0
StaticNotInstalledExit=3
SharedNotInstalledExit=4
HeadersNotInstalledExit=5

allInstalled=true
exitCode=$AllInstalledExit

interactiveMode=true

Help() {
    echo "Usage: $0 [options...]"
    echo "Options:"
    echo "  --non-interactive   Run in non-interactive mode"
    echo "  --interactive       Run in interactive mode"
    echo "  -h, --help          Show this help message"
}

ParseFlags() {
    for arg in "$@"; do
        case $arg in
        --non-interactive)
            interactiveMode=false
            ;;
        --interactive)
            interactiveMode=true
            ;;
        -h | --help)
            Help
            exit $SuccessExit
            ;;
        *)
            ShowError "Unknown option: $arg"
            exit $ArgsErrorExit
            ;;
        esac
    done
}

NotInstalled() {
    allInstalled=false
    exitCode=$((exitCode | $1))
}

RunBuildScript() {
    local staticInstalled="$1"
    local sharedInstalled="$2"
    local headersInstalled="$3"

    ShowTip "Select components to install."
    installLibs=false
    installHeaders=false

    if [[ $staticInstalled = false || $sharedInstalled = false ]]; then
        ReadYn choice "Do you want to install VaLib (libraries)?"
        case $choice in
        [Yy])
            installLibs=true ;;
        [Nn]) ;;
        *)
            ShowError $ErrorExit "Unknown option: $choice" ;;
        esac
    fi

    if [[ $headersInstalled = false ]]; then
        ReadYn choice "Do you want to install VaLib devel (headers)?"
        case $choice in
        [Yy])
            installHeaders=true ;;
        [Nn]) ;;
        *)
            ShowError $ErrorExit "Unknown option: $choice" ;;
        esac
    fi

    scriptFlags=()
    if [[ $installLibs == true ]]; then
        scriptFlags+=("--install-libs")
    fi
    if [[ $installHeaders == true ]]; then
        scriptFlags+=("--install-headers")
    fi

    ../build.sh --target=all "${scriptFlags[@]}"
}

Main() {
    ParseFlags "$@"

    staticInstalled=false
    sharedInstalled=false
    headersInstalled=false

    if [[ -f "/usr/lib/libvalib.a" || -f "/usr/local/lib/libvalib.a" ]]; then
        ShowInfo "VaLib static library is installed."
        staticInstalled=true
    else
        ShowWarn "VaLib static library is NOT installed."
        NotInstalled $StaticNotInstalledExit
    fi

    if [[ -f "/usr/lib/libvalib.so" || -f "/usr/local/lib/libvalib.so" ]]; then
        ShowInfo "VaLib shared library is installed."
        sharedInstalled=true
    else
        ShowWarn "VaLib shared library is NOT installed."
        NotInstalled $SharedNotInstalledExit
    fi

    modules=(
        "VaLib"
        "VaLib/Types"
        "VaLib/Utils"
    )

    allExists=true
    for module in "${modules[@]}"; do
        if [[ ! -d "/usr/local/include/$module" ]]; then
            allExists=false
        fi
    done

    if [[ $allExists = true && -f "/usr/local/include/VaLib.hpp" ]]; then
        ShowInfo "VaLib headers (devel version) is installed."
        headersInstalled=true
    else
        ShowWarn "VaLib headers (devel version) is NOT installed."
        NotInstalled $HeadersNotInstalledExit
    fi

    echo

    case $allInstalled in
    true)
        ShowSuccess "VaLib is fully installed on your system!"
        exit $AllInstalledExit
        ;;
    false)
        ShowWarn "Some VaLib components are not installed."
        if [[ $interactiveMode = true ]]; then
            ReadYn ans "Do you want to install missing components?"
            case $ans in
            [Yy])
                RunBuildScript "$staticInstalled" "$sharedInstalled" "$headersInstalled"
                ;;
            [Nn])
                exit $exitCode
                ;;
            *)
                ShowError $ErrorExit "Unknown option: $choice" ;;
            esac
        fi

        exit $exitCode
        ;;
    esac
}

Main "$@"
exit "$?"

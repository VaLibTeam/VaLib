#!/bin/env bash
# VaLib - Vast Library
# Licensed under GNU GPL v3 License. See LICENSE file.
# (C) 2025 VaLibTeam

cd "$(dirname "$0")" || exit 1
source "./utils.sh" || exit 1
set -e || exit 1
shopt -s nullglob || exit 1

AllInstalledExit=0
StaticNotInstalledExit=1
SharedNotInstalledExit=2
HeadersNotInstalledExit=1

allInstalled=true
exitCode=$AllInstalledExit

NotInstalled() {
    allInstalled=false
    exitCode=$((exitCode | $1))
}

Main() {
    if [[ -f "/usr/lib/libvalib.a" || -f "/usr/local/lib/libvalib.a" ]]; then
        ShowInfo "VaLib static library is installed."
    else
        ShowWarn "VaLib static library is NOT installed."
        NotInstalled $StaticNotInstalledExit
    fi

    if [[ -f "/usr/lib/libvalib.so" || -f "/usr/local/lib/libvalib.so" ]]; then
        ShowInfo "VaLib shared library is installed."
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
    else
        ShowWarn "VaLib headers (devel version) is NOT installed."
        NotInstalled $HeadersNotInstalledExit
    fi

    case $allInstalled in
    true)
        ShowSuccess "VaLib is fully installed on your system!"
        exit $AllInstalledExit
        ;;
    false)
        ShowWarn "Some VaLib components are not installed."
        exit $exitCode
        ;;
    esac
}

Main "$@"
exit "$?"

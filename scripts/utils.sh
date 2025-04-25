#!/bin/env bash

NotExit=n

colors=true

ShowError() {
    local code="$1"
    local msg="${*:2}"

    if [[ "$colors" == true ]]; then echo -e "\033[1;31m[ ERROR ]:\033[91m $msg\033[0m"
    else echo "[ ERROR ]: $msg"
    fi

    if [[ "$code" != "$NotExit" ]]; then
        exit "$code"
    fi
}

ShowSuccess() {
    local msg="${*}"

    if [[ "$colors" == true ]]; then echo -e "\033[1;32m[ SUCCESS ]:\033[92m $msg\033[0m"
    else echo "[ SUCCESS ]: $msg"
    fi
}

ShowWarn() {
    local msg="${*}"

    if [[ "$colors" == true ]]; then echo -e "\033[1;33m[ WARNING ]:\033[93m $msg\033[0m"
    else echo "[ WARNING ]: $msg"
    fi
}

ShowInfo() {
    local msg="${*}"

    if [[ "$colors" == true ]]; then echo -e "\033[1;34m[ INFO ]:\033[94m $msg\033[0m"
    else echo "[ INFO ]: $msg"
    fi
}

ShowYn() {
    local msg="${*}"

    if [[ "$colors" == true ]]; then echo -e "[\033[1;32mY\033[m/\033[31mn\033[0m] $msg: "
    else echo "[Y/n] $msg: "
    fi
}

UnsupportedPm=2

CanInstall() {
    local package="$1"

    if [[ $EUID -ne 0 ]]; then
        return 1
    fi

    if command -v dnf &>/dev/null; then
        dnf list available "$package" &>/dev/null
        return $?

    elif command -v apt &>/dev/null; then
        apt-cache show "$package" &>/dev/null
        return $?

    elif command -v pacman &>/dev/null; then
        pacman -Si "$package" &>/dev/null
        return $?

    elif command -v zypper &>/dev/null; then
        zypper --non-interactive se -x "$package" | grep -q "^i | $package "
        return $?

    else
        return $UnsupportedPm
    fi
}

Install() {
    package="$1"

    if command -v dnf &>/dev/null; then
        dnf install -y "$package" &>/dev/null
        return $?

    elif command -v apt &>/dev/null; then
        apt-get install "$package" &>/dev/null
        return $?

    elif command -v pacman &>/dev/null; then
        pacman -S --noconfirm "$package" &>/dev/null
        return $?

    elif command -v zypper &>/dev/null; then
        zypper --non-interactive install "$package" &>/dev/null
        return $?

    else
        return $UnsupportedPm
    fi
}

# ANSI COLORS
RED="\033[31m"
GREEN="\033[32m"
YELLOW="\033[33m"
BOLD="\033[1m"
RESET="\033[0m"

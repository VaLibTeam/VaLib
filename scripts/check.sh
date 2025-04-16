#!/bin/bash
# VaLib - Vast Library
# Licensed under GNU GPL v3 License. See LICENSE file.
# (C) 2025 VaLibTeam

source "scripts/utils.sh" || exit 1
set -e || exit 1
shopt -s nullglob || exit 1

commonHeader="// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam"

headerHeader="$commonHeader"
sourceHeader="$commonHeader"

# === Helpers Functions ===
Section() {
    echo -e "\n${BOLD}== $1 ==${RESET}"
}

CheckHeader() {
    lines=$(echo "$2" | wc -l)

    actualHeader=$(head -n "$lines" "$1")

    if [[ "$actualHeader" != "$2" ]]; then
        ShowWarn "Invalid header in: $1"
        return 1
    fi
    return 0
}

CheckPragmaOnce() {
    local pragmaLine
    pragmaLine=$(head -n 10 "$file" | grep "#pragma once" || true)

    if [[ -z "$pragmaLine" ]]; then
        ShowWarn "Missing '#pragma once' in: $1"
        return 1
    fi

    return 0
}

FindTodos() {
    results=$(grep -REn --include="*.cpp" --include="*.hpp" 'TODO\([^)]*, *".*"\)' . || true)

    if [[ -n "$results" ]]; then
        echo -e "${YELLOW}Found TODOs:${RESET}"

        # Find the maximum lengths for alignment
        maxFileLength=$(echo "$results" | cut -d: -f1 | awk '{ if (length > max) max = length } END { print max }')
        maxLineNoLength=$(echo "$results" | cut -d: -f2 | awk '{ if (length > max) max = length } END { print max }')

        echo "$results" | while IFS= read -r line; do
            file=$(echo "$line" | cut -d: -f1)
            lineNo=$(echo "$line" | cut -d: -f2)
            content=$(echo "$line" | cut -d: -f3-)
            printf "${BOLD}%-${maxFileLength}s:%-${maxLineNoLength}s${RESET} - %s\n" "$file" "$lineNo" "$content"
        done
    fi
}

# === Main ===
Section "Checking .hpp headers"
for file in $(find . -name "*.hpp"); do
    CheckHeader "$file" "$headerHeader"
    CheckPragmaOnce "$file"
done

Section "Checking .cpp headers"
for file in $(find . -name "*.cpp"); do
    CheckHeader "$file" "$sourceHeader"
done

Section "Finding TODOs"
FindTodos

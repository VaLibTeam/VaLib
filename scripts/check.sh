#!/bin/bash
# VaLib - Vast Library
# Licensed under GNU GPL v3 License. See LICENSE file.
# (C) 2025 VaLibTeam

source "scripts/utils.sh" || exit 1

HEADER_COMMON="// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam"

headersHeader="$HEADER_COMMON
#pragma once"

sourceHeaders="$HEADER_COMMON"

echo "== Checking .hpp headers =="
find . -name "*.hpp" | while read -r file; do
    head -n 4 "$file" | diff -q - <(echo "$headersHeader") >/dev/null
    if [ $? -ne 0 ]; then
        ShowWarn "Invalid header in: $file"
    fi
done

echo "== Checking .cpp headers =="
find . -name "*.cpp" | while read -r file; do
    head -n 3 "$file" | diff -q - <(echo "$sourceHeaders") >/dev/null
    if [ $? -ne 0 ]; then
        ShowWarn "Invalid header in: $file"
    fi
done

echo "== Finding TODOs =="
grep -R --include="*.cpp" --include="*.hpp" -n 'TODO([^)]*, *".*")' .

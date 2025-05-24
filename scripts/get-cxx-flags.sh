#!/bin/bash
# VaLib - Vast Library
# Licensed under GNU GPL v3 License. See LICENSE file.
# (C) 2025 VaLibTeam

# compile_flags.txt should be in root VaLib directory, not scripts/
cd .. || exit 1

flags=( -Wall -Wextra -std="c++20" -O2 -fPIC )
if [ -f "compile_flags.txt" ]; then
    readFlags=$(tr '\n' ' ' < compile_flags.txt)
    if [ -n "$readFlags" ]; then
        read -ra flags <<< "$readFlags"
    else
        flags=( -Wall -Wextra -std="c++20" -O2 -fPIC )
    fi
fi

# restore cwd
cd - > /dev/null || exit 1

CXXFLAGS=( "${flags[@]}" )

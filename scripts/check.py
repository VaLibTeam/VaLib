#!/usr/bin/env python3
import os
import sys

from pathlib import Path
from typing import Generator

from utils import showSuccess, showWarn, showInfo, showOk

# --- Headers ---
commonHeader: str = """
// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
""".strip()

headerHeader: str = commonHeader
sourceHeader: str = commonHeader

# --- Helpers ---
def readFileLines(path: str) -> list[str]:
    with open(path, 'r', encoding='utf-8') as f:
        return f.read().splitlines()

def writeFileLines(path: str, lines: list[str]):
    with open(path, 'w', encoding='utf-8') as f:
        f.write('\n'.join(lines) + '\n')

def checkHeader(lines: list[str], expected: str) -> bool:
    headerLines: list[str] = expected.splitlines()
    if len(lines) < len(headerLines):
        return False

    return lines[:len(headerLines)] == headerLines

def hasPragmaOnce(lines: list[str], expectedHeader: str) -> bool:
    headerLines: list[str] = expectedHeader.splitlines()
    if len(lines) <= len(headerLines):
        return False

    return lines[len(headerLines)].strip() == "#pragma once"

def fixFile(path: str, isHeader: bool, expectedHeader: str):
    lines: list[str] = readFileLines(path) if os.path.exists(path) else []
    headerLines: list[str] = expectedHeader.splitlines()

    # determine where real content starts
    start: int = 0
    if checkHeader(lines, expectedHeader):
        start = len(headerLines)
        if isHeader and hasPragmaOnce(lines, expectedHeader):
            start += 1

    # skip empty lines
    while start < len(lines) and lines[start].strip() == "":
        start += 1

    newLines: list[str] = headerLines[:]

    if isHeader and not hasPragmaOnce(lines, expectedHeader):
        newLines.append("#pragma once")

    newLines.append("")  # blank line
    newLines += lines[start:]

    writeFileLines(path, newLines)
    showInfo("Fixed:", path)

def processFile(path: str, fix: bool, isHeader: bool, expectedHeader: str) -> bool:
    lines: list[str] = readFileLines(path)

    if not checkHeader(lines, expectedHeader):
        showWarn("Invalid or missing header in:", path)
        if fix:
            fixFile(path, isHeader, expectedHeader)

        return True

    if isHeader and (not hasPragmaOnce(lines, expectedHeader)):
        showWarn("Missing #pragma once in:", path)
        if fix:
            fixFile(path, isHeader, expectedHeader)

        return True

    return False

def findFiles() -> Generator[str, None, None]:
    for root, _, files in os.walk("."):
        for file in files:
            if file.endswith(".cpp") or file.endswith(".hpp"):
                yield os.path.join(root, file)

def main() -> int:
    fix: bool = "--fix" in sys.argv

    fixed: bool = False
    for path in findFiles():
        ext: str = Path(path).suffix
        isHeader: bool = ext == ".hpp"
        header: str = headerHeader if isHeader else sourceHeader
        fixed = processFile(path, fix, isHeader, header) or fixed

    if not fixed:
        showOk("Nothing to do!")
    else:
        showSuccess("Done")

    return 0

if __name__ == "__main__":
    exit(main())

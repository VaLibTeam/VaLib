#!/usr/bin/env python3
from utils import VERSION as currentVersion
from utils import showError, showSuccess, showInfo, showTip
from utils import notExit

from pathlib import Path
import re
import sys, os

try:
    os.chdir(os.path.dirname(sys.argv[0]))
except:
    exit(1)

# --- exit codes ---
SuccessExit = 0
InvalidVersionFormatExit = 1
InvalidBumpTargetExit = 2
InvalidArgsExit = 3
FileNotFoundErrorExit = 4
ErrorExit = 5

# --- file paths ---
FILE_UTILS_PY = Path("./utils.py")
FILE_UTILS_SH = Path("./utils.sh")
FILE_VERSION_HPP = Path("../Include/VaLib/Version.hpp")

# --- regex patterns ---
PY_VER_PATTERN = r"^\s*(VERSION\s*:\s*str\s*=\s*)'(\d+)\.(\d+)\.(\d+)'"
SH_VER_PATTERN = r"^\s*(VERSION\s*=\s*)['\"](\d+)\.(\d+)\.(\d+)['\"]"

CPP_MAJOR_PATTERN = r"^\s*(#define\s+VaLib_VERSION_MAJOR\s+)(\d+)"
CPP_MINOR_PATTERN = r"^\s*(#define\s+VaLib_VERSION_MINOR\s+)(\d+)"
CPP_PATCH_PATTERN = r"^\s*(#define\s+VaLib_VERSION_PATCH\s+)(\d+)"

type Version = tuple[int, int, int]

class InvalidArgsError(Exception): pass
class UnknownCommandError(Exception): pass
class InvalidBumpTargetError(Exception): pass
class InvalidSetArgsError(Exception): pass

def parseVersion(versionStr: str) -> Version:
    parts: list[str] = versionStr.strip().split(".")
    if len(parts) != 3:
        raise ValueError("Version must be in format X.Y.Z")

    major, minor, patch = (int(p) for p in parts)
    return (major, minor, patch)

def bumpVersion(ver: Version, part: str):
    major, minor, patch = ver
    if part == "major":
        return (major + 1, 0, 0)
    elif part == "minor":
        return (major, minor + 1, 0)
    elif part == "patch":
        return (major, minor, patch + 1)
    else:
        raise InvalidBumpTargetError("Invalid bump target: choose from major, minor, patch")

def updateFile(path: Path, ver: Version):
    content: str = path.read_text()
    newContent: str = content

    flags = re.MULTILINE
    if path.resolve() == FILE_UTILS_PY.resolve():
        newContent = re.sub(PY_VER_PATTERN, lambda m: f"{m[1]}'{ver[0]}.{ver[1]}.{ver[2]}'", content, flags=flags)
    elif path.resolve() == FILE_UTILS_SH.resolve():
        newContent = re.sub(SH_VER_PATTERN, lambda m: f"{m[1]}'{ver[0]}.{ver[1]}.{ver[2]}'", content, flags=flags)
    elif path.resolve() == FILE_VERSION_HPP.resolve():
        newContent = re.sub(CPP_MAJOR_PATTERN, lambda m: f"{m[1]}{ver[0]}", content, flags=flags)
        newContent = re.sub(CPP_MINOR_PATTERN, lambda m: f"{m[1]}{ver[1]}", newContent, flags=flags)
        newContent = re.sub(CPP_PATCH_PATTERN, lambda m: f"{m[1]}{ver[2]}", newContent, flags=flags)

    if content == newContent:
        showInfo(f"No changes made to {path}. check regex?")
    path.write_text(newContent)

def main() -> int:
    prog: str = sys.argv[0]
    try:
        if len(sys.argv) < 2:
            raise InvalidArgsError(f"No arguments provided")

        cmd: str = sys.argv[1]
        args: list[str] = sys.argv[2:]
        current: Version = parseVersion(currentVersion)

        newVersion: Version
        match cmd:
            case "set":
                if len(args) != 1:
                    raise InvalidSetArgsError()
                newVersion = parseVersion(args[0])
            case "bump":
                if len(args) != 1 or args[0] not in ["major", "minor", "patch"]:
                    raise InvalidBumpTargetError(args[0])
                newVersion = bumpVersion(current, args[0])

            case _:
                raise UnknownCommandError(cmd)
        pass

        updateFile(FILE_UTILS_PY, newVersion)
        updateFile(FILE_UTILS_SH, newVersion)
        updateFile(FILE_VERSION_HPP, newVersion)

        showSuccess(f"Version updated to {newVersion[0]}.{newVersion[1]}.{newVersion[2]}")

    except InvalidArgsError as err:
        showError(notExit, str(err))
        showTip(f"Use '{prog} set X.Y.Z' or '{prog} bump [major|minor|patch]'")
        return InvalidArgsExit

    except UnknownCommandError as command:
        showError(InvalidArgsExit, f"Command not found: {str(command)}")
    except InvalidBumpTargetError as target:
        showError(InvalidBumpTargetExit, f"Invalid bump target: {str(target)}. Usage for bump: {prog} bump [major|minor|patch]")
    except InvalidSetArgsError:
        showError(InvalidArgsExit, f"Usage for set: {prog} set {'{major}.{minor}.{patch}'}")

    except FileNotFoundError as err:
        showError(FileNotFoundErrorExit, f"File not found: {str(err)}")
    except Exception as err:
        showError(ErrorExit, f"Internal error. {type(err)}: {str(err)}")

    return 0

if __name__ == "__main__":
    exit(main())

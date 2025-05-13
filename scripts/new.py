#!/usr/bin/env python3
from utils import *

import platform
import os
import subprocess
from datetime import datetime
import shlex

# -------- Constants --------- #
SuccessExit = 0
InvalidArgErrorExit = 1
ErrorExit = 2

CHANGELOG_PATH: str = 'CHANGELOG.md'

CHANGE_TYPES: dict[str, str] = {
    "add": "Added",
    "fix": "Fixed",
    "change": "Changed",
    "remove": "Removed",
}

MODULES: tuple[str, ...] = (
    "Types", "Utils", "FuncTools"
)

defaultEditor: str = "nvim"
if EDITOR := os.getenv("EDITOR"):
    defaultEditor = EDITOR

# -------- Functions --------- #
def openEditor() -> str:
    tmpFile: str = "/tmp/.changelog_message" if platform.system() == "Linux" else R"C:\Windows\Temp\.changelog_message"
    subprocess.call(shlex.split(defaultEditor) + [tmpFile,])

    if os.path.exists(tmpFile):
        with open(tmpFile, "r") as f:
            message: str = f.read().strip()
        os.remove(tmpFile)
        return message
    return ""

def ensureChangelogExists() -> str:
    if not os.path.exists(CHANGELOG_PATH):
        with open(CHANGELOG_PATH, "w") as f:
            f.write("# Changelog\n")
        pass
    pass

    with open(CHANGELOG_PATH, "r") as f:
        return f.read()

def updateChangelog(changeType: str, module: str, msg: str):
    today: str = datetime.now().strftime("%Y-%m-%d")
    sectionHeader: str = f"## [{VERSION}] - {today}"
    changeHeading: str = f"### {CHANGE_TYPES[changeType]}"

    content: str = ensureChangelogExists()
    lines = content.splitlines()

    total: str = f"{f'**[ {module} ]**' if (module in MODULES) else f'**( {module} )**'} {msg}"

    if sectionHeader not in content:
        newSection: str = f"\n{sectionHeader}\n{changeHeading}\n- {total}\n"
        with open(CHANGELOG_PATH, "a") as f:
            f.write(newSection)
        pass
    else:
        idx: int = lines.index(sectionHeader)
        insertionPoint = idx + 1
        foundHeading: bool = False

        while insertionPoint < len(lines):
            if lines[insertionPoint].startswith("## ["):
                break
            if lines[insertionPoint].strip() == changeHeading:
                foundHeading = True
                break
            insertionPoint += 1
        pass

        if foundHeading:
            headingIdx: int = insertionPoint + 1
            while headingIdx < len(lines) and (lines[headingIdx].strip().startswith("- ") or lines[headingIdx].strip() == ""):
                headingIdx += 1
            lines.insert(headingIdx, f"- {total}")
        else:
            insertionPoint += 1
            lines.insert(insertionPoint, f"{changeHeading}\n- {total}")
        pass

        with open(CHANGELOG_PATH, "w") as f:
            f.write("\n".join(lines))

def gitCommit(msg: str): subprocess.run(["git", "commit", "-m", msg])
def gitAdd():            subprocess.run(["git", "add", "."])

# -------- Main --------- #
def main() -> int:
    import sys
    if len(sys.argv) < 3 or sys.argv[1] not in CHANGE_TYPES:
        showError(InvalidArgErrorExit, f"Usage: {sys.argv[0]} {'|'.join(CHANGE_TYPES.keys())} {{module}} {{files...}}")
        return ErrorExit

    changeType: str = sys.argv[1]
    module: str = (sys.argv[2] + ": " + ", ".join(sys.argv[3:])) if len(sys.argv) > 2 else sys.argv[2]

    msg: str = openEditor()
    if not msg:
        showError(ErrorExit, "No message provided, aborting.")

    updateChangelog(changeType, module, msg)

    gitAdd()
    gitCommit(msg)

    return SuccessExit

if __name__ == "__main__":
    exit(main())

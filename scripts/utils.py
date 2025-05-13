import sys

# === ANSI & Messaging ===
RED = "\033[31m"
GREEN = "\033[32m"
YELLOW = "\033[33m"
BOLD = "\033[1m"
RESET = "\033[0m"

notExit = -123
colors = True

def showError(code: int, *msg):
    message = " ".join(msg)
    if colors:
        print(f"\033[1;31m[ ERROR ]:\033[91m {message}\033[0m")
    else:
        print(f"[ ERROR ]: {message}")
    
    if code != notExit:
        sys.exit(code)

def showSuccess(*msg):
    message = " ".join(msg)
    if colors:
        print(f"\033[1;32m[ SUCCESS ]:\033[92m {message}\033[0m")
    else:
        print(f"[ SUCCESS ]: {message}")

def showWarn(*msg):
    message = " ".join(msg)
    if colors:
        print(f"\033[1;33m[ WARNING ]:\033[93m {message}\033[0m")
    else:
        print(f"[ WARNING ]: {message}")

def showInfo(*msg):
    message = " ".join(msg)
    if colors:
        print(f"\033[1;34m[ INFO ]:\033[94m {message}\033[0m")
    else:
        print(f"[ INFO ]: {message}")

def showOk(*msg):
    message = " ".join(msg)
    if colors:
        print(f"\033[1;92m[ OK ]:\033[38;5;36m {message}\033[0m")
    else:
        print(f"[ OK ]: {message}")

def showTip(*msg):
    message = " ".join(msg)
    if colors:
        print(f"\033[1;35m[ TIP ]:\033[95m {message}\033[0m")
    else:
        print(f"[ TIP ]: {message}")
    
# VaLib Version
VERSION: str = '1.9.0'

import sys

# === ANSI & Messaging ===
RED = "\033[31m"
GREEN = "\033[32m"
YELLOW = "\033[33m"
BOLD = "\033[1m"
RESET = "\033[0m"

def showError(code, *msg):
    print(f"\033[1;31m[ ERROR ]:\033[91m {' '.join(msg)}{RESET}")
    sys.exit(code)

def showSuccess(*msg):
    print(f"\033[1;32m[ SUCCESS ]:\033[92m {' '.join(msg)}{RESET}")

def showWarn(*msg):
    print(f"\033[1;33m[ WARNING ]:\033[93m {' '.join(msg)}{RESET}")

def showInfo(*msg):
    print(f"\033[1;34m[ INFO ]:\033[94m {' '.join(msg)}{RESET}")
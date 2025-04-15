# Contributing to VaLib
Thank you for considering contributing to VaLib!
We welcome all kinds of contributions – from bug reports and feature suggestions to full pull requests.
This document outlines the coding style and conventions used in the project to ensure consistency and maintainability across the codebase.

Please read the following guidelines before submitting pull requests or patches.

## C++ Style Guide
1. **Naming Conventions**
   - Use `PascalCase` for class names and static method names.
   - Use `camelCase` for everything else, including variables, non-static methods, and function names.

2. **Control Structures**
   - Omit braces (`{}`) in conditional statements and loops only if the body is simple and clearly readable, such as a single `x++`, `break`, or a function call like `myFunction()`.

3. **Indentation**
   - Use 4 spaces per indentation level. Do not use tabs.

4. **Access Modifiers**
   - Prefer `protected` over `private` when possible to encourage extensible class designs.

5. **Namespaces**
   - Avoid `using namespace std;` entirely. Always use explicit namespace prefixes to prevent naming conflicts.

6. **Namespace and Class Prefixes**
   - All code should reside in the `va` namespace.
   - Alternatively, classes may be prefixed with `Va`, for example: `VaMyClassName`.
   - Exceptions are allowed if justified, but the default should follow this rule.

7. **Code Formatting**
      - Always use `clang-format` with the configuration provided in the `.clang-format` file included in the repository.
      - Please avoid modifying `.clang-format` unless there's a strong documented reason.

   8. **Testing and Benchmarking**
      - After adding a new class or functionality, always include a corresponding test in the `testing/` directory.
      - Optionally, add a benchmark if performance is a concern.
      - Small utility functions or trivial changes may be exempt from this rule, but use discretion.
   
## Bash Script Style Guide
1. **Naming Conventions**
   - Use `PascalCase` for function names.
   - Use `camelCase` for variable names, except for arrays of exit codes, which are written in `PascalCase`.

2. **Indentation**
   - Use 4 spaces for indentation. Avoid tabs.

3. **Dependency Checks**
   - When using external tools that are not included in most Linux distributions by default, ensure to check their presence with a conditional statement before use.

4. **Conditional Syntax**
   - Always use `[[ ... ]]` instead of `[ ... ]` for conditionals.

5. **Inline Conditionals**
   - Avoid short forms like `[[ ... ]] && command` unless the logic is extremely simple and improves readability.

6. **Modularity**
   - Break down logic into small, reusable functions. Avoid large monolithic scripts.

7. **Logging and Errors**
   - Do not use `echo` for error or warning messages.
   - Always source `scripts/utils.sh` and use predefined functions like `ShowError`, `ShowWarn`, etc.

## Submitting Contributions
- When submitting a pull request, provide a clear and concise description of the change and its purpose.
- If you encounter a bug or unexpected behavior, please report it via the issue tracker, even if you are not sure about the root cause.
- If you are unsure whether your proposed change aligns with the project's direction, feel free to open a discussion or draft pull request.

Consistency, clarity, and maintainability are key values in VaLib.
All contributions are welcome, as long as they follow these principles.

**Thank you for helping improve VaLib!**
# Contributing to VaLib
Thank you for considering contributing to VaLib!
We welcome all kinds of contributions – from bug reports and feature suggestions to full pull requests.
This document outlines the coding style and conventions used in the project to ensure consistency and maintainability across the codebase.

Please read the following guidelines before submitting pull requests or patches.

## Table of Contents
- [Licensing](#licensing)
- [Commit Style](#commit-style)
- [C++ Style Guide](#c-style-guide)
- [Error Handling](#error-handling)
- [Build](#build)
- [Testing](#testing)
- [Submitting Contributions](#submitting-contributions)

## Licensing
By contributing to this project, you agree that your code will be licensed under the same terms as VaLib (GNU General Public License v3.0 or later).

## Commit Style
- Use clear, concise commit messages written in imperative mood (e.g., `Add benchmark for MatrixMultiply`).
- Start the message with a capital letter.
- Avoid messages like `fix bug`, `update`, or `stuff`; instead, be descriptive.
- If the commit fixes a specific issue, reference it using `Fixes #issue_number`.

## C++ Style Guide
1. **Naming Conventions**
   - Use `PascalCase` for:
     - Class names, Static method names
     - Enum names, Enum values
   - Use `camelCase` for everything else, including variables, non-static methods, function names, parameters.

2. **Control Structures**
   - Omit braces (`{}`) in conditional statements and loops only if the body is simple and clearly readable, such as a single `x++`, `break`, or a function call.

3. **Indentation**
   - Use 4 spaces per indentation level. Do not use tabs.

4. **Access Modifiers**
   - Prefer `protected` over `private` when possible to encourage extensible class designs.

5. **Namespaces**
   - Avoid `using namespace std;` or `using namespace va;` entirely. Always use explicit namespace prefixes to prevent naming conflicts.

6. **Namespace and Class Prefixes**
   - All code should reside in the `va` namespace.
   - Alternatively, classes may be prefixed with `Va`, for example: `VaMyClassName`.
   - Exceptions are allowed if justified, but the default should follow this rule.
   - - Similar names are allowed but should not be overused. For example:
     - `VaMap` could be a class representing a map.
     - `va::map` could be a function that maps a list using a given function.
     - While this can improve readability in some cases, excessive use of similar names may lead to confusion and should be avoided.

7. **Testing and Benchmarking**
   - After adding a new class or functionality, include a corresponding test in the `testing/` directory.
   - Optionally, add a benchmark if performance is a concern.
   - Small utility functions or trivial changes may be exempt from this rule, but use discretion.

8. **Operators**
   - Modifying operators (`operator=`, `operator+=`, etc.) should be implemented as member functions.
   - Non-modifying operators (`operator==`, `operator+`, `operator<`, etc.) should be implemented as friend functions.
   - For non-modifying operators, use parameter names `lhs` and `rhs` for clarity.

## Error Handling
VaLib uses exceptions (`throw`/`try`/`catch`) for error handling where appropriate. Since exceptions only affect performance when thrown, they are suitable for signaling unexpected errors in non-critical paths. The API is documented to clarify which operations may throw.

Where performance is critical, VaLib provides alternative interfaces that do not perform any error checking and do not throw exceptions. These versions assume the input is valid and may result in undefined behavior (UB) if misused. This design puts responsibility on the user to ensure correctness, in exchange for maximum efficiency.

To assist with this, VaLib also offers explicit validation utilities, allowing users to check their inputs beforehand if needed, without relying on exceptions.

## Build
To build VaLib, follow the instructions provided in [BUILD.md](./BUILD.md).

## Testing
See [BUILD.md](./BUILD.md) for instructions on testing.

## Submitting Contributions
- When submitting a pull request, provide a clear and concise description of the change and its purpose.
- If you encounter a bug or unexpected behavior, please report it via the issue tracker, even if you are not sure about the root cause.
- If you are unsure whether your proposed change aligns with the project's direction, feel free to open a discussion or draft pull request.

Consistency, clarity, and maintainability are key values in VaLib.
All contributions are welcome, as long as they follow these principles.

**Thank you for helping improve VaLib!**

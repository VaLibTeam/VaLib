// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <Types/String.hpp>
#include <cstdio>
#include <iostream>

namespace va {

/**
 * @brief Formats a string without any arguments.
 * 
 * @param format The format string.
 * @return The same format string as no arguments are provided.
 */
inline VaString sprintf(const VaString& format);

/**
 * @brief Formats a string using a custom implementation of `printf`-style formatting.
 * 
 * Supported format specifiers:
 * - %d: Formats an integer.
 * - %f: Formats a floating-point number (supports float, @ref float32, double, @ref float64).
 * - %s: Formats a string (supports @ref VaString, and @ref VaImmutableString, c-style strings `char*`)
 * - %c: Formats a single character (char or @ref rune).
 * - %t: Formats a boolean as "true" or "false".
 * - %p: Formats an pointer.
 * @todo Add more format specifiers
 * 
 * Flags:
 * - `-`: Left-justifies the output within the specified width.
 * - `0`: Pads the output with zeros instead of spaces.
 * 
 * @tparam T The type of the current argument to format.
 * @tparam Args The types of the remaining arguments.
 * @param format The format string containing specifiers.
 * @param value The value to replace the first specifier in the format string.
 * @param args The remaining arguments to format.
 * @return A formatted string with the specifiers replaced by the provided arguments.
 */
template <typename T, typename... Args>
VaString sprintf(const VaString& format, T value, Args... args);

/**
 * @brief Prints a formatted string to the standard output.
 * 
 * This function uses the `sprintf` function to format the string and then prints it
 * to the standard output using `std::fprintf`.
 * 
 * @tparam T The type of the current argument to format.
 * @tparam Args The types of the remaining arguments.
 * @param format The format string containing specifiers.
 * @param value The value to replace the first specifier in the format string.
 * @param args The remaining arguments to format.
 */
template <typename T, typename... Args>
void printf(const VaString& format, T value, Args... args);

/**
 * @brief Prints a formatted string to the standard output followed by a newline.
 * 
 * This function uses the `sprintf` function to format the string and then prints it
 * to the standard output using `std::fprintf`, appending a newline character at the end.
 * 
 * @tparam T The type of the current argument to format.
 * @tparam Args The types of the remaining arguments.
 * @param format The format string containing specifiers.
 * @param value The value to replace the first specifier in the format string.
 * @param args The remaining arguments to format.
 */
template <typename T, typename... Args>
void printlnf(const VaString& format, T value, Args... args);

} // namespace va

#include <format.tpp>
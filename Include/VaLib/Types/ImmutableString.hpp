// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <VaLib/Types/BasicTypedef.hpp>
#include <VaLib/Meta/BasicDefine.hpp>

#include <iterator>

class VaString;

/**
 * @brief Immutable String implementation for VaLib.
 * @ingroup String.
 *
 * @note VaImmutableString is immutable, meaning it is not a good choice if you perform many string operations.
 * @note If you need a mutable string, you can use @ref VaString.
 */
class VaImmutableString {
  protected:
    /**
     * @brief Variable storing the length of the string.
     */
    Size len;

    /**
     * @brief Raw string data.
     *
     * @note Data is not null-terminated. The length is managed by the @ref len variable.
     */
    char* data;

  protected friends:
    friend class VaString;

  public:
    VaImmutableString();
    VaImmutableString(const std::string& str);
    VaImmutableString(const char* str);
    VaImmutableString(const char* str, Size size);
    VaImmutableString(const VaImmutableString& str);
    VaImmutableString(const VaString& str);
    VaImmutableString(VaImmutableString&& str) noexcept;

    ~VaImmutableString();

    /**
     * @brief Creates a VaImmutableString object from a given C-style string.
     * @tparam N The size of the input C-style string.
     * @param str A reference to a constant C-style string array.
     * @return VaImmutableString An instance of VaImmutableString initialized with the input string and its size.
     *
     * @note The input string must be a null-terminated C-style string.
     */
    template <Size N>
    static inline VaImmutableString Make(const char (&str)[N]) {
        return VaImmutableString(str, N);
    }

    VaImmutableString& operator=(const VaImmutableString& other);
    VaImmutableString& operator=(VaImmutableString&& other) noexcept;

    /**
     * @brief Concatenates two VaImmutableString objects into a new VaImmutableString.
     * @param str The VaImmutableString to concatenate with this one.
     * @return A new VaImmutableString containing the concatenated result.
     */
    VaImmutableString operator+(const VaImmutableString& str) const;

    /**
     * @brief Concatenates a VaImmutableString with a null-terminated C-style string.
     * @param str The C-style string to concatenate with this VaImmutableString.
     * @return A new VaImmutableString containing the concatenated result.
     */
    VaImmutableString operator+(const char* str) const;

    /**
     * @brief Appends a single character to this VaImmutableString.
     * @param ch The character to append.
     * @return A new VaImmutableString containing the result of appending the character.
     */
    VaImmutableString operator+(char ch) const;

    /**
     * @brief Appends the content of another VaImmutableString to this string.
     * @param other The VaImmutableString instance to append to this string.
     * @return VaImmutableString& A reference to the modified VaImmutableString instance.
     */
    inline VaImmutableString& operator+=(const VaImmutableString& other) {
        *this = *this + other;
        return *this;
    }

    /**
     * @brief Checks if two strings are the same.
     * @param other The other string to compare with this one
     */
    bool operator==(const VaImmutableString& other) const;

    /**
     * @brief Checks if two strings are *not* the same.
     * @param other The other string to compare with this one.
     *
     * @note Inverse of @ref operator==.
     */
    bool operator!=(const VaImmutableString& other) const;

    /**
     * @brief Converts VaImmutableString to a std::string from C++ stdlib.
     * @return std::string containing data from VaImmutableString (data is copied)
     */
    std::string toStdString() const;

    /**
     * @brief Converts VaImmutableString to a C-style string (char pointer).
     * @return C-style string, which is a copy of the VaImmutableString's data.
     *
     * @warning This function uses new[] to allocate memory for the new string. You must manually release this memory with delete[] later
     */
    char* toCStyleString() const;

    /**
     * @brief Accessing specific characters in the string.
     * @param index The index of the character to return.
     *
     * @warning The char type in C++ is only 1 byte, so if the string contains non-ANSI characters, this may not work as expected.
     * @warning Since VaImmutableString is *immutable*, this function returns a copy, not a reference or modifiable value.
     *
     * @throw If the index is out of the string's bounds, the function throws an IndexOutOfRangeError
     */
    char operator[](Size index) const;

    /**
     * @brief Finds the first occurrence of a substring within the string.
     * @param str The substring to search for.
     * @return The index of the first occurrence of the substring, or npos if not found.
     */
    Size find(const VaImmutableString& str) const;

    /**
     * @brief Finds the first occurrence of a substring within the string.
     * @param str The substring to search for.
     * @return The index of the first occurrence of the substring, or npos if not found.
     */
    Size find(const char* str) const;

    TODO(_maqix_, "Add substr function.");
    // VaImmutableString substr(Size start, Size len = npos) const;

    /**
     * @brief Returns whether the string is empty.
     */
    bool isEmpty() const;

    /**
     * @brief Constant representing no position.
     *
     * @note Returned by various search methods (e.g., @ref find) when nothing is found.
     */
    static constexpr Size npos = -1;

    /**
     * @brief Returns the length of the given VaImmutableString.
     * @param str The string whose length is returned.
     * @return The length of the string.
     */
    friend inline Size len(const VaImmutableString& str) { return str.len; }

  public iterators:
    // Both iterators are const because VaImmutableString is immutable
    using Iterator = const char*;
    using ConstIterator = const char*;
    using ReverseIterator = std::reverse_iterator<Iterator>;
    using ConstReverseIterator = std::reverse_iterator<ConstIterator>;

    inline Iterator begin() const { return data; };
    inline Iterator end() const { return data + len; };

    inline ConstIterator cbegin() const { return data; };
    inline ConstIterator cend() const { return data + len; };

    inline ReverseIterator rbegin() const { return ReverseIterator(end()); }
    inline ReverseIterator rend() const { return ReverseIterator(begin()); }

    inline ConstReverseIterator crbegin() const { return ConstReverseIterator(cend()); }
    inline ConstReverseIterator crend() const { return ConstReverseIterator(cbegin()); }
};

inline VaImmutableString operator"" _Vis(const char* str, Size size) {
    return VaImmutableString(str, size);
}

std::ostream& operator<<(std::ostream& os, VaImmutableString str);

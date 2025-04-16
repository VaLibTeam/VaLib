// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <VaLib/Types/BasicTypedef.hpp>
#include <memory>

#include <cstring>

#if __cplusplus >= 202002L
#include <span>
#endif

class VaImmutableString;

/**
 * @class VaString string implementation for VaLib
 * A dynamic string class for managing and manipulating character strings.
 * 
 * @note VaString is mutable.
 */
class VaString {
  protected:
    /**
     * @brief The current length of the string.
     */
    Size len;

    /**
     * @brief The current capacity of the string buffer.
     */
    Size cap;

    /**
     * @brief raw string data.
     * 
     * @note data is not null-terminated. The length is managed by the @ref len variable
     */
    char* data;

    /**
     * @brief Resizes the string buffer to a new capacity.
     * @param newCap The new capacity for the string buffer.
     */
    void resize(Size newCap);

    friend class VaImmutableString;

  public:
    /**
     * @brief Default constructor. Initializes an empty string.
     */
    VaString() noexcept;

    /**
     * @brief Constructs a VaString from a std::string.
     * @param str The std::string to initialize from.
     */
    VaString(const std::string& str) noexcept;

    /**
     * @brief Constructs a VaString from a C-style string.
     * @param str The C-style string to initialize from.
     */
    VaString(const char* str) noexcept;

    /**
     * @brief Constructs a VaString from a C-style string with a specified size
     * @param str The C-style string to initialize from
     * @param size The number of characters to copy
     */
    VaString(const char* str, Size size) noexcept;

    VaString(Size count, char c) noexcept;

    /**
     * @brief Copy constructor. Creates a copy of another VaString
     * @param other The VaString to copy from
     */
    VaString(const VaString& other) noexcept;

    /**
     * @brief Constructs a VaString from a VaImmutableString
     * @param other The VaImmutableString to initialize from
     */
    VaString(const VaImmutableString& other) noexcept;

    /**
     * @brief Move constructor. Transfers ownership from another VaString
     * @param other The VaString to move from
     */
    VaString(VaString&& other) noexcept;

    /**
     * @brief Destructor. Releases the allocated memory.
     */
    ~VaString() noexcept;

    /**
     * @brief Creates a VaString object from a given C-style string.
     * 
     * @tparam N The size of the input C-style string.
     * @param str A reference to a constant C-style string array.
     * @return VaString An instance of VaString initialized with the input string and its size.
     * 
     * @note The input string must be a null-terminated C-style string.
     */
    template <Size N>
    static inline VaString Make(const char (&str)[N]) {
        return VaString(str, N);
    }

    /**
     * @brief Ensures that the string has at least the specified minimum capacity.
     * 
     * If the current capacity of the string is less than the specified minimum 
     * capacity, the string's capacity is increased to meet the requirement.
     * 
     * @param minCap The minimum capacity to reserve for the string.
     */
    inline void reserve(Size minCap) {
        if (minCap > cap) resize(minCap);
    }

    Size hash() const {
        constexpr int64 fnvOffset = 14695981039346656037ull;
        constexpr int64 fnvPrime = 1099511628211ull;
    
        Size h = fnvOffset;
        for (Size i = 0; i < len; ++i) {
            h ^= static_cast<uint64>(static_cast<uint8>(data[i]));
            h *= fnvPrime;
        }
    
        return h;
    }

    /**
     * @brief Copy assignment operator. Copies the content of another VaString.
     * @param other The VaString to copy from.
     * @return Reference to the current object.
     */
    VaString& operator=(const VaString& other);

    /**
     * @brief Move assignment operator. Transfers ownership from another VaString
     * @param other The VaString to move from
     * @return Reference to the current object
     */
    VaString& operator=(VaString&& other) noexcept;

    /**
     * @brief Concatenates two VaStrings.
     * @param other The VaString to concatenate
     * @return A new VaString containing the concatenated result.
     */
    VaString operator+(const VaString& other) const;

    /**
     * @brief Concatenates a VaString with a C-style string.
     * @param str The C-style string to concatenate.
     * @return A new VaString containing the concatenated result.
     */
    VaString operator+(const char* str) const;

    /**
     * @brief Concatenates a VaString with a single character.
     * @param ch The character to concatenate.
     * @return A new VaString containing the concatenated result.
     */
    VaString operator+(char ch) const;

    /**
     * @brief Appends a substring to the current VaString object.
     * 
     * This function appends a specified number of characters from the given 
     * C-style string to the end of the current VaString object.
     * 
     * @param str A pointer to the C-style string to append.
     * @param strLen The number of characters to append from the provided string.
     *               If strLen is greater than the length of the string, behavior 
     *               is undefined.
     * 
     * @return A reference to the modified VaString object.
     */
    VaString& append(const char* str, Size strLen) noexcept;

    /**
     * @brief Appends a C-style string to the current VaString object.
     * 
     * This function appends the provided null-terminated C-style string (`str`)
     * to the current VaString object. The length of the string is determined
     * using `std::strlen`.
     * 
     * @param str A pointer to the null-terminated C-style string to append.
     * @return VaString& A reference to the current VaString object after the
     *         string has been appended.
     */
    inline VaString& append(const char* str) { return append(str, std::strlen(str)); }

    /**
     * @brief Appends another VaString to the current string.
     * @param other The VaString to append.
     * @return Reference to the current object.
     */
    VaString& operator+=(const VaString& other) noexcept;

    /**
     * @brief Appends a C-style string to the current string.
     * @param str The C-style string to append.
     * @return Reference to the current object.
     */
    inline VaString& operator+=(const char* str) { return append(str); }

    /**
     * @brief Appends a single character to the current string.
     * @param ch The character to append.
     * @return Reference to the current object.
     */
    VaString& operator+=(char ch) noexcept;

    /**
     * @brief Compares two VaStrings for equality.
     * @param other The VaString to compare with.
     * @return True if the strings are equal, false otherwise.
     */
    bool operator==(const VaString& other) const noexcept;

    /**
     * @brief Compares two VaStrings for inequality.
     * @param other The VaString to compare with.
     * @return True if the strings are not equal, false otherwise.
     */
    bool operator!=(const VaString& other) const noexcept;

    /**
     * @brief Provides access to a character at a specific index.
     * @param index The index of the character to access.
     * @return A reference to the character at the specified index.
     * 
     * @note No bounds checking is performed. Accessing an out-of-bounds index results in undefined behavior.
     */
    char& operator[](Size index) noexcept;

    /**
     * @brief Provides read-only access to a character at a specific index.
     * @param index The index of the character to access.
     * @return A constant reference to the character at the specified index.
     * 
     * @note No bounds checking is performed. Accessing an out-of-bounds index results in undefined behavior.
     */
    const char& operator[](Size index) const noexcept;

    /**
     * @brief Provides access to a character at a specific index with bounds checking.
     * @param index The index of the character to access.
     * @return A reference to the character at the specified index.
     * 
     * @throws IndexOutOfRangeError if the index is greater than or equal to the length of the string.
     */
    char& at(Size index);

    /**
     * @brief Provides read-only access to a character at a specific index with bounds checking.
     * @param index The index of the character to access.
     * @return A constant reference to the character at the specified index.
     * 
     * @throws IndexOutOfRangeError if the index is greater than or equal to the length of the string.
     */
    const char& at(Size index) const;

    /**
     * @brief Converts the VaString to a std::string.
     * @return A std::string containing the same content.
     */
    std::string toStdString() const;

    /**
     * @brief Converts the VaString to a C-style string.
     * @return A dynamically allocated C-style string containing the same content.
     * 
     * @warning This function uses new[] to allocate memory for the new string. You must manually release this memory with delete[] later
     */
    char* toCStyleString() const;

    /**
     * @brief Provides access to the internal C-style string representation.
     * @return A pointer to the internal character buffer of the string.
     * 
     * @note The returned pointer is not null-terminated and points to the internal data of the string.
     *       Modifying the data through this pointer will affect the VaString object.
     */
    char* getData() noexcept;

    /**
     * @brief  Provides access to the internal C-style string representation.
     * @return A constant pointer to the character array representing the string data.
     * 
     * @note The returned pointer isn't null-terminated and must not be modified.
     */
    const char* getData() const noexcept;

#if __cplusplus >= 202002L
    inline std::span<char> span() noexcept { return std::span<char>(data, len); }
    inline std::span<const char> span() const noexcept { return std::span<const char>(data, len); }
#endif

    /**
     * @brief Checks if the string is empty.
     * @return True if the string is empty, false otherwise.
     */
    bool isEmpty() const;

    /**
     * @brief Finds the first occurrence of a substring within the string.
     * @param substr The substring to search for.
     * @return The starting index of the substring if found, or npos if not found.
     */
    Size find(const VaString& substr) const;

    /**
     * @brief Extracts a substring from the string.
     * @param start The starting index of the substring.
     * @param length The length of the substring. Defaults to npos, which extracts to the end of the string.
     * @return A VaString containing the extracted substring.
     */
    VaString substr(Size start, Size length = npos) const;

    /**
     * @brief Inserts a substring into the current VaString object at a specified position.
     * @param pos The position at which to insert the substring. Must be less than or equal to the length of the string.
     * @param str A pointer to the C-style string to insert.
     * @param strLen The number of characters to insert from the provided string.
     * 
     * @return VaString& A reference to the modified VaString object.
     * 
     * @throws IndexOutOfRangeError if the position is greater than the length of the string.
     */
    VaString& insert(Size pos, const char* str, Size strLen);

    /**
     * @brief Inserts a C-style string into the current VaString object at a specified position.
     * @param pos The position at which to insert the string. Must be less than or equal to the length of the string.
     * @param str A pointer to the null-terminated C-style string to insert.
     * @return VaString& A reference to the modified VaString object.
     * 
     * @throws IndexOutOfRangeError if the position is greater than the length of the string.
     */
    inline VaString& insert(Size pos, const char* str) {
        return insert(pos, str, std::strlen(str));
    }

    /**
     * @brief Inserts another VaString into the current VaString object at a specified position.
     * @param pos The position at which to insert the VaString. Must be less than or equal to the length of the string.
     * @param other The VaString to insert.
     * @return VaString& A reference to the modified VaString object.
     * 
     * @throws IndexOutOfRangeError if the position is greater than the length of the string.
     */
    inline VaString& insert(Size pos, const VaString& other) {
        return insert(pos, other.data, other.len);
    }

    inline char* begin() noexcept { return data; }
    inline char* end() noexcept { return data + len; }
    inline const char* begin() const noexcept { return data; }
    inline const char* end() const noexcept { return data + len; }

    /**
     * @brief Represents an invalid position constant.
     */
    static constexpr Size npos = -1;

    /**
     * @brief Retrieves the length of the string.
     * @param str The VaString to query.
     * @return The length of the string.
     */
    friend inline Size len(const VaString& str) { return str.len; }

    /**
     * @brief Retrieves the capacity of the string buffer.
     * @param str The VaString to query.
     * @return The capacity of the string buffer.
     */
    friend inline Size cap(const VaString& str) { return str.cap; }
};

namespace std {

template <>
struct hash<VaString> {
    Size operator()(const VaString& str) {
        return str.hash();
    }
};

}

inline VaString operator"" _Vs(const char* str, Size size) { return VaString(str, size); }

std::ostream& operator<<(std::ostream& os, const VaString& str);
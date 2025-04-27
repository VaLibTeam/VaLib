// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam

#include <VaLib/Types/BasicTypedef.hpp>
#include <VaLib/Types/Error.hpp>
#include <VaLib/Types/ImmutableString.hpp>
#include <VaLib/Types/String.hpp>

#include <algorithm>
#include <cstring>
#include <stdexcept>
#include <string>

#define GROWTH_SEQ ((Size)(cap * 2))

VaString::VaString() noexcept : len(0), cap(0) { data = nullptr; }

VaString::VaString(const std::string& str) noexcept : len(str.size()), cap(str.size()) {
    data = new char[cap];
    std::memcpy(data, str.data(), len);
}

VaString::VaString(const VaImmutableString& str) noexcept : len(str.len), cap(str.len) {
    data = new char[cap];
    std::memcpy(data, str.data, len);
}

VaString::VaString(const char* str) noexcept : len(std::strlen(str)), cap(len) {
    data = new char[cap];
    std::memcpy(data, str, len);
}

VaString::VaString(const char* str, Size size) noexcept : len(size), cap(size) {
    data = new char[cap];
    std::memcpy(data, str, len);
}

VaString::VaString(Size count, char c) noexcept : len(count), cap(count) {
    data = new char[cap];
    std::memset(data, c, len);
}

VaString::VaString(const VaString& other) noexcept : len(other.len), cap(other.cap) {
    data = new char[cap];
    std::memcpy(data, other.data, len);
}

VaString::VaString(VaString&& other) noexcept : len(other.len), cap(other.cap), data(other.data) {
    other.data = nullptr;
    other.len = 0;
    other.cap = 0;
}

VaString::~VaString() noexcept { delete[] data; }

void VaString::resize(Size newCap) {
    if (newCap <= cap) return;

    char* newData = new char[newCap];
    if (data) {
        std::memcpy(newData, data, len);
        delete[] data;
    }
    data = newData;
    cap = newCap;
}

VaString& VaString::operator=(const VaString& other) {
    if (this != &other) {
        delete[] data;
        len = other.len;
        cap = other.cap;
        data = new char[cap];
        std::memcpy(data, other.data, len);
    }
    return *this;
}

VaString& VaString::operator=(VaString&& other) noexcept {
    if (this != &other) {
        delete[] data;
        data = other.data;
        len = other.len;
        cap = other.cap;
        other.data = nullptr;
        other.len = 0;
        other.cap = 0;
    }
    return *this;
}

VaString VaString::operator+(const VaString& other) const {
    VaString result(*this);
    result += other;
    return result;
}

VaString VaString::operator+(const char* str) const {
    VaString result(*this);
    result += str;
    return result;
}

VaString VaString::operator+(char ch) const {
    VaString result(*this);
    result += ch;
    return result;
}

VaString& VaString::operator+=(const VaString& other) noexcept {
    const Size newLen = len + other.len;
    if (newLen > cap) {
        resize(std::max(newLen, GROWTH_SEQ));
    }
    std::memcpy(data + len, other.data, other.len);
    len = newLen;
    return *this;
}

VaString& VaString::operator+=(VaString&& other) noexcept {
    const Size newLen = len + other.len;
    if (newLen > cap) {
        resize(std::max(newLen, GROWTH_SEQ));
    }
    std::memcpy(data + len, other.data, other.len);
    len = newLen;
    other.data = nullptr;  // Move semantics: zero out other
    other.len = 0;
    other.cap = 0;
    return *this;
}

VaString& VaString::append(const char* str, Size strLen) noexcept {
    const Size newLen = len + strLen;
    if (newLen > cap) {
        resize(std::max(newLen, GROWTH_SEQ));
    }
    std::memcpy(data + len, str, strLen);
    len = newLen;
    return *this;
}

VaString& VaString::operator+=(char ch) noexcept {
    const Size newLen = len + 1;
    if (newLen > cap) {
        resize(std::max(newLen, GROWTH_SEQ));
    }
    data[len] = ch;
    len = newLen;
    return *this;
}

bool VaString::operator==(const VaImmutableString& other) const noexcept {
    if (this->len != other.len) return false;
    return std::memcmp(this->data, other.data, this->len) == 0;
}

bool operator==(const VaString& lhs, const VaString& rhs) noexcept {
    if (&lhs == &rhs) return true;
    if (lhs.len != rhs.len) return false;

    return std::memcmp(lhs.data, rhs.data, lhs.len) == 0;
}

bool operator<(const VaString& lhs, const VaString& rhs) {
    Size minLen = lhs.len < rhs.len ? lhs.len : rhs.len;

    for (Size i = 0; i < minLen; i++) {
        if (lhs.data[i] < rhs.data[i]) return true;
        if (lhs.data[i] > rhs.data[i]) return false;
    }
    return lhs.len < rhs.len;
}

char& VaString::operator[](Size index) noexcept { return data[index]; }
const char& VaString::operator[](Size index) const noexcept { return data[index]; }

char& VaString::at(Size index) {
    if (index >= len) throw IndexOutOfRangeError(len, index);
    return data[index];
}

const char& VaString::at(Size index) const {
    if (index >= len) throw IndexOutOfRangeError(len, index);
    return data[index];
}

std::string VaString::toStdString() const { return std::string(data, len); }

char* VaString::toCStyleString() const {
    char* cstr = new char[len + 1];
    std::memcpy(cstr, data, len);
    cstr[len] = '\0';
    return cstr;
}

char* VaString::getData() noexcept { return data; }
const char* VaString::getData() const noexcept { return data; }

bool VaString::isEmpty() const { return len == 0; }

Size VaString::find(const VaString& substr) const {
    if (substr.len == 0 || len < substr.len) {
        return npos;
    }

    for (Size i = 0; i <= len - substr.len; ++i) {
        bool match = true;
        for (Size j = 0; j < substr.len; ++j) {
            if (data[i + j] != substr.data[j]) {
                match = false;
                break;
            }
        }
        if (match) {
            return i;
        }
    }
    return npos;
}

VaString VaString::substr(Size start, Size length) const {
    if (start >= len) {
        return "";
    }

    if (length == npos || start + length > len) {
        length = len - start;
    }

    return VaString(data + start, length);
}

VaString& VaString::insert(Size pos, const char* str, Size strLen) {
    if (pos > len) {
        throw IndexOutOfRangeError("Insert position is out of range.");
    }
    reserve(len + strLen);
    std::memmove(data + pos + strLen, data + pos, len - pos);
    std::memcpy(data + pos, str, strLen);
    len += strLen;
    return *this;
}

std::ostream& operator<<(std::ostream& os, const VaString& str) {
    return os.write(str.getData(), len(str));
}

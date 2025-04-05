// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam

#include <Types/ImmutableString.hpp>
#include <Types/String.hpp>

#include <cstring>
#include <stdexcept>

VaImmutableString::VaImmutableString() : len(0) { data = new char[0]; }

VaImmutableString::VaImmutableString(const std::string& str) {
    len = str.size();
    data = new char[len];
    std::memcpy(data, str.data(), len);
}

VaImmutableString::VaImmutableString(const char* str) : len(std::strlen(str)) {
    data = new char[len];
    std::memcpy(data, str, len);
}

VaImmutableString::VaImmutableString(const char* str, Size length) : len(length) {
    data = new char[len];
    std::memcpy(data, str, len);
}

VaImmutableString::VaImmutableString(const VaImmutableString& other) : len(other.len) {
    data = new char[len];
    std::memcpy(data, other.data, len);
}

VaImmutableString::VaImmutableString(const VaString& str) : len(str.len) {
    data = new char[len];
    std::memcpy(data, str.data, len);
}

VaImmutableString::VaImmutableString(VaImmutableString&& other) noexcept
    : len(other.len), data(other.data) {
    other.data = nullptr;
    other.len = 0;
}

VaImmutableString::~VaImmutableString() { delete[] data; }

VaImmutableString VaImmutableString::operator+(const VaImmutableString& other) const {
    Size newLen = len + other.len;
    char* newData = new char[newLen];

    std::memcpy(newData, data, len);
    std::memcpy(newData + len, other.data, other.len);

    return VaImmutableString(newData, newLen);
}

VaImmutableString VaImmutableString::operator+(const char* str) const {
    Size strLen = std::strlen(str);
    Size newLen = len + strLen;
    char* newData = new char[newLen];

    std::memcpy(newData, data, len);
    std::memcpy(newData + len, str, strLen);

    return VaImmutableString(newData, newLen);
}

VaImmutableString VaImmutableString::operator+(char c) const {
    Size newLen = len + 1;
    char* newData = new char[newLen];

    std::memcpy(newData, data, len);
    newData[len] = c;

    return VaImmutableString(newData, newLen);
}

bool VaImmutableString::operator==(const VaImmutableString& other) const {
    if (len != other.len) return false;
    return std::memcmp(data, other.data, len) == 0;
}

bool VaImmutableString::operator!=(const VaImmutableString& other) const {
    return !(*this == other);
}

VaImmutableString& VaImmutableString::operator=(const VaImmutableString& other) {
    if (this != &other) {
        delete[] data;

        len = other.len;
        data = new char[len];
        std::memcpy(data, other.data, len);
    }
    return *this;
}

VaImmutableString& VaImmutableString::operator=(VaImmutableString&& other) noexcept {
    if (this != &other) {
        delete[] data;

        data = other.data;
        len = other.len;

        other.data = nullptr;
        other.len = 0;
    }
    return *this;
}

std::string VaImmutableString::toStdString() const { return std::string(data, len); }

char* VaImmutableString::toCStyleString() const {
    char* cstr = new char[len + 1];
    std::memcpy(cstr, data, len);
    cstr[len] = '\0';
    return cstr;
}

char VaImmutableString::operator[](size_t index) const {
    if (index >= len) throw std::out_of_range("Index out of range");
    return data[index];
}

bool VaImmutableString::isEmpty() const { return len == 0; }

std::ostream& operator<<(std::ostream& os, VaImmutableString str) {
    os << str.toStdString();
    return os;
}
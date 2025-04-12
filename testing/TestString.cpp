// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam

#include <VaLib/Types/ImmutableString.hpp>
#include <VaLib/Types/String.hpp>
#include <VaLib/Utils/ToString.hpp>

#include <lib/testing.hpp>

#include <cstring>
#include <iostream>
#include <ostream>

bool testString(testing::Test& t) {
    t.setState("testing concatenation of two strings");
    VaString str = "Hello, ";
    VaString str2 = "world!";

    VaString helloworld = str + str2;

    if (helloworld != "Hello, world!") {
        return t.fail("Unexpected result when concatenating strings");
    }

    t.setState("testing conversions from other string types (std::string, char*, ...)");
    std::string stdstring = "TEST";
    VaString test(stdstring);

    if (test != "TEST" || test != stdstring) {
        return t.fail("Conversion from std::string to VaString failed");
    }

    const char* charptr = "123 ... hello, world?";
    test = charptr;

    if (test != "123 ... hello, world?" || test != charptr) {
        return t.fail("Conversion from const char* to VaString failed");
    }

    t.setState(
        "testing conversions to other string types (std::string, char*, VaImmutableString, ...)");
    test = "Hello, mars?";
    std::string s1 = test.toStdString();

    if (s1 != "Hello, mars?") return false;

    char* s2 = test.toCStyleString();
    if (std::strcmp(s2, "Hello, mars?") != 0) {
        return t.fail("Conversion from VaString to char* failed");
    }
    delete[] s2; //! important: free memory

    VaImmutableString imstr = "Goodbye world!";
    VaString s3 = imstr;

    if (s3 != "Goodbye world!" || s3 != imstr) {
        return t.fail("Conversion from VaImmutableString to VaString failed");
    }

    t.setState("testing a string with null terminators");
    VaString strWithNullTerminator = VaString::Make("\0Hello, \0World!\0\0");
    if (strWithNullTerminator != VaString::Make("\0Hello, \0World!\0\0") ||
        strWithNullTerminator[0] != '\0') {
        return t.fail("Unexpected result when creating a string with zero bytes");
    }

    return t.success();
}

int main() { return testing::run(testString); }
// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam

#include <Types/String.hpp>
#include <Types/ImmutableString.hpp>
#include <toString.hpp>

#include <lib/testing.hpp>

#include <iostream>
#include <ostream>
#include <cstring>

bool testString(testing::Test& t) {
    t.setState("testing concatenation of two strings");
    VaImmutableString str = "Hello, ";
    VaImmutableString str2 = "world!";

    VaImmutableString helloworld = str + str2;

    if (helloworld != "Hello, world!") {
        return t.fail("Unexpected result when concatenating strings");
    }

    t.setState("testing conversions from other string types (std::string, char*, ...)");
    std::string stdstring = "TEST";
    VaImmutableString test(stdstring);

    if (test != "TEST" || test != stdstring) {
        return t.fail("Conversion from std::string to VaString failed");
    }

    const char* charptr = "123 ... hello, world?";
    test = charptr;

    if (test != "123 ... hello, world?" || test != charptr) {
        return t.fail("Conversion from const char* to VaString failed");
    }

    t.setState("testing conversions to other string types (std::string, char*, VaString, ...)");
    test = "Hello, mars?";
    std::string s1 = test.toStdString();

    if (s1 != "Hello, mars?") return false;

    char* s2 = test.toCStyleString();
    if (std::strcmp(s2, "Hello, mars?") != 0) {
        return t.fail("Conversion from VaString to char* failed");
    }
    delete[] s2; //! important: free memory

    VaString mstr = "Goodbye world!";
    VaImmutableString s3 = mstr;

    if (s3 != "Goodbye world!" || s3 != mstr) {
        return t.fail("Conversion from VaImmutableString to VaString failed");
    }

    return t.success();
}

int main() {
    return testing::run(testString);
}
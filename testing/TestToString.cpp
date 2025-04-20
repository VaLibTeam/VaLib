#include <lib/testing.hpp>

#include <VaLib/Types/String.hpp>
#include <VaLib/Utils/ToString.hpp>

bool testToString(testing::Test& t) {
    using namespace va;

    // int
    VaList<VaPair<int, VaString>> intTests = {
        {0, "0"}, {1, "1"}, {-1, "-1"}, {818, "818"}, {-123, "-123"}, {1000000, "1000000"},
    };

    for (auto& [val, expected] : intTests) {
        if (toString(val) != expected) {
            return t.fail("int toString failed: " + toString(val));
        }
    }

    // uint
    VaList<VaPair<uint, VaString>> uintTests = {
        {0u, "0"},
        {1u, "1"},
        {123u, "123"},
        {4294967295u, "4294967295"},
    };

    for (auto& [val, expected] : uintTests) {
        if (toString(val) != expected) {
            return t.fail("uint toString failed: " + toString(val));
        }
    }

    // float
    VaList<VaPair<float32, VaString>> floatTests = {
        {0.0f, "0.0"},   {1.0f, "1.0"},       {-1.0f, "-1.0"},     {0.5f, "0.5"},
        {2.25f, "2.25"}, {10.125f, "10.125"}, {-0.125f, "-0.125"},
    };

    for (auto& [val, expected] : floatTests) {
        if (toString(val) != expected) {
            return t.fail("float toString failed: " + toString(val));
        }
    }

    // float64
    VaList<VaPair<float64, VaString>> float64Tests = {
        {0.0, "0.0"},   {1.0, "1.0"},       {-1.0, "-1.0"},         {0.5, "0.5"},
        {2.25, "2.25"}, {10.125, "10.125"}, {-0.125, "-0.125"},     {100.0, "100.0"},
        {0.25, "0.25"}, {0.375, "0.375"},   {123456.0, "123456.0"},
    };

    for (auto& [val, expected] : float64Tests) {
        if (toString(val) != expected) {
            return t.fail("double toString failed: " + toString(val));
        }
    }

    // c-string
    VaList<VaPair<const char*, VaString>> cstrTests = {
        {"hello", "hello"},
        {"", ""},
        {"world", "world"},
    };

    for (auto& [val, expected] : cstrTests) {
        if (toString(val) != expected) {
            return t.fail("char* toString failed: " + toString(val));
        }
    }

    // std::string
    VaList<VaPair<std::string, VaString>> stringTests = {
        {"hello", "hello"},
        {"", ""},
        {"VaLib", "VaLib"},

        {"If you're reading this", "If you're reading this"},
        {"just know I can't be bothered to write these tests",
         "just know I can't be bothered to write these tests"},
    };

    for (auto& [val, expected] : stringTests) {
        if (toString(val) != expected) {
            return t.fail("std::string toString failed: " + toString(val));
        }
    }

    return t.success();
}

int main() { return testing::run(testToString); }
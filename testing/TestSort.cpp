// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam

#define VaLib_USE_CONCEPTS

#include <lib/testing.hpp>

#include <Types/List.hpp>
#include <VaLib.hpp>

#include <Utils/sort.hpp>

bool testSort(testing::Test& t) {
    try {

        VaList<Function<void, VaSlice<int>&>> sorts = {
            va::sort::bubble<int>,
            va::sort::merge<int>,
            va::sort::quick<int>,
        };

        VaList<int> testData;
        for (int i = 0; i < len(sorts); i++) {
            testData = {1, 4, 5, 2, 5, 6, 2, 5, 7, 2, 5, 9, 2};
            VaSlice<int> slice = testData;
            sorts[i](slice);

            if (testData != VaList<int>{1, 2, 2, 2, 2, 4, 5, 5, 5, 5, 6, 7, 9}) {
                VaList<VaString> strArr;
                for (auto& n : testData) {
                    strArr.append(VaString(std::to_string(n)));
                }

                VaString msg = VaString("sort failed. expected: {1, 2, 2, 2, 2, 4, ...}, got: {") +
                               strArr.join(", ") + "}.";
                t.fail(msg);
            }
        }

        // VaList<int> testData = {1, 4, 5, 2, 5, 6, 2, 5, 7, 2, 5, 9, 2};
        // VaSlice<int> testSlice = testData;
        // va::sort::bubble(testSlice);

        // for (int i = 0; i < len(testData); i++) {
        //     std::cout << testData.at(i) << ", ";
        // }

        return t.success();
    } catch (ValueError err) {
        std::cout << err.what() + "\n";
        return false;
    }
}

int main() { return testing::run(testSort); }
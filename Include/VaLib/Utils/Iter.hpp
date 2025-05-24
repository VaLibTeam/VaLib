// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <VaLib/Meta/BasicDefine.hpp>

#include <iterator>

template <typename Container>
class VaReversedIter {
    Container& container;

public:
    VaReversedIter(Container& c) : container(c) {}

    auto begin() const { return std::rbegin(container); }
    auto end() const { return std::rend(container); }
};

#if __cplusplus >= CPP20
    template <typename Container>
    VaReversedIter(Container& c) -> VaReversedIter<Container>;
#endif

namespace va {

template <typename Container>
VaReversedIter<Container> reverseIterator(Container& c) {
    return VaReversedIter<Container>(c);
}

template <typename Container1, typename Container2>
bool iterableEql(const Container1& c1, const Container2& c2) {
    return std::equal(std::begin(c1), std::end(c1), std::begin(c2), std::end(c2));
}

}

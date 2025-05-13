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

#endif

namespace va {

template <typename Container>
VaReversedIter<Container> reverseIterator(Container& c) {
    return VaReversedIter<Container>(c);
}

}

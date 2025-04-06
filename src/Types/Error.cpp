// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam

#include <Types/Error.hpp>

BaseError::BaseError(VaString m) : msg(m) {}

VaString BaseError::what() const { return msg; }

IndexOutOfTheRangeError::IndexOutOfTheRangeError(VaString m) : BaseError(m) {}

IndexOutOfTheRangeError::IndexOutOfTheRangeError(Size range, Size index) {
    *this = va::sprintf("index out of range [%d] with length %d", index, range);
}
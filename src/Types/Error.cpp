// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam

#include <VaLib/Types/Error.hpp>
#include <VaLib/Utils/format.hpp>

BaseError::BaseError(VaString m) : msg(m) {}

VaString BaseError::what() const { return msg; }

IndexOutOfRangeError::IndexOutOfRangeError(VaString m) : IndexError(m) {}
IndexOutOfRangeError::IndexOutOfRangeError(Size range, Size index) {
    msg = va::sprintf("index out of range [%d] with length %d", index, range);
}

// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <VaLib/Types/BasicTypedef.hpp>
#include <VaLib/Types/Slice.hpp>

/// @file sort.hpp
/// @warning DEPRECATED.

namespace va {
namespace sort {

template <typename T>
void bubble(VaSlice<T>& slice) {
    for (Size i = 0; i < len(slice); i++) {
        for (Size j = 0; j < len(slice) - 1; j++) {
            if (slice[j] > slice[j + 1]) {
                std::swap(slice[j], slice[j + 1]);
            }
        }
    }
}

template <typename T>
void merge(VaSlice<T>& slice) {
    if (len(slice) <= 1) {
        return;
    }

    Size mid = len(slice) / 2;
    VaSlice<T> left = slice.subslice(0, mid);
    VaSlice<T> right = slice.subslice(mid, len(slice) - mid);

    merge(left);
    merge(right);

    VaList<T> merged;
    merged.reserve(len(slice));

    Size i = 0, j = 0;
    while (i < len(left) && j < len(right)) {
        if (left[i] <= right[j]) {
            merged.append(left[i++]);
        } else {
            merged.append(right[j++]);
        }
    }

    while (i < len(left)) {
        merged.append(left[i++]);
    }

    while (j < len(right)) {
        merged.append(right[j++]);
    }

    for (Size i = 0; i < len(merged); i++) {
        slice[i] = std::move(merged[i]);
    }
}

template <typename T>
void merge(VaList<T>& list) {
    sort::merge(VaSlice<T>(list));
}

template <typename T>
void quick(VaSlice<T>& slice) {
    if (len(slice) <= 1) {
        return;
    }

    T pivot = slice[len(slice) / 2];
    VaList<T> leftList, middleList, rightList;

    for (const T& val: slice) {
        if (val < pivot) {
            leftList.append(val);
        } else if (val == pivot) {
            middleList.append(val);
        } else {
            rightList.append(val);
        }
    }

    VaSlice<T> left = VaSlice<T>(leftList);
    VaSlice<T> right = VaSlice<T>(rightList);

    sort::quick(left);
    sort::quick(right);

    Size index = 0;
    for (const T& val: left) {
        slice[index++] = val;
    }
    for (const T& val: middleList) {
        slice[index++] = val;
    }
    for (const T& val: right) {
        slice[index++] = val;
    }
}

template <typename T>
void quick(VaList<T>& list) {
    sort::quick(VaSlice<T>(list));
}

}
} // namespace va::sort

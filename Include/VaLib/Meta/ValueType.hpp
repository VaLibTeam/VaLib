// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#if __cplusplus >= CPP17
    template <auto V>
    struct VaValueType {
        using Type = decltype(V);
    };

    template <auto V>
    using VaValueType_t = typename VaValueType<V>::Type;

    template <auto Val>
    class VaTypedValue {
      public:
        using Type = decltype(Val);
        static constexpr auto value = Val;
    };
#endif

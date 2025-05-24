// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <VaLib/Compability.hpp>
#include <type_traits>

using TrueType = std::true_type;
using FalseType = std::false_type;

namespace tt {

template <typename...>
using Void = void;

// --- Has Less Than
namespace detail {
template<typename T, typename = decltype(std::declval<T>() < std::declval<T>())>
TrueType HasLessThanHelper(int);

template<typename T>
FalseType HasLessThanHelper(...);
}

template<typename T>
struct HasLessThan: decltype(detail::HasLessThanHelper<T>(0)) {};

template<typename T>
constexpr bool HasLessThan_v = HasLessThan<T>::value;

// --- Has Greater Than
namespace detail {
template<typename T, typename = decltype(std::declval<T>() > std::declval<T>())>
TrueType HasGreaterThanHelper(int);

template<typename T>
FalseType HasGreaterThanHelper(...);
}

template<typename T>
struct HasGreaterThan: decltype(detail::HasGreaterThanHelper<T>(0)) {};

template<typename T>
constexpr bool HasGreaterThan_v = HasGreaterThan<T>::value;

// --- Has Equality Operator
template <typename T, typename = void>
struct HasEqualityOperator: FalseType {};

template <typename T>
struct HasEqualityOperator<T, Void<decltype(std::declval<T>() == std::declval<T>())>>: TrueType {};

template <typename T>
constexpr bool HasEqualityOperator_v = HasEqualityOperator<T>::value;

// --- Has Inequality Operator
template <typename T, typename = void>
struct HasInequalityOperator: FalseType {};

template <typename T>
struct HasInequalityOperator<T, Void<decltype(std::declval<T>() != std::declval<T>())>>: TrueType {};

template <typename T>
constexpr bool HasInequalityOperator_v = HasInequalityOperator<T>::value;

// --- Has hash method
template <typename T, typename = void>
struct HasHashMethodType: FalseType {};

template <typename T>
struct HasHashMethodType<T, Void<decltype(std::declval<T>().hash())>>: TrueType {};

template <typename T>
constexpr bool HasHashMethod = HasHashMethodType<T>::value;

// ----- Helper aliases ----- //
template <bool Expr, typename T = void>
using EnableIf = typename std::enable_if<Expr, T>::type;

template <bool Expr, typename T = void>
struct EnableIfType {
    using Type = typename std::enable_if<Expr, T>::type;
};

template <typename T>
using RemoveCV = typename std::remove_cv<T>::type;

template <typename T>
struct RemoveCVType {
    using Type = typename std::remove_cv<T>::type;
};

template <typename T>
using RemoveConst = typename std::remove_const<T>::type;

template <typename T>
struct RemoveConstType {
    using Type = typename std::remove_const<T>::type;
};

template <typename T>
using RemoveVolatile = typename std::remove_volatile<T>::type;

template <typename T>
struct RemoveVolatileType {
    using Type = typename std::remove_volatile<T>::type;
};

template <typename T>
using Decay = typename std::decay<T>::type;

template <typename T>
struct DecayType {
    using Type = typename std::decay<T>::type;
};

template <typename T>
using RemoveReference = typename std::remove_reference<T>::type;

template <typename T>
struct RemoveReferenceType {
    using Type = typename std::remove_reference<T>::type;
};

template <typename T>
using AddLValueReference = typename std::add_lvalue_reference<T>::type;

template <typename T>
struct AddLValueReferenceType {
    using Type = typename std::add_lvalue_reference<T>::type;
};

template <typename T>
using AddRValueReference = typename std::add_rvalue_reference<T>::type;

template <typename T>
struct AddRValueReferenceType {
    using Type = typename std::add_rvalue_reference<T>::type;
};

template <typename T>
using MakeSigned = typename std::make_signed<T>::type;

template <typename T>
struct MakeSignedType {
    using Type = typename std::make_signed<T>::type;
};

template <typename T>
using MakeUnsigned = typename std::make_unsigned<T>::type;

template <typename T>
struct MakeUnsignedType {
    using Type = typename std::make_unsigned<T>::type;
};

template <typename T>
using UnderlyingType = typename std::underlying_type<T>::type;

template <typename T>
struct UnderlyingTypeType {
    using Type = typename std::underlying_type<T>::type;
};

template <bool B, typename T, typename F>
using Conditional = typename std::conditional<B, T, F>::type;

template <bool B, typename T, typename F>
struct ConditionalType {
    using Type = typename std::conditional<B, T, F>::type;
};

template <typename... Ts>
using CommonType = typename std::common_type<Ts...>::type;

template <typename... Ts>
struct CommonTypeType {
    using Type = typename std::common_type<Ts...>::type;
};

template <typename T>
using RemovePointer = typename std::remove_pointer<T>::type;

template <typename T>
struct RemovePointerType {
    using Type = typename std::remove_pointer<T>::type;
};

template <typename T>
using AddPointer = typename std::add_pointer<T>::type;

template <typename T>
struct AddPointerType {
    using Type = typename std::add_pointer<T>::type;
};

template <typename T>
using ResultOf = typename std::result_of<T>::type;

template <typename T>
struct ResultOfType {
    using Type = typename std::result_of<T>::type;
};

template <typename T>
using RemoveCVRef = tt::RemoveReference<tt::RemoveCV<T>>;

template <typename T>
struct RemoveCVRefType {
    using Type = tt::RemoveReference<tt::RemoveCV<T>>;
};

// Traits
template <typename T>
constexpr bool IsConst = std::is_const<T>::value;

template <typename T>
struct IsConstType {
    static constexpr bool value = std::is_const<T>::value;
};

template <typename T>
constexpr bool IsVolatile = std::is_volatile<T>::value;

template <typename T>
struct IsVolatileType {
    static constexpr bool value = std::is_volatile<T>::value;
};

template <typename T>
constexpr bool IsPointer = std::is_pointer<T>::value;

template <typename T>
struct IsPointerType {
    static constexpr bool value = std::is_pointer<T>::value;
};

template <typename T>
constexpr bool IsReference = std::is_reference<T>::value;

template <typename T>
struct IsReferenceType {
    static constexpr bool value = std::is_reference<T>::value;
};

template <typename T>
constexpr bool IsLValueReference = std::is_lvalue_reference<T>::value;

template <typename T>
struct IsLValueReferenceType {
    static constexpr bool value = std::is_lvalue_reference<T>::value;
};

template <typename T>
constexpr bool IsRValueReference = std::is_rvalue_reference<T>::value;

template <typename T>
struct IsRValueReferenceType {
    static constexpr bool value = std::is_rvalue_reference<T>::value;
};

template <typename T>
constexpr bool IsArray = std::is_array<T>::value;

template <typename T>
struct IsArrayType {
    static constexpr bool value = std::is_array<T>::value;
};

template <typename T>
constexpr bool IsFunction = std::is_function<T>::value;

template <typename T>
struct IsFunctionType {
    static constexpr bool value = std::is_function<T>::value;
};

template <typename T>
constexpr bool IsArithmetic = std::is_arithmetic<T>::value;

template <typename T>
struct IsArithmeticType {
    static constexpr bool value = std::is_arithmetic<T>::value;
};

template <typename T>
constexpr bool IsIntegral = std::is_integral<T>::value;

template <typename T>
struct IsIntegralType {
    static constexpr bool value = std::is_integral<T>::value;
};

template <typename T>
constexpr bool IsFloatingPoint = std::is_floating_point<T>::value;

template <typename T>
struct IsFloatingPointType {
    static constexpr bool value = std::is_floating_point<T>::value;
};

template <typename T>
constexpr bool IsEnum = std::is_enum<T>::value;

template <typename T>
struct IsEnumType {
    static constexpr bool value = std::is_enum<T>::value;
};

template <typename T>
constexpr bool IsClass = std::is_class<T>::value;

template <typename T>
struct IsClassType {
    static constexpr bool value = std::is_class<T>::value;
};

template <typename T>
constexpr bool IsUnion = std::is_union<T>::value;

template <typename T>
struct IsUnionType {
    static constexpr bool value = std::is_union<T>::value;
};

template <typename T>
constexpr bool IsVoid = std::is_void<T>::value;

template <typename T>
struct IsVoidType {
    static constexpr bool value = std::is_void<T>::value;
};

template <typename T, typename U>
constexpr bool IsSame = std::is_same<T, U>::value;

template <typename T, typename U>
struct IsSameType {
    static constexpr bool value = std::is_same<T, U>::value;
};

template <typename Base, typename Derived>
constexpr bool IsBaseOf = std::is_base_of<Base, Derived>::value;

template <typename Base, typename Derived>
struct IsBaseOfType {
    static constexpr bool value = std::is_base_of<Base, Derived>::value;
};

template <typename From, typename To>
constexpr bool IsConvertible = std::is_convertible<From, To>::value;

template <typename From, typename To>
struct IsConvertibleType {
    static constexpr bool value = std::is_convertible<From, To>::value;
};

template <typename T>
constexpr bool IsSigned = std::is_signed<T>::value;

template <typename T>
struct IsSignedType {
    static constexpr bool value = std::is_signed<T>::value;
};

template <typename T>
constexpr bool IsUnsigned = std::is_unsigned<T>::value;

template <typename T>
struct IsUnsignedType {
    static constexpr bool value = std::is_unsigned<T>::value;
};

template <typename T>
constexpr bool IsTriviallyDestructible = std::is_trivially_destructible<T>::value;

template <typename T>
struct IsTriviallyDestructibleType {
    static constexpr bool value = std::is_trivially_destructible<T>::value;
};

template <typename T>
constexpr bool IsTriviallyCopyable = std::is_trivially_copyable<T>::value;

template <typename T>
struct IsTriviallyCopyableType {
    static constexpr bool value = std::is_trivially_copyable<T>::value;
};

template <typename T>
constexpr bool IsPod = std::is_trivial<T>::value && std::is_standard_layout<T>::value;

template <typename T>
struct IsPodType {
    static constexpr bool value = std::is_trivial<T>::value && std::is_standard_layout<T>::value;
};

template <typename T>
constexpr bool IsEmpty = std::is_empty<T>::value;

template <typename T>
struct IsEmptyType {
    static constexpr bool value = std::is_empty<T>::value;
};

template <typename T>
constexpr bool IsPolymorphic = std::is_polymorphic<T>::value;

template <typename T>
struct IsPolymorphicType {
    static constexpr bool value = std::is_polymorphic<T>::value;
};

template <typename T, typename... Args>
constexpr bool IsConstructible = std::is_constructible<T, Args...>::value;

template <typename T, typename... Args>
struct IsConstructibleType {
    static constexpr bool value = std::is_constructible<T, Args...>::value;
};

template <typename T>
constexpr bool IsDefaultConstructible = std::is_default_constructible<T>::value;

template <typename T>
struct IsDefaultConstructibleType {
    static constexpr bool value = std::is_default_constructible<T>::value;
};

template <typename T>
constexpr bool IsCopyConstructible = std::is_copy_constructible<T>::value;

template <typename T>
struct IsCopyConstructibleType {
    static constexpr bool value = std::is_copy_constructible<T>::value;
};

template <typename T>
constexpr bool IsMoveConstructible = std::is_move_constructible<T>::value;

template <typename T>
struct IsMoveConstructibleType {
    static constexpr bool value = std::is_move_constructible<T>::value;
};

template <typename T, typename... Args>
constexpr bool IsNoexceptConstructible = std::is_nothrow_constructible<T, Args...>::value;

template <typename T, typename... Args>
struct IsNoexceptConstructibleType {
    static constexpr bool value = std::is_nothrow_constructible<T, Args...>::value;
};

template <typename T>
constexpr bool IsNoexceptDefaultConstructible = std::is_nothrow_default_constructible<T>::value;

template <typename T>
struct IsNoexceptDefaultConstructibleType {
    static constexpr bool value = std::is_nothrow_default_constructible<T>::value;
};

template <typename T>
constexpr bool IsNoexceptCopyConstructible = std::is_nothrow_copy_constructible<T>::value;

template <typename T>
struct IsNoexceptCopyConstructibleType {
    static constexpr bool value = std::is_nothrow_copy_constructible<T>::value;
};

template <typename T>
constexpr bool IsNoexceptMoveConstructible = std::is_nothrow_move_constructible<T>::value;

template <typename T>
struct IsNoexceptMoveConstructibleType {
    static constexpr bool value = std::is_nothrow_move_constructible<T>::value;
};

template <typename T>
constexpr bool IsDestructible = std::is_destructible<T>::value;

template <typename T>
struct IsDestructibleType {
    static constexpr bool value = std::is_destructible<T>::value;
};

template <typename T>
constexpr bool IsNoexceptDestructible = std::is_nothrow_destructible<T>::value;

template <typename T>
struct IsNoexceptDestructibleType {
    static constexpr bool value = std::is_nothrow_destructible<T>::value;
};

template <typename T, typename U>
constexpr bool IsAssignable = std::is_assignable<T, U>::value;

template <typename T, typename U>
struct IsAssignableType {
    static constexpr bool value = std::is_assignable<T, U>::value;
};

template <typename T, typename U>
constexpr bool IsNoexceptAssignable = std::is_nothrow_assignable<T, U>::value;

template <typename T, typename U>
struct IsNoexceptAssignableType {
    static constexpr bool value = std::is_nothrow_assignable<T, U>::value;
};

template <typename T>
constexpr bool IsTriviallyConstructible = std::is_trivially_constructible<T>::value;

template <typename T>
struct IsTriviallyConstructibleType {
    static constexpr bool value = std::is_trivially_constructible<T>::value;
};

template <typename T>
constexpr bool IsTriviallyDefaultConstructible = std::is_trivially_default_constructible<T>::value;

template <typename T>
struct IsTriviallyDefaultConstructibleType {
    static constexpr bool value = std::is_trivially_default_constructible<T>::value;
};

template <typename T>
constexpr bool IsTriviallyCopyConstructible = std::is_trivially_copy_constructible<T>::value;

template <typename T>
struct IsTriviallyCopyConstructibleType {
    static constexpr bool value = std::is_trivially_copy_constructible<T>::value;
};

template <typename T>
constexpr bool IsTriviallyMoveConstructible = std::is_trivially_move_constructible<T>::value;

template <typename T>
struct IsTriviallyMoveConstructibleType {
    static constexpr bool value = std::is_trivially_move_constructible<T>::value;
};

template <typename T>
constexpr bool IsTriviallyCopyAssignable = std::is_trivially_copy_assignable<T>::value;

template <typename T>
struct IsTriviallyCopyAssignableType {
    static constexpr bool value = std::is_trivially_copy_assignable<T>::value;
};

template <typename T>
constexpr bool IsTriviallyMoveAssignable = std::is_trivially_move_assignable<T>::value;

template <typename T>
struct IsTriviallyMoveAssignableType {
    static constexpr bool value = std::is_trivially_move_assignable<T>::value;
};

} // namespace tt

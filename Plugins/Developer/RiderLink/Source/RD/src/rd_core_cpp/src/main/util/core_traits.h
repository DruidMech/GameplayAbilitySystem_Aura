#ifndef RD_CORE_CPP_TRAITS_H
#define RD_CORE_CPP_TRAITS_H

#include <types/Void.h>

#include <type_traits>
#include <string>

namespace rd
{
class IPolymorphicSerializable;

namespace util
{
// region non_std

template <typename T, typename U>
constexpr bool is_same_v = std::is_same<T, U>::value;

template <typename Base, typename Derived>
constexpr bool is_base_of_v = std::is_base_of<Base, Derived>::value;

template <bool B>
using bool_constant = std::integral_constant<bool, B>;

template <class B>
struct negation : bool_constant<!bool(B::value)>
{
};

template <class...>
struct disjunction : std::false_type
{
};
template <class B1>
struct disjunction<B1> : B1
{
};
template <class B1, class... Bn>
struct disjunction<B1, Bn...> : std::conditional_t<bool(B1::value), B1, disjunction<Bn...>>
{
};

template <class...>
struct conjunction : std::true_type
{
};
template <class B1>
struct conjunction<B1> : B1
{
};
template <class B1, class... Bn>
struct conjunction<B1, Bn...> : std::conditional_t<bool(B1::value), conjunction<Bn...>, B1>
{
};

template <class...>
using void_t = void;

template <typename F, typename... Args>
struct is_invocable
	: std::is_constructible<std::function<void(Args...)>, std::reference_wrapper<typename std::remove_reference<F>::type>>
{
};

template <typename R, typename F, typename... Args>
struct is_invocable_r
	: std::is_constructible<std::function<R(Args...)>, std::reference_wrapper<typename std::remove_reference<F>::type>>
{
};

template <class F, class... Ts>
constexpr bool is_invocable_v = is_invocable<F, Ts...>::value;

#ifdef __cpp_lib_is_invocable
template <class> struct result_of;

template <class F, class... TN>
struct result_of<F(TN...)>
{
	using type = std::invoke_result_t<F, TN...>;
};
#else
template<class... TN> using result_of = std::result_of<TN...>;
#endif
template<class... TN> using result_of_t = typename result_of<TN...>::type;

template <typename T>
constexpr bool is_enum_v = std::is_enum<T>::value;

// TO-DO: is is_pod actually required for memcpy-like serialization?
template <class T>
constexpr bool is_pod_v = std::is_trivial<T>::value && std::is_standard_layout<T>::value;
// endregion

template <typename T>
/*inline */ constexpr bool is_void = std::is_same<T, Void>::value;

// region in_heap

template <typename T>
//		using in_heap = disjunction<std::is_abstract<T>, std::is_same<T, std::wstring>>;
using in_heap = disjunction<std::is_base_of<IPolymorphicSerializable, T>, std::is_same<T, std::wstring>>;

template <typename T>
/*inline */ constexpr bool in_heap_v = in_heap<T>::value;

static_assert(in_heap_v<std::wstring>, "std::wstring should be placed in shared memory");
static_assert(!in_heap_v<int>, "int shouldn't be placed in shared memory");

// endregion

// region literal

template <typename T>
struct is_wstring_literal :
	/*std::is_same<
			T,
			std::add_lvalue_reference_t<const wchar_t[std::extent<std::remove_reference_t<T>>::value]>*/
	std::is_convertible<T, std::wstring>
{
};

template <typename T, bool = is_wstring_literal<T>::value>
struct not_string_literal
{
	using type = std::wstring;
};

template <>
struct not_string_literal<std::wstring, true>
{
	using type = std::wstring;
};

template <typename T>
struct not_string_literal<T, false>
{
	using type = std::decay_t<T>;
};

static_assert(is_wstring_literal<decltype(L" ")>::value, "is_wstring trait doesn't work");
static_assert(is_wstring_literal<decltype(L" ")&&>::value, "is_wstring trait doesn't work");
static_assert(is_wstring_literal<decltype(L" ")&>::value, "is_wstring trait doesn't work");
static_assert(is_wstring_literal<wchar_t const (&)[1]>::value, "is_wstring trait doesn't work");
static_assert(!is_wstring_literal<int>::value, "is_wstring trait doesn't work");
//		static_assert(is_wstring_literal<std::wstring>::value, "is_wstring trait doesn't work");

// endregion
}	 // namespace util
}	 // namespace rd

#endif	  // RD_CORE_CPP_TRAITS_H

#ifndef RD_CPP_GEN_UTIL_H
#define RD_CPP_GEN_UTIL_H

#include <std/hash.h>
#include <std/allocator.h>

#include <cstdlib>

namespace rd
{

template <template <class, class> class C, typename T, typename A = allocator<T>>
size_t contentHashCode(C<T, A> const& list) noexcept
{
	size_t __r = 0;
	for (auto const& e : list)
	{
		__r = __r * 31 + hash<T>()(e);
	}
	return __r;
	// TO-DO faster for integrals
}

template <typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
size_t contentDeepHashCode(T const& value) noexcept
{
	return rd::hash<T>()(value);
}

template<class T>
using remove_all_t = std::remove_reference_t<std::remove_cv_t<T>>;

// optional and rd::Wrapper
template <class T, typename = std::enable_if_t<!std::is_integral<T>::value>>
typename std::enable_if_t<std::is_same<decltype(T{}.has_value()), bool>::value, size_t> contentDeepHashCode(T const& value) noexcept
{
	return rd::hash<remove_all_t<T>>()(value);
}

// containers
template <class T, typename = std::enable_if_t<!std::is_integral<T>::value>>
typename std::enable_if_t<std::is_integral<remove_all_t<decltype(*begin(T{}))>>::value, size_t> contentDeepHashCode(T const& value) noexcept
{
	return contentHashCode(value);
}

// containers of non-integral types
template <class T, typename = std::enable_if_t<!std::is_integral<T>::value>>
typename std::enable_if_t<!std::is_integral<remove_all_t<decltype(*begin(T{}))>>::value, size_t> contentDeepHashCode(T const& value) noexcept
{
	size_t result = 1;
	for (auto const& x : value)
	{
		result = 31 * result + contentDeepHashCode<remove_all_t<decltype(x)>>(x);
	}
	return result;
}

}	 // namespace rd

#endif	  // RD_CPP_GEN_UTIL_H

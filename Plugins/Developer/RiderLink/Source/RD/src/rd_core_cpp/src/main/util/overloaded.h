#ifndef RD_CPP_OVERLOADED_H
#define RD_CPP_OVERLOADED_H

namespace rd
{
namespace util
{
template <typename... Ts>
struct overloaded;

template <typename T>
struct overloaded<T> : T
{
	overloaded(T t) : T(t)
	{
	}

	using T::operator();
};

template <typename T, class... Ts>
struct overloaded<T, Ts...> : T, overloaded<Ts...>
{
	overloaded(T t, Ts... rest) : T(t), overloaded<Ts...>(rest...)
	{
	}

	using T::operator();
	using overloaded<Ts...>::operator();
};

template <typename... Ts>
overloaded<Ts...> make_visitor(Ts... lambdas)
{
	return overloaded<Ts...>(lambdas...);
}
}	 // namespace util
}	 // namespace rd

#endif	  // RD_CPP_OVERLOADED_H

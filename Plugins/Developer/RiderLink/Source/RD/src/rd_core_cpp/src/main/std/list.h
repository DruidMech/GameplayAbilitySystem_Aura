#ifndef RD_CPP_LIST_H
#define RD_CPP_LIST_H

#include <vector>
#include <cstdint>

namespace rd
{
template <typename T>
int32_t size(T const& value) = delete;

// c++17 has std::size for std::vector
#if __cplusplus < 201703L

template <typename T, typename A>
int32_t size(std::vector<T, A> const& value)
{
	return static_cast<int32_t>(value.size());
}
#else	
template <typename T, typename A>
int32_t size(std::vector<T, A> const& value)
{
	return std::size(value);
}
#endif

template <typename T, typename A>
void resize(std::vector<T, A>& value, int32_t size)
{
	value.resize(size);
}
}	 // namespace rd

#endif	  // RD_CPP_LIST_H

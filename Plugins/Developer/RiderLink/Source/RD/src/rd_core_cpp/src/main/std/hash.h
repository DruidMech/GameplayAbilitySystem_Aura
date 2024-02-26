#ifndef RD_CPP_HASH_H
#define RD_CPP_HASH_H

#include <cstddef>
#include <functional>

namespace rd
{
template <typename T>
struct hash
{
	size_t operator()(const T& value) const noexcept
	{
		return std::hash<T>()(value);
	}
};
}	 // namespace rd

#endif	  // RD_CPP_HASH_H

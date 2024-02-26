#ifndef RD_CPP_HASHING_H
#define RD_CPP_HASHING_H

#include "nonstd/string_view.hpp"

#include <cstdint>
#include <cstdlib>
#include <string>

namespace rd
{
namespace util
{
using hash_t = int64_t;
using constexpr_hash_t = uint64_t;	  // hash_t;

constexpr constexpr_hash_t DEFAULT_HASH = 19;
constexpr constexpr_hash_t HASH_FACTOR = 31;

// PLEASE DO NOT CHANGE IT!!! IT'S EXACTLY THE SAME ON C# SIDE
constexpr hash_t hashImpl(constexpr_hash_t initial, char const* begin, char const* end)
{
	return (begin == end) ? initial : hashImpl(initial * HASH_FACTOR + *begin, begin + 1, end);
}

/*template<size_t N>
constexpr hash_t getPlatformIndependentHash(char const (&that)[N], constexpr_hash_t initial = DEFAULT_HASH) {
	return static_cast<hash_t>(hashImpl(initial, &that[0], &that[N - 1]));
}*/

constexpr hash_t getPlatformIndependentHash(string_view that, constexpr_hash_t initial = DEFAULT_HASH)
{
	return static_cast<hash_t>(hashImpl(initial, &that[0], &that[that.length() - 1] + 1));
}

constexpr hash_t getPlatformIndependentHash(int32_t const& that, constexpr_hash_t initial = DEFAULT_HASH)
{
	return static_cast<hash_t>(initial * HASH_FACTOR + static_cast<constexpr_hash_t>(that + 1));
}

constexpr hash_t getPlatformIndependentHash(int64_t const& that, constexpr_hash_t initial = DEFAULT_HASH)
{
	return static_cast<hash_t>(initial * HASH_FACTOR + static_cast<constexpr_hash_t>(that + 1));
}
}	 // namespace util
}	 // namespace rd
#endif	  // RD_CPP_HASHING_H

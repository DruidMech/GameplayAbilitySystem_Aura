#ifndef RD_CPP_UNORDERED_SET_H
#define RD_CPP_UNORDERED_SET_H

#include "hash.h"

#include <unordered_set>

namespace rd
{
template <class _Value, class _Hash = hash<_Value>, class _Pred = std::equal_to<_Value>, class _Alloc = std::allocator<_Value> >
using unordered_set = std::unordered_set<_Value, _Hash, _Pred, _Alloc>;
}

#endif	  // RD_CPP_UNORDERED_SET_H

#ifndef RD_CPP_UNORDERED_MAP_H
#define RD_CPP_UNORDERED_MAP_H

#include "hash.h"

#include <unordered_map>

namespace rd
{
template <class _Key, class _Tp, class _Hash = hash<_Key>, class _Pred = std::equal_to<_Key>,
	class _Alloc = std::allocator<std::pair<const _Key, _Tp> > >
using unordered_map = std::unordered_map<_Key, _Tp, _Hash, _Pred, _Alloc>;
}

#endif	  // RD_CPP_UNORDERED_MAP_H

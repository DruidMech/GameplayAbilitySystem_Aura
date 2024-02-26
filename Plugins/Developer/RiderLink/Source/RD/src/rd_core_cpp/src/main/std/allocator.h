#ifndef RD_CPP_ALLOCATOR_H
#define RD_CPP_ALLOCATOR_H

#include <memory>

namespace rd
{
template <typename T>
using allocator = std::allocator<T>;
}

#endif	  // RD_CPP_ALLOCATOR_H

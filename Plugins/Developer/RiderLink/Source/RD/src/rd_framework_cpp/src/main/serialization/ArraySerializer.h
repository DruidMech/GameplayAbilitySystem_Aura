#ifndef RD_CPP_ARRAYSERIALIZER_H
#define RD_CPP_ARRAYSERIALIZER_H

#include "serialization/SerializationCtx.h"
#include "framework_traits.h"

#include <vector>

namespace rd
{
template <typename S, template <class, class> class C, typename T = typename util::read_t<S>,
	typename A = allocator<value_or_wrapper<T>>>
class ArraySerializer
{
public:
	static C<value_or_wrapper<T>, A> read(SerializationCtx& ctx, Buffer& buffer)
	{
		return buffer.read_array<C, T, A>([&] { return S::read(ctx, buffer); });
	}

	static void write(SerializationCtx& ctx, Buffer& buffer, C<value_or_wrapper<T>, A> const& value)
	{
		buffer.write_array<C, T, A>(value, [&](T const& inner_value) { S::write(ctx, buffer, inner_value); });
	}
};
}	 // namespace rd

#endif	  // RD_CPP_ARRAYSERIALIZER_H

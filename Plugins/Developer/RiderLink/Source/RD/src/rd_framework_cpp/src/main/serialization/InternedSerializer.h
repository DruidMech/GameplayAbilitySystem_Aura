#ifndef RD_CPP_INTERNEDSERIALIZER_H
#define RD_CPP_INTERNEDSERIALIZER_H

#include "serialization/SerializationCtx.h"
#include "serialization/Polymorphic.h"
#include "framework_traits.h"

namespace rd
{
template <typename S, util::hash_t InternKey, typename T = typename util::read_t<S>>
class InternedSerializer
{
public:
	static Wrapper<T> read(SerializationCtx& ctx, Buffer& buffer)
	{
		return ctx.readInterned<T, InternKey>(buffer, [&](SerializationCtx&, Buffer&) { return S::read(ctx, buffer); });
	}

	static void write(SerializationCtx& ctx, Buffer& buffer, Wrapper<T> const& value)
	{
		ctx.writeInterned<T, InternKey>(buffer, value,
			[&](SerializationCtx&, Buffer&, T const& inner_value) mutable -> void { S::write(ctx, buffer, inner_value); });
	}
};
}	 // namespace rd
#endif	  // RD_CPP_INTERNEDSERIALIZER_H

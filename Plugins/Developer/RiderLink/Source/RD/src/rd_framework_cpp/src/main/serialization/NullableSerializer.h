#ifndef RD_CPP_NULLABLESERIALIZER_H
#define RD_CPP_NULLABLESERIALIZER_H

#include "serialization/Polymorphic.h"
#include "serialization/AbstractPolymorphic.h"
#include "types/wrapper.h"
#include "framework_traits.h"

#include <type_traits>

namespace rd
{
template <typename S, typename R = void>
class NullableSerializer
{
	using T = typename util::read_t<S>;

public:
	static opt_or_wrapper<T> read(SerializationCtx& ctx, Buffer& buffer)
	{
		return buffer.read_nullable<T>([&]() -> T { return S::read(ctx, buffer); });
	}

	static void write(SerializationCtx& ctx, Buffer& buffer, optional<T> const& value)
	{
		buffer.write_nullable<T>(value, [&](T const& inner_value) { S::write(ctx, buffer, inner_value); });
	}

	static void write(SerializationCtx& ctx, Buffer& buffer, Wrapper<T> const& value)
	{
		buffer.write_nullable<T>(value, [&](T const& inner_value) { S::write(ctx, buffer, inner_value); });
	}
};

template <typename S>
class NullableSerializer<S, std::enable_if_t<is_wrapper_v<util::read_t<S>>>>
{
	using W = typename util::read_t<S>;
	using T = typename W::type;

public:
	static Wrapper<T> read(SerializationCtx& ctx, Buffer& buffer)
	{
		return buffer.read_nullable<T>([&]() -> Wrapper<T> { return S::read(ctx, buffer); });
	}

	static void write(SerializationCtx& ctx, Buffer& buffer, Wrapper<T> const& value)
	{
		buffer.write_nullable<T>(value, [&](T const& inner_value) { S::write(ctx, buffer, inner_value); });
	}
};
}	 // namespace rd

#endif	  // RD_CPP_NULLABLESERIALIZER_H

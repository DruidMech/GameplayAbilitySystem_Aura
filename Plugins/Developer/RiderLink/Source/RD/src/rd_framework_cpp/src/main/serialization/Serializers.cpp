#include "Serializers.h"

#include "serialization/AbstractPolymorphic.h"

namespace rd
{
constexpr RdId STRING_PREDEFINED_ID = RdId(10);

RdId Serializers::real_rd_id(const IUnknownInstance& value)
{
	return value.unknownId;
}

RdId Serializers::real_rd_id(const IPolymorphicSerializable& value)
{
	return RdId(util::getPlatformIndependentHash(value.type_name()));
}

RdId Serializers::real_rd_id(const std::wstring& /*value*/)
{
	return STRING_PREDEFINED_ID;
}

void Serializers::real_write(SerializationCtx& /*ctx*/, Buffer& buffer, IUnknownInstance const& value)
{
	value.unknownId.write(buffer);
}

void Serializers::real_write(SerializationCtx& ctx, Buffer& buffer, IPolymorphicSerializable const& value)
{
	value.write(ctx, buffer);
}

void Serializers::real_write(SerializationCtx& ctx, Buffer& buffer, std::wstring const& value)
{
	Polymorphic<std::wstring>::write(ctx, buffer, value);
}

void Serializers::register_in()
{
	readers[STRING_PREDEFINED_ID] = [](SerializationCtx& ctx, Buffer& buffer) -> InternedAny {
		return {wrapper::make_wrapper<std::wstring>(Polymorphic<std::wstring>::read(ctx, buffer))};
	};
}

Serializers::Serializers()
{
	register_in();
}
}	 // namespace rd

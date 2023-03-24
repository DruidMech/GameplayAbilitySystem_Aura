#include "SerializationCtx.h"

namespace rd
{
//	SerializationCtx::SerializationCtx(const IProtocol &protocol) : serializers(protocol.serializers.get()) {}

//	SerializationCtx::SerializationCtx(const Serializers *const serializers) : serializers(serializers) {}

SerializationCtx::SerializationCtx(const Serializers* serializers, roots_t intern_roots)
	: serializers(serializers), intern_roots(std::move(intern_roots))
{
}

SerializationCtx SerializationCtx::withInternRootsHere(
	RdBindableBase const& owner, std::initializer_list<std::string> new_roots) const
{
	roots_t next_roots = intern_roots;
	for (const auto& item : new_roots)
	{
		auto const& name = "InternRoot-" + item;
		InternRoot const& root = owner.getOrCreateExtension<InternRoot>(name);
		withId(root, owner.get_id().mix(".").mix(name));
		next_roots.emplace(util::getPlatformIndependentHash(item), &root);
	}
	return SerializationCtx(serializers, std::move(next_roots));
}

Serializers const& SerializationCtx::get_serializers() const
{
	return *serializers;
}
}	 // namespace rd

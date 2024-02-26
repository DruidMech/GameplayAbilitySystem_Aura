#include "InternRoot.h"

#include "serialization/AbstractPolymorphic.h"
#include "serialization/InternedAnySerializer.h"

namespace rd
{
InternRoot::InternRoot()
{
	async = true;
}

IScheduler* InternRoot::get_wire_scheduler() const
{
	return &intern_scheduler;
}

void InternRoot::on_wire_received(Buffer buffer) const
{
	optional<InternedAny> value = InternedAnySerializer::read(get_serialization_context(), buffer);
	if (!value)
	{
		return;
	}
	const int32_t remote_id = buffer.read_integral<int32_t>();
	set_interned_correspondence(remote_id ^ 1, *std::move(value));
	RD_ASSERT_MSG(((remote_id & 1) == 0), "Remote sent ID marked as our own, bug?");
}

void InternRoot::bind(Lifetime lf, IRdDynamic const* parent, string_view name) const
{
	RD_ASSERT_MSG(!is_bound(), "Trying to bound already bound "s + to_string(this->location) + " to " + to_string(parent->get_location()))

	lf->bracket(
		[this, parent, &name] {
			this->parent = parent;
			location = parent->get_location().sub(name, ".");
		},
		[this] {
			location = location.sub("<<unbound>>", "::");
			this->parent = nullptr;
			rdid = RdId::Null();
		});

	{
		// if something's interned before bind
		std::lock_guard<decltype(lock)> guard(lock);
		my_items_lis.clear();
		other_items_list.clear();
		inverse_map.clear();
	}
	get_protocol()->get_wire()->advise(lf, this);
}

void InternRoot::identify(const Identities& /*identities*/, RdId const& id) const
{
	RD_ASSERT_MSG(rdid.isNull(), "Already has RdId: " + to_string(rdid) + ", entities: $this");
	RD_ASSERT_MSG(!id.isNull(), "Assigned RdId mustn't be null, entities: $this");

	rdid = id;
}

void InternRoot::set_interned_correspondence(int32_t id, InternedAny&& value) const
{
	RD_ASSERT_MSG(!is_index_owned(id), "Setting interned correspondence for object that we should have written, bug?")

	std::lock_guard<decltype(lock)> guard(lock);
	other_items_list[id / 2] = value;
	inverse_map[value] = id;
}
}	 // namespace rd

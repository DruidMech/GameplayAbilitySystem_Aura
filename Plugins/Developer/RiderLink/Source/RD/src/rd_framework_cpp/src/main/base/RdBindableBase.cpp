#include "RdBindableBase.h"

#include "reactive/base/SignalX.h"

namespace rd
{
std::string RdBindableBase::toString() const
{
	return "location=" + to_string(location) + ",rdid=" + to_string(rdid);
}

bool RdBindableBase::is_bound() const
{
	return parent != nullptr;
}

void RdBindableBase::bind(Lifetime lf, IRdDynamic const* parent, string_view name) const
{
	RD_ASSERT_MSG(!is_bound(), ("Trying to bind already bound this to " + to_string(parent->get_location())));
	lf->bracket(
		[this, lf, parent, &name] {
			this->parent = parent;
			location = parent->get_location().sub(name, ".");
			this->bind_lifetime = lf;
		},
		[this, lf]() {
			this->bind_lifetime = lf;
			location = location.sub("<<unbound>>", "::");
			this->parent = nullptr;
			rdid = RdId::Null();
		});

	get_protocol()->get_scheduler()->assert_thread();

	priorityAdviseSection([this, lf]() { init(lf); });
}

// must be overriden if derived class have bindable members
void RdBindableBase::identify(const Identities& identities, RdId const& id) const
{
	RD_ASSERT_MSG(rdid.isNull(), "Already has RdId: " + to_string(rdid) + ", entities: $this");
	RD_ASSERT_MSG(!id.isNull(), "Assigned RdId mustn't be null, entities: $this");

	this->rdid = id;
	for (const auto& it : bindable_extensions)
	{
		identifyPolymorphic(*(it.second), identities, id.mix(".").mix(it.first));
	}
}

const IProtocol* RdBindableBase::get_protocol() const
{
	if (is_bound())
	{
		auto protocol = parent->get_protocol();
		if (protocol != nullptr)
		{
			return protocol;
		}
	}
	throw std::invalid_argument("Not bound: " + to_string(location));
}

const RName& RdBindableBase::get_location() const
{
	return location;
}

RdId RdBindableBase::get_id() const
{
	return rdid;
}

void RdBindableBase::set_id(RdId id) const
{
	rdid = id;
}

SerializationCtx& RdBindableBase::get_serialization_context() const
{
	if (is_bound())
	{
		return parent->get_serialization_context();
	}
	else
	{
		throw std::invalid_argument("Not bound: " + to_string(location));
	}
}

void RdBindableBase::init(Lifetime lifetime) const
{
	for (const auto& it : bindable_extensions)
	{
		if (it.second != nullptr)
		{
			bindPolymorphic(*(it.second), lifetime, this, it.first);
		}
	}
}
}	 // namespace rd

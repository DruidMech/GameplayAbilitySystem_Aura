#include "IProtocol.h"

#include "scheduler/base/IScheduler.h"
#include "protocol/Identities.h"
#include "base/IWire.h"
#include "base/IProtocol.h"
//#include "serialization/SerializationCtx.h"

#include <utility>

namespace rd
{
IProtocol::IProtocol()
{
}

IProtocol::IProtocol(std::shared_ptr<Identities> identity, IScheduler* scheduler, std::shared_ptr<IWire> wire)
	: identity(std::move(identity)), scheduler(scheduler), wire(std::move(wire))
{
}

const IProtocol* IProtocol::get_protocol() const
{
	return this;
}

IScheduler* IProtocol::get_scheduler() const
{
	return scheduler;
}

const IWire* IProtocol::get_wire() const
{
	return wire.get();
}

const Serializers& IProtocol::get_serializers() const
{
	return *serializers;
}

const Identities* IProtocol::get_identity() const
{
	return identity.get();
}

const RName& IProtocol::get_location() const
{
	return location;
}

IProtocol::~IProtocol() = default;
}	 // namespace rd

#include "RdExtBase.h"

#include "lifetime/Lifetime.h"
#include "base/RdPropertyBase.h"
#include "protocol/Protocol.h"
#include "scheduler/SynchronousScheduler.h"

namespace rd
{
const IProtocol* RdExtBase::get_protocol() const
{
	return extProtocol ? extProtocol.get() : RdReactiveBase::get_protocol();
}

// must be overriden if derived ext have bindable members
void RdExtBase::init(Lifetime lifetime) const
{
	//    Protocol.initializationLogger.traceMe { "binding" }

	auto parentProtocol = RdReactiveBase::get_protocol();
	std::shared_ptr<IWire> parentWire = parentProtocol->wire;

	//    serializersOwner.registry(parentProtocol.serializers);

	IScheduler* sc = parentProtocol->get_scheduler();
	extWire->realWire = parentWire.get();
	lifetime->bracket(
		[&] {
			extProtocol =
				std::make_shared<Protocol>(parentProtocol->identity, sc, std::static_pointer_cast<IWire>(extWire), lifetime);
		},
		[this] { extProtocol = nullptr; });

	parentWire->advise(lifetime, this);

	// it's critical to advise before 'Ready' is sent because we advise on SynchronousScheduler

	lifetime->bracket([this, parentWire] { sendState(*parentWire, ExtState::Ready); },
		[this, parentWire] { sendState(*parentWire, ExtState::Disconnected); });

	for (auto const& it : bindable_extensions)
	{
		bindPolymorphic(*(it.second), lifetime, this, it.first);
	}
	traceMe(Protocol::initializationLogger, "created and bound");
}

void RdExtBase::on_wire_received(Buffer buffer) const
{
	ExtState remoteState = buffer.read_enum<ExtState>();
	traceMe(spdlog::get("logReceived"), "remote: " + to_string(remoteState));

	switch (remoteState)
	{
		case ExtState::Ready:
		{
			sendState(*extWire->realWire, ExtState::ReceivedCounterpart);
			extWire->connected.set(true);
			break;
		}
		case ExtState::ReceivedCounterpart:
		{
			extWire->connected.set(true);	 // don't set anything if already set
			break;
		}
		case ExtState::Disconnected:
		{
			extWire->connected.set(false);
			break;
		}
	}

	int64_t counterpartSerializationHash = buffer.read_integral<int64_t>();
	if (serializationHash != counterpartSerializationHash)
	{
		RD_ASSERT_MSG(false, "serializationHash of ext " + to_string(location) +
								 " doesn't match to counterpart: maybe you forgot to generate models?\n"
								 "our: " +
								 to_string(serializationHash) + ", counterpart:" + to_string(counterpartSerializationHash))
	}
}

void RdExtBase::sendState(IWire const& wire, ExtState state) const
{
	wire.send(rdid, [&](Buffer& buffer) {
		buffer.write_enum<ExtState>(state);
		buffer.write_integral<int64_t>(serializationHash);
	});
}

void RdExtBase::traceMe(std::shared_ptr<spdlog::logger> logger, string_view message) const
{
	logger->trace("ext {} {}:: {}", to_string(location), to_string(rdid), std::string(message));
}

IScheduler* RdExtBase::get_wire_scheduler() const
{
	return &SynchronousScheduler::Instance();
}

std::string to_string(RdExtBase::ExtState state)
{
	switch (state)
	{
		case RdExtBase::ExtState::Ready:
			return "Ready";
		case RdExtBase::ExtState::ReceivedCounterpart:
			return "ReceivedCounterpart";
		case RdExtBase::ExtState::Disconnected:
			return "Disconnected";
	}
	return {};
}
}	 // namespace rd

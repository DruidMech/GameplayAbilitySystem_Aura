#include "protocol/Protocol.h"

#include "serialization/SerializationCtx.h"
#include "intern/InternRoot.h"

#include "spdlog/sinks/stdout_color_sinks.h"

#include <utility>

namespace rd
{
std::shared_ptr<spdlog::logger> Protocol::initializationLogger =
	spdlog::stderr_color_mt<spdlog::synchronous_factory>("initializationLogger", spdlog::color_mode::automatic);

constexpr string_view Protocol::InternRootName;

void Protocol::initialize() const
{
	internRoot = std::make_unique<InternRoot>();

	context = std::make_unique<SerializationCtx>(
		serializers.get(), SerializationCtx::roots_t{{util::getPlatformIndependentHash("Protocol"), internRoot.get()}});

	internRoot->set_id(RdId::Null().mix(InternRootName));
	scheduler->queue([this] { internRoot->bind(lifetime, this, InternRootName); });
}

Protocol::Protocol(std::shared_ptr<Identities> identity, IScheduler* scheduler, std::shared_ptr<IWire> wire, Lifetime lifetime)
	: IProtocol(std::move(identity), scheduler, std::move(wire)), lifetime(lifetime)
{
	// initialize();
}

Protocol::Protocol(Identities::IdKind kind, IScheduler* scheduler, std::shared_ptr<IWire> wire, Lifetime lifetime)
	: IProtocol(std::make_shared<Identities>(kind), scheduler, std::move(wire)), lifetime(lifetime)
{
	// initialize();
}

Protocol::~Protocol() = default;

SerializationCtx& Protocol::get_serialization_context() const
{
	if (!context)
	{
		initialize();
	}
	return *context;
}

}	 // namespace rd

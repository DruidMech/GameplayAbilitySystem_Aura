#include "RdReactiveBase.h"

#include "spdlog/sinks/stdout_color_sinks.h"

namespace rd
{
static std::shared_ptr<spdlog::logger> logReceived =
	spdlog::stderr_color_mt<spdlog::synchronous_factory>("logReceived", spdlog::color_mode::automatic);
std::shared_ptr<spdlog::logger> logSend =
	spdlog::stderr_color_mt<spdlog::synchronous_factory>("logSend", spdlog::color_mode::automatic);

RdReactiveBase::RdReactiveBase(RdReactiveBase&& other) : RdBindableBase(std::move(other)) /*, async(other.async)*/
{
	async = other.async;
}

RdReactiveBase& RdReactiveBase::operator=(RdReactiveBase&& other)
{
	async = other.async;
	static_cast<RdBindableBase&>(*this) = std::move(other);
	return *this;
}

const IWire* RdReactiveBase::get_wire() const
{
	return get_protocol()->get_wire();
}

void RdReactiveBase::assert_threading() const
{
	if (!async)
	{
		get_default_scheduler()->assert_thread();
	}
}

void RdReactiveBase::assert_bound() const
{
	if (!is_bound())
	{
		throw std::invalid_argument("Not bound");
	}
}

const Serializers& RdReactiveBase::get_serializers() const
{
	return *get_protocol()->serializers.get();
}

IScheduler* RdReactiveBase::get_default_scheduler() const
{
	return get_protocol()->get_scheduler();
}

IScheduler* RdReactiveBase::get_wire_scheduler() const
{
	return get_default_scheduler();
}
}	 // namespace rd

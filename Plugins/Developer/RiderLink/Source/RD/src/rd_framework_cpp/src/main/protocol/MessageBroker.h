#ifndef RD_CPP_MESSAGEBROKER_H
#define RD_CPP_MESSAGEBROKER_H

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:4251)
#endif

#include "base/IRdReactive.h"

#include "std/unordered_map.h"

#include "spdlog/spdlog.h"

#include <queue>

#include <rd_framework_export.h>

namespace rd
{
class RD_FRAMEWORK_API Mq
{
public:
	// region ctor/dtor

	Mq() = default;

	Mq(Mq const&) = delete;

	Mq& operator=(Mq const&) = delete;

	Mq(Mq&&) = default;

	Mq& operator=(Mq&&) = default;
	// endregion

	mutable std::queue<Buffer> default_scheduler_messages;
	std::vector<Buffer> custom_scheduler_messages;
};

class RD_FRAMEWORK_API MessageBroker final
{
private:
	IScheduler* default_scheduler = nullptr;
	mutable rd::unordered_map<RdId, IRdReactive const*> subscriptions;
	mutable rd::unordered_map<RdId, Mq> broker;

	mutable std::recursive_mutex lock;

	static std::shared_ptr<spdlog::logger> logger;

	void invoke(const IRdReactive* that, Buffer msg, bool sync = false) const;

public:
	// region ctor/dtor

	explicit MessageBroker(IScheduler* defaultScheduler);
	// endregion

	void dispatch(RdId id, Buffer message) const;

	void advise_on(Lifetime lifetime, IRdReactive const* entity) const;
};
}	 // namespace rd
#if defined(_MSC_VER)
#pragma warning(pop)
#endif


#endif	  // RD_CPP_MESSAGEBROKER_H

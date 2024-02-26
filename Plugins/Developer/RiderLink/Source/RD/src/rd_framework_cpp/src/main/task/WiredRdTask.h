#ifndef RD_CPP_WIREDRDTASK_H
#define RD_CPP_WIREDRDTASK_H

#include "RdTask.h"
#include "WiredRdTaskImpl.h"
#include "base/RdReactiveBase.h"
#include "scheduler/base/IScheduler.h"

namespace rd
{
template <typename T, typename S = Polymorphic<T>>
class WiredRdTask final : public RdTask<T, S>
{
	mutable std::shared_ptr<detail::WiredRdTaskImpl<T, S>> impl{};

public:
	// region ctor/dtor
	WiredRdTask() = delete;

	WiredRdTask(Lifetime lifetime, RdReactiveBase const& call, RdId rdid, IScheduler* scheduler)
		: impl(std::make_shared<detail::WiredRdTaskImpl<T, S>>(lifetime, call, rdid, scheduler, RdTask<T, S>::result))
	{
	}

	WiredRdTask(WiredRdTask const& other) = default;

	WiredRdTask& operator=(WiredRdTask const& other) = default;

	WiredRdTask(WiredRdTask&& other) = default;

	WiredRdTask& operator=(WiredRdTask&& other) = default;

	virtual ~WiredRdTask() = default;
	// endregion
};
}	 // namespace rd

#endif	  // RD_CPP_WIREDRDTASK_H

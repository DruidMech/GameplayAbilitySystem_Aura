#include "InternScheduler.h"

#include "guards.h"

namespace rd
{
thread_local int32_t InternScheduler::active_counts = 0;

InternScheduler::InternScheduler()
{
	out_of_order_execution = true;
}

void InternScheduler::queue(std::function<void()> action)
{
	util::increment_guard<int32_t> guard(active_counts);
	action();
}

void InternScheduler::flush()
{
}

bool InternScheduler::is_active() const
{
	return active_counts > 0;
}
}	 // namespace rd

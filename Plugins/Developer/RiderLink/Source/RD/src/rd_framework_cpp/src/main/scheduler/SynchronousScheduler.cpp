#include "SynchronousScheduler.h"

#include "guards.h"

namespace rd
{
static thread_local int32_t SynchronousScheduler_active_count = 0;

void SynchronousScheduler::queue(std::function<void()> action)
{
	util::increment_guard<int32_t> guard(SynchronousScheduler_active_count);
	action();
}

void SynchronousScheduler::flush()
{
}

bool SynchronousScheduler::is_active() const
{
	return SynchronousScheduler_active_count > 0;
}
}	 // namespace rd
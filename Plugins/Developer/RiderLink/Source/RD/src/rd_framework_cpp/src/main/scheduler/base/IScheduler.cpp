#include "IScheduler.h"

#include "spdlog/spdlog.h"

#include <functional>
#include <sstream>

namespace rd
{
void IScheduler::assert_thread() const
{
	if (!is_active())
	{
		std::ostringstream msg;
		msg << "Illegal scheduler for current action. Must be " << thread_id << ", was " << std::this_thread::get_id();
		spdlog::error(msg.str());
	}
}

void IScheduler::invoke_or_queue(std::function<void()> action)
{
	if (is_active())
	{
		action();
	}
	else
	{
		queue(action);
	}
}
}	 // namespace rd

#include "LifetimeImpl.h"

#include <utility>

namespace rd
{
#if __cplusplus < 201703L
LifetimeImpl::counter_t LifetimeImpl::get_id = 0;
#endif

LifetimeImpl::LifetimeImpl(bool is_eternal) : eternaled(is_eternal), id(LifetimeImpl::get_id++)
{
}

void LifetimeImpl::terminate()
{
	if (is_eternal())
		return;

	terminated = true;

	// region thread-safety section

	actions_t actions_copy;
	{
		std::lock_guard<decltype(actions_lock)> guard(actions_lock);
		actions_copy = std::move(actions);

		actions.clear();
	}
	// endregion

	for (auto it = actions_copy.rbegin(); it != actions_copy.rend(); ++it)
	{
		it->second();
	}
}

bool LifetimeImpl::is_terminated() const
{
	return terminated;
}

bool LifetimeImpl::is_eternal() const
{
	return eternaled;
}

void LifetimeImpl::attach_nested(std::shared_ptr<LifetimeImpl> nested)
{
	if (nested->is_terminated() || is_eternal())
		return;

	std::function<void()> action = [nested] { nested->terminate(); };
	counter_t action_id = add_action(action);
	nested->add_action([this, id = action_id] { actions.erase(id); });
}

LifetimeImpl::~LifetimeImpl()
{
	/*if (!is_eternal() && !is_terminated()) {
		spdlog::error("forget to terminate lifetime with id: {}", to_string(id));
		terminate();
	}*/
}
}	 // namespace rd

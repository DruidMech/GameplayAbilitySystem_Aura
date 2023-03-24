#ifndef RD_CPP_CORE_LIFETIME_H
#define RD_CPP_CORE_LIFETIME_H

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:4251)
#endif

#include <std/hash.h>

#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <atomic>
#include <utility>

#include <thirdparty.hpp>

#include <rd_core_export.h>

namespace rd
{
class RD_CORE_API LifetimeImpl final
{
public:
	friend class LifetimeDefinition;

	friend class Lifetime;

	using counter_t = int32_t;

private:
	bool eternaled = false;
	std::atomic<bool> terminated{false};

	counter_t id = 0;

	counter_t action_id_in_map = 0;
	using actions_t = ordered_map<int, std::function<void()>, rd::hash<int>>;
	actions_t actions;

	void terminate();

	std::mutex actions_lock;

public:
	// region ctor/dtor
	explicit LifetimeImpl(bool is_eternal = false);

	LifetimeImpl(LifetimeImpl const& other) = delete;

	~LifetimeImpl();
	// endregion

	template <typename F>
	counter_t add_action(F&& action)
	{
		std::lock_guard<decltype(actions_lock)> guard(actions_lock);

		if (is_eternal())
		{
			return -1;
		}
		if (is_terminated())
		{
			throw std::invalid_argument("Already Terminated");
		}

		actions[action_id_in_map] = std::forward<F>(action);
		return action_id_in_map++;
	}

	void remove_action(counter_t i)
	{
		std::lock_guard<decltype(actions_lock)> guard(actions_lock);

		actions.erase(i);
	}

#if __cplusplus >= 201703L
	static inline counter_t get_id = 0;
#else
	static counter_t get_id;
#endif

	template <typename F, typename G>
	void bracket(F&& opening, G&& closing)
	{
		if (is_terminated())
			return;
		opening();
		add_action(std::forward<G>(closing));
	}

	bool is_terminated() const;

	bool is_eternal() const;

	void attach_nested(std::shared_ptr<LifetimeImpl> nested);
};
}	 // namespace rd
#if defined(_MSC_VER)
#pragma warning(pop)
#endif


#endif	  // RD_CPP_CORE_LIFETIME_H

#ifndef RD_CPP_RDTASK_H
#define RD_CPP_RDTASK_H

#include "RdTaskImpl.h"
#include "serialization/Polymorphic.h"

#include <functional>

namespace rd
{
/**
 * \brief Represents a task that can be asynchronously executed.
 *
 * \tparam T type of stored value
 * \tparam S "SerDes" for value
 */
template <typename T, typename S = Polymorphic<T>>
class RdTask
{
protected:
	using WT = value_or_wrapper<T>;

	using TRes = RdTaskResult<T, S>;

	mutable std::shared_ptr<detail::RdTaskImpl<T, S>> impl{std::make_shared<detail::RdTaskImpl<T, S>>()};

	Property<RdTaskResult<T, S>>* result{&impl->result};

public:
	using result_type = RdTaskResult<T, S>;

	template <typename, typename, typename, typename>
	friend class RdEndpoint;
	// region ctor/dtor

	RdTask() = default;

	RdTask(RdTask const& other) = default;

	RdTask& operator=(RdTask const& other) = default;

	RdTask(RdTask&& other) = default;

	RdTask& operator=(RdTask&& other) = default;

	virtual ~RdTask() = default;
	// endregion

	static RdTask from_result(WT value)
	{
		RdTask<T, S> res;
		res.set(std::move(value));
		return res;
	}

	void set(WT value) const
	{
		typename TRes::Success t(std::move(value));
		impl->result.set(std::move(t));
	}

	void set_result(TRes value) const
	{
		impl->result.set(std::move(value));
	}

	void set_result_if_empty(TRes value) const
	{
		impl->result.set_if_empty(std::move(value));
	}

	void cancel() const
	{
		impl->result.set(typename TRes::Cancelled());
	}

	void fault(std::exception const& e) const
	{
		impl->result.set(typename TRes::Fault(e));
	}

	bool has_value() const
	{
		return impl->result.has_value();
	}

	const TRes& value_or_throw() const
	{
		if (impl->result.has_value())
		{
			return impl->result.get();
		}
		else
		{
			throw std::invalid_argument("task is empty");
		}
	}

	bool is_succeeded() const
	{
		return has_value() && value_or_throw().is_succeeded();
	};

	bool is_canceled() const
	{
		return has_value() && value_or_throw().is_canceled();
	};

	bool is_faulted() const
	{
		return has_value() && value_or_throw().is_faulted();	// TO-DO atomic
	}

	void advise(Lifetime lifetime, std::function<void(TRes const&)> handler) const
	{
		impl->result.advise(lifetime, [handler = std::move(handler)](optional<TRes> const& opt_value) {
			if (opt_value)
			{
				handler(*opt_value);
			}
		});
	}
};
}	 // namespace rd

#endif	  // RD_CPP_RDTASK_H

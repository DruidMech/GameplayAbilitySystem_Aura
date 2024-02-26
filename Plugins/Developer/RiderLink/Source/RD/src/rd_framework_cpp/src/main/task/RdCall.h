#ifndef RD_CPP_RDCALL_H
#define RD_CPP_RDCALL_H

#include "serialization/Polymorphic.h"
#include "RdTask.h"
#include "RdTaskResult.h"
#include "scheduler/SynchronousScheduler.h"
#include "WiredRdTask.h"

#include <thread>

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

namespace rd
{
/**
 * \brief Represents an API provided by the remote process which can be invoked through the protocol.
 *
 * \tparam TReq type of request
 * \tparam TRes type of response
 * \tparam ReqSer "SerDes" for request
 * \tparam ResSer "SerDes" for response
 */
template <typename TReq, typename TRes, typename ReqSer = Polymorphic<TReq>, typename ResSer = Polymorphic<TRes> >
class RdCall : public virtual RdReactiveBase, public ISerializable
{
	using WTReq = value_or_wrapper<TReq>;
	using WTRes = value_or_wrapper<TRes>;

	mutable optional<RdId> sync_task_id;

public:
	// region ctor/dtor
	RdCall() = default;

	RdCall(RdCall&&) = default;

	RdCall& operator=(RdCall&&) = default;

	virtual ~RdCall() = default;
	// endregion

	static RdCall<TReq, TRes, ReqSer, ResSer> read(SerializationCtx& /*ctx*/, Buffer& buffer)
	{
		RdCall<TReq, TRes, ReqSer, ResSer> res;
		const RdId& id = RdId::read(buffer);
		withId(res, id);
		return res;
	}

	void write(SerializationCtx& /*ctx*/, Buffer& buffer) const override
	{
		rdid.write(buffer);
	}

	void init(Lifetime lifetime) const override
	{
		RdBindableBase::init(lifetime);
		bind_lifetime = lifetime;
		get_wire()->advise(lifetime, this);
	}

	/**
	 * \brief Invokes the API with the parameters given as [request] and waits for the result.
	 *
	 * \param request value to deliver
	 * \return result of remote invoking
	 */
	WiredRdTask<TRes, ResSer> sync(TReq const& request, std::chrono::milliseconds timeout = std::chrono::milliseconds(200)) const
	{
		auto task = start_internal(request, true, &SynchronousScheduler::Instance());
		auto time_at_start = std::chrono::system_clock::now();
		while (!task.has_value() && !(*bind_lifetime)->is_terminated() &&
			   ((std::chrono::system_clock::now() - time_at_start) < timeout))
		{
			std::this_thread::yield();
		}
		spdlog::debug("Time elapsed: {}, has_value={}", to_string(std::chrono::system_clock::now() - time_at_start),
			to_string(task.has_value()));
		task.value_or_throw().unwrap();	   // check for existing value
		sync_task_id = nullopt;
		return task;
	}

	IScheduler* get_wire_scheduler() const override
	{
		return &SynchronousScheduler::Instance();
	}

	/**
	 * \brief Asynchronously invokes the API with the parameters given as [request] and waits for the result.
	 *
	 * \param request value of request
	 * \param responseScheduler to assign value
	 * \return task which will have its result value.
	 */
	WiredRdTask<TRes, ResSer> start(TReq const& request, IScheduler* responseScheduler = nullptr) const
	{
		return start_internal(request, false, responseScheduler ? responseScheduler : get_default_scheduler());
	}

	void on_wire_received(Buffer buffer) const override
	{
		RD_ASSERT_MSG(false, "RdCall.on_wire_received called")
	}

private:
	WiredRdTask<TRes, ResSer> start_internal(TReq const& request, bool sync, IScheduler* scheduler) const
	{
		assert_bound();
		if (!async)
		{
			assert_threading();
		}

		RdId task_id = get_protocol()->get_identity()->next(rdid);
		WiredRdTask<TRes, ResSer> task{*bind_lifetime, *this, task_id, scheduler};

		if (sync)
		{
			if (sync_task_id.has_value())
			{
				throw std::invalid_argument(
					"Already exists sync task for call " + to_string(location) + ", taskId = " + rd::to_string(*sync_task_id));
			}
			sync_task_id = task_id;
		}

		get_wire()->send(rdid, [&](Buffer& buffer) {
			spdlog::get("logSend")->trace("call {}::{} send {} request {} : {}", to_string(location), to_string(rdid), (sync ? "SYNC" : "ASYNC"),
				to_string(task_id), to_string(request));
			task_id.write(buffer);
			ReqSer::write(get_serialization_context(), buffer, request);
		});

		return task;
	}

public:
	friend bool operator==(const RdCall& lhs, const RdCall& rhs)
	{
		return &lhs == &rhs;
	}

	friend bool operator!=(const RdCall& lhs, const RdCall& rhs)
	{
		return !(rhs == lhs);
	}

	friend std::string to_string(RdCall const& /*value*/)
	{
		return "RdCall";
	}
};
}	 // namespace rd

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#endif	  // RD_CPP_RDCALL_H

#ifndef RD_CPP_RDTASKRESULT_H
#define RD_CPP_RDTASKRESULT_H

#include "serialization/Polymorphic.h"
#include "util/overloaded.h"
#include "types/wrapper.h"
#include "util/core_util.h"

#include "thirdparty.hpp"

#include <exception>
#include <functional>

namespace rd
{
/**
 * \brief Advanced monad result. It is in of following states: Success, Cancelled, Fault;
 * Success -  Execution completed. Result stores in it.
 * Cancelled - Task was cancelled on callee side.
 * Fault - Something went wrong and reason stores in it.
 * \tparam T type of result
 * \tparam S "SerDes" for T
 */
template <typename T, typename S = Polymorphic<T> >
class RdTaskResult final : public ISerializable
{
	using WT = value_or_wrapper<T>;

public:
	class Success
	{
	public:
		mutable WT value;

		explicit Success(WT&& value) : value(std::move(value))
		{
		}
	};

	class Cancelled
	{
	};

	class Fault
	{
	public:
		std::wstring reason_type_fqn;
		std::wstring reason_message;
		std::wstring reason_as_text;

		Fault(std::wstring reason_type_fqn, std::wstring reason_message, std::wstring reason_as_text)
			: reason_type_fqn(std::move(reason_type_fqn))
			, reason_message(std::move(reason_message))
			, reason_as_text(std::move(reason_as_text))
		{
		}

		explicit Fault(const std::exception& e)
		{
			reason_message = to_wstring(to_string(e));
		}
	};

	// region ctor/dtor

	template <typename F>
	RdTaskResult(F&& v) : v(std::forward<F>(v))
	{
	}

	RdTaskResult(RdTaskResult const&) = default;

	RdTaskResult(RdTaskResult&&) = default;

	RdTaskResult& operator=(RdTaskResult&&) = default;

	virtual ~RdTaskResult() = default;
	// endregion

	static RdTaskResult<T, S> read(SerializationCtx& ctx, Buffer& buffer)
	{
		const int32_t kind = buffer.read_integral<int32_t>();
		switch (kind)
		{
			case 0:
			{
				return Success(std::move(S::read(ctx, buffer)));
			}
			case 1:
			{
				return Cancelled();
			}
			case 2:
			{
				auto reason_type_fqn = buffer.read_wstring();
				auto reason_message = buffer.read_wstring();
				auto reason_as_text = buffer.read_wstring();
				return Fault(std::move(reason_type_fqn), std::move(reason_message), std::move(reason_as_text));
			}
			default:
				throw std::invalid_argument("Fail on RdTaskResult reading with kind: " + std::to_string(kind));
		}
	}

	void write(SerializationCtx& ctx, Buffer& buffer) const override
	{
		visit(util::make_visitor(
				  [&ctx, &buffer](Success const& value) {
					  buffer.write_integral<int32_t>(0);
					  S::write(ctx, buffer, value.value);
				  },
				  [&buffer](Cancelled const&) { buffer.write_integral<int32_t>(1); },
				  [&buffer](Fault const& value) {
					  buffer.write_integral<int32_t>(2);
					  buffer.write_wstring(value.reason_type_fqn);
					  buffer.write_wstring(value.reason_message);
					  buffer.write_wstring(value.reason_as_text);
				  }),
			v);
	}

	T const& unwrap() const
	{
		return visit(util::make_visitor([](Success const& value) -> T const& { return wrapper::get<T>(value.value); },
						 [](Cancelled const&) -> T const& { throw std::invalid_argument("Task finished in Cancelled state"); },
						 [](Fault const& value) -> T const& { throw std::runtime_error(to_string(value.reason_message)); }),
			v);
	}

	bool is_succeeded() const
	{
		return v.index() == 0;
	}

	bool is_canceled() const
	{
		return v.index() == 1;
	}

	void as_canceled(std::function<void(Cancelled const&)> f) const
	{
		f(rd::get<Cancelled>(v));
	}

	bool is_faulted() const
	{
		return v.index() == 2;
	}

	void as_faulted(std::function<void(Fault const&)> f)
	{
		f(rd::get<Fault>(v));
	}

	friend bool operator==(const RdTaskResult& lhs, const RdTaskResult& rhs)
	{
		return &lhs == &rhs;
	}

	friend bool operator!=(const RdTaskResult& lhs, const RdTaskResult& rhs)
	{
		return !(rhs == lhs);
	}

	friend std::string to_string(RdTaskResult const& taskResult)
	{
		return visit(util::make_visitor([](Success const& value) -> std::string { return to_string(value.value); },
						 [](Cancelled const&) -> std::string { return "Cancelled state"; },
						 [](Fault const& value) -> std::string { return to_string(value.reason_message); }),
			taskResult.v);
	}

private:
	mutable variant<Success, Cancelled, Fault> v;
};
}	 // namespace rd

#endif	  // RD_CPP_RDTASKRESULT_H

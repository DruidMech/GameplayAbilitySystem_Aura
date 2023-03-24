#ifndef RD_CPP_RDREACTIVEBASE_H
#define RD_CPP_RDREACTIVEBASE_H

#include "base/RdBindableBase.h"
#include "base/IRdReactive.h"
#include "guards.h"

#include "spdlog/spdlog.h"

#include <rd_framework_export.h>

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

namespace rd
{
// region predeclared

class IWire;

class IProtocol;

class Serializers;
// endregion

class RD_FRAMEWORK_API RdReactiveBase : public RdBindableBase, public IRdReactive
{
public:
	// region ctor/dtor

	RdReactiveBase() = default;

	RdReactiveBase(RdReactiveBase&& other);

	RdReactiveBase& operator=(RdReactiveBase&& other);

	virtual ~RdReactiveBase() = default;
	// endregion

	const IWire* get_wire() const;

	mutable bool is_local_change = false;

	// delegated

	const Serializers& get_serializers() const;

	IScheduler* get_default_scheduler() const;

	IScheduler* get_wire_scheduler() const override;

	void assert_threading() const;

	void assert_bound() const;

	template <typename F>
	auto local_change(F&& action) const -> typename util::result_of_t<F()>
	{
		if (is_bound() && !async)
		{
			assert_threading();
		}

		RD_ASSERT_MSG(!is_local_change, "!isLocalChange for RdReactiveBase with id:" + to_string(rdid));

		util::bool_guard bool_guard(is_local_change);
		return action();
	}
};
}	 // namespace rd

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#endif	  // RD_CPP_RDREACTIVEBASE_H

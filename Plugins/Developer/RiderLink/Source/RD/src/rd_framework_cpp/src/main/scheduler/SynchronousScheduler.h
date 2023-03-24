#ifndef RD_CPP_SYNCHRONOUSSCHEDULER_H
#define RD_CPP_SYNCHRONOUSSCHEDULER_H

#include "scheduler/base/IScheduler.h"

#include "guards.h"

#include <rd_framework_export.h>

namespace rd
{
class RD_FRAMEWORK_API SynchronousScheduler : public IScheduler
{
public:
	// region ctor/dtor

	SynchronousScheduler() = default;

	SynchronousScheduler(SynchronousScheduler const&) = delete;

	SynchronousScheduler(SynchronousScheduler&&) = delete;

	virtual ~SynchronousScheduler() = default;
	// endregion

	void queue(std::function<void()> action) override;

	void flush() override;

	bool is_active() const override;

	/**
	 * \brief global synchronous scheduler for whole application.
	 */
	static SynchronousScheduler& Instance()
	{
		static SynchronousScheduler globalSynchronousScheduler;
		return globalSynchronousScheduler;
	}
};

}	 // namespace rd

#endif	  // RD_CPP_SYNCHRONOUSSCHEDULER_H

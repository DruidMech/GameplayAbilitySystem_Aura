#ifndef RD_CPP_SINGLETHREADSCHEDULER_H
#define RD_CPP_SINGLETHREADSCHEDULER_H

#include "base/SingleThreadSchedulerBase.h"

#include "lifetime/Lifetime.h"

#include <rd_framework_export.h>

namespace rd
{
class RD_FRAMEWORK_API SingleThreadScheduler : public SingleThreadSchedulerBase
{
public:
	Lifetime lifetime;

	SingleThreadScheduler(Lifetime lifetime, std::string name);
};
}	 // namespace rd

#endif	  // RD_CPP_SINGLETHREADSCHEDULER_H

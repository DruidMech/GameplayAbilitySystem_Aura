#ifndef RD_CPP_IRDWIREABLE_H
#define RD_CPP_IRDWIREABLE_H

#include "protocol/RdId.h"
#include "scheduler/base/IScheduler.h"
#include "protocol/Buffer.h"

#include <rd_framework_export.h>

namespace rd
{
class RD_FRAMEWORK_API IRdWireable
{
public:
	mutable RdId rdid = RdId::Null();
};
}	 // namespace rd

#endif	  // RD_CPP_IRDWIREABLE_H

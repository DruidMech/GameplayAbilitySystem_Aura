#ifndef RD_CPP_IUNKNOWNINSTANCE_H
#define RD_CPP_IUNKNOWNINSTANCE_H

#include "protocol/RdId.h"

#include <rd_framework_export.h>

namespace rd
{
class RD_FRAMEWORK_API IUnknownInstance
{
public:
	RdId unknownId{0};

	IUnknownInstance();

	explicit IUnknownInstance(const RdId& unknownId);

	explicit IUnknownInstance(RdId&& unknownId);
};
}	 // namespace rd

#endif	  // RD_CPP_IUNKNOWNINSTANCE_H

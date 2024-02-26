#ifndef RD_CPP_WIREBASE_H
#define RD_CPP_WIREBASE_H

#include "reactive/Property.h"
#include "base/IWire.h"
#include "protocol/MessageBroker.h"

#include <rd_framework_export.h>

namespace rd
{
class RD_FRAMEWORK_API WireBase : public IWire
{
protected:
	IScheduler* scheduler = nullptr;

	MessageBroker message_broker;

public:
	// region ctor/dtor
	explicit WireBase(IScheduler* scheduler) : scheduler(scheduler), message_broker(scheduler)
	{
	}

	virtual ~WireBase() = default;
	// endregion

	virtual void advise(Lifetime lifetime, RdReactiveBase const* entity) const override;
};
}	 // namespace rd

#endif	  // RD_CPP_WIREBASE_H

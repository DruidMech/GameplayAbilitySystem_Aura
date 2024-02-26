#include "WireBase.h"

namespace rd
{
void WireBase::advise(Lifetime lifetime, const RdReactiveBase* entity) const
{
	message_broker.advise_on(lifetime, entity);
}
}	 // namespace rd

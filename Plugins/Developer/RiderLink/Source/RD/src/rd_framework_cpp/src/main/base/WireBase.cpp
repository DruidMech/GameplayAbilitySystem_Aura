#include "WireBase.h"

namespace rd
{
void WireBase::advise(Lifetime lifetime, const IRdReactive* entity) const
{
	message_broker.advise_on(lifetime, entity);
}
}	 // namespace rd

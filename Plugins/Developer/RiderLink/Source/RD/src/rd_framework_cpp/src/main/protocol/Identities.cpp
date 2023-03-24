#include "protocol/Identities.h"

namespace rd
{
constexpr Identities::IdKind Identities::SERVER;
constexpr Identities::IdKind Identities::CLIENT;

Identities::Identities(IdKind dynamicKind) : id_acc(dynamicKind == IdKind::Client ? BASE_CLIENT_ID : BASE_SERVER_ID)
{
}

RdId Identities::next(const RdId& parent) const
{
	RdId result = parent.mix(id_acc.fetch_add(2));
	return result;
}
}	 // namespace rd

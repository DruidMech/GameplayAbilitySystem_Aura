#include "protocol/RdId.h"

#include "protocol/Identities.h"

namespace rd
{
RdId RdId::read(Buffer& buffer)
{
	const auto number = buffer.read_integral<hash_t>();
	return RdId(number);
}

void RdId::write(Buffer& buffer) const
{
	buffer.write_integral(hash);
}

std::string to_string(RdId const& id)
{
	return std::to_string(id.hash);
}

bool operator==(RdId const& left, RdId const& right)
{
	return left.hash == right.hash;
}

bool operator!=(const RdId& lhs, const RdId& rhs)
{
	return !(rhs == lhs);
}
}	 // namespace rd

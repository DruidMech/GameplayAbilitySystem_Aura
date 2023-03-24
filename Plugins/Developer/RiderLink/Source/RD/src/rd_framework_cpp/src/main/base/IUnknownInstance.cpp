#include "IUnknownInstance.h"

namespace rd
{
IUnknownInstance::IUnknownInstance()
{
}

IUnknownInstance::IUnknownInstance(const RdId& unknownId) : unknownId(unknownId)
{
}

IUnknownInstance::IUnknownInstance(RdId&& unknownId) : unknownId(std::move(unknownId))
{
}
}	 // namespace rd

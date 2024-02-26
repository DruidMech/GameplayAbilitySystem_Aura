#include "DefaultAbstractDeclaration.h"

namespace rd
{
const std::string DefaultAbstractDeclaration::not_registered_error_message =
	"Maybe you forgot to invoke 'register()' method of corresponding Toplevel. "
	"Usually it should be done automatically during 'bind()' invocation but in complex cases you should do it manually.";

Wrapper<DefaultAbstractDeclaration> DefaultAbstractDeclaration::readUnknownInstance(
	SerializationCtx& /*ctx*/, Buffer& /*buffer*/, RdId const& unknownId, int32_t /*size*/)
{
	throw std::invalid_argument("Can't find reader by id: " + to_string(unknownId) + not_registered_error_message);
}

std::string DefaultAbstractDeclaration::type_name() const
{
	return "DefaultAbstractDeclaration";
}

bool DefaultAbstractDeclaration::equals(ISerializable const& /*serializable*/) const
{
	return false;
}

void DefaultAbstractDeclaration::write(SerializationCtx& /*ctx*/, Buffer& /*buffer*/) const
{
	throw std::invalid_argument("DefaultAbstractDeclaration couldn't be written");
}
}	 // namespace rd
#include "RName.h"

#include "thirdparty.hpp"

namespace rd
{
class RNameImpl
{
public:
	// region ctor/dtor
	RNameImpl(RName parent, string_view localName, string_view separator);

	RNameImpl(const RName& other) = delete;
	RNameImpl(RName&& other) noexcept = delete;
	RNameImpl& operator=(const RNameImpl& other) = delete;
	RNameImpl& operator=(RNameImpl&& other) noexcept = delete;
	// endregion

	friend std::string to_string(RNameImpl const& value);

private:
	RName parent;
	std::string local_name, separator;
};

RNameImpl::RNameImpl(RName parent, string_view localName, string_view separator)
	: parent(parent), local_name(localName), separator(separator)
{
}

RName::RName(RName parent, string_view localName, string_view separator)
	: impl(std::make_shared<RNameImpl>(std::move(parent), localName, separator))
{
}

RName RName::sub(string_view localName, string_view separator) const
{
	return RName(*this, localName, separator);
}

std::string to_string(RName const& value)
{
	std::string res;
	if (value.impl)
	{
		res = to_string(*value.impl);
	}
	return res;
}

std::string to_string(RNameImpl const& value)
{
	if (value.parent)
	{
		std::string res;
		res = to_string(value.parent);
		res += value.separator;
		res += value.local_name;
		return res;
	}
	return value.local_name;
}

RName::RName(string_view local_name) : RName(RName(), local_name, "")
{
}

}	 // namespace rd

#ifndef RD_CPP_VOID_H
#define RD_CPP_VOID_H

#include <functional>
#include <string>

namespace rd
{
/**
 * \brief For using in idle events
 */
class Void
{
	friend inline bool operator==(const Void&, const Void&)
	{
		return true;
	}

	friend inline bool operator!=(const Void&, const Void&)
	{
		return false;
	}
};

inline std::string to_string(Void const&)
{
	return "void";
}
}	 // namespace rd

namespace std
{
template <>
struct hash<rd::Void>
{
	size_t operator()(const rd::Void&) const noexcept
	{
		return 0;
	}
};
}	 // namespace std

#endif	  // RD_CPP_VOID_H

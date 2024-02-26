#ifndef RD_CPP_DATETIME_H
#define RD_CPP_DATETIME_H

#include <std/hash.h>

#include <ctime>
#include <string>

#include <rd_core_export.h>

namespace rd
{
/**
 * \brief Wrapper around time_t to be synchronized with "Date" in Kt and "DateTime" in C#.
 */
class RD_CORE_API DateTime
{
public:
	std::time_t seconds;

	explicit DateTime(time_t seconds);

	friend bool RD_CORE_API operator<(const DateTime& lhs, const DateTime& rhs);

	friend bool RD_CORE_API operator>(const DateTime& lhs, const DateTime& rhs);

	friend bool RD_CORE_API operator<=(const DateTime& lhs, const DateTime& rhs);

	friend bool RD_CORE_API operator>=(const DateTime& lhs, const DateTime& rhs);

	friend bool RD_CORE_API operator==(const DateTime& lhs, const DateTime& rhs);

	friend bool RD_CORE_API operator!=(const DateTime& lhs, const DateTime& rhs);

	//"1970-01-01 03:01:38" for example
	friend std::string RD_CORE_API to_string(DateTime const& time);
};
}	 // namespace rd
namespace rd
{
template <>
struct RD_CORE_API hash<rd::DateTime>
{
	size_t operator()(const rd::DateTime& value) const noexcept;
};
}	 // namespace rd

#endif	  // RD_CPP_DATETIME_H

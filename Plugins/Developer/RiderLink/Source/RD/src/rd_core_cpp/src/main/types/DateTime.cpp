#include "DateTime.h"

#include <sstream>
#include <iomanip>

namespace rd
{
DateTime::DateTime(time_t seconds) : seconds(seconds)
{
}

bool operator<(const DateTime& lhs, const DateTime& rhs)
{
	return lhs.seconds < rhs.seconds;
}

bool operator>(const DateTime& lhs, const DateTime& rhs)
{
	return rhs < lhs;
}

bool operator<=(const DateTime& lhs, const DateTime& rhs)
{
	return !(rhs < lhs);
}

bool operator>=(const DateTime& lhs, const DateTime& rhs)
{
	return !(lhs < rhs);
}

std::string to_string(DateTime const& time)
{
	std::stringstream ss;
	ss << std::put_time(std::localtime(&time.seconds), "%F %T");
	return ss.str();
}

bool operator==(const DateTime& lhs, const DateTime& rhs)
{
	return lhs.seconds == rhs.seconds;
}

bool operator!=(const DateTime& lhs, const DateTime& rhs)
{
	return !(rhs == lhs);
}

size_t hash<rd::DateTime>::operator()(const rd::DateTime& value) const noexcept
{
	return rd::hash<decltype(value.seconds)>()(value.seconds);
}
}	 // namespace rd

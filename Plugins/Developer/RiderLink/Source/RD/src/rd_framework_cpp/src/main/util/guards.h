#ifndef RD_CPP_GUARDS_H
#define RD_CPP_GUARDS_H

namespace rd
{
namespace util
{
template <typename T>
class increment_guard
{
	T& x;

public:
	explicit increment_guard(T& new_x) : x(new_x)
	{
		++x;
	}

	~increment_guard()
	{
		--x;
	}
};

class bool_guard
{
	bool& x;

public:
	explicit bool_guard(bool& new_x) : x(new_x)
	{
		x = true;
	}

	~bool_guard()
	{
		x = false;
	}
};
}	 // namespace util
}	 // namespace rd

#endif	  // RD_CPP_GUARDS_H

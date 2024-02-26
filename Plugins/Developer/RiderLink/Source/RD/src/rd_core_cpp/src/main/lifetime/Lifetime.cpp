#include "Lifetime.h"

#include <memory>

#include <thirdparty.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace rd
{
/*thread_local */ Lifetime::Allocator Lifetime::allocator;

LifetimeImpl* Lifetime::operator->() const
{
	return ptr.operator->();
}

std::once_flag onceFlag;

Lifetime::Lifetime(bool is_eternal) : ptr(std::allocate_shared<LifetimeImpl, Allocator>(allocator, is_eternal))
{
	std::call_once(onceFlag, [] {
		spdlog::set_default_logger(spdlog::stderr_color_mt<spdlog::synchronous_factory>("default", spdlog::color_mode::automatic));
	});
}

Lifetime Lifetime::create_nested() const
{
	Lifetime lw(false);
	ptr->attach_nested(lw.ptr);
	return lw;
}

Lifetime const& Lifetime::Eternal()
{
	static Lifetime ETERNAL(true);
	return ETERNAL;
}

bool operator==(Lifetime const& lw1, Lifetime const& lw2)
{
	return lw1.ptr == lw2.ptr;
}

bool operator!=(Lifetime const& lw1, Lifetime const& lw2)
{
	return !(lw1 == lw2);
}
}	 // namespace rd

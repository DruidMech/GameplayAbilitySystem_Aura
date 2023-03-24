#ifndef RD_CPP_CORE_SEQUENTIAL_LIFETIMES_H
#define RD_CPP_CORE_SEQUENTIAL_LIFETIMES_H

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:4251)
#endif

#include "LifetimeDefinition.h"
#include "Lifetime.h"

#include <rd_core_export.h>

namespace rd
{
class RD_CORE_API SequentialLifetimes
{
private:
	std::shared_ptr<LifetimeDefinition> current_def = LifetimeDefinition::get_shared_eternal();
	Lifetime parent_lifetime;

public:
	// region ctor/dtor
	SequentialLifetimes() = delete;

	SequentialLifetimes(SequentialLifetimes const&) = delete;

	SequentialLifetimes& operator=(SequentialLifetimes const&) = delete;

	SequentialLifetimes(SequentialLifetimes&&) = delete;

	SequentialLifetimes& operator=(SequentialLifetimes&&) = delete;

	explicit SequentialLifetimes(Lifetime parent_lifetime);
	// endregion

	Lifetime next();

	void terminate_current();

	bool is_terminated() const;

	void set_current_lifetime(std::shared_ptr<LifetimeDefinition> new_def);
};
}	 // namespace rd
#if defined(_MSC_VER)
#pragma warning(pop)
#endif


#endif	  // RD_CPP_CORE_SEQUENTIAL_LIFETIMES_H

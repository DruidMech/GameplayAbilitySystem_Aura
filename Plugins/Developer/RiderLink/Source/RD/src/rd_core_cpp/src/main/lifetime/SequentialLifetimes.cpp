#include "SequentialLifetimes.h"

namespace rd
{
SequentialLifetimes::SequentialLifetimes(Lifetime parent_lifetime) : parent_lifetime(std::move(parent_lifetime))
{
	this->parent_lifetime->add_action([this] { set_current_lifetime(LifetimeDefinition::get_shared_eternal()); });
}

Lifetime SequentialLifetimes::next()
{
	std::shared_ptr<LifetimeDefinition> new_def = std::make_shared<LifetimeDefinition>(parent_lifetime);
	set_current_lifetime(new_def);
	return current_def->lifetime;
}

void SequentialLifetimes::terminate_current()
{
	set_current_lifetime(LifetimeDefinition::get_shared_eternal());
}

bool SequentialLifetimes::is_terminated() const
{
	return current_def->is_eternal() || current_def->is_terminated();
}

void SequentialLifetimes::set_current_lifetime(std::shared_ptr<LifetimeDefinition> new_def)
{
	std::shared_ptr<LifetimeDefinition> prev = current_def;
	current_def = new_def;
	prev->terminate();
}
}	 // namespace rd

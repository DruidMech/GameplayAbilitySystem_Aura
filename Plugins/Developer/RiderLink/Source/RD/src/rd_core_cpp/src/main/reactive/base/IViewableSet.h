#ifndef RD_CPP_IVIEWABLESET_H
#define RD_CPP_IVIEWABLESET_H

#include "interfaces.h"
#include "viewable_collections.h"

#include <lifetime/LifetimeDefinition.h>
#include <util/core_util.h>

#include <std/unordered_map.h>

#include <thirdparty.hpp>

namespace rd
{
namespace detail
{
template <typename T>
class SetEvent
{
public:
	SetEvent(AddRemove kind, T const* value) : kind(kind), value(value)
	{
	}

	AddRemove kind;
	T const* value;

	friend std::string to_string(SetEvent const& e)
	{
		return to_string(e.kind) + ":" + to_string(*e.value);
	}
};
}	 // namespace detail

/**
 * \brief A set allowing its contents to be observed.
 * \tparam T type of stored values (may be abstract)
 */
template <typename T, typename A = allocator<T>>
class IViewableSet : public IViewable<T>, public ISource<detail::SetEvent<T>>
{
protected:
	using WT = value_or_wrapper<T>;
	mutable rd::unordered_map<Lifetime,
		ordered_map<T const*, LifetimeDefinition, wrapper::TransparentHash<T>, wrapper::TransparentKeyEqual<T>>>
		lifetimes;

public:
	// region ctor/dtor

	IViewableSet() = default;

	IViewableSet(IViewableSet&&) = default;

	IViewableSet& operator=(IViewableSet&&) = default;

	virtual ~IViewableSet() = default;
	// endregion

	/**
	 * \brief Represents an addition or removal of an element in the set.
	 */
	using Event = typename detail::SetEvent<T>;

	/**
	 * \brief Adds a subscription for additions and removals of set elements. When the subscription is initially
	 * added, [handler] is called with [AddRemove::Add] events for all elements currently in the set.
	 *
	 * \param lifetime lifetime of subscription.
	 * \param handler to be called.
	 */
	void advise(Lifetime lifetime, std::function<void(AddRemove, T const&)> handler) const
	{
		this->advise(lifetime, [handler](Event e) { handler(e.kind, *e.value); });
	}

	/**
	 * \brief Adds a subscription to changes of the contents of the set.
	 *
	 * \details When [handler] is initially added, it is called receiving all elements currently in the set.
	 * Every time an object is added to the set, the [handler] is called receiving the new element.
	 * The [Lifetime] instance passed to the handler expires when the element is removed from the set.
	 *
	 * \param lifetime
	 * \param handler
	 */
	void view(Lifetime lifetime, std::function<void(Lifetime, T const&)> handler) const override
	{
		advise(lifetime, [this, lifetime, handler](AddRemove kind, T const& key) {
			switch (kind)
			{
				case AddRemove::ADD:
				{
					/*auto const &[it, inserted] = lifetimes[lifetime].emplace(key, LifetimeDefinition(lifetime));*/
					auto const& it = lifetimes[lifetime].emplace(&key, lifetime);
					RD_ASSERT_MSG(it.second, "lifetime definition already exists in viewable set by key:" + to_string(key));
					handler(it.first->second.lifetime, key);
					break;
				}
				case AddRemove::REMOVE:
				{
					RD_ASSERT_MSG(lifetimes.at(lifetime).count(key) > 0,
						"attempting to remove non-existing lifetime in viewable set by key:" + to_string(key));
					LifetimeDefinition def = std::move(lifetimes.at(lifetime).at(key));
					lifetimes.at(lifetime).erase(key);
					def.terminate();
					break;
				}
			}
		});
	}

	/**
	 * \brief Adds a subscription for additions and removals of set elements. When the subscription is initially
	 * added, [handler] is called with [AddRemove.Add] events for all elements currently in the set.
	 *
	 * \param lifetime lifetime of subscription.
	 * \param handler to be called.
	 */
	void advise(Lifetime lifetime, std::function<void(Event const&)> handler) const override = 0;

	virtual bool add(WT) const = 0;

	virtual bool addAll(std::vector<WT> elements) const = 0;

	virtual void clear() const = 0;

	virtual bool remove(T const&) const = 0;

	virtual size_t size() const = 0;

	virtual bool contains(T const&) const = 0;

	virtual bool empty() const = 0;

	template <typename... Args>
	bool emplace_add(Args&&... args) const
	{
		return add(WT{std::forward<Args>(args)...});
	}
};
}	 // namespace rd

static_assert(
	std::is_move_constructible<rd::IViewableSet<int>::Event>::value, "Is move constructible from IViewableSet<int>::Event");

#endif	  // RD_CPP_IVIEWABLESET_H

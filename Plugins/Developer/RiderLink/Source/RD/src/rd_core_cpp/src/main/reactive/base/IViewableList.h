#ifndef RD_CPP_IVIEWABLELIST_H
#define RD_CPP_IVIEWABLELIST_H

#include "interfaces.h"
#include "viewable_collections.h"

#include <lifetime/LifetimeDefinition.h>
#include <util/overloaded.h>
#include <types/wrapper.h>

#include <std/unordered_map.h>

#include <vector>
#include <utility>
#include <algorithm>

#include "thirdparty.hpp"

namespace rd
{
namespace detail
{
template <typename T>
class ListEvent
{
public:
	class Add
	{
	public:
		int32_t index;
		T const* new_value;

		Add(int32_t index, T const* new_value) : index(index), new_value(new_value)
		{
		}
	};

	class Update
	{
	public:
		int32_t index;
		T const* old_value;
		T const* new_value;

		Update(int32_t index, T const* old_value, T const* new_value) : index(index), old_value(old_value), new_value(new_value)
		{
		}
	};

	class Remove
	{
	public:
		int32_t index;
		T const* old_value;

		Remove(int32_t index, T const* old_value) : index(index), old_value(old_value)
		{
		}
	};

	variant<Add, Update, Remove> v;

	ListEvent(Add x) : v(x)
	{
	}

	ListEvent(Update x) : v(x)
	{
	}

	ListEvent(Remove x) : v(x)
	{
	}

	int32_t get_index() const
	{
		return visit(util::make_visitor([](Add const& e) { return e.index; }, [](Update const& e) { return e.index; },
						 [](Remove const& e) { return e.index; }),
			v);
	}

	T const* get_new_value() const
	{
		return visit(util::make_visitor([](Add const& e) { return e.new_value; }, [](Update const& e) { return e.new_value; },
						 [](Remove const& /*e*/) { return static_cast<T const*>(nullptr); }),
			v);
	}

	friend std::string to_string(ListEvent const& e)
	{
		std::string res = visit(
			util::make_visitor(
				[](typename ListEvent::Add const& e) { return "Add " + std::to_string(e.index) + ":" + to_string(*e.new_value); },
				[](typename ListEvent::Update const& e) {
					return "Update " + std::to_string(e.index) + ":" +
						   //                       to_string(e.old_value) + ":" +
						   to_string(*e.new_value);
				},
				[](typename ListEvent::Remove const& e) { return "Remove " + std::to_string(e.index); }),
			e.v);
		return res;
	}
};
}	 // namespace detail
/**
 * \brief A list allowing its contents to be observed.
 * \tparam T type of stored values (may be abstract)
 */
template <typename T>
class IViewableList : public IViewable<std::pair<size_t, T const*>>, public ISource<detail::ListEvent<T>>
{
protected:
	using WT = value_or_wrapper<T>;

public:
	/**
	 * \brief Represents an addition, update or removal of an element in the list.
	 */
	using Event = typename detail::ListEvent<T>;

protected:
	mutable rd::unordered_map<Lifetime, std::vector<LifetimeDefinition>> lifetimes;

public:
	// region ctor/dtor

	IViewableList() = default;

	IViewableList(IViewableList&&) = default;

	IViewableList& operator=(IViewableList&&) = default;

	virtual ~IViewableList() = default;
	// endregion

	/**
	 * \brief Adds a subscription to additions and removals of list elements. When a list element is updated,
	 * the [handler] is called twice: to report the removal of the old element and the addition of the new one.
	 * \param lifetime lifetime of subscription.
	 * \param handler to be called.
	 */
	void advise_add_remove(Lifetime lifetime, std::function<void(AddRemove, size_t, T const&)> handler) const
	{
		advise(lifetime, [handler](Event e) {
			visit(util::make_visitor([handler](typename Event::Add const& e) { handler(AddRemove::ADD, e.index, *e.new_value); },
					  [handler](typename Event::Update const& e) {
						  handler(AddRemove::REMOVE, e.index, *e.old_value);
						  handler(AddRemove::ADD, e.index, *e.new_value);
					  },
					  [handler](typename Event::Remove const& e) { handler(AddRemove::REMOVE, e.index, *e.old_value); }),
				e.v);
		});
	}

	/**
	 * \brief Adds a subscription to changes of the contents of the list.
	 * \param lifetime lifetime of subscription.
	 * \param handler to be called.
	 */
	void view(Lifetime lifetime, std::function<void(Lifetime lifetime, std::pair<size_t, T const*> const&)> handler) const override
	{
		view(lifetime, [handler](Lifetime lt, size_t idx, T const& v) { handler(lt, std::make_pair(idx, &v)); });
	}

	/**
	 * \brief @see view	above
	 */
	void view(Lifetime lifetime, std::function<void(Lifetime, size_t, T const&)> handler) const
	{
		advise_add_remove(lifetime, [this, lifetime, handler](AddRemove kind, size_t idx, T const& value) {
			switch (kind)
			{
				case AddRemove::ADD:
				{
					LifetimeDefinition def(lifetime);
					std::vector<LifetimeDefinition>& v = lifetimes[lifetime];
					auto it = v.emplace(v.begin() + idx, std::move(def));
					handler(it->lifetime, idx, value);
					break;
				}
				case AddRemove::REMOVE:
				{
					LifetimeDefinition def = std::move(lifetimes.at(lifetime)[idx]);
					std::vector<LifetimeDefinition>& v = lifetimes.at(lifetime);
					v.erase(v.begin() + idx);
					def.terminate();
					break;
				}
			}
		});
	}

	void advise(Lifetime lifetime, std::function<void(Event const&)> handler) const override = 0;

	virtual bool add(WT element) const = 0;

	virtual bool add(size_t index, WT element) const = 0;

	virtual WT removeAt(size_t index) const = 0;

	virtual bool remove(T const& element) const = 0;

	virtual T const& get(size_t index) const = 0;

	virtual WT set(size_t index, WT element) const = 0;

	virtual bool addAll(size_t index, std::vector<WT> elements) const = 0;

	virtual bool addAll(std::vector<WT> elements) const = 0;

	virtual void clear() const = 0;

	virtual bool removeAll(std::vector<WT> elements) const = 0;

	virtual size_t size() const = 0;

	virtual bool empty() const = 0;

	template <typename... Args>
	bool emplace_add(Args&&... args) const
	{
		return add(WT{std::forward<Args>(args)...});
	}

	template <typename... Args>
	bool emplace_add(size_t index, Args&&... args) const
	{
		return add(index, WT{std::forward<Args>(args)...});
	}

	template <typename... Args>
	WT emplace_set(size_t index, Args&&... args) const
	{
		return set(index, WT{std::forward<Args>(args)...});
	}

	template <typename U>
	friend typename std::enable_if<(!std::is_abstract<U>::value), std::vector<U>>::type convert_to_list(
		IViewableList<U> const& list);

protected:
	virtual const std::vector<Wrapper<T>>& getList() const = 0;
};

template <typename T>
typename std::enable_if<(!std::is_abstract<T>::value), std::vector<T>>::type convert_to_list(IViewableList<T> const& list)
{
	std::vector<T> res(list.size());
	std::transform(list.getList().begin(), list.getList().end(), res.begin(), [](Wrapper<T> const& ptr) { return *ptr; });
	return res;
}
}	 // namespace rd

static_assert(
	std::is_move_constructible<rd::IViewableList<int>::Event>::value, "Is move constructible from IViewableList<int>::Event");

#endif	  // RD_CPP_IVIEWABLELIST_H

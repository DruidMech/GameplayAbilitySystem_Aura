#ifndef RD_CPP_CORE_VIEWABLESET_H
#define RD_CPP_CORE_VIEWABLESET_H

#include "base/IViewableSet.h"
#include "reactive/base/SignalX.h"

#include <std/allocator.h>
#include <util/core_util.h>

namespace rd
{
/**
 * \brief complete class which has @code IViewableSet<T>'s properties
 * \tparam T
 */
template <typename T, typename A = allocator<T>>
class ViewableSet : public IViewableSet<T, A>
{
public:
	using Event = typename IViewableSet<T>::Event;

	using IViewableSet<T, A>::advise;

private:
	using WT = typename IViewableSet<T, A>::WT;
	using WA = typename std::allocator_traits<A>::template rebind_alloc<Wrapper<T>>;

	Signal<Event> change;
	using data_t = ordered_set<Wrapper<T>, wrapper::TransparentHash<T>, wrapper::TransparentKeyEqual<T>, WA>;
	mutable data_t set;

public:
	// region ctor/dtor

	ViewableSet() = default;

	ViewableSet(ViewableSet&&) = default;

	ViewableSet& operator=(ViewableSet&&) = default;

	virtual ~ViewableSet() = default;
	// endregion

	// region iterators
public:
	class iterator
	{
		friend class ViewableSet<T>;

		typename data_t::iterator it_;

		explicit iterator(const typename data_t::iterator& it) : it_(it)
		{
		}

	public:
		using iterator_category = std::random_access_iterator_tag;
		using value_type = T;
		using difference_type = std::ptrdiff_t;
		using pointer = T const*;
		using reference = T const&;

		iterator(const iterator& other) = default;

		iterator(iterator&& other) noexcept = default;

		iterator& operator=(const iterator& other) = default;

		iterator& operator=(iterator&& other) noexcept = default;

		iterator& operator++()
		{
			++it_;
			return *this;
		}

		iterator operator++(int)
		{
			auto it = *this;
			++*this;
			return it;
		}

		iterator& operator--()
		{
			--it_;
			return *this;
		}

		iterator operator--(int)
		{
			auto it = *this;
			--*this;
			return it;
		}

		iterator& operator+=(difference_type delta)
		{
			it_ += delta;
			return *this;
		}

		iterator& operator-=(difference_type delta)
		{
			it_ -= delta;
			return *this;
		}

		iterator operator+(difference_type delta) const
		{
			auto it = *this;
			return it += delta;
		}

		iterator operator-(difference_type delta) const
		{
			auto it = *this;
			return it -= delta;
		}

		difference_type operator-(iterator const& other) const
		{
			return it_ - other.it_;
		}

		bool operator<(iterator const& other) const noexcept
		{
			return this->it_ < other.it_;
		}

		bool operator>(iterator const& other) const noexcept
		{
			return this->it_ > other.it_;
		}

		bool operator==(iterator const& other) const noexcept
		{
			return this->it_ == other.it_;
		}

		bool operator!=(iterator const& other) const noexcept
		{
			return !(*this == other);
		}

		bool operator<=(iterator const& other) const noexcept
		{
			return (this->it_ < other.it_) || (*this == other);
		}

		bool operator>=(iterator const& other) const noexcept
		{
			return (this->it_ > other.it_) || (*this == other);
		}

		reference operator*() const noexcept
		{
			return **it_;
		}

		pointer operator->() const noexcept
		{
			return (*it_).get();
		}
	};

	using reverse_iterator = std::reverse_iterator<iterator>;

	iterator begin() const
	{
		return iterator(set.begin());
	}

	iterator end() const
	{
		return iterator(set.end());
	}

	reverse_iterator rbegin() const
	{
		return reverse_iterator(end());
	}

	reverse_iterator rend() const
	{
		return reverse_iterator(begin());
	}

	// endregion

	bool add(WT element) const override
	{
		/*auto const &[it, success] = set.emplace(std::make_unique<T>(std::move(element)));*/
		auto const& it = set.emplace(std::move(element));
		if (!it.second)
		{
			return false;
		}
		change.fire(Event(AddRemove::ADD, &(wrapper::get<T>(*it.first))));
		return true;
	}

	bool addAll(std::vector<WT> elements) const override
	{
		for (auto&& element : elements)
		{
			ViewableSet::add(std::move(element));
		}
		return true;
	}

	void clear() const override
	{
		std::vector<Event> changes;
		for (auto const& element : set)
		{
			changes.push_back(Event(AddRemove::REMOVE, &(*element)));
		}
		for (auto const& e : changes)
		{
			change.fire(e);
		}
		set.clear();
	}

	bool remove(T const& element) const override
	{
		if (!ViewableSet::contains(element))
		{
			return false;
		}
		auto it = set.find(element);
		change.fire(Event(AddRemove::REMOVE, &(wrapper::get<T>(*it))));
		set.erase(it);
		return true;
	}

	void advise(Lifetime lifetime, std::function<void(Event const&)> handler) const override
	{
		for (auto const& x : set)
		{
			handler(Event(AddRemove::ADD, &(*x)));
		}
		change.advise(lifetime, handler);
	}

	size_t size() const override
	{
		return set.size();
	}

	bool contains(T const& element) const override
	{
		return set.count(element) > 0;
	}

	bool empty() const override
	{
		return set.empty();
	}

	template <typename... Args>
	bool emplace_add(Args&&... args) const
	{
		return add(WT{std::forward<Args>(args)...});
	}
};
}	 // namespace rd

static_assert(std::is_move_constructible<rd::ViewableSet<int>>::value, "Is move constructible from ViewableSet<int>");

#endif	  // RD_CPP_CORE_VIEWABLESET_H

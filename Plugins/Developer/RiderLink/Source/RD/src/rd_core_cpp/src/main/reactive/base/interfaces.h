#ifndef RD_CPP_CORE_INTERFACES_H
#define RD_CPP_CORE_INTERFACES_H

#include <lifetime/Lifetime.h>
#include <types/wrapper.h>
#include <util/core_traits.h>

#include <functional>
#include <type_traits>

namespace rd
{
/**
 * \brief An object that allows to subscribe to events.
 * \tparam T type of events
 */
template <typename T>
class ISource
{
public:
	virtual ~ISource() = default;

	/**
	 * \brief Adds an event subscription.
	 * \param lifetime lifetime of subscription.
	 * \param handler to be called, every time an event occurs.
	 */
	virtual void advise(Lifetime lifetime, std::function<void(T const&)> handler) const = 0;

	/**
	 * \brief @code advise with Eternal lifetime
	 */
	template <typename F>
	void advise_eternal(F&& handler) const
	{
		advise(Lifetime::Eternal(), std::forward<F>(handler));
	}

	/**
	 * \brief @code Void specialisation of @code advise method, at @tparam T=Void
	 */
	void advise(Lifetime lifetime, std::function<void()> handler) const
	{
		advise(lifetime, [handler = std::move(handler)](Void) { handler(); });
	}
};

/**
 * \brief An object that allows to subscribe to changes of its contents.
 * \tparam T type of content
 */
template <typename T>
class IViewable
{
public:
	virtual ~IViewable() = default;

	virtual void view(Lifetime lifetime, std::function<void(Lifetime, T const&)

											 >
											 handler) const = 0;
};

/**
 * \brief An object which has a collection of event listeners and can broadcast an event to the listeners.
 * \tparam T type of events
 */
template <typename T>
class ISignal : public ISource<T>
{
protected:
	using WT = value_or_wrapper<T>;

public:
	virtual ~ISignal() = default;

	virtual void fire(T const& value) const = 0;

	/**
	 * \brief @code fire specialisation at T=Void
	 */
	template <typename U = T>
	typename std::enable_if_t<util::is_void<U>> fire() const
	{
		fire(Void{});
	}
};
}	 // namespace rd

#endif	  // RD_CPP_CORE_INTERFACES_H

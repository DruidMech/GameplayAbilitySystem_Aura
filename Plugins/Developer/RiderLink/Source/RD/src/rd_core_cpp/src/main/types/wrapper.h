#ifndef RD_CPP_WRAPPER_H
#define RD_CPP_WRAPPER_H

#include <util/core_traits.h>
#include <std/allocator.h>
#include <std/hash.h>
#include <std/to_string.h>

#include <thirdparty.hpp>

#include <type_traits>
#include <memory>
#include <utility>

namespace rd
{
template <typename T, typename A = std::allocator<T>>
class Wrapper;

template <typename T, typename R = void>
struct helper
{
	using value_or_wrapper_type = T;
	using opt_or_wrapper_type = optional<T>;
	using property_storage = optional<T>;
	using raw_type = T;
};

template <typename T>
struct helper<T, typename std::enable_if_t<util::in_heap_v<T>>>
{
	using value_or_wrapper_type = Wrapper<T>;
	using opt_or_wrapper_type = Wrapper<T>;
	using property_storage = Wrapper<T>;
	using raw_type = T;
};

/*template<typename T>
struct helper<optional<T>> {
	using value_or_wrapper_type = Wrapper<T>;
	using opt_or_wrapper_type = Wrapper<T>;
	using property_storage = optional<optional<T>>;
	using raw_type = T;
};*/

template <typename T>
struct helper<Wrapper<T>>
{
	using value_or_wrapper_type = Wrapper<T>;
	using opt_or_wrapper_type = Wrapper<T>;
	using property_storage = Wrapper<Wrapper<T>>;
	using raw_type = T;
};

template <typename T>
using value_or_wrapper = typename helper<T>::value_or_wrapper_type;

template <typename T>
using opt_or_wrapper = typename helper<T>::opt_or_wrapper_type;

template <typename T>
using property_storage = typename helper<T>::property_storage;

template <typename T>
using raw_type = typename helper<T>::raw_type;

template <typename>
struct is_wrapper : std::false_type
{
};

template <typename T>
struct is_wrapper<Wrapper<T>> : std::true_type
{
};

template <typename T>
constexpr bool is_wrapper_v = is_wrapper<T>::value;

/**
 * \brief wrapper over value of any type. It supports semantic of shared ownership due to shared_ptr as storage.
 * \tparam T type of value
 */
template <typename T, typename A>
class Wrapper final : public std::shared_ptr<T>
{
private:
	template <typename, typename>
	friend class Wrapper;

	using Base = std::shared_ptr<T>;

	A alloc;

public:
	using type = T;

	// region ctor/dtor

	Wrapper() = default;

	Wrapper(Wrapper const&) = default;

	Wrapper& operator=(Wrapper const&) = default;

	Wrapper(Wrapper&&) = default;

	Wrapper& operator=(Wrapper&&) = default;

	constexpr explicit Wrapper(std::nullptr_t) noexcept
	{
	}

	constexpr Wrapper(nullopt_t) noexcept
	{
	}

	template <typename R, typename = typename std::enable_if_t<util::is_base_of_v<T, R>>>
	Wrapper(Wrapper<R> const& other) : Base(std::static_pointer_cast<T>(static_cast<std::shared_ptr<R>>(other)))
	{
	}

	template <typename R, typename = typename std::enable_if_t<util::is_base_of_v<T, R>>>
	Wrapper(Wrapper<R>&& other) : Base(std::static_pointer_cast<T>(static_cast<std::shared_ptr<R>>(std::move(other))))
	{
	}

	template <typename F, typename G = typename util::not_string_literal<F&&>::type,
		typename = typename std::enable_if_t<
			/*util::negation<
				util::disjunction<
					std::is_null_pointer<std::decay_t<F>>,
					std::is_same<Wrapper<T>, std::decay_t<F>>,
					detail::is_optional<std::decay_t<F>>
				>
			>::value*/
			util::conjunction<std::is_constructible<std::shared_ptr<T>, std::shared_ptr<G>>,
				util::negation<std::is_abstract<G>>>::value>>
	Wrapper(F&& value) : Base(std::allocate_shared<G>(alloc, std::forward<F>(value)))
	{
	}

	template <typename F>
	Wrapper(std::shared_ptr<F> const& ptr) noexcept : Base(std::static_pointer_cast<T>(ptr))
	{
	}

	template <typename F>
	Wrapper(std::shared_ptr<F>&& ptr) noexcept : Base(std::static_pointer_cast<T>(std::move(ptr)))
	{
	}

	template <typename U = T, typename R = typename std::enable_if_t<!std::is_abstract<std::decay_t<U>>::value>>
	Wrapper(optional<U>&& opt)
	{
		if (opt)
		{
			*this = std::allocate_shared<U>(alloc, *std::move(opt));
		}
	}

	template <typename R>
	static Wrapper<T> dynamic(Wrapper<R> const& w)
	{
		return Wrapper<T>(std::dynamic_pointer_cast<T>(w));
	}

	template <typename R>
	static Wrapper<T> dynamic(Wrapper<R>&& w)
	{
		return Wrapper<T>(std::dynamic_pointer_cast<T>(std::move(w)));
	}

	~Wrapper() = default;
	// endregion

	constexpr bool has_value() const
	{
		return operator bool();
	}

	constexpr T& operator*() &
	{
		return *static_cast<Base&>(*this);
	};

	constexpr T const& operator*() const&
	{
		return *static_cast<Base const&>(*this);
	};

	/*constexpr T &&operator*() &&{
		return *ptr.get();
	};*/

	T const* operator->() const
	{
		return Base::operator->();
	}

	T* operator->()
	{
		return Base::operator->();
	}

	explicit operator bool() const noexcept
	{
		return Base::operator bool();
	}

	friend bool operator==(const Wrapper& lhs, const Wrapper& rhs)
	{
		bool is_lhs = (bool) lhs;
		bool is_rhs = (bool) rhs;
		if (is_lhs != is_rhs)
		{
			return false;
		}
		if (!is_lhs && !is_rhs)
		{
			return true;
		}
		return *lhs.get() == *rhs.get();
	}

	friend bool operator!=(const Wrapper& lhs, const Wrapper& rhs)
	{
		return !(rhs == lhs);
	}

	friend std::string to_string(Wrapper const& value)
	{
		return value.has_value() ? to_string(*value) : "nullptr"s;
	}
};

namespace wrapper
{
template <typename T>
decltype(auto) get(T&& w)
{
	return std::forward<T>(w);
}

template <typename T>
decltype(auto) get(T const& w)
{
	return w;
}

template <typename T>
T& get(Wrapper<T>& w)
{
	return *w;
}

template <typename T>
T const& get(Wrapper<T> const& w)
{
	return *w;
}

template <typename T>
T&& get(Wrapper<T>&& w)
{
	return *std::move(std::move(w));
}

/*template<typename T>
Wrapper<T> make_wrapper(std::unique_ptr<T> &&value) {
	return Wrapper<T>(std::move(value));
}*/

template <typename T>
typename std::enable_if_t<!util::in_heap_v<T>, T> unwrap(Wrapper<T>&& ptr)
{
	return std::move(*ptr);
}

template <typename T>
typename std::enable_if_t<util::in_heap_v<T>, Wrapper<T>> unwrap(Wrapper<T>&& ptr)
{
	return Wrapper<T>(std::move(ptr));
}

template <typename T, typename... Args>
Wrapper<T> make_wrapper(Args&&... args)
{
	return Wrapper<T>(std::make_shared<T>(std::forward<Args>(args)...));
}

template <typename T, typename A, typename... Args>
Wrapper<T> allocate_wrapper(const A& alloc, Args&&... args)
{
	return Wrapper<T>(std::allocate_shared<T, A>(alloc, std::forward<Args>(args)...));
}
/*template<typename T>
constexpr Wrapper<T> null_wrapper = Wrapper<T>(nullptr);*/
}	 // namespace wrapper

template <typename T>
struct hash<rd::Wrapper<T>>
{
	size_t operator()(const rd::Wrapper<T>& value) const noexcept
	{
		return rd::hash<T>()(*value);
	}
};
}	 // namespace rd

static_assert(rd::is_wrapper<rd::Wrapper<std::wstring>>::value, "is wrapper doesn't work");

#endif	  // RD_CPP_WRAPPER_H

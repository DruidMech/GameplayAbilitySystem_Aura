#ifndef RD_CPP_IPROPERTYBASE_H
#define RD_CPP_IPROPERTYBASE_H

#include "interfaces.h"
#include "SignalX.h"

#include <types/wrapper.h>
#include <lifetime/SequentialLifetimes.h>

#include "thirdparty.hpp"

namespace rd
{
template <typename T>
class IPropertyBase : public ISource<T>, public IViewable<T>
{
protected:
	mutable property_storage<T> value;

	Signal<T> change, before_change;

	using WT = value_or_wrapper<T>;

public:
	bool has_value() const
	{
		return (bool) (value);
	}

	// region ctor/dtor

	IPropertyBase() = default;

	IPropertyBase(IPropertyBase&& other) = default;

	IPropertyBase& operator=(IPropertyBase&& other) = default;

	template <typename F>
	explicit IPropertyBase(F&& value) : value(std::forward<F>(value))
	{
	}

	virtual ~IPropertyBase() = default;
	// endregion

	virtual void advise_before(Lifetime lifetime, std::function<void(T const&)> handler) const = 0;

	void view(Lifetime lifetime, std::function<void(Lifetime, T const&)> handler) const override
	{
		if (lifetime->is_terminated())
			return;

		Lifetime lf = lifetime.create_nested();
		std::shared_ptr<SequentialLifetimes> seq = std::make_shared<SequentialLifetimes>(lf);

		this->advise_before(lf, [lf, seq](T const& /*v*/) {
			if (!lf->is_terminated())
			{
				seq->terminate_current();
			}
		});

		this->advise(lf, [lf, seq, handler](T const& v) {
			if (!lf->is_terminated())
			{
				handler(seq->next(), v);
			}
		});
	}
};
}	 // namespace rd

#endif	  // RD_CPP_IPROPERTYBASE_H

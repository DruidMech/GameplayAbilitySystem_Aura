#ifndef RD_CPP_RDPROPERTY_H
#define RD_CPP_RDPROPERTY_H

#include "base/RdPropertyBase.h"
#include "serialization/Polymorphic.h"
#include "serialization/ISerializable.h"
#include "std/allocator.h"

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

namespace rd
{
/**
 * \brief Reactive property for connection through wire.

 * \tparam T type of stored value
 * \tparam S "SerDes" for value
 */
template <typename T, typename S = Polymorphic<T>, typename A = allocator<T>>
class RdProperty final : public RdPropertyBase<T, S>, public ISerializable
{
public:
	using value_type = T;

	// region ctor/dtor

	RdProperty() = default;

	RdProperty(RdProperty const&) = delete;

	RdProperty& operator=(RdProperty const&) = delete;

	RdProperty(RdProperty&&) = default;

	RdProperty& operator=(RdProperty&&) = default;

	template <typename F>
	explicit RdProperty(F&& value) : RdPropertyBase<T, S>(std::forward<F>(value))
	{
	}

	virtual ~RdProperty() = default;
	// endregion

	static RdProperty<T, S> read(SerializationCtx& ctx, Buffer& buffer)
	{
		RdId id = RdId::read(buffer);
		bool not_null = buffer.read_bool();	   // not null/
		(void) not_null;
		auto value = S::read(ctx, buffer);
		RdProperty<T, S> property;
		property.value = std::move(value);
		withId(property, id);
		return property;
	}

	void write(SerializationCtx& ctx, Buffer& buffer) const override
	{
		this->rdid.write(buffer);
		buffer.write_bool(true);
		S::write(ctx, buffer, this->get());
	}

	void advise(Lifetime lifetime, std::function<void(T const&)> handler) const override
	{
		RdPropertyBase<T, S>::advise(lifetime, std::move(handler));
	}

	RdProperty<T, S>& slave()
	{
		this->is_master = false;
		return *this;
	}

	void identify(Identities const& identities, RdId const& id) const override
	{
		RdBindableBase::identify(identities, id);
		if (!this->optimize_nested && this->has_value())
		{
			identifyPolymorphic(this->get(), identities, identities.next(id));
		}
	}

	friend bool operator==(const RdProperty& lhs, const RdProperty& rhs)
	{
		return &lhs == &rhs;
	}

	friend bool operator!=(const RdProperty& lhs, const RdProperty& rhs)
	{
		return !(rhs == lhs);
	}

	friend std::string to_string(RdProperty const& value)
	{
		return to_string(static_cast<Property<T> const&>(value));
	}
};
}	 // namespace rd

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

static_assert(std::is_move_constructible<rd::RdProperty<int>>::value, "Is move constructible from RdProperty<int>");

#endif	  // RD_CPP_RDPROPERTY_H

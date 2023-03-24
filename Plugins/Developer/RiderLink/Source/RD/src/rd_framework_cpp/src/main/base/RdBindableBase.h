#ifndef RD_CPP_RDBINDABLEBASE_H
#define RD_CPP_RDBINDABLEBASE_H

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:4251)
#endif

#include "IRdBindable.h"
#include "base/IProtocol.h"

#include "thirdparty.hpp"

#include <rd_framework_export.h>

namespace rd
{
class RD_FRAMEWORK_API RdBindableBase : public virtual IRdBindable /*, IPrintable*/
{
protected:
	mutable RName location;
	mutable IRdDynamic const* parent = nullptr;
	mutable RdId rdid = RdId::Null();

	mutable optional<Lifetime> bind_lifetime;

	bool is_bound() const;

	const IProtocol* get_protocol() const override;

	virtual std::string toString() const;

public:
	// region ctor/dtor

	RdBindableBase() : location("<<not bound>>")
	{
	}

	RdBindableBase(RdBindableBase&& other) = default;

	RdBindableBase& operator=(RdBindableBase&& other) = default;

	~RdBindableBase() override = default;
	// endregion

	// need to implement in subclasses
	virtual void init(Lifetime lifetime) const;

	const RName& get_location() const override;

	RdId get_id() const override;

	void set_id(RdId id) const override;

	void bind(Lifetime lf, IRdDynamic const* parent, string_view name) const override;

	void identify(const Identities& identities, RdId const& id) const override;

	SerializationCtx& get_serialization_context() const override;

	mutable ordered_map<std::string, std::shared_ptr<IRdBindable>> bindable_extensions;	   // TO-DO concurrency
	// mutable std::map<std::string, std::any> non_bindable_extensions;//TO-DO concurrency

	template <typename T, typename... Args>
	auto getOrCreateExtension(std::string name, Args&&... args) const ->
		typename std::enable_if_t<util::is_base_of_v<IRdBindable, T>, T> const&
	{
		auto it = bindable_extensions.find(name);
		if (it != bindable_extensions.end())
		{
			return *dynamic_cast<T const*>(it->second.get());
		}
		std::shared_ptr<T> new_extension = std::make_shared<T>(std::forward<Args>(args)...);
		T const& res = *new_extension.get();
		if (bind_lifetime.has_value())
		{
			auto protocol = get_protocol();
			new_extension->identify(*protocol->get_identity(), rdid.mix(".").mix(name));
			new_extension->bind(*bind_lifetime, this, name);
		}
		bindable_extensions.emplace(name, std::move(new_extension));
		return res;
	}

	/* template<typename T>
 std::enable_if_t<!util::is_base_of_v<IRdBindable, T>, T> const &
 getOrCreateExtension(std::string const &name, std::function<T()> create) const {
	 if (non_bindable_extensions.count(name) == 0) {
		 return std::any_cast<T const &>(non_bindable_extensions[name] = create());
	 }
	 return std::any_cast<T const &>(non_bindable_extensions.at(name));
 }*/
};

// T : RdBindableBase
template <typename T>
T& withId(T& that, RdId const& id)
{
	RD_ASSERT_MSG(that.get_id() == RdId::Null(), "this.id != RdId.NULL_ID, but " + to_string(that.get_id()));
	RD_ASSERT_MSG(id != RdId::Null(), "id != RdId.NULL_ID");

	that.set_id(id);
	return that;
}

template <typename T>
T& statics(T& that, int64_t id)
{
	RD_ASSERT_MSG((id > 0 && id < RdId::MAX_STATIC_ID), ("Expected id > 0 && id < RdId.MaxStaticId, got " + std::to_string(id)));
	return withId(that, RdId(static_cast<int64_t>(id)));
}

template <typename T>
T& withIdFromName(T& that, std::string const& name)
{
	return withId(that, RdId::Null().mix(name));
}
}	 // namespace rd
#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#endif	  // RD_CPP_RDBINDABLEBASE_H

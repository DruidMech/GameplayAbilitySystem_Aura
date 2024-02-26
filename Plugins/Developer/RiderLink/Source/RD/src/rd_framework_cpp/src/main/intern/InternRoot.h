#ifndef RD_CPP_INTERNROOT_H
#define RD_CPP_INTERNROOT_H

#include "base/RdReactiveBase.h"
#include "InternScheduler.h"
#include "lifetime/Lifetime.h"
#include "types/wrapper.h"
#include "serialization/RdAny.h"
#include "util/core_traits.h"

#include "tsl/ordered_map.h"

#include <vector>
#include <string>
#include <mutex>

#include <rd_framework_export.h>

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4250)
#pragma warning(disable : 4251)
#endif

namespace rd
{
// region predeclared

class Identities;

// endregion

/**
 * \brief Node in graph for storing interned objects.
 */
class RD_FRAMEWORK_API InternRoot final : public RdReactiveBase
{
private:
	// template<typename T>
	mutable std::vector<InternedAny> my_items_lis;

	// template<typename T>
	mutable ordered_map<int32_t, InternedAny> other_items_list;
	// template<typename T>
	mutable ordered_map<InternedAny, int32_t, any::TransparentHash, any::TransparentKeyEqual> inverse_map;

	mutable InternScheduler intern_scheduler;

	mutable std::recursive_mutex lock;

	void set_interned_correspondence(int32_t id, InternedAny&& value) const;

	static constexpr bool is_index_owned(int32_t id);

public:
	// region ctor/dtor

	InternRoot();
	// endregion

	template <typename T>
	int32_t intern_value(Wrapper<T> value) const;

	template <typename T>
	Wrapper<T> un_intern_value(int32_t id) const;

	IScheduler* get_wire_scheduler() const override;

	void bind(Lifetime lf, IRdDynamic const* parent, string_view name) const override;

	void identify(const Identities& identities, RdId const& id) const override;

	void on_wire_received(Buffer buffer) const override;
};
}	 // namespace rd

#include "serialization/InternedAnySerializer.h"

namespace rd
{
/*template<typename T>
std::vector<value_or_wrapper<T>> InternRoot::myItemsList = {};

template<typename T>
ordered_map<int32_t, value_or_wrapper<T>> InternRoot::otherItemsList = {};

template<typename T>
ordered_map<value_or_wrapper<T>, int32_t> InternRoot::inverseMap = {};*/

constexpr bool InternRoot::is_index_owned(int32_t id)
{
	return !static_cast<bool>(id & 1);
}

template <typename T>
Wrapper<T> InternRoot::un_intern_value(int32_t id) const
{
	// don't need lock because value's already exists and never removes
	return any::get<T>(is_index_owned(id) ? my_items_lis[id / 2] : other_items_list[id / 2]);
}

template <typename T>
int32_t InternRoot::intern_value(Wrapper<T> value) const
{
	InternedAny any = any::make_interned_any<T>(value);

	std::lock_guard<decltype(lock)> guard(lock);

	auto it = inverse_map.find(any);
	int32_t index = 0;
	if (it == inverse_map.end())
	{
		get_protocol()->get_wire()->send(this->rdid, [this, &index, value, any](Buffer& buffer) {
			InternedAnySerializer::write<T>(get_serialization_context(), buffer, wrapper::get<T>(value));
			{
				std::lock_guard<decltype(lock)> guard(lock);
				index = static_cast<int32_t>(my_items_lis.size()) * 2;
				my_items_lis.emplace_back(any);
			}
			buffer.write_integral<int32_t>(index);
		});
	}
	else
	{
		index = it->second;
	}
	if (inverse_map.count(any) == 0)
	{
		inverse_map[any] = index;
	}
	return index;
}
}	 // namespace rd
#if defined(_MSC_VER)
#pragma warning(pop)
#endif


#endif	  // RD_CPP_INTERNROOT_H

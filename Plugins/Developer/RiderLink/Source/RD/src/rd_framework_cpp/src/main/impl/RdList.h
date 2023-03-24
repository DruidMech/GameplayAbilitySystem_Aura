#ifndef RD_CPP_RDLIST_H
#define RD_CPP_RDLIST_H

#include "reactive/ViewableList.h"
#include "base/RdReactiveBase.h"
#include "serialization/Polymorphic.h"
#include "std/allocator.h"

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

namespace rd
{
/**
 * \brief Reactive list for connection through wire.
 *
 * \tparam T type of stored values
 * \tparam S "SerDes" for values
 * \tparam A allocator for values
 */
template <typename T, typename S = Polymorphic<T>, typename A = allocator<T>>
class RdList final : public RdReactiveBase, public ViewableList<T, A>, public ISerializable
{
private:
	using WT = typename IViewableList<T>::WT;

	//		mutable ViewableList<T> list;
	using list = ViewableList<T>;
	mutable int64_t next_version = 1;

	std::string logmsg(Op op, int64_t version, int32_t key, T const* value = nullptr) const
	{
		return "list " + to_string(location) + " " + to_string(rdid) + ":: " + to_string(op) + ":: key = " + std::to_string(key) +
			   ((version > 0) ? " :: version = " + std::to_string(version) : "") +
			   " :: value = " + (value ? to_string(*value) : "");
	}

public:
	using Event = typename IViewableList<T>::Event;

	using value_t = T;
	// region ctor/dtor

	RdList() = default;

	RdList(RdList&&) = default;

	RdList& operator=(RdList&&) = default;

	virtual ~RdList() = default;
	// endregion

	static RdList<T, S> read(SerializationCtx& /*ctx*/, Buffer& buffer)
	{
		RdList<T, S> result;
		int64_t next_version = buffer.read_integral<int64_t>();
		RdId id = RdId::read(buffer);

		result.next_version = next_version;
		withId(result, std::move(id));
		return result;
	}

	void write(SerializationCtx& /*ctx*/, Buffer& buffer) const override
	{
		buffer.write_integral<int64_t>(next_version);
		rdid.write(buffer);
	}

	static const int32_t versionedFlagShift = 2;	// update when changing Op

	bool optimize_nested = false;

	void init(Lifetime lifetime) const override
	{
		RdBindableBase::init(lifetime);

		local_change([this, lifetime] {
			advise(lifetime, [this, lifetime](typename IViewableList<T>::Event e) {
				if (!is_local_change)
					return;

				if (!optimize_nested)
				{
					T const* new_value = e.get_new_value();
					if (new_value)
					{
						const IProtocol* iProtocol = get_protocol();
						const Identities* identity = iProtocol->get_identity();
						identifyPolymorphic(*new_value, *identity, identity->next(rdid));
					}
				}

				get_wire()->send(rdid, [this, e](Buffer& buffer) {
					Op op = static_cast<Op>(e.v.index());

					buffer.write_integral<int64_t>(static_cast<int64_t>(op) | (next_version++ << versionedFlagShift));
					buffer.write_integral<int32_t>(static_cast<const int32_t>(e.get_index()));

					T const* new_value = e.get_new_value();
					if (new_value)
					{
						S::write(this->get_serialization_context(), buffer, *new_value);
					}
					spdlog::get("logSend")->trace(logmsg(op, next_version - 1, e.get_index(), new_value));
				});
			});
		});

		get_wire()->advise(lifetime, this);

		if (!optimize_nested)
		{
			this->view(lifetime, [this](Lifetime lf, size_t index, T const& value) {
				bindPolymorphic(value, lf, this, "[" + std::to_string(index) + "]");
			});
		}
	}

	void on_wire_received(Buffer buffer) const override
	{
		int64_t header = (buffer.read_integral<int64_t>());
		int64_t version = header >> versionedFlagShift;
		Op op = static_cast<Op>((header & ((1 << versionedFlagShift) - 1L)));
		int32_t index = (buffer.read_integral<int32_t>());

		RD_ASSERT_MSG(version == next_version,
			("Version conflict for " + to_string(location) + "}. Expected version " + std::to_string(next_version) + ", received " +
				std::to_string(version) + ". Are you modifying a list from two sides?"));

		next_version++;

		switch (op)
		{
			case Op::ADD:
			{
				auto value = S::read(this->get_serialization_context(), buffer);

				spdlog::get("logReceived")->trace(logmsg(op, version, index, &(wrapper::get<T>(value))));

				(index < 0) ? list::add(std::move(value)) : list::add(static_cast<size_t>(index), std::move(value));
				break;
			}
			case Op::UPDATE:
			{
				auto value = S::read(this->get_serialization_context(), buffer);

				spdlog::get("logReceived")->trace(logmsg(op, version, index, &(wrapper::get<T>(value))));

				list::set(static_cast<size_t>(index), std::move(value));
				break;
			}
			case Op::REMOVE:
			{
				spdlog::get("logReceived")->trace(logmsg(op, version, index));

				list::removeAt(static_cast<size_t>(index));
				break;
			}
			case Op::ACK:
				break;
		}
	}

	void advise(Lifetime lifetime, std::function<void(Event const&)> handler) const override
	{
		if (is_bound())
		{
			assert_threading();
		}
		list::advise(lifetime, handler);
	}

	bool add(WT element) const override
	{
		return local_change([this, element = std::move(element)]() mutable { return list::add(std::move(element)); });
	}

	bool add(size_t index, WT element) const override
	{
		return local_change([this, index, element = std::move(element)]() mutable { return list::add(index, std::move(element)); });
	}

	bool remove(T const& element) const override
	{
		return local_change([&] { return list::remove(element); });
	}

	WT removeAt(size_t index) const override
	{
		return local_change([&] { return list::removeAt(index); });
	}

	T const& get(size_t index) const override
	{
		return list::get(index);
	};

	WT set(size_t index, WT element) const override
	{
		return local_change([&] { return list::set(index, std::move(element)); });
	}

	void clear() const override
	{
		return local_change([&] { list::clear(); });
	}

	size_t size() const override
	{
		return list::size();
	}

	bool empty() const override
	{
		return list::empty();
	}

	std::vector<Wrapper<T>> const& getList() const override
	{
		return list::getList();
	}

	bool addAll(size_t index, std::vector<WT> elements) const override
	{
		return local_change([&] { return list::addAll(index, std::move(elements)); });
	}

	bool addAll(std::vector<WT> elements) const override
	{
		return local_change([&] { return list::addAll(std::move(elements)); });
	}

	bool removeAll(std::vector<WT> elements) const override
	{
		return local_change([&] { return list::removeAll(std::move(elements)); });
	}

	friend std::string to_string(RdList const& value)
	{
		std::string res = "[";
		for (auto const& p : value)
		{
			res += to_string(p) + ",";
		}
		return res + "]";
	}
	// region iterators

	using iterator = typename ViewableList<T>::iterator;

	using reverse_iterator = typename ViewableList<T>::reverse_iterator;
	// endregion
};
}	 // namespace rd

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

static_assert(std::is_move_constructible<rd::RdList<int>>::value, "Is move constructible RdList<int>");

#endif	  // RD_CPP_RDLIST_H

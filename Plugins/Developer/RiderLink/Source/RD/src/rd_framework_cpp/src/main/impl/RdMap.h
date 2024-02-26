#ifndef RD_CPP_RDMAP_H
#define RD_CPP_RDMAP_H

#include "reactive/ViewableMap.h"
#include "base/RdReactiveBase.h"
#include "serialization/Polymorphic.h"
#include "util/shared_function.h"

#include <cstdint>

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

namespace rd
{
/**
 * \brief Reactive map for connection through wire.
 *
 * \tparam K type of stored keys
 * \tparam V type of stored values
 * \tparam KS "SerDes" for keys
 * \tparam VS "SerDes" for values
 * \tparam KA allocator for keys
 * \tparam VA allocator for values
 */
template <typename K, typename V, typename KS = Polymorphic<K>, typename VS = Polymorphic<V>, typename KA = std::allocator<K>,
	typename VA = std::allocator<V>>
class RdMap final : public RdReactiveBase, public ViewableMap<K, V, KA, VA>, public ISerializable
{
private:
	using WK = typename IViewableMap<K, V>::WK;
	using WV = typename IViewableMap<K, V>::WV;
	using OV = typename IViewableMap<K, V>::OV;

	using map = ViewableMap<K, V>;
	mutable int64_t next_version = 0;
	mutable ordered_map<K const*, int64_t, wrapper::TransparentHash<K>, wrapper::TransparentKeyEqual<K>> pendingForAck;

	std::string logmsg(Op op, int64_t version, K const* key, V const* value = nullptr) const
	{
		return "map " + to_string(location) + " " + to_string(rdid) + ":: " + to_string(op) + ":: key = " + to_string(*key) +
			   ((version > 0) ? " :: version = " + std::to_string(version) : "") +
			   " :: value = " + (value ? to_string(*value) : "");
	}

	std::string logmsg(Op op, int64_t version, K const* key, optional<WV> const& value) const
	{
		return logmsg(op, version, key, value ? &(wrapper::get(*value)) : nullptr);
	}

public:
	bool is_master = false;

	bool optimize_nested = false;

	using Event = typename IViewableMap<K, V>::Event;

	using key_type = K;
	using value_type = V;

	// region ctor/dtor

	RdMap() = default;

	RdMap(RdMap&&) = default;

	RdMap& operator=(RdMap&&) = default;

	virtual ~RdMap() = default;
	// endregion

	static RdMap<K, V, KS, VS> read(SerializationCtx& /*ctx*/, Buffer& buffer)
	{
		RdMap<K, V, KS, VS> res;
		RdId id = RdId::read(buffer);
		withId(res, id);
		return res;
	}

	void write(SerializationCtx& /*ctx*/, Buffer& buffer) const override
	{
		rdid.write(buffer);
	}

	static const int32_t versionedFlagShift = 8;

	void init(Lifetime lifetime) const override
	{
		RdBindableBase::init(lifetime);

		local_change([this, lifetime]() {
			advise(lifetime, [this, lifetime](Event e) {
				if (!is_local_change)
					return;

				V const* new_value = e.get_new_value();
				if (new_value)
				{
					const IProtocol* iProtocol = get_protocol();
					const Identities* identity = iProtocol->get_identity();
					identifyPolymorphic(*new_value, *identity, identity->next(rdid));
				}

				get_wire()->send(rdid, [this, e](Buffer& buffer) {
					int32_t versionedFlag = ((is_master ? 1 : 0)) << versionedFlagShift;
					Op op = static_cast<Op>(e.v.index());

					buffer.write_integral<int32_t>(static_cast<int32_t>(op) | versionedFlag);

					int64_t version = is_master ? ++next_version : 0L;

					if (is_master)
					{
						pendingForAck.emplace(e.get_key(), version);
						buffer.write_integral(version);
					}

					KS::write(this->get_serialization_context(), buffer, *e.get_key());

					V const* new_value = e.get_new_value();
					if (new_value)
					{
						VS::write(this->get_serialization_context(), buffer, *new_value);
					}

					spdlog::get("logSend")->trace("SEND{}", logmsg(op, next_version - 1, e.get_key(), new_value));
				});
			});
		});

		get_wire()->advise(lifetime, this);

		if (!optimize_nested)
			this->view(lifetime, [this](Lifetime lf, std::pair<K const*, V const*> entry) {
				bindPolymorphic(entry.second, lf, this, "[" + to_string(*entry.first) + "]");
			});
	}

	void on_wire_received(Buffer buffer) const override
	{
		int32_t header = buffer.read_integral<int32_t>();
		bool msg_versioned = (header >> versionedFlagShift) != 0;
		Op op = static_cast<Op>(header & ((1 << versionedFlagShift) - 1));

		int64_t version = msg_versioned ? buffer.read_integral<int64_t>() : 0;

		WK key = KS::read(this->get_serialization_context(), buffer);

		if (op == Op::ACK)
		{
			std::string errmsg;
			if (!msg_versioned)
			{
				errmsg = "Received " + to_string(Op::ACK) + " while msg hasn't versioned flag set";
			}
			else if (!is_master)
			{
				errmsg = "Received " + to_string(Op::ACK) + " when not a Master";
			}
			else
			{
				if (pendingForAck.count(key) > 0)
				{
					int64_t pendingVersion = pendingForAck.at(key);
					if (pendingVersion < version)
					{
						errmsg = "Pending version " + std::to_string(pendingVersion) + " < " + to_string(Op::ACK) + " version `" +
								 std::to_string(version);
					}
					else
					{
						// side effect
						if (pendingVersion == version)
						{
							pendingForAck.erase(key);	 // else we don't need to remove, silently drop
						}
						// return good result
					}
				}
				else
				{
					errmsg = "No pending for " + to_string(Op::ACK);
				}
			}
			if (errmsg.empty())
			{
				spdlog::get("logReceived")->trace(logmsg(Op::ACK, version, &(wrapper::get<K>(key))));
			}
			else
			{
				spdlog::get("logReceived")->error(logmsg(Op::ACK, version, &(wrapper::get<K>(key))) + " >> " + errmsg);
			}
		}
		else
		{
			Buffer serialized_key;
			KS::write(this->get_serialization_context(), serialized_key, wrapper::get<K>(key));

			bool is_put = (op == Op::ADD || op == Op::UPDATE);
			optional<WV> value;
			if (is_put)
			{
				value = VS::read(this->get_serialization_context(), buffer);
			}

			if (msg_versioned || !is_master || pendingForAck.count(key) == 0)
			{
				spdlog::get("logReceived")->trace("RECV{}", logmsg(op, version, &(wrapper::get<K>(key)), value));
				if (value.has_value())
				{
					map::set(std::move(key), *std::move(value));
				}
				else
				{
					map::remove(wrapper::get<K>(key));
				}
			}
			else
			{
				spdlog::get("logReceived")->trace("{} >> REJECTED", logmsg(op, version, &(wrapper::get<K>(key)), value));
			}

			if (msg_versioned)
			{
				auto writer =
					util::make_shared_function([version, serialized_key = std::move(serialized_key)](Buffer& innerBuffer) mutable {
						innerBuffer.write_integral<int32_t>((1u << versionedFlagShift) | static_cast<int32_t>(Op::ACK));
						innerBuffer.write_integral<int64_t>(version);
						// KS::write(this->get_serialization_context(), innerBuffer, wrapper::get<K>(key));
						innerBuffer.write_byte_array_raw(serialized_key.getArray());
						// logSend.trace(logmsg(Op::ACK, version, serialized_key));
					});
				get_wire()->send(rdid, std::move(writer));
				if (is_master)
				{
					spdlog::get("logReceived")->error("Both ends are masters: {}", to_string(location));
				}
			}
		}
	}

	void advise(Lifetime lifetime, std::function<void(Event const&)> handler) const override
	{
		if (is_bound())
		{
			assert_threading();
		}
		map::advise(lifetime, handler);
	}

	V const* get(K const& key) const override
	{
		return local_change([&] { return map::get(key); });
	}

	V const* set(WK key, WV value) const override
	{
		return local_change([&]() mutable { return map::set(std::move(key), std::move(value)); });
	}

	OV remove(K const& key) const override
	{
		return local_change([&] { return map::remove(key); });
	}

	void clear() const override
	{
		return local_change([&] { return map::clear(); });
	}

	size_t size() const override
	{
		return map::size();
	}

	bool empty() const override
	{
		return map::empty();
	}

	friend std::string to_string(RdMap const& value)
	{
		std::string res = "[";
		for (auto it = value.begin(); it != value.end(); ++it)
		{
			res += to_string(it.key()) + "=>" + to_string(it.value()) + ",";
		}
		return res + "]";
	}
};
}	 // namespace rd

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

static_assert(std::is_move_constructible<rd::RdMap<int, int>>::value, "Is move constructible RdMap<int, int>");
static_assert(std::is_move_assignable<rd::RdMap<int, int>>::value, "Is move constructible RdMap<int, int>");
static_assert(std::is_default_constructible<rd::RdMap<int, int>>::value, "Is default constructible RdMap<int, int>");

#endif	  // RD_CPP_RDMAP_H

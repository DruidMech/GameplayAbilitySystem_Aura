#ifndef RD_CPP_IWIRE_H
#define RD_CPP_IWIRE_H

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:4251)
#endif

#include "reactive/base/interfaces.h"
#include "base/IRdReactive.h"
#include "reactive/Property.h"

#include <rd_framework_export.h>

namespace rd
{
class RdReactiveBase;
/**
 * \brief Sends and receives serialized object data over a network or a similar connection.
 */
class RD_FRAMEWORK_API IWire
{
public:
	Property<bool> connected{false};
	Property<bool> heartbeatAlive{false};

	// region ctor/dtor

	IWire() = default;

	IWire(IWire&&) = default;

	virtual ~IWire() = default;
	// endregion

	/**
	 * \brief Sends a data block with the given [id] and the given [writer] function that can write the data.
	 * \param id of recipient.
	 * \param writer is used to serialise data before send.
	 */
	virtual void send(RdId const& id, std::function<void(Buffer& buffer)> writer) const = 0;

	/**
	 * \brief Adds a [handler] for receiving updated values of the object with the given [id]. The handler is removed
	 * when the given [lifetime] is terminated.
	 * \param lifetime lifetime of subscription.
	 * \param entity to be subscripted
	 */
	virtual void advise(Lifetime lifetime, RdReactiveBase const* entity) const = 0;
};
}	 // namespace rd
#if defined(_MSC_VER)
#pragma warning(pop)
#endif


#endif	  // RD_CPP_IWIRE_H
